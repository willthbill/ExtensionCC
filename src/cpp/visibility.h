#pragma once

#include <CGAL/Triangular_expansion_visibility_2.h>
#include <CGAL/Arr_segment_traits_2.h>
#include <CGAL/Arrangement_2.h>
#include "cgal.h"
#include "intersection_predicates.h"

typedef CGAL::Arr_segment_traits_2<K>                           Traits_2;
typedef CGAL::Arrangement_2<Traits_2>                           Arrangement_2;
typedef Arrangement_2::Halfedge_const_handle                    Halfedge_const_handle;
typedef Arrangement_2::Face_handle                              Face_handle;
typedef Arrangement_2::Face_const_handle                        Face_const_handle;
typedef CGAL::Triangular_expansion_visibility_2<Arrangement_2>  TEV;

class VisibilityDataStructure {
private:

    Halfedge_const_handle* get_he_of_point(Point p);

public:

    TEV* tev;
    Polygon_with_holes pol;
    Face_const_handle face_pol;
    std::map<Point,std::vector<Halfedge_const_handle>> p2he;
    Arrangement_2 env;
    IntersectionPredicates pred;

    VisibilityDataStructure();
    // VisibilityDataStructure(const VisibilityDataStructure& other);
    VisibilityDataStructure(Polygon_with_holes& pol); 

    Polygon query(Point p); 

    void delete_datastructures();
};
