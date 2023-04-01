#include<bits/stdc++.h>
#include"../cgal.h"

std::vector<Polygon> triangulate_polygon_delaunay_cgal(Polygon_with_holes);

std::vector<Polygon> triangulate_polygon_delaunay_constrained_cgal (Polygon_with_holes, std::vector<Segment>);

std::map<int,int> get_mapping_to_delaunay_triangulation(Polygon_with_holes& polygon, std::vector<Polygon>& contained_triangulation);

