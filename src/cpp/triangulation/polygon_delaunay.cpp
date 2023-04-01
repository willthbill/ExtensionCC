#include<bits/stdc++.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_face_base_with_info_2.h>
#include "../cgal.h"
#include "../geometry_utils.h"
#include "../common.h"
#include "../com.h"
#include "polygon_delaunay.h"

// from https://doc.cgal.org/latest/Triangulation_2/index.html#Section_2D_Triangulations_Constrained_Delaunay

void mark_domains(
    CDT& ct,
    Face_handle start,
    int index,
    std::list<CDT::Edge>& border
) {
    if(start->info().nesting_level != -1){
        return;
    }
    std::list<Face_handle> queue;
    queue.push_back(start);
    while(! queue.empty()){
        Face_handle fh = queue.front();
        queue.pop_front();
        if(fh->info().nesting_level == -1){
            fh->info().nesting_level = index;
            for(int i = 0; i < 3; i++){
                CDT::Edge e(fh,i);
                Face_handle n = fh->neighbor(i);
                if(n->info().nesting_level == -1){
                    if(ct.is_constrained(e)) border.push_back(e);
                    else queue.push_back(n);
                }
            }
        }
    }
}
//explore set of facets connected with non constrained edges,
//and attribute to each such set a nesting level.
//We start from facets incident to the infinite vertex, with a nesting
//level of 0. Then we recursively consider the non-explored facets incident
//to constrained edges bounding the former set and increase the nesting level by 1.
//Facets in the domain are those with an odd nesting level.
void mark_domains(CDT& cdt) {
    for(CDT::Face_handle f : cdt.all_face_handles()){
        f->info().nesting_level = -1;
    }
    std::list<CDT::Edge> border;
    mark_domains(cdt, cdt.infinite_face(), 0, border);
    while(! border.empty()) {
        CDT::Edge e = border.front();
        border.pop_front();
        Face_handle n = e.first->neighbor(e.second);
        if(n->info().nesting_level == -1){
            mark_domains(cdt, n, e.first->info().nesting_level+1, border);
        }
    }
}

using namespace std;

pair<CDT*,vector<pair<Polygon,Face_handle>>> _triangulate_polygon_delaunay(Polygon_with_holes polygon) {
    // cout << "COMPUTING STANDARD DELAUNAY TRIANGULATION" << endl;
    Polygon boundary = polygon.outer_boundary();
    vector<Polygon> holes;
    for(auto& hole : polygon.holes()) holes.push_back(hole);
    //Insert the polygons into a constrained triangulation
    CDT* cdt = new CDT();
    cdt->insert_constraint(boundary.vertices_begin(), boundary.vertices_end(), true);
    for(auto& hole : holes) cdt->insert_constraint(hole.vertices_begin(), hole.vertices_end(), true);
    //Mark facets that are inside the domain bounded by the polygon
    mark_domains(*cdt);
    map<vector<Point>,Face_handle> mp;
    vector<Polygon> polygons;
    for (Face_handle f : cdt->finite_face_handles())
    {
        if (f->info().in_domain()) {
            auto& p1 = f->vertex(0)->point();
            auto& p2 = f->vertex(1)->point();
            auto& p3 = f->vertex(2)->point();
            Polygon p;
            p.push_back(p1);
            p.push_back(p2);
            p.push_back(p3);
            auto sorted = sort_polygon(p);
            polygons.push_back(sorted);
            mp[sorted.vertices()] = f;
        }
    }
    vector<pair<Polygon,Face_handle>> res;
    polygons = sort_polygons(polygons);
    foe(pol, polygons) {
        res.push_back({pol, mp[pol.vertices()]});
    }
    return {cdt, res};
}

pair<CDT*,vector<Face_handle>> triangulate_polygon_delaunay_ds(Polygon_with_holes polygon) {
    auto [cdt, vec] = _triangulate_polygon_delaunay(polygon);
    vector<Face_handle> faces;
    foe(e, vec) faces.push_back(e.se);
    return {cdt, faces};
}

vector<Polygon> triangulate_polygon_delaunay_cgal(Polygon_with_holes polygon) {
    auto [cdt, vec] = _triangulate_polygon_delaunay(polygon);
    delete cdt;
    vector<Polygon> polygons;
    foe(e, vec) polygons.push_back(e.fi);
    return polygons;
}

/*pair<CDT*,vector<Face_handle>> triangulate_polygon_delaunay_ds(Polygon_with_holes polygon) {
    Polygon boundary = polygon.outer_boundary();
    vector<Polygon> holes;
    for(auto& hole : polygon.holes()) holes.push_back(hole);
    CDT* cdt = new CDT();
    cdt->insert_constraint(boundary.vertices_begin(), boundary.vertices_end(), true);
    for(auto& hole : holes) cdt->insert_constraint(hole.vertices_begin(), hole.vertices_end(), true);
    mark_domains(*cdt);
    vector<Face_handle> res;
    for (Face_handle f : cdt->finite_face_handles()) {
        if (f->info().in_domain()) {
            res.push_back(f);
        }
    }
    return {cdt, res};
}

vector<Polygon> triangulate_polygon_delaunay_cgal(Polygon_with_holes polygon) {
    // cout << "COMPUTING STANDARD DELAUNAY TRIANGULATION" << endl;
    Polygon boundary = polygon.outer_boundary();
    vector<Polygon> holes;
    for(auto& hole : polygon.holes()) holes.push_back(hole);
    //Insert the polygons into a constrained triangulation
    CDT cdt;
    cdt.insert_constraint(boundary.vertices_begin(), boundary.vertices_end(), true);
    for(auto& hole : holes) cdt.insert_constraint(hole.vertices_begin(), hole.vertices_end(), true);
    //Mark facets that are inside the domain bounded by the polygon
    mark_domains(cdt);
    vector<Polygon> res;
    for (Face_handle f : cdt.finite_face_handles())
    {
        if (f->info().in_domain()) {
            auto& p1 = f->vertex(0)->point();
            auto& p2 = f->vertex(1)->point();
            auto& p3 = f->vertex(2)->point();
            Polygon p;
            p.push_back(p1);
            p.push_back(p2);
            p.push_back(p3);
            res.push_back(p);
        }
    }
    return res;
}*/

vector<vector<pair<OUTPUT_TYPE,OUTPUT_TYPE>>> triangulate_polygon_delaunay(vector<pair<int,int>> boundary_points, vector<vector<pair<int,int>>> holes_points) {
    auto polygon = pointsandholes2polygon(boundary_points, holes_points);
    vector<Polygon> tmp = triangulate_polygon_delaunay_cgal(polygon);
    return cgal2pysolution(tmp);
}
