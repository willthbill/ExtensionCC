#include<bits/stdc++.h>
#include "visibility.h"

#include "geometry_utils.h"
#include "common.h"
#include "debug.h"

using namespace std;

Polygon get_boundary_as_polygon(Face_handle& fh) {
    Arrangement_2::Ccb_halfedge_circulator curr = fh->outer_ccb();
    vector<Point> points;
    points.pb(curr->source()->point());
    points.pb(curr->target()->point());
    while (++curr != fh->outer_ccb()) {
        points.pb(curr->target()->point());
    }
    points.pop_back();
    Polygon res (points.begin(), points.end());
    return res;
}

VisibilityDataStructure::VisibilityDataStructure() {}

/*VisibilityDataStructure::VisibilityDataStructure(const VisibilityDataStructure& other) {
}*/

VisibilityDataStructure::VisibilityDataStructure(Polygon_with_holes& _pol) {
    pol = _pol;
    pred = IntersectionPredicates(pol);
    // half-edges structure
    set<pair<Point,Point>> edges;
    for(auto edge : pol.outer_boundary().edges()) {
        edges.insert(pair<Point,Point>{edge.source(), edge.target()});
    }
    for(Polygon hole : pol.holes()) {
        for(auto edge : hole.edges()) {
            edges.insert(pair<Point,Point>{edge.source(), edge.target()});
        }
    }
    vector<Segment> edgearr;
    for(auto& p : edges) edgearr.pb(Segment(p.fi, p.se));
    CGAL::insert_non_intersecting_curves(env,edgearr.begin(), edgearr.end());
    for(Halfedge_const_handle halfedge : env.halfedge_handles()) {
        Point source = halfedge->source()->point();
        Point target = halfedge->target()->point();
        if(edges.count({source, target})) {
            p2he[target].pb(halfedge);
        }
    }
    // find face equal to pol
    bool found = false;
    for(auto face : env.face_handles()) {
        if(!face->is_unbounded() && get_boundary_as_polygon(face) == pol.outer_boundary()) {
            assert(found == false);
            found = true;
            face_pol = face;
        }
    }
    assert(found);
    tev = new TEV(env);
    // tev.attach(env);
}

Halfedge_const_handle* VisibilityDataStructure::get_he_of_point(Point p) {
    if(p2he.count(p)) {
        auto& he = p2he[p][0];
        return &he;
    }
    vector<Segment> segments = pred.get_segments_with_point_on(p);
    if(segments.size() == 0) return NULL;
    auto& segment = segments[0];
    assert(segment.has_on(p));
    foe(he, p2he[segment.min()]) {
        Segment seg (he->source()->point(), he->target()->point());
        if(is_equal(seg, segment)) {
            assert(seg.has_on(p));
            return &he;
        }
    }
    foe(he, p2he[segment.max()]) {
        Segment seg (he->source()->point(), he->target()->point());
        if(is_equal(seg, segment)) {
            assert(seg.has_on(p));
            return &he;
        }
    }
    assert(false);
}

// p must be in the polygon
Polygon VisibilityDataStructure::query(Point p) {
    Arrangement_2 output_arr;
    Face_handle fh;
    Halfedge_const_handle* he = get_he_of_point(p);
    if(he == NULL) {
        fh = tev->compute_visibility(p, face_pol, output_arr);
    } else {
        fh = tev->compute_visibility(p, *he, output_arr);
    }
    return get_boundary_as_polygon(fh);
}

void VisibilityDataStructure::delete_datastructures() {
    delete tev;
    pred.delete_datastructures();
}
