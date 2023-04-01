#pragma once

#include<bits/stdc++.h>
#include"cgal.h"

std::vector<Point> get_points(Polygon_with_holes& pol);

std::vector<Point> get_points(Polygon& pol);

std::vector<std::pair<Point,Point>> get_sides(Polygon_with_holes& pol);

std::vector<std::pair<Point,Point>> get_sides(Polygon& pol);

Polygon get_convex_hull_of_polygons(std::vector<Polygon> polygons);

Polygon get_convex_hull_of_polygons(Polygon& p1, Polygon& p2);

std::vector<Polygon_with_holes> to_polygon_vector(Polygon_set& pset);

bool is_inside_polygon_set(Polygon_set p1, Polygon_set& p2);

template<typename T>
Polygon_set to_polygon_set(T pol) {
    Polygon_set res;
    res.insert(pol);
    return res;
}

bool is_equal(Polygon_set p1, Polygon_set& p2);

template<typename A,typename B>
bool is_equal(A& p1, B& p2) {
    auto pset1 = to_polygon_set(p1);
    auto pset2 = to_polygon_set(p2);
    return is_equal(pset1, pset2);
}

Polygon_set get_union_of_polygon_sets(std::vector<Polygon_set> psets);

bool is_equal(Segment s1, Segment s2);

Polygon sort_polygon(Polygon& polygon);

std::vector<Polygon> sort_polygons(std::vector<Polygon> polygons);

std::string decompose_fraction(FT fraction);

Point get_centroid(Polygon& pol);

int get_segment_intersection_type(Segment& a, Segment& b);

bool is_segment_on_a_side(std::vector<Segment>& sides, Segment& segment);

std::pair<Point,Point> convert2pair(Segment seg);

FT compose_fraction(std::string number);

Polygon get_polygon_for_point(Point p);

