#include <bits/stdc++.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "../dualgraph.h"
#include "../triangulation/triangulation.h"
#include "../com.h"
#include "../cgal.h"

#include "../debug.h"
#include "../common.h"

using namespace std;

namespace py = pybind11;

vector<vector<pair<OUTPUT_TYPE,OUTPUT_TYPE>>> simple_merge_convex_partition(vector<pair<int,int>> boundary_points, vector<vector<pair<int,int>>> holes_points) {
    vector<Polygon> triangulation = triangulate_polygon_delaunay_cgal(pointsandholes2polygon(boundary_points, holes_points));
    DualGraph graph (triangulation);
    vector<int> nodes = graph.get_nodes();
    deque<pair<int,int>> merges;
    for(auto& node : nodes) {
        for(auto& nb : graph.adj[node]) {
            if(graph.check_union_all(node, nb)) {
                merges.emplace_back(node, nb);
            }
        }
    }
    int cnt = 0;
    while(merges.size()) {
        int a = merges.front().fi;
        int b = merges.front().se;
        merges.pop_front();
        if(!(graph.does_node_exist(a) && graph.does_node_exist(b))) {
            continue;
        }
        cnt++;
        int node = graph.merge_nodes(a,b);
        for(auto& nb : graph.adj[node]) {
            if(graph.check_union_all(node, nb)) {
                merges.emplace_back(node, nb);
            }
        }
    }
    auto polygons = graph.get_polygons();
    return cgal2pysolution(polygons);
}

PYBIND11_MODULE(simple_merge_convex_partition, m) {
    m.doc() = "simple_merge_convex_partition";
    m.def("simple_merge_convex_partition", &simple_merge_convex_partition, "polygon convex cover using simple merge strategy");
}
