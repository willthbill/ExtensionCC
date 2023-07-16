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

vi VisibilityGraph::get_neighbours_by_condition(int source, function<bool(int)> condition) {
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

long long VisibilityGraph::get_hash() {
    long long res = 0ll;
    long long M = 1000000007ll;
    long long A = 348734871ll;
    long long B = 748738483ll;
    vector<long long> data = {
        (long long)polygon.outer_boundary().size(),
        (long long)polygon.number_of_holes()
    };
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
    for(int i = 0; i < sz(data); i++) {
        auto& v = data[i];
        res += A % M * v % M * i % M + B;
        res %= M;
    }
    return res;
}

string VisibilityGraph::get_cache_filename() {
    long long hash = get_hash();
    string filename = "ch_graph===" + to_string(hash) + ".txt";
    return filename;
}
    
fs::path VisibilityGraph::get_full_cache_filename() {
    fs::path dir (CACHE_DIR);
    fs::create_directories(dir);
    fs::path file (get_cache_filename());
    fs::path full_path = dir / file;
    return full_path;
}

void VisibilityGraph::write_to_cache() {
    string filename = get_full_cache_filename().string();
    assert(dualgraph.size() == adj.size());
    int n = adj.size();
    set<pair<int,int>> edges;
    foe(e, adj) foe(nb, e.se) edges.insert({e.fi, nb});
    int m = edges.size();
    cout << "--- Writing to file: " << filename << endl;
    std::ofstream file(filename);
    file << n << " " << m << "\n";
    foe(edge, edges) {
        file << edge.fi << " " << edge.se << "\n";
    }
    file.close();
}

bool VisibilityGraph::read_from_cache() {
    if(!fs::exists(get_full_cache_filename())) return false;
    string filename = get_full_cache_filename().string();
    cout << "--- Reading from file: " << filename << endl;
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

VisibilityGraph::VisibilityGraph(Polygon_with_holes _polygon, vector<Polygon> _triangulation, string cache_directory) :
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

VisibilityGraph::VisibilityGraph(){}

void VisibilityGraph::add_all_edges_parallel() {

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

    int cnt = 0;
#pragma omp parallel for
    for(auto& node : dualgraph.get_nodes()) {
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
        if(cnt % 1000 == 1) cout << "--- Building visibility graph part 1 [progress = " << cnt << "/" << sz(dualgraph) << "]" << endl;
    }

    cnt = 0;
#pragma omp parallel for
    for(auto& node : dualgraph.get_nodes()) {
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
        if(cnt % 1000 == 1) cout << "--- Building visibility graph part 2 [progress = " << cnt << "/" << sz(dualgraph) << "]" << endl;
    }

    for(auto& node : dualgraph.get_nodes()) {
        Polygon pol = dualgraph.get_polygon(node);
        auto vertices = pol.vertices();
        for(int& nb : potential_adj[node]) {
            if(potential_edges.count({node, nb}) && potential_edges.count({nb, node})) {
                adj[node].insert(nb);
            }
        }
    }

    /*debug("CHECKING");
    for(auto& a : polgraph.get_nodes()) {
        for(auto& b : polgraph.get_nodes()) {
            if(a >= b) continue;
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
}

void VisibilityGraph::add_all_edges() {
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
            Polygon vispol = visds.query(p);
            region_preds[p] = new IntersectionPredicates(vispol);
            region_preds[p]->setup_star_shaped(p);
            region_times[p] = ++region_time;
            region_queue.insert({region_time, p});
        }
        return region_preds[p];
    };
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
        for(auto& nb : nbs) {
            assert(nb != node);
            potential_edges.insert({node, nb});
        }
        potential_adj[node] = nbs;
    }
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
            }
        }
    }
}

void VisibilityGraph::add_edges_from_mapping(std::map<int,int>& mapper, VisibilityGraph& other) {
    for(auto& node : dualgraph.get_nodes()) adj[node] = {};
    map<int,vector<int>> rev_mapper;
    foe(e, mapper) rev_mapper[e.se].push_back(e.fi);
    for(auto& node : dualgraph.get_nodes()) {
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

Polygon VisibilityGraph::get_convex_hull_of_nodes(std::vector<int> nodes) {
    vector<Polygon> pols;
    for(int& node : nodes) {
        pols.push_back(dualgraph.get_polygon(node));
    }
    return get_convex_hull_of_polygons(pols);
}

void VisibilityGraph::remove_edge(int a, int b) {
    if(adj[a].count(b)) {
        adj[a].erase(b);
    }
    if(adj[b].count(a)) {
        adj[b].erase(a);
    }
}

vector<Polygon> VisibilityGraph::get_convex_hulls_of_decomposition(vector<vector<int>> decomposition) {
    vector<Polygon> res;
    for(auto& nodes : decomposition) {
        res.pb(get_convex_hull_of_nodes(nodes));
    }
    return res;
} 

void VisibilityGraph::delete_datastructures() {
    pred.delete_datastructures();
    visds.delete_datastructures();
}

FT VisibilityGraph::get_dist(int a, int b) {
    return CGAL::squared_distance(centroids[a], centroids[b]);
}

set<int> VisibilityGraph::bfs_dual_graph(int source, function<bool(int)> is_completely_visible_accurate, bool is_symmetric) {
    set<int> visible; // nodes visible from source
    set<int> visited; // visibility component
    set<int> boundary; // boundary to the visibility component
    set<int> invisible; // nodes invisible from source
    queue<int> q;

    // gets sides of node that should be next to visible polygons
    map<int,set<pair<Point,Point>>> cache_needed_sides;
    auto setup_needed_sides = [&](int node) {
        if(cache_needed_sides.count(node)) return;
        auto ch = get_convex_hull_of_polygons(dualgraph.node2pol[source], dualgraph.node2pol[node]);
        auto& pol = dualgraph.node2pol[node];
        set<pair<Point,Point>> needed_sides;
        vector<Segment> ch_sides; for(auto e : ch.edges()) ch_sides.push_back(e);
        for(auto side : pol.edges()) {
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
    };
    
    // if true then completely visible. if false then inconclusive
    auto is_completely_visible_inaccurate = [&](int node) {
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
        return needed_sides.size() == 0;
    };

    // if true then not completely visible. if false then inconclusive
    auto is_invisible_inaccurate = [&](int node) {
        setup_needed_sides(node);
        auto& needed_sides = cache_needed_sides[node];
        foe(side, needed_sides) {
            if(segments_on_polygon_edge.count(side)) {
                return true;
            }
        }
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
                    invisible.insert(nb);
                    continue;
                }
                boundary.insert(nb);
            }
        }
        // here we do one expensive check and continue above afterwards
        vector<int> to_erase_from_boundary;
        foe(node, boundary) {
            if(is_completely_visible_accurate(node)) {
                visible.insert(node);
                q.push(node);
                break;
            } else {
                to_erase_from_boundary.push_back(node);
                invisible.insert(node);
            }
        }
        foe(node, to_erase_from_boundary) boundary.erase(node);
    }

    invisible_cache[source] = invisible;
    visible.erase(source);
    assert(visited.size() == visible.size() + 1);
    return visible;
}

set<int> VisibilityGraph::get_fully_visible_bfs_edges_inexact_single_node(int source) {
    auto source_pol = dualgraph.get_polygon(source);
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

void VisibilityGraph::add_fully_visible_bfs_edges_inexact() {
    vi order = dualgraph.get_dfs_order();
    map<int,set<int>> potential_nbs;
    int cnt = 0;
    foe(node, order) {
        potential_nbs[node] = get_fully_visible_bfs_edges_inexact_single_node(node);
        cnt++;
        if(cnt % 1000 == 1) cout << "Computing partial visibility graph [progress = ] " << cnt << "/" << sz(order) << "]" << endl;
    }
    foe(node, order) {
        foe(nb, potential_nbs[node]) {
            if(potential_nbs[nb].count(node)) {
                adj[node].insert(nb);
            }
        }
    }
}

int caught = 0;
set<int> VisibilityGraph::get_fully_visible_bfs_edges_exact_single_node(int source) {
    auto is_completely_visible_accurate = [&](int node) {
        auto ch = get_convex_hull_of_polygons(dualgraph.node2pol[source], dualgraph.node2pol[node]);
        return pred.is_completely_inside_for_convex_pol(ch);
    };
    return bfs_dual_graph(source, is_completely_visible_accurate, true);
}

int time_for_all = 0;

void VisibilityGraph::add_fully_visible_bfs_edges_exact() {
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

ll VisibilityGraph::m() {
    ll ans = 0;
    foe(node, dualgraph.get_nodes()) {
        assert(!adj[node].count(node));
        ans += (ll)adj[node].size();
    }
    return ans;
}

void VisibilityGraph::add_edges_to_make_undirected() {
    foe(node, dualgraph.get_nodes()) {
        foe(nb, adj[node]) {
            adj[nb].insert(node);
        }
    }
}
