#include<bits/stdc++.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_face_base_with_info_2.h>

#include "../cgal.h"
#include "polygon_delaunay.h"
#include "../common.h"
#include "../debug.h"
#include "../geometry_utils.h"

using namespace std;

Polygon face2polygon(Face_handle& fh) {
    auto& p1 = fh->vertex(0)->point();
    auto& p2 = fh->vertex(1)->point();
    auto& p3 = fh->vertex(2)->point();
    Polygon triangle; triangle.push_back(p1), triangle.push_back(p2), triangle.push_back(p3);
    return triangle;
}

map<int,int> get_mapping(vector<Polygon>& containing_triangulation, vector<Polygon>& contained_triangulation, CDT* cdt) {
    auto get_triangle_key = [&](Polygon triangle) -> vector<Point> {
        vector<Point> points = triangle.vertices();
        sort(all(points));
        return points;
    };
    map<vector<Point>,int> containing_mp;
    for(int i = 0; i < containing_triangulation.size(); i++) {
        auto& triangle = containing_triangulation[i];
        containing_mp[get_triangle_key(triangle)] = i;
    }
    map<int,int> res;
    for(int i = 0; i < contained_triangulation.size(); i++) {
        // cout << "mapping " << i << "th triangle out of " << contained_triangulation.size() << endl;
        auto& triangle = contained_triangulation[i];
        Point centroid = get_centroid(triangle);
        Face_handle fh = cdt->locate(centroid);
        auto key = get_triangle_key(face2polygon(fh));
        /*if(containing_mp.count(key) == 0 && check_valid) {
            res[i] = -1;
            continue;
        }*/
        res[i] = containing_mp[key];
    }
    return res;
}

vector<Polygon> convert_faces_to_triangles(vector<Face_handle> faces) {
    vector<Polygon> res;
    foe(fh, faces) res.push_back(face2polygon(fh));
    return res;
}

// terrible design and naming............
map<int,int> get_mapping_to_delaunay_triangulation(Polygon_with_holes& polygon, vector<Polygon>& contained_triangulation) {
    // cout << "COMPUTING STANDARD DELAUNAY TRIANGULATION" << endl;
    auto [cdt, faces] = triangulate_polygon_delaunay_ds(polygon);
    vector<Polygon> containing_triangulation = convert_faces_to_triangles(faces);
    // cout << "CREATING MAPPING" << endl;
    auto mapping = get_mapping(containing_triangulation, contained_triangulation, cdt);
    delete cdt;
    return mapping;
}

// there cannot be any constraints within the holes!!!
// it assummes that holes are triangulated the same way whether there are extra constraints or not
vector<Polygon> triangulate_polygon_delaunay_constrained_cgal(Polygon_with_holes polygon, vector<Segment> constraints) {
    Polygon boundary = polygon.outer_boundary();
    CDT cdt;
    cdt.insert_constraint(boundary.vertices_begin(), boundary.vertices_end(), true);
    for(auto& hole : polygon.holes()) {
        cdt.insert_constraint(hole.vertices_begin(), hole.vertices_end(), true);
    }
    vector<Polygon> res;
    for(auto& seg : constraints) {
        cdt.insert_constraint(seg.min(), seg.max());
        /*Polygon pol;
        pol.push_back(seg.min());
        pol.push_back(seg.max());
        res.push_back(pol);*/
    }
    // return res;

    auto [cdt_unconstrained, valid_faces_cgal] = triangulate_polygon_delaunay_ds(polygon);
    set<vector<Point>> valid_faces;
    auto get_face_key = [&](Face_handle& fh) -> vector<Point> {
        auto& p1 = fh->vertex(0)->point();
        auto& p2 = fh->vertex(1)->point();
        auto& p3 = fh->vertex(2)->point();
        vector<Point> points = {p1,p2,p3};
        sort(all(points));
        return points;
    };
    for (Face_handle f : valid_faces_cgal) {
        valid_faces.insert(get_face_key(f));
    }
    int cnt = 0;
    // cout << "COMPUTING CONSTRAINED TRIANGULATION" << cnt << endl;
    for (Face_handle f : cdt.finite_face_handles()) {
        cnt++;
        // if(cnt == 1 || cnt % 10000 == 5000) cout << "handling face " << cnt << endl;
        auto& p1 = f->vertex(0)->point();
        auto& p2 = f->vertex(1)->point();
        auto& p3 = f->vertex(2)->point();
        Polygon triangle; triangle.push_back(p1), triangle.push_back(p2), triangle.push_back(p3);
        Point centroid (0,0); // always inside triangle
        for(auto& p : triangle) centroid += Vector(Point(0,0), p) / 3.0;
        Face_handle fh = cdt_unconstrained->locate(centroid);
        if(valid_faces.count(get_face_key(fh))) {
            res.push_back(triangle);
        }
    }
    res = sort_polygons(res);
    delete cdt_unconstrained;
    return res;
}
