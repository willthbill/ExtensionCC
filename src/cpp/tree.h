#include <bits/stdc++.h>

#include <CGAL/Range_segment_tree_traits.h>
#include <CGAL/Range_tree_k.h>
#include <CGAL/Range_segment_tree_traits.h>
#include <CGAL/Segment_tree_k.h>

#include "cgal.h"

typedef CGAL::Range_segment_tree_set_traits_2<K> Traits_range;
typedef CGAL::Range_tree_2<Traits_range> Range_tree_2_type;
typedef Traits_range::Interval Interval_range;
typedef Traits_range::Key Key_range;

class SegmentQueryUtil {

private:

    Range_tree_2_type* pointset_structure = NULL;

public:

    SegmentQueryUtil();

    void init_pointset(std::vector<Point> points); 

    std::vector<Point> query_pointset(Point bottomleft, Point upperright);

    void delete_datastructures();

};
