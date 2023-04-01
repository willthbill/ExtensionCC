#include <bits/stdc++.h>
#include <boost/filesystem.hpp>
#include <CGAL/Boolean_set_operations_2.h>
#include <omp.h>

#include "cgal.h"
#include "dualgraph.h"
#include "geometry_utils.h"
#include "visibility.h"
#include "common.h"
#include "debug.h"
#include "chgraph.h"

using namespace std;
namespace fs = boost::filesystem;

vi CHGraph::get_neighbours_by_condition(int source, function<bool(int)> condition) {
    vector<bool> vis (dualgraph.get_mx_node_id() + 1);
    assert(dualgraph.does_node_exist(source));
    queue<int> q; q.push(source);
    vector<int> res;
    while(!q.empty()) {
        int node = q.front(); q.pop();
        if(vis[node]) continue;
        vis[node] = true;
        if(!condition(node)) continue;
        if(node != source) res.pb(node);
        for(auto& e : dualgraph.adj[node]) q.push(e);
    }
    return res;
}

long long CHGraph::get_hash() {
    long long res = 0;
    long long M = 1000000007ll;
    long long A = 348734871ll;
    long long B = 748738483ll;
    vector<long long> data = {(long long)polygon.outer_boundary().size(), (long long)polygon.number_of_holes()};
    {
        auto points = get_points(polygon);
        foe(p, points) {
            data.pb(CGAL::to_double(p.x()));
            data.pb(CGAL::to_double(p.y()));
        }
    }
    data.pb(dualgraph.size());
    for(auto& pol : dualgraph.get_polygons()) {
        auto points = get_points(pol);
        foe(p, points) {
            data.pb(CGAL::to_double(p.x()));
            data.pb(CGAL::to_double(p.y()));
        }
    }
    for(auto& v : data) {
        res += A * v + B;
        res %= M;
    }
    return res;
}

string CHGraph::get_cache_filename() {
    long long hash = get_hash();
    string filename = "ch_graph===" + to_string(hash) + ".txt";
    return filename;
}
    
fs::path CHGraph::get_full_cache_filename() {
    fs::path dir (CACHE_DIR);
    fs::create_directories(dir);
    fs::path file (get_cache_filename());
    fs::path full_path = dir / file;
    return full_path;
}

void CHGraph::write_to_cache() {
    string filename = get_full_cache_filename().string();
    assert(dualgraph.size() == adj.size());
    int n = adj.size();
    set<pair<int,int>> edges;
    foe(e, adj) foe(nb, e.se) edges.insert({e.fi, nb}); // will be min(a,b), max(a,b)
    int m = edges.size();
    // cout << "Writing to file: " << filename << endl;
    std::ofstream file(filename);
    file << n << " " << m << "\n";
    foe(edge, edges) {
        file << edge.fi << " " << edge.se << "\n";
    }
    file.close();
}

bool CHGraph::read_from_cache() {
    if(!fs::exists(get_full_cache_filename())) return false;
    string filename = get_full_cache_filename().string();
    // cout << "Reading from file: " << filename << endl;
    std::ifstream file(filename);
    int n; long long m; file >> n >> m;
    assert(n == dualgraph.size());
    for(auto& node : dualgraph.get_nodes()) adj[node] = {};
    for(int _ = 0; _ < m; _++) {
        int a,b; file >> a >> b;
        adj[a].insert(b);
        adj[b].insert(a);
    }
    file.close();
    return true;
}

// order is not taken into account when computing hash unfortunately.
// Thus order of triangulation must be the same every time.
CHGraph::CHGraph(Polygon_with_holes _polygon, vector<Polygon> _triangulation, string cache_directory) :
    polygon(_polygon),
    triangulation(_triangulation),
    pred(IntersectionPredicates(polygon)),
    visds(VisibilityDataStructure(polygon)),
    dualgraph(DualGraph(_triangulation))
{
    CACHE_DIR = CACHE_DIR_START + "/" + cache_directory;

    for(auto node : dualgraph.get_nodes()) {
        auto pol = dualgraph.get_polygon(node);
        Point centroid = get_centroid(pol);
        centroids[node] = centroid;
    }
    foe(node, dualgraph.get_nodes()) {
        adj[node] = {};
    }
}

CHGraph::CHGraph(){}

void CHGraph::add_all_edges_parallel() {
    // cout << "BEGINNING INITIALIZATION OF CH GRAPH" << endl;
    // cout << "SETTING UP" << endl;
    // cout << "Number of triangles: " << dualgraph.size() << endl;

    for(auto& node : dualgraph.get_nodes()) adj[node] = {};

    auto delete_region_pred = [&](IntersectionPredicates* pred) {
        pred->delete_datastructures();
        delete pred;
    };
    auto get_region_pred = [&](Point p) -> IntersectionPredicates* {
        Polygon vispol = visds.query(p);
        auto pred = new IntersectionPredicates(vispol);
        pred->setup_star_shaped(p);
        return pred;
    };

    set<pair<int,int>> potential_edges;
    map<int,vi> potential_adj;

    // cout << "FINDING POTENTIAL NEIGHBOURS" << endl;
    int cnt = 0;
#pragma omp parallel for
    for(auto& node : dualgraph.get_nodes()) {
        // cout << "Finding potential neighbours of " << node << " (last node = " << dualgraph.size() - 1 << ")" << endl;
        Polygon pol = dualgraph.get_polygon(node);
        auto vertices = pol.vertices();
        auto pred0 = get_region_pred(vertices[0]);
        auto pred1 = get_region_pred(vertices[1]);
        auto pred2 = get_region_pred(vertices[2]);
        vi nbs = get_neighbours_by_condition(node, [&](int nb) {
            auto pol = dualgraph.get_polygon(nb);
            return !pred0->is_completely_outside_star_shaped(pol) &&
                   !pred1->is_completely_outside_star_shaped(pol) &&
                   !pred2->is_completely_outside_star_shaped(pol);
        });
        delete_region_pred(pred0);
        delete_region_pred(pred1);
        delete_region_pred(pred2);
        // cout << "Number of neighbours to consider: " << nbs.size() << endl;
        for(auto& nb : nbs) {
            assert(nb != node);
#pragma omp critical
            {
                potential_edges.insert({node, nb});
            }
        }
        potential_adj[node] = nbs;
#pragma omp critical
        {
            cnt++;
        }
        // cout << "Number of finished nodes (part 0): " << cnt << " out of " << dualgraph.size() << endl;
    }

    cnt = 0;
    // cout << "FILTERING POTENTIAL NEIGHBOURS PART 1" << endl;
#pragma omp parallel for
    for(auto& node : dualgraph.get_nodes()) {
        // cout << "Filtering potential neighbours (part 1) of " << node << " (last node = " << dualgraph.size() - 1 << ")" << endl;
        // cout << "Number of neighbours to consider: " << potential_adj[node].size() << endl;
        Polygon pol = dualgraph.get_polygon(node);
        auto vertices = pol.vertices();
        auto pred0 = get_region_pred(vertices[0]);
        auto pred1 = get_region_pred(vertices[1]);
        auto pred2 = get_region_pred(vertices[2]);
        for(int& nb : potential_adj[node]) {
            auto nb_pol = dualgraph.get_polygon(nb);
            if(
                !(
                    potential_edges.count({nb, node}) &&
                    pred0->is_completely_inside_star_shaped_for_convex_pol(nb_pol) && // no these first as then we wont create the below polygons
                    pred1->is_completely_inside_star_shaped_for_convex_pol(nb_pol) &&
                    pred2->is_completely_inside_star_shaped_for_convex_pol(nb_pol)
                )
            ) {
#pragma omp critical
                {
                    potential_edges.erase({node, nb});
                }
            }
        }
        delete_region_pred(pred0);
        delete_region_pred(pred1);
        delete_region_pred(pred2);
        {
            cnt++;
        }
        // cout << "Number of finished nodes (part 1): " << cnt << " out of " << dualgraph.size() << endl;
    }

    // cout << "FILTERING POTENTIAL NEIGHBOURS PART 2" << endl;
    for(auto& node : dualgraph.get_nodes()) {
        // cout << "Filtering potential neighbours (part 2) of " << node << " (last node = " << dualgraph.size() - 1 << ")" << endl;
        // cout << "Number of neighbours to consider: " << potential_adj[node].size() << endl;
        Polygon pol = dualgraph.get_polygon(node);
        auto vertices = pol.vertices();
        for(int& nb : potential_adj[node]) {
            if(potential_edges.count({node, nb}) && potential_edges.count({nb, node})) {
                adj[node].insert(nb);
            }
        }
        // cout << "Final number of neighbours: " << adj[node].size() << endl;
    }

    /*debug("CHECKING");
    for(auto& a : polgraph.get_nodes()) {
        for(auto& b : polgraph.get_nodes()) {
            if(a >= b) continue;
            debug(a,b);
            Polygon_2 ch_edge = get_convex_hull_of_polygons({polgraph.get_polygon(a), polgraph.get_polygon(b)});
            if(input_pred.is_completely_inside_slow(ch_edge)) {
                assert(adj[a].count(b));
                assert(adj[b].count(a));
            } else {
                assert(adj[a].count(b) == 0);
                assert(adj[b].count(a) == 0);
            }
        }
    }*/

    // cout << "FINISIHED INITIALIZING CH GRAPH" << endl;
}

void CHGraph::add_all_edges() {

    long long time_total = 0ll, time_vispols = 0ll, time_candidates = 0ll, time_neighbours = 0ll;
    long long total_number_of_vispol_calculations = 0;
    long long total_number_of_considered_nodes = 0;
    long long total_number_of_neighbour_nodes = 0;
    //time_inexpensive += TO_MICRO(NOW(), start);

    TIME start_total = NOW();

    for(auto& node : dualgraph.get_nodes()) adj[node] = {};
    map<Point,IntersectionPredicates*> region_preds;
    set<pair<long long,Point>> region_queue;
    map<Point,long long> region_times;
    long long region_time = 0;
    auto delete_region_pred = [&](Point p) {
        if(region_preds.count(p) == 0) return;
        region_preds[p]->delete_datastructures();
        delete region_preds[p];
        region_preds.erase(p);
    };
    auto get_region_pred = [&](Point p) -> IntersectionPredicates* {
        while(region_preds.size() > 100) {
            auto p = *region_queue.begin();
            while(p.fi != region_times[p.se]) {
                delete_region_pred(p.se);
                p = *region_queue.begin();
            }
            region_queue.erase(p);
            delete_region_pred(p.se);
        }
        if(region_preds.count(p) == 0) {
            total_number_of_vispol_calculations++;
            TIME start_vispol = NOW();
            Polygon vispol = visds.query(p);
            region_preds[p] = new IntersectionPredicates(vispol);
            region_preds[p]->setup_star_shaped(p);
            region_times[p] = ++region_time;
            region_queue.insert({region_time, p});
            time_vispols += TO_MICRO(NOW(), start_vispol);
        }
        return region_preds[p];
    };
    TIME start_candidates = NOW();
    set<pair<int,int>> potential_edges;
    map<int,vi> potential_adj;
    for(auto& node : dualgraph.get_nodes()) {
        Polygon pol = dualgraph.get_polygon(node);
        auto vertices = pol.vertices();
        vi nbs = get_neighbours_by_condition(node, [&](int nb) {
            auto pol = dualgraph.get_polygon(nb);
            return !get_region_pred(vertices[0])->is_completely_outside_star_shaped(pol) &&
                   !get_region_pred(vertices[1])->is_completely_outside_star_shaped(pol) &&
                   !get_region_pred(vertices[2])->is_completely_outside_star_shaped(pol);
        });
        total_number_of_considered_nodes += sz(nbs);
        for(auto& nb : nbs) {
            assert(nb != node);
            potential_edges.insert({node, nb});
        }
        potential_adj[node] = nbs;
    }
    time_candidates += TO_MICRO(NOW(), start_candidates);

    TIME start_neighbours = NOW();
    for(auto& node : dualgraph.get_nodes()) {
        Polygon pol = dualgraph.get_polygon(node);
        auto vertices = pol.vertices();
        for(int& nb : potential_adj[node]) {
            auto nb_pol = dualgraph.get_polygon(nb);
            if(
                !(
                    potential_edges.count({nb, node}) &&
                    get_region_pred(vertices[0])->is_completely_inside_star_shaped_for_convex_pol(nb_pol) && // no these first as then we wont create the below polygons
                    get_region_pred(vertices[1])->is_completely_inside_star_shaped_for_convex_pol(nb_pol) &&
                    get_region_pred(vertices[2])->is_completely_inside_star_shaped_for_convex_pol(nb_pol)
                )
            ) {
                potential_edges.erase({node, nb});
            }
        }
    }

    foe(p, region_preds) delete p.se;

    for(auto& node : dualgraph.get_nodes()) {
        Polygon pol = dualgraph.get_polygon(node);
        auto vertices = pol.vertices();
        for(int& nb : potential_adj[node]) {
            if(potential_edges.count({node, nb}) && potential_edges.count({nb, node})) {
                adj[node].insert(nb);
                total_number_of_neighbour_nodes++;
            }
        }
    }
    time_neighbours += TO_MICRO(NOW(), start_neighbours);

    time_total += TO_MICRO(NOW(), start_total);

    cout << "\n\n\nStats for all edges computation" << endl;
    cout << "  Number of vispol constructions: " << total_number_of_vispol_calculations << endl;
    cout << "  Number of candidate nodes: " << total_number_of_considered_nodes << endl;
    cout << "  Number of neighbour nodes: " << total_number_of_neighbour_nodes << endl;
    cout << "  Time total: " << COUT_TIME(MICRO_TO_MS(time_total)) << endl;
    cout << "  Time vispols: " << COUT_TIME(MICRO_TO_MS(time_vispols)) << endl;
    cout << "  Time candidates: " << COUT_TIME(MICRO_TO_MS(time_candidates)) << endl;
    cout << "  Time neighbours: " << COUT_TIME(MICRO_TO_MS(time_neighbours)) << endl;
    cout << "\n\n\n";
}

void CHGraph::add_edges_from_mapping(std::map<int,int>& mapper, CHGraph& other) {
    // cout << "ADDING MAPPER EDGES" << endl;
    for(auto& node : dualgraph.get_nodes()) adj[node] = {};
    map<int,vector<int>> rev_mapper;
    foe(e, mapper) rev_mapper[e.se].push_back(e.fi);
    for(auto& node : dualgraph.get_nodes()) {
        // cout << "Finding neighbours of " << node << " (last node = " << dualgraph.size() - 1 << ")" << endl;
        vector<int> nodes2check = {mapper[node]};
        for(auto& other_node : other.adj[mapper[node]]) nodes2check.push_back(other_node);
        for(auto& other_node : nodes2check) {
            for(auto& nb : rev_mapper[other_node]) {
                if(nb == node) continue;
                adj[node].insert(nb); // TODO slow!!!
            }
        }
    }
}

Polygon CHGraph::get_convex_hull_of_nodes(std::vector<int> nodes) {
    vector<Polygon> pols;
    for(int& node : nodes) {
        pols.push_back(dualgraph.get_polygon(node));
    }
    return get_convex_hull_of_polygons(pols);
}

void CHGraph::remove_edge(int a, int b) {
    if(adj[a].count(b)) {
        adj[a].erase(b);
    }
    if(adj[b].count(a)) {
        adj[b].erase(a);
    }
}

vector<Polygon> CHGraph::get_convex_hull_of_decomposition(vector<vector<int>> decomposition) {
    vector<Polygon> res;
    for(auto& nodes : decomposition) {
        res.pb(get_convex_hull_of_nodes(nodes));
    }
    return res;
} 

void CHGraph::delete_datastructures() {
    pred.delete_datastructures();
    visds.delete_datastructures();
}

FT CHGraph::get_dist(int a, int b) {
    return CGAL::squared_distance(centroids[a], centroids[b]);
}

long long time_total = 0ll, time_expensive = 0ll, time_inexpensive = 0ll, time_needed_sides = 0ll;
long long number_of_expensive_calls = 0ll, number_of_inexpensive_calls = 0ll, cnt_inaccurate_matches = 0ll;

set<int> CHGraph::bfs_dual_graph(int source, function<bool(int)> is_completely_visible_accurate, bool is_symmetric) {

    // cout << "Finding neighbours of " << source << " using BFS in dual graph (last node = " << dualgraph.size() - 1 << ")" << endl;

    TIME start_total = NOW();

    set<int> visible; // nodes visible from source
    set<int> visited; // visibility component
    set<int> boundary; // boundary to the visibility component
    set<int> invisible; // nodes invisible from source
    queue<int> q;

    // gets sides of node that should be next to visible polygons
    map<int,set<pair<Point,Point>>> cache_needed_sides;
    auto setup_needed_sides = [&](int node) {
        if(cache_needed_sides.count(node)) return;
        TIME start = NOW();
        auto ch = get_convex_hull_of_polygons(dualgraph.node2pol[source], dualgraph.node2pol[node]);
        auto& pol = dualgraph.node2pol[node];
        set<pair<Point,Point>> needed_sides;
        vector<Segment> ch_sides; for(auto e : ch.edges()) ch_sides.push_back(e);
        for(auto side : pol.edges()) {
            /*if(!is_segment_on_a_side(ch_sides, side)) {
                needed_sides.insert({side.min(), side.max()});
            }*/
            bool ok = true;
            foe(ch_side, ch_sides) {
                if(side == ch_side) {
                    ok = false;
                    break;
                }
            }
            if(!ok) continue;
            if(is_segment_on_a_side(ch_sides, side)) {
                ok = false;
            }
            if(!ok) continue;
            needed_sides.insert({side.min(), side.max()});
        }
        assert(needed_sides.size() > 0);
        cache_needed_sides[node] = needed_sides;
        time_needed_sides += TO_MICRO(NOW(), start);
    };
    
    // if true then completely visible. if false then inconclusive
    auto is_completely_visible_inaccurate = [&](int node) {
        TIME start = NOW();
        number_of_inexpensive_calls++;
        setup_needed_sides(node);
        auto needed_sides = cache_needed_sides[node];
        foe(nb, dualgraph.adj[node]) {
            if(!visible.count(nb)) continue;
            for(auto side : dualgraph.node2pol[nb].edges()) {
                pair<Point,Point> key = {side.min(), side.max()};
                if(needed_sides.count(key)) {
                    needed_sides.erase(key);
                    break;
                }
            }
        }
        time_inexpensive += TO_MICRO(NOW(), start);
        return needed_sides.size() == 0;
    };

    // if true then not completely visible. if false then inconclusive
    auto is_invisible_inaccurate = [&](int node) {
        TIME start = NOW();
        number_of_inexpensive_calls++;
        setup_needed_sides(node);
        auto& needed_sides = cache_needed_sides[node];
        foe(side, needed_sides) {
            if(segments_on_polygon_edge.count(side)) {
                time_inexpensive += TO_MICRO(NOW(), start);
                return true;
            }
        }
        time_inexpensive += TO_MICRO(NOW(), start);
        return false;
    };
    
    boundary.insert(source);
    visible.insert(source);
    q.push(source);
    while(q.size()) {
        // here we try to find visible nodes
        while(!q.empty()) {
            int node = q.front(); q.pop();
            if(visited.count(node)) continue;
            boundary.erase(node);
            visited.insert(node);
            foe(nb, dualgraph.adj[node]) {
                if(visible.count(nb) || invisible.count(nb)) continue; // already know everything about nb
                if(invisible_cache[nb].count(source)) {
                    invisible.insert(nb);
                    continue;
                }
                if(adj[source].count(nb) || (is_symmetric && adj[nb].count(source))) {
                    boundary.insert(nb);
                    visible.insert(nb);
                    q.push(nb);
                    continue;
                }
                if(is_completely_visible_inaccurate(nb)) {
                    boundary.insert(nb);
                    visible.insert(nb);
                    q.push(nb);
                    continue;
                }
                if(is_invisible_inaccurate(nb)) {
                    cnt_inaccurate_matches++;
                    invisible.insert(nb);
                    continue;
                }
                boundary.insert(nb);
            }
        }
        // here we do one expensive check and continue above afterwards
        vector<int> to_erase_from_boundary;
        foe(node, boundary) {
            /*assert(visible.count(node) == 0);
            assert(is_completely_visible_inaccurate(node) == false);
            assert(invisible.count(node) == 0);
            assert(!(adj[source].count(node) || (is_symmetric && adj[node].count(source))));*/
            number_of_expensive_calls++;
            TIME start = NOW();
            if(is_completely_visible_accurate(node)) {
                time_expensive += TO_MICRO(NOW(), start);
                visible.insert(node);
                q.push(node);
                break;
            } else {
                time_expensive += TO_MICRO(NOW(), start);
                to_erase_from_boundary.push_back(node);
                invisible.insert(node);
            }
        }
        foe(node, to_erase_from_boundary) boundary.erase(node);
    }

    invisible_cache[source] = invisible;

    visible.erase(source);

    time_total += TO_MICRO(NOW(), start_total);

    // cout << "Number of expensive checks: " << expensive_checks << endl;
    // cout << "Number of invisible: " << invisible.size() << endl;
    // cout << "Number of visible: " << visible.size() << endl;
    // cout << "Number of expensive calls: " << number_of_expensive_calls << endl;
    // cout << "Number of inexpensive calls: " << number_of_inexpensive_calls << endl;
    // cout << "Number of inaccurate matches: " << cnt_inaccurate_matches << endl;
    // cout << "Time total: " << MICRO_TO_MS(time_total) << endl;
    // cout << "Time expensive: " << MICRO_TO_MS(time_expensive) << endl;
    // cout << "Time inexpensive: " << MICRO_TO_MS(time_inexpensive) << endl;
    // cout << "Time needed_sides: " << MICRO_TO_MS(time_needed_sides) << endl;
    assert(visited.size() == visible.size() + 1);
    return visible;
}

set<int> CHGraph::get_fully_visible_bfs_edges_inexact_single_node(int source) {
    auto source_pol = dualgraph.get_polygon(source);
    /*auto centroid = get_centroid(source_pol);
    auto vispol_centroid = visds.query(centroid);
    IntersectionPredicates pred_centroid (vispol_centroid);
    pred_centroid.setup_star_shaped(centroid);*/
    auto vispol0 = visds.query(source_pol[0]);
    auto vispol1 = visds.query(source_pol[1]);
    auto vispol2 = visds.query(source_pol[2]);
    IntersectionPredicates pred0 (vispol0);
    pred0.setup_star_shaped(source_pol[0]);
    IntersectionPredicates pred1 (vispol1);
    pred1.setup_star_shaped(source_pol[1]);
    IntersectionPredicates pred2 (vispol2);
    pred2.setup_star_shaped(source_pol[2]);
    auto is_completely_visible_accurate = [&](int node) {
        auto pol = dualgraph.get_polygon(node);
        bool res = //pred_centroid.is_completely_inside_star_shaped_for_convex_pol(pol) &&
                   pred0.is_completely_inside_star_shaped_for_convex_pol(pol) &&
                   pred1.is_completely_inside_star_shaped_for_convex_pol(pol) &&
                   pred2.is_completely_inside_star_shaped_for_convex_pol(pol);
        return res;
    };
    auto res = bfs_dual_graph(source, is_completely_visible_accurate, false);
    pred0.delete_datastructures();
    pred1.delete_datastructures();
    pred2.delete_datastructures();
    return res;
}

void CHGraph::add_fully_visible_bfs_edges_inexact() {
    vi order = dualgraph.get_dfs_order();
    map<int,set<int>> potential_nbs;
    int cnt = 0;
    foe(node, order) {
        cnt++;
        // cout << "Computing the " << cnt << "th" << " adj out of " << order.size() << endl; 
        // cout << "processing node " << node << endl;
        potential_nbs[node] = get_fully_visible_bfs_edges_inexact_single_node(node);
    }
    foe(node, order) {
        // cout << "postprocessing node " << node << endl;
        foe(nb, potential_nbs[node]) {
            if(potential_nbs[nb].count(node)) {
                adj[node].insert(nb);
            }
        }
    }
    foe(e, adj) {
        // cout << "Final number of neighbours of " << e.fi << " is " << e.se.size() << endl;
    }
}

int caught = 0;
set<int> CHGraph::get_fully_visible_bfs_edges_exact_single_node(int source) {
    /*auto source_pol = get_polygon(source);
    auto vispol = visds.query(get_centroid(source_pol));
    IntersectionPredicates pred_source (vispol);
    pred_source.setup_star_shaped(get_centroid(source_pol));
    auto vispol0 = visds.query(source_pol[0]);
    auto vispol1 = visds.query(source_pol[1]);
    auto vispol2 = visds.query(source_pol[2]);
    IntersectionPredicates pred0 (vispol0);
    pred0.setup_star_shaped(source_pol[0]);
    IntersectionPredicates pred1 (vispol1);
    pred1.setup_star_shaped(source_pol[1]);
    IntersectionPredicates pred2 (vispol2);
    pred2.setup_star_shaped(source_pol[2]);*/
    auto is_completely_visible_accurate = [&](int node) {
        /*auto pol = get_polygon(node);
        auto cent = get_centroid(pol);
        if(
            !pred_source.is_point_inside_star_shaped_polygon(cent) ||
            !pred0.is_point_inside_star_shaped_polygon(cent) ||
            !pred1.is_point_inside_star_shaped_polygon(cent) ||
            !pred2.is_point_inside_star_shaped_polygon(cent)
        ) {
            caught++;
            return false;
        }*/
        auto ch = get_convex_hull_of_polygons(dualgraph.node2pol[source], dualgraph.node2pol[node]);
        return pred.is_completely_inside_for_convex_pol(ch);
    };
    /*pred_source.delete_datastructures();
    pred0.delete_datastructures();
    pred1.delete_datastructures();
    pred2.delete_datastructures();
    debug(caught);*/
    return bfs_dual_graph(source, is_completely_visible_accurate, true);
}

int time_for_all = 0;

void CHGraph::add_fully_visible_bfs_edges_exact() {
    time_total = 0, time_expensive = 0, time_inexpensive = 0, time_needed_sides = 0;
    number_of_expensive_calls = 0, number_of_inexpensive_calls = 0, cnt_inaccurate_matches = 0;
    // cout << "CALCULATING TRIANGULATION SIDES ON INPUT-POLYGON BOUNDARY";
    /*foe(side, get_sides(polygon)) {
        Segment_2 seg (side.fi, side.se);
        assert(pred.has_intersecting_segment_on_seg(seg));
    }*/
    foe(triangle, triangulation) {
        foe(side, get_sides(triangle)) {
            Segment seg (side.fi, side.se);
            if(pred.has_intersecting_segment_on_seg(seg)) {
                segments_on_polygon_edge.insert(side);
            }
        }
    }
    vi order = dualgraph.get_dfs_order();
    int cnt = 0;
    foe(source, order) {
        cnt++;
        // cout << "Computing the " << cnt << "th" << " adj out of " << order.size() << endl; 
        TIME start = NOW();
        foe(node, get_fully_visible_bfs_edges_exact_single_node(source)) {
            adj[source].insert(node);
        }
        time_for_all += TO_MICRO(NOW(), start);
    }
    /*foe(e, adj) {
        // cout << "Final number of neighbours of " << e.fi << " is " << e.se.size() << endl;
    }*/
    // cout << "Time total: " << MICRO_TO_MS(time_for_all) << endl;

    cout << "\n\n\nStats for BFS exact graph computation" << endl;
    cout << "  Number of expensive calls: " << number_of_expensive_calls << endl;
    cout << "  Number of inexpensive calls: " << number_of_inexpensive_calls << endl;
    cout << "  Number of inaccurate matches: " << cnt_inaccurate_matches << endl;
    cout << "  Time total: " << COUT_TIME(MICRO_TO_MS(time_total)) << endl;
    cout << "  Time expensive: " << COUT_TIME(MICRO_TO_MS(time_expensive)) << endl;
    cout << "  Time inexpensive: " << COUT_TIME(MICRO_TO_MS(time_inexpensive)) << endl;
    cout << "  Time needed_sides: " << COUT_TIME(MICRO_TO_MS(time_needed_sides)) << endl;
    cout << "\n\n\n";
}

ll CHGraph::m() {
    ll ans = 0;
    foe(node, dualgraph.get_nodes()) {
        assert(!adj[node].count(node));
        ans += (ll)adj[node].size();
    }
    return ans;
}

void CHGraph::add_edges_to_make_undirected() {
    foe(node, dualgraph.get_nodes()) {
        foe(nb, adj[node]) {
            adj[nb].insert(node);
        }
    }
}
