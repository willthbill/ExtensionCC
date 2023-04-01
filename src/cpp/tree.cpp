#include <bits/stdc++.h>

#include "cgal.h"
#include "tree.h"

#include "common.h"
#include "debug.h"

using namespace std;

SegmentQueryUtil::SegmentQueryUtil(){}

void SegmentQueryUtil::init_pointset(vector<Point> points) {
    vector<Key_range> input;
    for(auto& data : points) input.pb(Key_range(data));
    pointset_structure = new Range_tree_2_type(input.begin(), input.end()); // TODO: memory leaks
}

vector<Point> SegmentQueryUtil::query_pointset(Point bottomleft, Point upperright) {
    vector<Point> output; // faster with list?
    Interval_range win(bottomleft, upperright);
    pointset_structure->window_query(win, back_inserter(output));
    return output;
}

void SegmentQueryUtil::delete_datastructures() {
    if(pointset_structure != NULL) {
        delete pointset_structure;
    }
}
