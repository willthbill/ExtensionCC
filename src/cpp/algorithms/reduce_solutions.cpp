#include <bits/stdc++.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "../cgal.h"
#include "../com.h"
#include "../reduce.h"

#include "../common.h"
#include "../debug.h"

namespace py = pybind11;

using namespace std;

#define InputPolygonStr vector<pair<string,string>>
#define InputPolygonInt vector<pair<int,int>>
#define InputPolygonWithHolesInt pair<InputPolygonInt,vector<InputPolygonInt>>

vector<vector<pair<OUTPUT_TYPE,OUTPUT_TYPE>>> reduce_solution(
    InputPolygonWithHolesInt input_polygon,
    vector<InputPolygonStr> input_cover
) {
    auto polygon = pointsandholes2polygon(input_polygon.fi, input_polygon.se);
    auto cover = cover2polygonlist(input_cover);
    Reducer reducer (polygon, cover);
    auto polygons = reducer.reduce_repeated();
    return cgal2pysolution(polygons);
}

PYBIND11_MODULE(reduce_solutions, m) {
    m.doc() = "reduce_solutions";
    m.def("reduce_solution", &reduce_solution);
}
