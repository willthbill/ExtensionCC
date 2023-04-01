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

    auto delaunay_triangulation = triangulate_polygon_delaunay_cgal(input_polygon);

    PartitionConstructor partition (input_polygon);
    partition.add_extension_segments();
    partition.add_segments_from_polygons(delaunay_triangulation);
    auto extension_triangulation = partition.get_constrained_delaunay_triangulation();

    auto mapping = get_mapping_to_delaunay_triangulation(input_polygon, extension_triangulation);
    CHGraph chgraph_delaunay (input_polygon, delaunay_triangulation, "delaunay_new");
    if(!chgraph_delaunay.read_from_cache()) {
        chgraph_delaunay.add_all_edges();
        chgraph_delaunay.write_to_cache();
    }
    CHGraph chgraph_extension (input_polygon, extension_triangulation, "extension_single_delaunay_triangulation");
    if(!chgraph_extension.read_from_cache()) {
        chgraph_extension.add_edges_from_mapping(mapping, chgraph_delaunay);
        chgraph_extension.write_to_cache();
    }

    CliqueCover cover (chgraph_extension); 
    auto polygons = cover.get_cliques_smalladj_naive();

    chgraph_delaunay.delete_datastructures();
    chgraph_extension.delete_datastructures();

    return cgal2pysolution(polygons);
}

PYBIND11_MODULE(delaunay_extension_mapping, m) {
    m.doc() = "delaunay_extension_mapping";
    m.def("get_convex_cover", &get_convex_cover);
}
