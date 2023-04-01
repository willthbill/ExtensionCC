#include "../triangulation/triangulation.h"
#include "../com.h"
#include "../clique.h"
#include "../debug.h"
#include "../polygon_cover.h"
#include <bits/stdc++.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <CGAL/ch_akl_toussaint.h>
#include <CGAL/Boolean_set_operations_2.h>
#include "../common.h"

namespace py = pybind11;

using namespace std;

/*Polygon get_convex_hull_of_polygons(vector<Polygon> polygons) {
    vector<Point> pset;
    for(auto& pol : polygons) {
        for(Point& p : pol) pset.push_back(p);
    }
    Polygon res;
    CGAL::ch_akl_toussaint(pset.begin(), pset.end(), back_inserter(res));
    return res;
}

template<typename T>
Polygon_set to_polygon_set(T& pol) {
    Polygon_set res;
    res.insert(pol);
    return res;
}

vector<Polygon_with_holes> to_polygon_vector(Polygon_set& pset) {
    vector<Polygon_with_holes> pols;
    pset.polygons_with_holes (back_inserter(pols));
    return pols;
}

Polygon_set get_intersection(Polygon_set& p1, Polygon_set& p2) {
    Polygon_set intersection; intersection.intersection(p1,p2);
    return intersection;
}

// is p1 inside p2
bool is_inside_polygon_set(Polygon_set p1, Polygon_set& p2) {
    p1.difference(p2);
    return p1.is_empty();
}

bool is_equal(Polygon_set p1, Polygon_set& p2) {
    return is_inside_polygon_set(p1, p2) && is_inside_polygon_set(p2, p1);
}

template<typename A,typename B>
bool is_equal(A& p1, B& p2) {
    auto pset1 = to_polygon_set(p1);
    auto pset2 = to_polygon_set(p2);
    return is_equal(pset1, pset2);
}*/

// TODO: speedup using locate function of polygon_set???
vector<vector<pair<OUTPUT_TYPE,OUTPUT_TYPE>>> get_clique_decomposition(vector<pair<int,int>> boundary_points, vector<vector<pair<int,int>>> holes_points) {
    assert(false);
    /*auto input_polygon = pointsandholes2polygon(boundary_points, holes_points);
    auto triangulation = triangulate_polygon_delaunay_cgal(pointsandholes2polygon(boundary_points, holes_points));
    auto subpolygons = get_polygon_partition(triangulation);
    vector<Polygon> polygons;
    for(auto& sub : subpolygons) {
        auto& triangles = sub.se;
        cout << "SETTING UP" << endl;
        Polygon conv_sub = get_convex_hull_of_polygons({sub.fi}); // copy ahhhh
        Polygon_set checker; checker.intersection(
            to_polygon_set(conv_sub),
            to_polygon_set(input_polygon)
        );
        int n = triangles.size();
        map<int,set<int>> adj;
        for(int i = 0; i < n; i++) adj[i] = {};
        cout << "BUILDING ADJ" << endl;
        for(int i = 0; i < n; i++) for(int j = i + 1; j < n; j++) {
            Polygon ch_edge = get_convex_hull_of_polygons({triangles[i], triangles[j]});
            Polygon_set intersection; intersection.intersection(
                checker,
                to_polygon_set(ch_edge)
            );
            auto arr = to_polygon_vector(intersection);
            if(arr.size() == 1 && is_equal(ch_edge, arr[0])) {
                adj[i].insert(j);
                adj[j].insert(i);
            }
        }
        cout << "FINDING CLIQUES" << endl;
        CliqueCover cliquecover (adj);
        // vector<vector<int>> cliques = cliquecover.get_cliques();
        vector<Polygon> clique_polygons = cliquecover.get_cliques_smalladj_naive();
        for(auto& p : clique_polygons) {
            polygons.push_back(p);
        }
        continue;
        // the below maybe does nothing???
        cout << "REMOVING UNNECESSARY CLIQUES" << endl;
        int m = cliques.size();
        vector<Polygon_set> clique_polygon_sets;
        for(auto& p : clique_polygons) {
            clique_polygon_sets.push_back(to_polygon_set(p));
        }
        set<int> considering; // TODO can be speed up, by sorting by area or something...
        for(int i = 0; i < m; i++) considering.insert(i);
        auto remove_covered_components = [&]() {
            vector<int> tocheck;
            for(auto& i : considering) tocheck.push_back(i);
            for(auto& i : tocheck) {
                bool ok = 1;
                for(auto& j : considering) {
                    if(i == j) continue;
                    if(is_inside_polygon_set(clique_polygon_sets[i], clique_polygon_sets[j])) {
                        ok = 0;
                    }
                }
                if(!ok) {
                    considering.erase(i);
                }
            }
        };
        set<int> sure_of;
        auto is_component_covered_by_rest = [&](int idx) {
            Polygon_set others;
            for(auto& other : considering) {
                if(other == idx) continue;
                others.join(clique_polygon_sets[other]);
            }
            return is_inside_polygon_set(clique_polygon_sets[idx], others);
        };
        auto add_to_sure = [&](int idx) {
            for(auto& other : considering) {
                if(other == idx) continue;
                clique_polygon_sets[other].difference(clique_polygon_sets[idx]);
            }
            considering.erase(idx);
            sure_of.insert(idx);
        };
        auto add_noncovered_components = [&]() {
            vector<int> tocheck;
            for(auto& i : considering) tocheck.push_back(i);
            for(auto& idx : tocheck) {
                if(is_component_covered_by_rest(idx)) {
                    add_to_sure(idx);
                }
            }
        };
        auto add_best_component = [&]() {
            int idx = *considering.begin();
            add_to_sure(idx);
        };
        while(considering.size()) {
            remove_covered_components();
            int size_before = sure_of.size();
            add_noncovered_components();
            int size_after = sure_of.size();
            if(size_before == size_after && considering.size()) {
                add_best_component();
            }
        }
        cout << "PUSHING TO SOLUTION" << endl;
        for(auto& idx : sure_of) {
            polygons.push_back(clique_polygons[idx]);
        }
    }
    return cgal2pysolution(polygons);*/
    return {};
}

PYBIND11_MODULE(clique_decomposition, m) {
    m.doc() = "clique_decomposition";
    m.def("get_clique_decomposition", &get_clique_decomposition);
}
