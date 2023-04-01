#include <bits/stdc++.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "../cgal.h"
#include "../geometry_utils.h"
#include "../triangulation/triangulation.h"
#include "../partition_constructor.h"
#include "../chgraph.h"
#include "../com.h"

#include "../debug.h"
#include "../common.h"

namespace py = pybind11;
using namespace std;

vector<vector<pair<OUTPUT_TYPE,OUTPUT_TYPE>>> list_graphs(vector<pair<int,int>> boundary_points, vector<vector<pair<int,int>>> holes_points) {
    Polygon_with_holes input_polygon = pointsandholes2polygon(boundary_points, holes_points);

    PartitionConstructor partition (input_polygon);

    vector<Polygon> polygons;
    Polygon pol; pol.push_back(Point(0,0)); pol.push_back(Point(0,1)); pol.push_back(Point(1,1));
    polygons.push_back(pol);

    if(getenv("GRAPH_TYPE")) {
        if(strcmp(getenv("GRAPH_TYPE"), "DELAUNAY_FULL") == 0) {
            auto triangulation = triangulate_polygon_delaunay_cgal(input_polygon);
            CHGraph chgraph (input_polygon, triangulation, "delaunay_new");
            if(chgraph.read_from_cache()) {
                return cgal2pysolution(polygons);
            } else {
                return {};
            }
        } else if(strcmp(getenv("GRAPH_TYPE"), "EXTENSION_FULL") == 0) {
            partition.add_extension_segments();
            auto triangulation = partition.get_constrained_delaunay_triangulation();
            CHGraph chgraph (input_polygon, triangulation, "extension_triangulation");
            if(chgraph.read_from_cache()) {
                return cgal2pysolution(polygons);
            } else {
                return {};
            }
        } else if(strcmp(getenv("GRAPH_TYPE"), "EXTENSION_VISIBLE") == 0) {
            partition.add_extension_segments();
            auto extension_triangulation = partition.get_constrained_delaunay_triangulation();
            CHGraph chgraph_extension (input_polygon, extension_triangulation, "extension_fully_visible_bfs_exact_nohelp");
            if(chgraph_extension.read_from_cache()) {
                return cgal2pysolution(polygons);
            } else {
                return {};
            }
        } else if(strcmp(getenv("GRAPH_TYPE"), "EXTENSION_CONSTRAINED_VISIBLE") == 0) {
            cerr << "not implemented" << endl;
            assert(false);
        } else {
            cerr << "invalid GRAPH_TYPE" << endl;
            assert(false);
        }
    } else {
        cerr << "environment variable GRAPH_TYPE is not set" << endl;
        assert(false);
    }
    assert(1 == 2);
}

PYBIND11_MODULE(list_graphs, m) {
    m.doc() = "list_graphs";
    m.def("list_graphs", &list_graphs);
}
