#include<bits/stdc++.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include"polygon_delaunay.cpp"
#include"polygon_delaunay_constrained.cpp"

namespace py = pybind11;

PYBIND11_MODULE(triangulation, m) {
    m.doc() = "triangulation";
    m.def("triangulate_polygon_delaunay", &triangulate_polygon_delaunay, "polygon delaunay triangulation");
}
