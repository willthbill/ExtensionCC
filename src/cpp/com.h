#pragma once

#include<bits/stdc++.h>
#include "cgal.h"

typedef std::string OUTPUT_TYPE;

Polygon points2polygon(std::vector<std::pair<int,int>> points);

Polygon points2polygon(std::vector<std::pair<std::string,std::string>> points);

std::vector<std::vector<std::pair<OUTPUT_TYPE,OUTPUT_TYPE>>> cgal2pysolution(std::vector<Polygon>& tmp);

Polygon_with_holes pointsandholes2polygon(std::vector<std::pair<int,int>> points, std::vector<std::vector<std::pair<int,int>>> holes_points);

Polygon_with_holes pointsandholes2polygon(std::vector<std::pair<std::string,std::string>> points, std::vector<std::vector<std::pair<std::string,std::string>>> holes_points);

std::vector<Polygon> cover2polygonlist(std::vector<std::vector<std::pair<std::string,std::string>>> input_cover);

