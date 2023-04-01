#include <bits/stdc++.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "../cgal.h"
#include "../geometry_utils.h"
#include "../com.h"
#include "../partition_constructor.h"
#include "../chgraph.h"
#include "../clique.h"
#include "../triangulation/triangulation.h"

#include "../debug.h"
#include "../common.h"

namespace py = pybind11;
using namespace std;

vector<vector<pair<OUTPUT_TYPE,OUTPUT_TYPE>>> get_convex_cover(vector<pair<int,int>> boundary_points, vector<vector<pair<int,int>>> holes_points) {
    Polygon_with_holes input_polygon = pointsandholes2polygon(boundary_points, holes_points);

    PartitionConstructor partition (input_polygon);
    partition.add_extension_segments();
    auto extension_triangulation = partition.get_constrained_delaunay_triangulation();

    CHGraph chgraph_extension (input_polygon, extension_triangulation, "extension_fully_visible_bfs_exact_nohelp");
    //if(!chgraph_extension.read_from_cache()) {
        if(getenv("USE_INEXACT") && strcmp(getenv("USE_INEXACT"), "1") == 0) {
            chgraph_extension.add_fully_visible_bfs_edges_inexact();
        } else {
            chgraph_extension.add_fully_visible_bfs_edges_exact();
        }
        chgraph_extension.write_to_cache();
    //}

    chgraph_extension.delete_datastructures();

    vector<Polygon> tmp;
    return cgal2pysolution(tmp);
}

PYBIND11_MODULE(fully_visible_extension_graph_calculation, m) {
    m.doc() = "fully_visible_extension_graph_calculation";
    m.def("get_convex_cover", &get_convex_cover);
}
