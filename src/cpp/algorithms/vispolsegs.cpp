#include <bits/stdc++.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "../cgal.h"
#include "../geometry_utils.h"
#include "../com.h"
#include "../partition_constructor.h"
#include "../chgraph.h"
#include "../clique.h"

#include "../debug.h"
#include "../common.h"

namespace py = pybind11;
using namespace std;

vector<vector<pair<OUTPUT_TYPE,OUTPUT_TYPE>>> get_convex_cover(vector<pair<int,int>> boundary_points, vector<vector<pair<int,int>>> holes_points) {
    Polygon_with_holes input_polygon = pointsandholes2polygon(boundary_points, holes_points);
    PartitionConstructor partition (input_polygon);
    partition.add_visibility_polygon_boundary_segments();

    /*auto pols = partition.get_segment_polygons();
    return cgal2pysolution(pols);*/

    auto extension_triangulation = partition.get_constrained_delaunay_triangulation();

    // return cgal2pysolution(extension_triangulation);

    CHGraph chgraph_extension (input_polygon, extension_triangulation, "vispolsegs");
    if(!chgraph_extension.read_from_cache()) {
        //return {}; // only run on cached results
        // chgraph_extension.add_all_edges_parallel();
        chgraph_extension.add_all_edges_parallel();
        chgraph_extension.write_to_cache();
    }

    CliqueCover clique_cover (chgraph_extension);
    auto polygons = clique_cover.get_cliques_vcc();

    chgraph_extension.delete_datastructures();

    return cgal2pysolution(polygons);
}

PYBIND11_MODULE(vispolsegs, m) {
    m.doc() = "vispolsegs";
    m.def("get_convex_cover", &get_convex_cover);
}
