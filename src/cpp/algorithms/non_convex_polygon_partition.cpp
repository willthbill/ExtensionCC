// will fail, for testing purposes

#include "../triangulation/triangulation.h"
#include "../com.h"
#include "../cgal.h"
#include "../polygon_cover.h"
#include <bits/stdc++.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <CGAL/Polygon_set_2.h>
#include "../common.h"

typedef CGAL::Polygon_set_2<K> Polygon_set_2;

namespace py = pybind11;

using namespace std;

vector<vector<pair<OUTPUT_TYPE,OUTPUT_TYPE>>> non_convex_polygon_partition(vector<pair<int,int>> boundary_points, vector<vector<pair<int,int>>> holes_points) {
    auto triangulation = triangulate_polygon_delaunay_cgal(pointsandholes2polygon(boundary_points, holes_points));
    auto t = get_polygon_partition(triangulation);
    vector<Polygon> polygons;
    for(auto& e : t) polygons.push_back(e.fi);
    return cgal2pysolution(polygons);
}

PYBIND11_MODULE(non_convex_polygon_partition, m) {
    m.doc() = "non_convex_polygon_partition";
    m.def("non_convex_polygon_partition", &non_convex_polygon_partition);
}
