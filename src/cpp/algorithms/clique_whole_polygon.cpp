#include <bits/stdc++.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <CGAL/ch_akl_toussaint.h>
#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/Range_segment_tree_traits.h>
#include <CGAL/Range_tree_k.h>
#include <CGAL/Range_segment_tree_traits.h>
#include <CGAL/Segment_tree_k.h>

#include "../triangulation/triangulation.h"
#include "../dualgraph.h"
#include "../com.h"
#include "../clique.h"
#include "../debug.h"
#include "../common.h"

namespace py = pybind11;
using namespace std;

/*Polygon get_convex_hull_of_polygons(vector<Polygon> polygons) {
    vector<Point> pset;
    for(auto& pol : polygons) {
        for(Point& p : pol) pset.push_back(p);
    }
    Polygon res;
    CGAL::ch_akl_toussaint(pset.begin(), pset.end(), back_inserter(res));
    return res;
}

template<typename T>
Polygon_set to_polygon_set(T& pol) {
    Polygon_set res;
    res.insert(pol);
    return res;
}

vector<Polygon_with_holes> to_polygon_vector(Polygon_set& pset) {
    vector<Polygon_with_holes> pols;
    pset.polygons_with_holes (back_inserter(pols));
    return pols;
}

Polygon_set get_intersection(Polygon_set p1, Polygon_set p2) {
    Polygon_set intersection; intersection.intersection(p1,p2);
    return intersection;
}

bool is_inside_polygon_set(Polygon_set p1, Polygon_set& p2) {
    p1.difference(p2);
    return p1.is_empty();
}

bool is_equal(Polygon_set p1, Polygon_set& p2) {
    return is_inside_polygon_set(p1, p2) && is_inside_polygon_set(p2, p1);
}

template<typename A,typename B>
bool is_equal(A& p1, B& p2) {
    auto pset1 = to_polygon_set(p1);
    auto pset2 = to_polygon_set(p2);
    return is_equal(pset1, pset2);
}

typedef CGAL::Range_segment_tree_set_traits_2<K> Traits_range;
typedef CGAL::Range_tree_2<Traits_range> Range_tree_2_type;
typedef Traits_range::Interval Interval_range;
typedef Traits_range::Key Key_range;

typedef CGAL::Segment_tree_map_traits_2<K, pair<Point,Point>> Traits_segment;
typedef CGAL::Segment_tree_2<Traits_segment> Segment_tree_2_type;
typedef Traits_segment::Interval Interval_segment;
typedef Traits_segment::Pure_interval Pure_interval_segment;
typedef Traits_segment::Key Key_segment;

Range_tree_2_type build_range_tree(vector<Point> points) {
    vector<Key_range> input;
    for(auto& data : points) input.pb(Key_range(data));
    Range_tree_2_type range_tree(input.begin(), input.end());
    return range_tree;
}

vector<Point> query_range_tree(Range_tree_2_type& range_tree, Point upperleft, Point bottomright) {
    vector<Point> output; // faster with list?
    Interval_range win(upperleft, bottomright);
    range_tree.window_query(win, back_inserter(output));
    return output;
}

Segment_tree_2_type build_segment_tree(vector<pair<Point,Point>> points) {
    vector<Interval_segment> input;
    for(auto& data : points) input.pb(Interval_segment(
        Pure_interval_segment(
            Point(min(data.fi.x(),data.se.x()) - 1, min(data.fi.y(),data.se.y()) - 1),
            Point(max(data.fi.x(),data.se.x()) + 1, max(data.fi.y(),data.se.y()) + 1)
        ), data
    ));
    Segment_tree_2_type segment_tree(input.begin(), input.end());
    return segment_tree;
}

vector<pair<Point,Point>> query_segment_tree(Segment_tree_2_type& segment_tree, Point upperleft, Point bottomright) {
    vector<Interval_segment> intervals; // faster with list?
    Interval_segment win(Pure_interval_segment(upperleft, bottomright),{Point(0,0),Point(0,0)});
    segment_tree.window_query(win, back_inserter(intervals));
    vector<pair<Point,Point>> output;
    foe(interval, intervals) output.pb(interval.se);
    return output;
}

vector<int> get_closest_nodes(PolygonGraph& graph, int source, int number_of_nodes_to_get) {
    vector<bool> vis (graph.get_mx_node_id() + 1);
    assert(graph.does_node_exist(source));
    queue<int> q; q.push(source);
    vector<int> res;
    while(res.size() < number_of_nodes_to_get && !q.empty()) {
        int node = q.front(); q.pop();
        if(vis[node]) continue;
        vis[node] = true;
        if(node != source) res.pb(node);
        for(auto& e : graph.adj[node]) q.push(e);
    }
    return res;
}

vector<Point> get_points(Polygon_with_holes_2& pol) {
    vector<Point> points;
    for(auto& p : pol.outer_boundary()) points.pb(p);
    for(auto& hole : pol.holes()) {
        for(auto& p : hole) points.pb(p);
    }
    mkunique(points);
    return points;
}

vector<pair<Point,Point>> get_sides(Polygon_with_holes_2& pol) {
    vector<pair<Point,Point>> sides;
    for(auto e : pol.outer_boundary().edges()) {
        pair<Point, Point> key = {e.min(), e.max()};
        sides.pb(key);
    }
    for(auto& hole: pol.holes()) {
        for(auto e : hole.edges()) {
            pair<Point, Point> key = {e.min(), e.max()};
            sides.pb(key);
        }
    }
    mkunique(sides);
    return sides;
}*/

// shortest path in point graph?
// shortest path in triangulation "share edge" graph?
// if a graph was already computed for one triangle, then we can use that computation for computing the neighbouring triangles?
// keep a dynamic window of the area to search for neightbours to build adj
// find polygon input_part by knowing whether the points are on the boundary or in a whole

vector<vector<pair<OUTPUT_TYPE,OUTPUT_TYPE>>> get_clique_decomposition(vector<pair<int,int>> boundary_points, vector<vector<pair<int,int>>> holes_points) {
    assert(false);
    return {};
    /*int time_range = 0, time_segment = 0, time_sides = 0, time_points = 0, time_bbox = 0, time_convexhull = 0, time_bfs = 0;
    int time_triangulation = 0, time_build_range = 0, time_build_segment = 0, time_adj = 0, time_cliques = 0;

    Polygon_with_holes_2 input_polygon = pointsandholes2polygon(boundary_points, holes_points);

    TIME start = NOW();
    cout << "TRIANGULATING AND SETUP" << endl;
    PolygonGraph graph (triangulate_polygon_delaunay_cgal(input_polygon));
    time_triangulation += TO_MICRO(NOW(), start);

    start = NOW();
    cout << "BUILDING RANGE TREE" << endl;
    auto point_ds = build_range_tree(get_points(input_polygon));
    time_build_range += TO_MICRO(NOW(), start);
    
    start = NOW();
    cout << "BUILDING SEGMENT TREE" << endl;
    auto side_ds = build_segment_tree(get_sides(input_polygon));
    time_build_segment += TO_MICRO(NOW(), start);

    auto is_inside_input_polygon_slow = [&](Polygon& pol) -> bool {
        Polygon_set intersection; intersection.intersection(
            to_polygon_set(input_polygon),
            to_polygon_set(pol)
        );
        auto arr = to_polygon_vector(intersection);
        return arr.size() == 1 && is_equal(pol, arr[0]);
    };

    auto is_inside_input_polygon = [&](Polygon& pol) -> bool {
        TIME start = NOW();
        auto bbox = pol.bbox();
        time_bbox += TO_MICRO(NOW(), start);

        start = NOW();
        vector<Point> points = query_range_tree(
            point_ds,
            Point(bbox.xmin() - 1, bbox.ymin() - 1), // TODO: fix this!
            Point(bbox.xmax() + 1, bbox.ymax() + 1)
        );
        time_range += TO_MICRO(NOW(), start);

        start = NOW();
        for(auto& p : points) {
            if(pol.bounded_side(p) == CGAL::ON_BOUNDED_SIDE) { // strictly inside pol
                return false;
            }
        }
        time_points += TO_MICRO(NOW(), start);

        start = NOW();
        vector<pair<Point,Point>> sides = query_segment_tree(
            side_ds,
            Point(bbox.xmin() - 1, bbox.ymin() - 1), // TODO: fix this!
            Point(bbox.xmax() + 1, bbox.ymax() + 1)
        );
        time_segment += TO_MICRO(NOW(), start);

        start = NOW();
        bool res = true;
        for(auto& side : sides) {
            bool is_on_a_line = false;
            set<Point> touches;
            for(auto e : pol.edges()) {
                auto v = intersection(Segment_2(side.fi, side.se), e);
                if (v) {
                    if (const Point *p = boost::get<Point>(&*v)) {
                        // side and e intersects at a point
                        touches.insert(*p);
                    } else {
                        // side and e intersects at a segment, thus side is not inside pol since pol is convex
                        const Segment_2 *s = boost::get<Segment_2>(&*v);
                        is_on_a_line = true;
                        break;
                    }
                }
            }
            if(is_on_a_line) continue;
            if(touches.size() >= 2) {
                res = false;
                break;
            }
        }
        time_sides += TO_MICRO(NOW(), start);

        return res;
    };

    start = NOW();
    cout << "BUILDING ADJ" << endl;
    map<int,set<int>> adj;
    for(auto& node : graph.get_nodes()) adj[node] = {};
    vector<unordered_set<int>> checked (graph.get_mx_node_id() + 1);
    for(auto& node : graph.get_nodes()) {
        TIME start = NOW();
        vi nodes = get_closest_nodes(graph, node, 100);
        time_bfs += TO_MICRO(NOW(), start);
        foe(nb, nodes) {
            if(checked[node].count(nb)) continue;
            TIME start = NOW();
            Polygon ch_edge = get_convex_hull_of_polygons({graph.get_polygon(node), graph.get_polygon(nb)});
            time_convexhull += TO_MICRO(NOW(), start);
            checked[node].insert(nb);
            checked[nb].insert(node);
            // assert(is_inside_input_polygon(ch_edge) == is_inside_input_polygon_slow(ch_edge));
            if(is_inside_input_polygon(ch_edge)) {
                adj[node].insert(nb);
                adj[nb].insert(node); // nb might add node, but this depends. we might as well add nb now.
            }
        }
    }
    time_adj += TO_MICRO(NOW(), start);

    start = NOW();
    // ignore the condition that the convex hull of the cliques should be in the input polygon
    cout << "FINDING CLIQUES" << endl;
    CliqueCover cliquecover (adj);
    vector<vector<int>> cliques = cliquecover.get_cliques_smalladj_naive();
    vector<Polygon> clique_polygons;
    for(auto& clique : cliques) {
        vector<Polygon> clique_triangles;
        for(int& node : clique) {
            clique_triangles.push_back(graph.get_polygon(node));
        }
        clique_polygons.push_back(get_convex_hull_of_polygons(clique_triangles));
    }
    time_cliques += TO_MICRO(NOW(), start);

    cout << "FINISHING" << endl;
    vector<Polygon> polygons;
    for(auto& p : clique_polygons) {
        polygons.push_back(p);
    }
    debug(MICRO_TO_MS(time_bbox), MICRO_TO_MS(time_range), MICRO_TO_MS(time_points), MICRO_TO_MS(time_segment), MICRO_TO_MS(time_sides), MICRO_TO_MS(time_convexhull), MICRO_TO_MS(time_bfs));
    debug(MICRO_TO_MS(time_triangulation), MICRO_TO_MS(time_build_range), MICRO_TO_MS(time_build_segment), MICRO_TO_MS(time_adj), MICRO_TO_MS(time_cliques));
    return cgal2pysolution(polygons);*/
}

PYBIND11_MODULE(clique_whole_polygon, m) {
    m.doc() = "clique_whole_polygon";
    m.def("get_clique_decomposition", &get_clique_decomposition);
}
