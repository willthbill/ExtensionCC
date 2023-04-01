#include <bits/stdc++.h>
#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/ch_akl_toussaint.h>
#include <CGAL/convex_hull_2.h>
#include <CGAL/ch_jarvis.h>
#include <CGAL/Gmpz.h>

#include "cgal.h"
#include "common.h"
#include "geometry_utils.h"

using namespace std;

vector<Point> get_points(Polygon& pol) {
    vector<Point> points;
    for(auto& p : pol) points.pb(p);
    mkunique(points);
    return points;
}

vector<Point> get_points(Polygon_with_holes& pol) {
    vector<Point> points;
    for(auto& p : pol.outer_boundary()) points.pb(p);
    for(auto& hole : pol.holes()) {
        for(auto& p : hole) points.pb(p);
    }
    mkunique(points);
    return points;
}

vector<pair<Point,Point>> get_sides(Polygon& pol) {
    vector<pair<Point,Point>> sides;
    for(auto e : pol.edges()) {
        pair<Point, Point> key = {e.min(), e.max()};
        sides.pb(key);
    }
    return sides;
}

vector<pair<Point,Point>> get_sides(Polygon_with_holes& pol) {
    vector<pair<Point,Point>> sides;
    for(auto e : pol.outer_boundary().edges()) {
        pair<Point, Point> key = {e.min(), e.max()};
        sides.pb(key);
    }
    for(auto& hole: pol.holes()) {
        for(auto e : hole.edges()) {
            pair<Point, Point> key = {e.min(), e.max()};
            sides.pb(key);
        }
    }
    mkunique(sides);
    return sides;
}

Polygon get_convex_hull_of_polygons(Polygon& p1, Polygon& p2) {
    vector<Point> pset;
    for(Point& p : p1) pset.push_back(p);
    for(Point& p : p2) pset.push_back(p);
    Polygon res;
    CGAL::ch_akl_toussaint(pset.begin(), pset.end(), back_inserter(res));
    return res;
}

Polygon get_convex_hull_of_polygons(vector<Polygon> polygons) {
    vector<Point> pset;
    for(auto& pol : polygons) {
        for(Point& p : pol) pset.push_back(p);
    }
    Polygon res;
    CGAL::ch_akl_toussaint(pset.begin(), pset.end(), back_inserter(res));
    //CGAL::ch_jarvis(pset.begin(), pset.end(), back_inserter(res));
    // CGAL::convex_hull_2(pset.begin(), pset.end(), back_inserter(res));
    return res;
}

vector<Polygon_with_holes> to_polygon_vector(Polygon_set& pset) {
    vector<Polygon_with_holes> pols;
    pset.polygons_with_holes (back_inserter(pols));
    return pols;
}

bool is_inside_polygon_set(Polygon_set p1, Polygon_set& p2) {
    p1.difference(p2);
    return p1.is_empty();
}

bool is_equal(Polygon_set p1, Polygon_set& p2) {
    return is_inside_polygon_set(p1, p2) && is_inside_polygon_set(p2, p1);
}

Polygon_set get_union_of_polygon_sets(vector<Polygon_set> psets) {
    Polygon_set res;
    for(auto& pset : psets) {
        res.join(pset);
    }
    return res;
}

bool is_equal(Segment s1, Segment s2) {
    return s1.min() == s2.min() && s1.max() == s2.max();
}

// deterministic assuming the polygon does not have two points that are equal
Polygon sort_polygon(Polygon& polygon) {
    vector<Point> vertices = polygon.vertices();
    int mnidx = 0;
    for(int i = 0; i < vertices.size(); i++) {
        if(vertices[i] < vertices[mnidx]) {
            mnidx = i;
        }
    }
    rotate(vertices.begin(), vertices.begin() + mnidx, vertices.end());
    return Polygon(all(vertices));
}

vector<Polygon> sort_polygons(vector<Polygon> polygons) {
    fon(i, sz(polygons)) polygons[i] = sort_polygon(polygons[i]);
    vector<pair<vector<Point>,int>> arr;
    fon(i, sz(polygons)) {
        arr.push_back({polygons[i].vertices(), i});
    }
    sort(all(arr));
    vector<Polygon> res (sz(polygons));
    fon(i, sz(polygons)) res[i] = sort_polygon(polygons[arr[i].se]);
    return res;
}

string decompose_fraction(FT fraction) {
    stringstream ss; ss << fraction;
    return ss.str();
}

FT compose_fraction(string number) {
    if(number.find(".") != string::npos) {
        return FT(stod(number));
    } else {
        return FT (number);
    }
}

// always inside triangle
Point get_centroid(Polygon& pol) {
    Point centroid (0,0);
    for(auto& p : pol) centroid += Vector(Point(0,0), p) / pol.size();
    return centroid;
}

/*pair<double,double> decompose_fraction(FT fraction) {
    return pair<double,double>{fraction.numerator().to_double(), fraction.denominator().to_double()};
}*/

int get_segment_intersection_type(Segment& a, Segment& b) {
    // if(a == b) return 2;
    /*auto amin = a.min();
    auto amax = a.max();
    auto bmin = b.min();
    auto bmax = b.max();
    if(amin == bmin || amin == bmax || amax == bmin || amax == bmax) return 1;*/
    auto v = intersection(a,b);
    if (v) {
        if (const Point *p = boost::get<Point>(&*v)) {
            // side and e intersects at a point
            return 1;
        } else {
            return 2;
        }
    }
    return 0;
}

// does segment overlap with an interval with a side
bool is_segment_on_a_side(vector<Segment>& sides, Segment& segment) {
    for(auto& side : sides) {
        if(get_segment_intersection_type(side, segment) == 2) {
            return true;
        }
    }
    return false;
}

pair<Point,Point> convert2pair(Segment seg) {
    pair<Point,Point> res = {seg.min(), seg.max()};
    return res;
}

Polygon get_polygon_for_point(Point p) {
    Polygon res;
    res.push_back(p);
    return res;
}
