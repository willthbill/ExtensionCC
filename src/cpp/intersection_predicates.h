#pragma once

#include<bits/stdc++.h>

#include <CGAL/Range_segment_tree_traits.h>
#include <CGAL/Range_tree_k.h>
#include <CGAL/Range_segment_tree_traits.h>
#include <CGAL/Segment_tree_k.h>

#include "cgal.h"

typedef CGAL::Range_segment_tree_set_traits_2<K> Traits_range;
typedef CGAL::Range_tree_2<Traits_range> Range_tree_2_type;
typedef Traits_range::Interval Interval_range;
typedef Traits_range::Key Key_range;
typedef CGAL::Segment_tree_map_traits_2<K, std::pair<Point,Point>> Traits_segment;
typedef CGAL::Segment_tree_2<Traits_segment> Segment_tree_2_type;
typedef Traits_segment::Interval Interval_segment;
typedef Traits_segment::Pure_interval Pure_interval_segment;
typedef Traits_segment::Key Key_segment;

class IntersectionPredicates {

private:

    Range_tree_2_type* point_structure;
    Segment_tree_2_type* side_structure;
    Polygon_with_holes polygon;
    std::map<Point,int> point2holeidx;

    void build_point_structure();

    std::vector<Point> get_points_inside(Point upperleft, Point bottomright);

    void build_side_structure();

    std::vector<std::pair<Point,Point>> get_sides_inside(Point upperleft, Point bottomright);

public:

    bool inited = false;

    IntersectionPredicates();

    IntersectionPredicates(Polygon_with_holes& pol, bool do_build=true);

    IntersectionPredicates(Polygon& pol, bool do_build=true);

    void delete_datastructures();

    bool setup_for_stars = false;
    Point center;
    Point forbidden;
    std::vector<std::pair<Direction,int>> directions;
    Polygon star;

    void setup_star_shaped(Point _center);

    // it is not totally reliable. if it is on a side then there is no guarantee
    bool is_point_inside_star_shaped_polygon(Point& p);

    // pol must be party contained already, positive area of overlapping part
    bool is_completely_inside_for_convex_pol(Polygon& pol);

    // boundaries does not touch either
    bool is_completely_outside_star_shaped(Polygon pol);

    // boundaries may touch
    bool is_completely_inside_star_shaped_for_convex_pol(Polygon pol);

    bool is_completely_inside_slow(Polygon pol);

    // boundaries does not touch either
    bool is_completely_outside_slow(Polygon pol);

    bool is_point_in_polygon_slow(Point p);

    std::vector<Segment> get_segments_with_point_on(Point p);

    bool has_intersecting_segment_on_seg(Segment seg);

    Polygon get_hole_in_interior(Polygon& pol);

};
