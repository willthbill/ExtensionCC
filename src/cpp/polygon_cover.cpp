#include <bits/stdc++.h>

#include "dualgraph.h"
#include "find_components.h"
#include "cgal.h"
#include "polygon_cover.h"
#include "debug.h"
#include "common.h"

using namespace std;

vector<pair<Polygon,vector<Polygon>>> get_polygon_partition(vector<Polygon> triangulation) {
    DualGraph graph (triangulation);
    auto get_polygon_for_component = [&](vector<int>& comp) -> Polygon_with_holes {
        // can be done faster by splitting into 2 halfs and joining them recursively
        Polygon_set pset;
        for(auto& node : comp) {
            auto pol = graph.get_polygon(node);
            if(pol.orientation() == -1) pol.reverse_orientation(); // bad practice!
            pset.join(pol);
        }
        assert(pset.number_of_polygons_with_holes() == 1);
        list<Polygon_with_holes> pols;
        pset.polygons_with_holes (back_inserter(pols));
        for(auto& pol : pols) {
            return pol;
        }
        assert(false);
    };
    ComponentFinder compfinder (graph.adj, [&](vector<int> comp) -> bool {
        if(comp.size() == 0) return true;
        return !get_polygon_for_component(comp).has_holes();
    });
    vector<vector<int>> components = compfinder.find_components();
    vector<pair<Polygon,vector<Polygon>>> polygons;
    for(auto& comp : components) {
        pair<Polygon,vector<Polygon>> polygon = {get_polygon_for_component(comp).outer_boundary(), {}};
        for(auto& node : comp) {
            auto pol = graph.get_polygon(node);
            polygon.se.push_back(pol); // triangle
        }
        polygons.push_back(polygon);
    }
    return polygons;
}
