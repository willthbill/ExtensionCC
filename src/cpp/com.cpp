#include <CGAL/number_utils.h>
#include "cgal.h"
#include"com.h"
#include"debug.h"
#include"geometry_utils.h"

using namespace std;

Polygon points2polygon(vector<pair<int,int>> points) {
    Polygon pol;
    for(auto& p : points) pol.push_back(Point(p.first, p.second));
    return pol;
}

Polygon points2polygon(vector<pair<string,string>> points) {
    Polygon pol;
    for(auto& p : points) pol.push_back(Point(
        compose_fraction(p.first),
        compose_fraction(p.second)
    ));
    return pol;
}

// typedef FT::Numerator_type Numerator_type;
// typedef FT::Denominator_type Denominator_type;

vector<vector<pair<OUTPUT_TYPE,OUTPUT_TYPE>>> cgal2pysolution(vector<Polygon>& tmp) {
    vector<vector<pair<OUTPUT_TYPE,OUTPUT_TYPE>>> res;
    for(int i = 0; i < tmp.size(); i++) {
        res.push_back({});
        for(auto& p : tmp[i]) {
            res[i].push_back({
                decompose_fraction(p.x()),
                decompose_fraction(p.y())
            });
        }
    }
    return res;
}

Polygon_with_holes pointsandholes2polygon(vector<pair<int,int>> points, vector<vector<pair<int,int>>> holes_points) {
    Polygon outer_polygon = points2polygon(points);
    vector<Polygon> holes;
    for(auto& hole : holes_points) holes.push_back(points2polygon(hole));
    Polygon_with_holes res(outer_polygon, holes.begin(), holes.end());
    return res;
}


Polygon_with_holes pointsandholes2polygon(vector<pair<string,string>> points, vector<vector<pair<string,string>>> holes_points) {
    Polygon outer_polygon = points2polygon(points);
    vector<Polygon> holes;
    for(auto& hole : holes_points) holes.push_back(points2polygon(hole));
    Polygon_with_holes res(outer_polygon, holes.begin(), holes.end());
    return res;
}

vector<Polygon> cover2polygonlist(vector<vector<pair<string,string>>> input_cover) {
    vector<Polygon> cover;
    for(auto& pol : input_cover) cover.push_back(points2polygon(pol));
    return cover;
}
