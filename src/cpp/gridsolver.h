#include <bits/stdc++.h>

#include "cgal.h"
#include "dualgraph.h"

class Compress {
    std::map<FT,int> mp;
    std::map<int,FT> revmp;
    int mx;
public:
    Compress();
    Compress(std::vector<FT> vals); 
    FT rev(int val); 
    int get(FT val); 
    int get_mx(); 
    Point compress_point(Point p);
    Point rev_compress_point(Point p);
    Polygon compress_polygon(Polygon p);
    Polygon rev_compress_polygon(Polygon p);
    Polygon_with_holes compress_polygon_with_holes(Polygon_with_holes p);
};

class GridSolver {

private:

    int n;
    Polygon_with_holes polygon;
    std::vector<std::vector<int>> grid;
    std::vector<std::vector<int>> nodesingrid;
    DualGraph dualgraph;
    Compress compress;

public:

    GridSolver(Polygon_with_holes pol);

    void init_grid(); 

    std::vector<Polygon> get_convex_cover(); 

};
