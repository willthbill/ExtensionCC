#include "intersection_predicates.h"
#include "geometry_utils.h"
#include <CGAL/Boolean_set_operations_2.h>
#include <bits/stdc++.h>

#include "common.h"
#include "debug.h"

using namespace std;

void IntersectionPredicates::build_point_structure() {
    vector<Point> points = get_points(polygon);
    vector<Key_range> input;
    for(auto& data : points) input.pb(Key_range(data));
    point_structure = new Range_tree_2_type(input.begin(), input.end()); // TODO: memory leaks
    // point_structure.make_tree(input.begin(), input.end());
}

vector<Point> IntersectionPredicates::get_points_inside(Point upperleft, Point bottomright) {
    vector<Point> output; // faster with list?
    Interval_range win(upperleft, bottomright);
    point_structure->window_query(win, back_inserter(output));
    return output;
}

void IntersectionPredicates::build_side_structure() {
    vector<pair<Point,Point>> sides = get_sides(polygon);
    vector<Interval_segment> input;
    for(auto& data : sides) input.pb(Interval_segment(
        Pure_interval_segment(
            Point(min(data.fi.x(),data.se.x()) - 1, min(data.fi.y(),data.se.y()) - 1),
            Point(max(data.fi.x(),data.se.x()) + 1, max(data.fi.y(),data.se.y()) + 1)
        ), data
    ));
    side_structure = new Segment_tree_2_type(input.begin(), input.end());
    // side_structure.make_tree(input.begin(), input.end());
}

vector<pair<Point,Point>> IntersectionPredicates::get_sides_inside(Point upperleft, Point bottomright) {
    vector<Interval_segment> intervals; // faster with list?
    Interval_segment win(Pure_interval_segment(upperleft, bottomright),{Point(0,0),Point(0,0)});
    side_structure->window_query(win, back_inserter(intervals));
    vector<pair<Point,Point>> output;
    foe(interval, intervals) output.pb(interval.se);
    return output;
}

IntersectionPredicates::IntersectionPredicates(){}

IntersectionPredicates::IntersectionPredicates(Polygon_with_holes& pol, bool do_build) {
    inited = true;
    polygon = pol;
    if(do_build) {
        build_point_structure();
        build_side_structure();
        {
            auto holes = pol.holes();
            for(int i = 0; i < holes.size(); i++) {
                foe(p, holes[i]) {
                    point2holeidx[p] = i;
                }
            }
        }
    }
}

IntersectionPredicates::IntersectionPredicates(Polygon& pol, bool do_build) {
    inited = true;
    polygon = Polygon_with_holes(pol);
    if(do_build) {
        build_point_structure();
        build_side_structure();
    }
}

void IntersectionPredicates::delete_datastructures() {
    if(point_structure != NULL) {
        delete point_structure;
        point_structure = NULL;
    }
    if(side_structure != NULL) {
        delete side_structure;
        side_structure = NULL;
    }
}

int time_binary_search = 0;

void IntersectionPredicates::setup_star_shaped(Point _center) {
    // cout << "Time binary search: " << MICRO_TO_MS(time_binary_search) << endl;
    setup_for_stars = true;
    center = _center;
    assert(polygon.number_of_holes() == 0);
    star = polygon.outer_boundary();
    assert(star.size() >= 3);
    forbidden = Point(-1e9, -1e9);
    bool found = false;
    for(int i = 0; i < star.size(); i++) {
        if(star[i] == center) {
            assert(!found);
            forbidden = star[(i+1) % star.size()]; // if endpoint of seg == part of star boundary
            found = true;
        }
    }
    if(!found) {
        for(int i = 0; i < star.size(); i++) {
            Segment seg (star[i], star[(i+1) % star.size()]);
            if(seg.has_on(center)) {
                assert(!found);
                forbidden = star[(i+1) % star.size()]; // if on a segment
                found = true;
            }
        }
    }
    for(int i = 0; i < star.size(); i++) {
        auto& p = star[i];
        if(p == center) continue; // right?
        Vector vec (center, p);
        directions.pb({vec.direction(), i});
    }
    sort(all(directions));
}

bool IntersectionPredicates::is_point_inside_star_shaped_polygon(Point& p) {
    // TIME start = NOW();
    assert(setup_for_stars);
    if(p == center) return true;
    Vector vec (center, p);
    Segment segment (center, p);
    int idx = lower_bound(all(directions), pair<Direction,int>{vec.direction(),-1}) - directions.begin();
    if(idx == directions.size()) idx = 0;
    int point_idx = directions[idx].se;
    if(star[point_idx] == forbidden) {
        // time_binary_search += TO_MICRO(NOW(), start);
        return false;
    }
    for(int delta = -2; delta <= 2; delta++) {
        int dir_idx = (idx + delta + directions.size()); while(dir_idx >= directions.size()) dir_idx -= directions.size();
        int point_idx = directions[dir_idx].se;
        int star_idx = (point_idx - 1 + star.size()); while(star_idx >= star.size()) star_idx -= star.size();
        // Point p1 = star[(point_idx - 1 + star.size()) % star.size()];
        Point p1 = star[star_idx];
        Point p2 = star[point_idx];
        if(p2 == forbidden) continue; // important if center is on a segment, this condition implies the next one if center is on star boundary
        if(p1 == center || p2 == center) continue; // segment will always intersect test_segment otherwise
        Segment test_segment (p1, p2);
        if(intersection(test_segment, segment)) {
            // time_binary_search += TO_MICRO(NOW(), start);
            return false;
        }
    }
    // time_binary_search += TO_MICRO(NOW(), start);
    return true;
}

bool IntersectionPredicates::is_completely_inside_for_convex_pol(Polygon& pol) {
    auto bbox = pol.bbox();
    vector<Point> points = get_points_inside(
        Point(bbox.xmin() - 1, bbox.ymin() - 1), // TODO: fix this!
        Point(bbox.xmax() + 1, bbox.ymax() + 1)
    );
    for(auto& p : points) {
        if(pol.bounded_side(p) == CGAL::ON_BOUNDED_SIDE) { // strictly inside pol
            return false;
        }
    }
    vector<pair<Point,Point>> sides = get_sides_inside(
        Point(bbox.xmin() - 1, bbox.ymin() - 1), // TODO: fix this!
        Point(bbox.xmax() + 1, bbox.ymax() + 1)
    );
    for(auto& side : sides) {
        bool is_on_a_line = false;
        Point touch (-1e9, -1e9);
        bool seen_touch = false;
        bool flag_touch = false;
        for(auto e : pol.edges()) {
            auto v = intersection(Segment(side.fi, side.se), e);
            if (v) {
                if (const Point *p = boost::get<Point>(&*v)) {
                    // side and e intersects at a point
                    if(flag_touch) continue;
                    if(seen_touch && *p != touch) {
                        flag_touch = true;
                        continue;
                    } else {
                        touch = *p;
                    }
                    seen_touch = true;
                } else {
                    // side and e intersects at a segment, thus side is not inside pol since pol is convex
                    // const Segment_2 *s = boost::get<Segment_2>(&*v);
                    is_on_a_line = true;
                    break;
                }
            }
        }
        if(is_on_a_line) continue;
        if(flag_touch) {
            return false;
        }
    }
    return true;
    /*if(is_not_completely_outside) return true;
    // else if(is_star_shaped) return is_point_inside_star_shaped_polygon(pol[0]);
    // else return CGAL::oriented_side(pol[0], polygon) == CGAL::POSITIVE;
    else return is_completely_inside_slow(pol);*/
}

bool IntersectionPredicates::is_completely_outside_star_shaped(Polygon pol) {
    auto bbox = pol.bbox();
    vector<Point> points = get_points_inside(
        Point(bbox.xmin() - 1, bbox.ymin() - 1), // TODO: fix this!
        Point(bbox.xmax() + 1, bbox.ymax() + 1)
    );
    for(auto& p : points) {
        auto side = pol.bounded_side(p);
        if(side == CGAL::ON_BOUNDED_SIDE || side == CGAL::ON_BOUNDARY) {
            return false;
        }
    }
    vector<pair<Point,Point>> sides = get_sides_inside(
        Point(bbox.xmin() - 1, bbox.ymin() - 1), // TODO: fix this!
        Point(bbox.xmax() + 1, bbox.ymax() + 1)
    );
    for(auto& side : sides) {
        for(auto e : pol.edges()) {
            auto v = intersection(Segment(side.fi, side.se), e);
            if(v) {
                return false;
            }
        }
    }
    // assert(is_point_in_polygon_slow(pol[0]) == is_point_inside_star_shaped_polygon(pol[0]));
    if(is_point_inside_star_shaped_polygon(pol[0])) return false;
    return true;
}

bool IntersectionPredicates::is_completely_inside_star_shaped_for_convex_pol(Polygon pol) {
    auto bbox = pol.bbox();
    vector<Point> points = get_points_inside(
        Point(bbox.xmin() - 1, bbox.ymin() - 1), // TODO: fix this!
        Point(bbox.xmax() + 1, bbox.ymax() + 1)
    );
    for(auto& p : points) {
        auto side = pol.bounded_side(p);
        if(side == CGAL::ON_BOUNDED_SIDE) {
            return false;
        }
    }
    vector<pair<Point,Point>> sides = get_sides_inside(
        Point(bbox.xmin() - 1, bbox.ymin() - 1), // TODO: fix this!
        Point(bbox.xmax() + 1, bbox.ymax() + 1)
    );
    for(auto& side : sides) {
        bool is_on_a_line = false;
        Point touch (-1e9, -1e9);
        bool seen_touch = false;
        bool flag_touch = false;
        for(auto e : pol.edges()) {
            auto v = intersection(Segment(side.fi, side.se), e);
            if (v) {
                if (const Point *p = boost::get<Point>(&*v)) {
                    // side and e intersects at a point
                    if(flag_touch) continue;
                    if(seen_touch && *p != touch) {
                        flag_touch = true;
                        continue;
                    } else {
                        touch = *p;
                    }
                    seen_touch = true;
                } else {
                    // side and e intersects at a segment, thus side is not inside pol since pol is convex
                    // const Segment_2 *s = boost::get<Segment_2>(&*v);
                    is_on_a_line = true;
                    break;
                }
            }
        }
        if(is_on_a_line) continue;
        if(flag_touch) {
            return false;
        }
    }

    Point centroid (0,0);
    for(auto& p : pol) centroid += Vector(Point(0,0), p) / pol.size();
    // assert(is_completely_inside_slow(pol) == is_point_inside_star_shaped_polygon(centroid));
    // assert(is_point_in_polygon_slow(centroid) == is_point_inside_star_shaped_polygon(centroid));
    return is_point_inside_star_shaped_polygon(centroid);
}

bool IntersectionPredicates::is_completely_inside_slow(Polygon pol) {
    Polygon_set intersection; intersection.intersection(
        to_polygon_set(polygon),
        to_polygon_set(pol)
    );
    auto arr = to_polygon_vector(intersection);
    return arr.size() == 1 && is_equal(pol, arr[0]);
}

bool IntersectionPredicates::has_intersecting_segment_on_seg(Segment seg) {
    auto bbox = seg.bbox();
    vector<pair<Point,Point>> sides = get_sides_inside(
        Point(bbox.xmin() - 1, bbox.ymin() - 1), // TODO: fix this!
        Point(bbox.xmax() + 1, bbox.ymax() + 1)
    );
    for(auto& side : sides) {
        auto v = intersection(Segment(side.fi, side.se), seg);
        if (v) {
            if (const Point *p = boost::get<Point>(&*v)) {
            } else {
                return true;
            }
        }
    }
    return false;
}

vector<Segment> IntersectionPredicates::get_segments_with_point_on(Point p) {
    vector<Segment> sides;
    for(auto side_p : get_sides_inside(
        Point(p.x() - 1, p.y() - 1),
        Point(p.x() + 1, p.y() + 1)
    )) {
        Segment side (side_p.fi, side_p.se);
        if(side.has_on(p)) {
            sides.pb(side);
        }
    }
    return sides;
}

Polygon IntersectionPredicates::get_hole_in_interior(Polygon& pol) {
    auto bbox = pol.bbox();
    vector<Point> points = get_points_inside(
        Point(bbox.xmin() - 1, bbox.ymin() - 1), // TODO: fix this!
        Point(bbox.xmax() + 1, bbox.ymax() + 1)
    );
    for(auto& p : points) {
        auto side =  pol.bounded_side(p);
        if(point2holeidx.count(p) && side == CGAL::ON_BOUNDED_SIDE) {
            return polygon.holes()[point2holeidx[p]];
        }
    }
    assert(false);
}

bool IntersectionPredicates::is_completely_outside_slow(Polygon pol) {
    Polygon_set intersection; intersection.intersection(
        to_polygon_set(polygon),
        to_polygon_set(pol)
    );
    Polygon_set union_set; union_set.join(
        to_polygon_set(polygon),
        to_polygon_set(pol)
    );
    auto arr = to_polygon_vector(union_set);
    return arr.size() == 2; // RIGHT?
}

bool IntersectionPredicates::is_point_in_polygon_slow(Point p) {
    auto os = CGAL::oriented_side(p, polygon);
    return os == CGAL::ON_ORIENTED_BOUNDARY || os == CGAL::POSITIVE;
}
