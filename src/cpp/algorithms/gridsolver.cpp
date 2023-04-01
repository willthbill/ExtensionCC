#include <bits/stdc++.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "../cgal.h"
#include "../com.h"
#include "../gridsolver.h"

#include "../debug.h"
#include "../common.h"

namespace py = pybind11;
using namespace std;

vector<vector<pair<OUTPUT_TYPE,OUTPUT_TYPE>>> get_convex_cover(vector<pair<int,int>> boundary_points, vector<vector<pair<int,int>>> holes_points) {
    Polygon_with_holes input_polygon = pointsandholes2polygon(boundary_points, holes_points);
    GridSolver solver (input_polygon);
    auto polygons = solver.get_convex_cover();
    return cgal2pysolution(polygons);
}

PYBIND11_MODULE(gridsolver, m) {
    m.doc() = "gridsolver";
    m.def("get_convex_cover", &get_convex_cover);
}
