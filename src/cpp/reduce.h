#pragma once

#include<bits/stdc++.h>
#include "cgal.h"

class Reducer {
private:
    std::vector<Polygon> cover;
    Polygon_with_holes polygon;
public:
    Reducer(Polygon_with_holes _polygon, std::vector<Polygon> _cover);
    std::vector<Polygon> reduce();
    std::vector<Polygon> reduce_repeated();
};
