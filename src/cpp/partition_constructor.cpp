#include <bits/stdc++.h>

#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/squared_distance_2.h>

#include "cgal.h"
#include "geometry_utils.h"
#include "visibility.h"
#include "partition_constructor.h"
#include "triangulation/triangulation.h"
#include "intersection_predicates.h"
#include "rng.h"

#include "debug.h"
#include "common.h"

using namespace std;

PartitionConstructor::PartitionConstructor(Polygon_with_holes _polygon) {
    polygon = _polygon;
}

void PartitionConstructor::add_visibility_polygon_boundary_segments() {
    // cout << "FINDING VISIBILITY POLYGONS" << endl;
    VisibilityDataStructure visds (polygon);
    int cnt = 0;
    auto points = get_points(polygon);
    foe(p, points) {
        cnt++;
        // if(cnt % 500 == 0) cout << "Finding visiblity polygon segments of the " << cnt << "th side" << endl;
        Polygon vispol = visds.query(p);
        for(auto seg : vispol.edges()) {
            segments.push_back(seg);
        }
    }
    // cout << "size of segments now: " << segments.size() << endl;
}

void PartitionConstructor::add_extension_concave_triangles_segments() {
    IntersectionPredicates pred (polygon);
    auto process_points = [&](Polygon pol) {
        int n = pol.size();
        assert(n >= 2);
        fon(i, n) {
            // if(i % 5000 == 0 || i == n - 1) cout << "processing triangle of point " << i << endl;
            int a = (i - 1 + n) % n;
            int b = i;
            int c = (i + 1) % n;
            int d = (i + 2) % n;
            Line l1 (pol[a], pol[b]);
            Line l2 (pol[d], pol[c]);
            auto v = CGAL::intersection(l1, l2);
            Point p;
            if(v) {
                if (const Point *intersection_point = boost::get<Point>(&*v)) {
                    p = *intersection_point;
                } else {
                    continue; // will almost never happen
                }
            } else {
                continue; // will almost never happen
            }
            // bool valid = (bool)v;// && pred.is_point_in_polygon_slow(p);
            assert(v);
            segments.push_back(Segment(pol[b], p));
            segments.push_back(Segment(pol[c], p));
        }
    };
    process_points(polygon.outer_boundary());
    foe(hole, polygon.holes()) {
        process_points(hole);
    }
}

void PartitionConstructor::add_extension_corner_changes_segments() {
    vector<Segment> extensions = get_extension_segments();
    map<Point,vector<Segment>> p2segs;
    foe(seg, extensions) {
        p2segs[seg.min()].push_back(seg);
        p2segs[seg.max()].push_back(seg);
    }
    int cnt = 0;
    int cnt_concave_sides = 0;
    int cnt_colinear_sides = 0;
    auto process_points = [&](Polygon pol) {
        if(!pol.is_counterclockwise_oriented()) {
            assert(!pol.is_collinear_oriented());
            pol.reverse_orientation();
        }
        int n = pol.size();
        vector<int> state (n);
        fon(i, n) {
            auto p1 = pol[(i-1+n) % n];
            auto p2 = pol[i];
            auto p3 = pol[(i+1) % n];
            if(CGAL::left_turn(p1,p2,p3)) {
                state[i] = 1;
            } else if(CGAL::collinear(p1,p2,p3)) {
                cnt_colinear_sides++;
                state[i] = 0;
            } else {
                cnt_concave_sides++;
                state[i] = -1;
            }
        }
        fon(i, n) {
            if(state[i] == state[(i-1+n)%n] && state[i] == state[(i+1+n)%n]) continue;
            cnt++;
            /*int idx = i + 1;
            while(state[idx%n] == 0) idx++; // skip collinear
            idx %= n;*/
            foe(seg, p2segs[pol[i]]) {
                segments.push_back(seg);
            }
        }
    };
    process_points(polygon.outer_boundary());
    foe(hole, polygon.holes()) {
        process_points(hole);
    }
    // cout << "number of points on corner changes: " << cnt << endl;
    // cout << "number of concave sides: " << cnt_concave_sides << endl;
    // cout << "number of collinear sides: " << cnt_colinear_sides << endl;
}

double get_avg_side_length(vector<pair<Point,Point>> sides) {
    double avg = 0;
    double total = sides.size();
    foe(side, sides) {
        Segment seg (side.fi, side.se);
        double len = sqrt(seg.squared_length().to_double());
        avg += len / total;
    }
    return avg;
}

// magic is a lowerbound. When above magic*avg segments are included with lower probability.
void PartitionConstructor::add_extension_avgbound_sampling_segments(int magic) {
    // cout << "using magic number: " << magic << endl;
    double avg = get_avg_side_length(get_sides(polygon));
    // cout << "avg sides length: " << avg << endl;
    double start = avg * ((double)magic);
    vector<Segment> extensions = get_extension_segments();
    double mxnum = 1000000000.0;
    RNG rng (0,(int)mxnum,42);
    foe(seg, extensions) {
        auto len = sqrt(seg.squared_length().to_double());
        assert(len > 0);
        double prop = start / len;
        // just doing things safely
        if(prop >= 1.0 || (double)(rng.get()) < mxnum * prop) {
            segments.push_back(seg);
        }
    }
}

void PartitionConstructor::add_extension_avgbound_throw_away_segments(int magic) {
    // cout << "using magic number: " << magic << endl;
    double avg = get_avg_side_length(get_sides(polygon));
    // cout << "avg sides length: " << avg << endl;
    int mxsize = ceil(avg * ((double)magic));
    add_extension_constant_throw_away_segments(mxsize);
}

void PartitionConstructor::add_extension_constant_throw_away_segments(int MAX_SIZE) {
    vector<Segment> extensions = get_extension_segments();
    // cout << "finding extension segments with size less than " << MAX_SIZE << endl;
    foe(seg, extensions) {
        auto len = sqrt(seg.squared_length().to_double());
        if(len > MAX_SIZE) continue;
        segments.push_back(seg);
    }
}

void PartitionConstructor::add_extension_division_subsegments() {
    vector<Segment> extensions = get_extension_segments();
    foe(seg, extensions) {
        const int AM = 0;
        const int SIZE = 100;
        auto len = sqrt(seg.squared_length().to_double());
        auto len_begin = len / SIZE;
        auto len_end = len / SIZE;
        auto left = len - len_begin - len_end;
        auto len_middle = left / (2 * AM + 1);
        Point zero (0,0);
        auto add_segment = [&](FT start, FT l) {
            auto st = Vector(zero, seg.source()) + Vector(seg) / len * start;
            auto en = Vector(zero, seg.source()) + Vector(seg) / len * start + Vector(seg) / len * l;
            segments.push_back(Segment(Point(st.x(), st.y()), Point(en.x(), en.y())));
        };
        add_segment(0, len_begin);
        for(int i = 1; i <= AM; i++) {
            add_segment(len_begin + i * len_middle + (i - 1) * len_middle, len_middle);
        }
        add_segment(len - len_end, len_end);
    }
}

void PartitionConstructor::add_extension_segments() {
    foe(seg, get_extension_segments()) {
        segments.pb(seg);
    }
    // cout << "size of segments now: " << segments.size() << endl;
}

void PartitionConstructor::add_all_vispairs_segments() {
    // cout << "FINDING VISPAIR SEGMENTS" << endl;
    VisibilityDataStructure visds (polygon);
    int cnt = 0;
    auto points = get_points(polygon);
    set<Point> pointset;
    foe(p, points) pointset.insert(p);
    RNG rng (1,100,42);
    foe(p, points) {
        cnt++;
        // if(cnt % 500 == 0) cout << "Finding vispair segments of the " << cnt << "th side" << endl;
        Polygon vispol = visds.query(p);
        foe(nb, vispol) {
            if(pointset.count(nb)) continue;
            if(rng.get() <= 10) segments.push_back(Segment(p, nb));
        }
    }
    // cout << "size of segments now: " << segments.size() << endl;
}

vector<Segment> PartitionConstructor::get_extension_segments() {
    // cout << "FINDING EXTENSION SEGMENTS" << endl;
    VisibilityDataStructure visds (polygon);
    auto _sides = get_sides(polygon);
    vector<pair<Point,Point>> sides;
    foe(side, _sides) {
        sides.push_back(side);
        sides.push_back({side.se, side.fi});
    }
    vector<Segment> segments;
    for(int i = 0; i < sides.size(); i++) {
        // if(i == 0 || i == sides.size() - 1 || i % 10000 == 5000) cout << "Finding extension segments of the " << i+1 << "th side" << endl;
        auto& p1 = sides[i].fi;
        auto& p2 = sides[i].se;
        Polygon vispol = visds.query(p1);
        int cnt = 0;
        Line line (p1, p2);
        map<Direction,Segment> finals;
        for(auto seg : vispol.edges()) {
            auto v = CGAL::intersection(line, seg);
            bool ok = 1;
            Segment ext_seg;
            Direction dir;
            if(v) {
                if (const Point *intersection_point = boost::get<Point>(&*v)) {
                    auto d1 = CGAL::squared_distance(*intersection_point, p1);
                    auto d2 = CGAL::squared_distance(*intersection_point, p2);
                    if(d1 == 0 || d2 == 0) {
                        ok = 0;
                    } else if(d1 < d2) {
                        cnt++;
                        ext_seg = Segment(p1, *intersection_point);
                        dir = Direction(Vector(p1, *intersection_point));
                    } else {
                        cnt++;
                        ext_seg = Segment(p2, *intersection_point);
                        dir = Direction(Vector(p2, *intersection_point));
                    }
                } else {
                    const Segment *intersection_segment = boost::get<Segment>(&*v);
                    auto mn = (*intersection_segment).min();
                    auto mx = (*intersection_segment).max();
                    auto d1 = squared_distance(p1, mn);
                    auto d2 = squared_distance(p2, mn);
                    auto d3 = squared_distance(p1, mx);
                    auto d4 = squared_distance(p2, mx);
                    if(d1 == 0 || d2 == 0 || d3 == 0 || d4 == 0) {
                        ok = 0;
                    } else if(d1 <= d2 && d1 <= d3 && d1 <= d4) {
                        cnt++;
                        ext_seg = Segment(p1, mn);
                        dir = Direction(Vector(p1, mn));
                    } else if(d2 <= d1 && d2 <= d3 && d2 <= d4) {
                        cnt++;
                        ext_seg = Segment(p2, mn);
                        dir = Direction(Vector(p2, mn));
                    } else if(d3 <= d1 && d3 <= d2 && d3 <= d4) {
                        cnt++;
                        ext_seg = Segment(p1, mx);
                        dir = Direction(Vector(p1, mx));
                    } else if(d4 <= d1 && d4 <= d2 && d4 <= d3) {
                        cnt++;
                        ext_seg = Segment(p2, mx);
                        dir = Direction(Vector(p2, mx));
                    } else {
                        assert(false);
                    }
                }
            } else {
                ok = 0;
            }
            if(ok) {
                if(ext_seg.squared_length() >= finals[dir].squared_length()) {
                    finals[dir] = ext_seg;
                }
            }
        }
        foe(e, finals) {
            segments.pb(e.se);
        }
    }
    // cout << "Total number of extension segments: " << segments.size() << endl;
    return segments;
}

void PartitionConstructor::add_segments_from_polygons(vector<Polygon>& polygons) {
    foe(pol, polygons) {
        auto ph = Polygon_with_holes(pol);
        auto sides = get_sides(ph);
        foe(side, sides) {
            segments.push_back(Segment(side.fi, side.se));
        }
    }
}

void PartitionConstructor::add_grid_segments(int size) {
    // cout << "gridlines: " << size << endl;
    auto bbox = polygon.bbox();
    {
        FT xrange = bbox.xmax() - bbox.xmin();
        FT stepsize = xrange / size;
        for(FT x = bbox.xmin(); x <= bbox.xmax(); x += stepsize) {
            segments.push_back(Segment(
                Point(x, bbox.ymin()),
                Point(x, bbox.ymax())
            ));
        }
    }
    {
        FT yrange = bbox.ymax() - bbox.ymin();
        FT stepsize = yrange / size;
        for(FT y = bbox.ymin(); y <= bbox.ymax(); y += stepsize) {
            segments.push_back(Segment(
                Point(bbox.xmin(), y),
                Point(bbox.xmax(), y)
            ));
        }
    }
}

vector<Polygon> PartitionConstructor::get_constrained_delaunay_triangulation() {
    // cout << "COMPUTING CONSTRAINED TRIANGULATION" << endl;
    // cout << "number of segments in constrained triangulation: " << segments.size() << endl;
    return triangulate_polygon_delaunay_constrained_cgal(polygon, segments);
}

vector<Polygon> PartitionConstructor::get_segment_polygons() {
    // cout << "COMPUTING SEGMENT POLYGONS" << endl;
    // cout << "number of segments: " << segments.size() << endl;
    vector<Polygon> res;
    foe(e, segments) {
        Polygon p;
        p.push_back(e.min());
        p.push_back(e.max());
        res.push_back(p);
    }
    return res;
}

int PartitionConstructor::get_number_of_segments() {
    return sz(segments);
}
