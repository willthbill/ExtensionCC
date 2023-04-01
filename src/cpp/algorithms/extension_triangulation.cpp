#include <bits/stdc++.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/squared_distance_2.h>

#include "../cgal.h"
#include "../com.h"
#include "../partition_constructor.h"

#include "../debug.h"
#include "../common.h"

namespace py = pybind11;
using namespace std;

vector<vector<pair<OUTPUT_TYPE,OUTPUT_TYPE>>> get_extension_triangulation(vector<pair<int,int>> boundary_points, vector<vector<pair<int,int>>> holes_points) {
    Polygon_with_holes input_polygon = pointsandholes2polygon(boundary_points, holes_points);
    PartitionConstructor partition (input_polygon);
    partition.add_extension_segments();
    auto triangulation = partition.get_constrained_delaunay_triangulation();
    return cgal2pysolution(triangulation);
}

PYBIND11_MODULE(extension_triangulation, m) {
    m.doc() = "extension_triangulation";
    m.def("get_extension_triangulation", &get_extension_triangulation);
}
