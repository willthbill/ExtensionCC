#include <bits/stdc++.h>

#include "cgal.h"

class PartitionConstructor {

    Polygon_with_holes polygon;
    std::vector<Segment> segments;

public:
    PartitionConstructor (Polygon_with_holes _polygon);

    void add_visibility_polygon_boundary_segments();

    void add_extension_segments();

    void add_all_vispairs_segments();

    std::vector<Segment> get_extension_segments();

    void add_extension_corner_changes_segments();

    void add_extension_concave_triangles_segments();

    void add_extension_avgbound_sampling_segments(int);

    void add_extension_avgbound_throw_away_segments(int);

    void add_extension_constant_throw_away_segments(int);

    void add_extension_division_subsegments();

    void add_segments_from_polygons(std::vector<Polygon>& polygons);

    void add_grid_segments(int size);

    std::vector<Polygon> get_constrained_delaunay_triangulation(); 

    std::vector<Polygon> get_segment_polygons(); 

    int get_number_of_segments();
};
