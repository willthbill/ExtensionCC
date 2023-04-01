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
    TIME start_total = NOW();

    Polygon_with_holes input_polygon = pointsandholes2polygon(boundary_points, holes_points);

    TIME start_partition = NOW();
    TIME start_segments = NOW();
    PartitionConstructor partition (input_polygon);
    partition.add_extension_segments();
    TIME end_segments = NOW();
    TIME start_triangulation = NOW();
    auto triangulation = partition.get_constrained_delaunay_triangulation();
    TIME end_triangulation = NOW();
    TIME end_partition = NOW();

    CHGraph chgraph (input_polygon, triangulation, "extension_triangulation_benchmark");
    TIME start_graph = NOW();
    chgraph.add_all_edges();
    TIME end_graph = NOW();
    chgraph.write_to_cache();

    TIME start_clique = NOW();
    CliqueCover cover (chgraph);
    auto polygons = cover.get_cliques_vcc();
    TIME end_clique = NOW();

    chgraph.delete_datastructures();

    TIME end_total = NOW();

    cout << "\n\n\nInput-polygon stats" << endl;
    cout << "  number of points: " << ((int)(get_points(input_polygon).size())) << endl;
    cout << "  number of points on boundary: " << ((int)(input_polygon.outer_boundary().size())) << endl;
    int n_holes = 0; foe(e, input_polygon.holes()) n_holes++;
    cout << "  number of holes: " << n_holes << endl;
    cout << "\n\n\nSize stats" << endl;
    cout << "  number of constraints used in partition: " << partition.get_number_of_segments() << endl;
    cout << "  number of intersections should be linear in the size of the triangulation" << endl;
    cout << "  size of triangulation: " << sz(triangulation) << endl;
    cout << "  size of graph (|V|, |E|): " << "(" << cover.n() << ", " << cover.m() << ")" << endl;
    cout << "  size of solution: " << sz(polygons) << endl;
    cout << "\n\n\nTime stats" << endl;
    cout << "  time segments computation: " << COUT_TIME(TO_MICRO(end_segments, start_segments)) << endl;
    cout << "  time triangulation computation: " << COUT_TIME(TO_MICRO(end_triangulation, start_triangulation)) << endl;
    cout << "  time partition computation (segments + triangulation): " << COUT_TIME(TO_MICRO(end_partition, start_partition)) << endl;
    cout << "  time visibility graph computation: " << COUT_TIME(TO_MICRO(end_graph, start_graph)) << endl;
    cout << "  time clique-cover computation: " << COUT_TIME(TO_MICRO(end_clique, start_clique)) << endl;
    cout << "  time total: " << COUT_TIME(TO_MICRO(end_total, start_total)) << endl;
    cout << "\n\n\n";
    cout << "extension triangulation variant with full visibility graph and chalupa" << endl;

    return cgal2pysolution(polygons);
}

PYBIND11_MODULE(clique_visibility_extension, m) {
    m.doc() = "clique_visibility_extension";
    m.def("get_convex_cover", &get_convex_cover);
}
