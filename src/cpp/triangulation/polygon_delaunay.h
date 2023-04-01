#pragma once

#include <bits/stdc++.h>

#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_face_base_with_info_2.h>

#include "../cgal.h"

struct FaceInfo2
{
  FaceInfo2(){}
  int nesting_level;
  bool in_domain(){
    return nesting_level%2 == 1;
  }
};

typedef CGAL::Triangulation_vertex_base_2<K>                      Vb;
typedef CGAL::Triangulation_face_base_with_info_2<FaceInfo2,K>    Fbb;
typedef CGAL::Constrained_triangulation_face_base_2<K,Fbb>        Fb;
typedef CGAL::Triangulation_data_structure_2<Vb,Fb>               TDS;
typedef CGAL::Exact_predicates_tag                                Itag;
typedef CGAL::Constrained_Delaunay_triangulation_2<K, TDS, Itag>  CDT;
typedef CDT::Face_handle                                          Face_handle;

std::pair<CDT*,std::vector<Face_handle>> triangulate_polygon_delaunay_ds(Polygon_with_holes polygon);
std::vector<Polygon> triangulate_polygon_delaunay_cgal(Polygon_with_holes);
