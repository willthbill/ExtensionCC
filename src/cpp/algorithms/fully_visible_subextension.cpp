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

    assert(getenv("SUBEXT_TYPE"));

    PartitionConstructor partition (input_polygon);
    string chgraph_type = "unset";
    bool use_exact = true;
    bool use_all = false;
    if(getenv("SUBEXT_TYPE") && strcmp(getenv("SUBEXT_TYPE"), "DIVISION") == 0) {
        chgraph_type = "extension_fully_visible_bfs_exact_nohelp_division";
        partition.add_extension_division_subsegments();
    } else if(getenv("SUBEXT_TYPE") && strcmp(getenv("SUBEXT_TYPE"), "CONST_THROW_AWAY") == 0) {
        assert(getenv("MAX_LENGTH"));
        int mxlength = stoi(getenv("MAX_LENGTH"));
        chgraph_type = "extension_fully_visible_bfs_exact_nohelp_throw_away_" + to_string(mxlength);
        partition.add_extension_constant_throw_away_segments(mxlength);
    } else if(getenv("SUBEXT_TYPE") && strcmp(getenv("SUBEXT_TYPE"), "CORNER_CHANGES") == 0) {
        chgraph_type = "extension_fully_visible_bfs_inexact_nohelp_corner_changes";
        partition.add_extension_corner_changes_segments();
        use_exact = false;
    } else if(getenv("SUBEXT_TYPE") && strcmp(getenv("SUBEXT_TYPE"), "AVGBOUND") == 0) {
        assert(getenv("MAGIC"));
        int magic = stoi(getenv("MAGIC"));
        chgraph_type = "extension_fully_visible_bfs_exact_nohelp_avgbound_" + to_string(magic);
        partition.add_extension_avgbound_throw_away_segments(magic);
    } else if(getenv("SUBEXT_TYPE") && strcmp(getenv("SUBEXT_TYPE"), "CONCAVETRI") == 0) {
        assert(getenv("GRIDLINES"));
        int gridlines = stoi(getenv("GRIDLINES"));
        chgraph_type = "report_extension_fully_visible_bfs_exact_nohelp_concavetriangles_" + to_string(gridlines);
        partition.add_extension_concave_triangles_segments();
        // partition.add_extension_corner_changes_segments();
        if(gridlines > 0) partition.add_grid_segments(gridlines);
        use_all = true;
    } else if(getenv("SUBEXT_TYPE") && strcmp(getenv("SUBEXT_TYPE"), "AVGBOUNDSAMPLING") == 0) {
        assert(getenv("MAGIC"));
        int magic = stoi(getenv("MAGIC"));
        chgraph_type = "extension_fully_visible_bfs_inexact_nohelp_avgboundsampling_" + to_string(magic);
        partition.add_extension_avgbound_sampling_segments(magic);
    } else {
        assert(false);
    }

    /*auto extension_polygons = partition.get_segment_polygons();
    return cgal2pysolution(extension_polygons);*/

    auto extension_triangulation = partition.get_constrained_delaunay_triangulation();
    return cgal2pysolution(extension_triangulation);

    CHGraph chgraph_extension (input_polygon, extension_triangulation, chgraph_type);

    if(!chgraph_extension.read_from_cache()) {
        //return {}; // only run on cached results
        if(use_all) {
            chgraph_extension.add_all_edges();
        } else {
            if(use_exact) chgraph_extension.add_fully_visible_bfs_edges_exact();
            else chgraph_extension.add_fully_visible_bfs_edges_inexact();
        }
        chgraph_extension.write_to_cache();
    }

    // return {};

    CliqueCover clique_cover (chgraph_extension);
    auto polygons = clique_cover.get_cliques_vcc();

    chgraph_extension.delete_datastructures();

    return cgal2pysolution(polygons);
}

PYBIND11_MODULE(fully_visible_subextension, m) {
    m.doc() = "fully_visible_subextension";
    m.def("get_convex_cover", &get_convex_cover);
}
