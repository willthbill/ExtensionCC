#include <bits/stdc++.h>
#include <CGAL/number_utils.h>

#include "gridsolver.h"
#include "cgal.h"
#include "dualgraph.h"
#include "geometry_utils.h"
#include "intersection_predicates.h"

#include "common.h"
#include "debug.h"

using namespace std;

typedef pair<int,int> pi;
typedef pair<pi,pi> ppi;

pi ZERO = {0,0};

pi operator +(pi a, pi b) {
    return {a.fi + b.fi, a.se + b.se};
}

pi operator -(pi a, pi b) {
    return {a.fi - b.fi, a.se - b.se};
}

pi operator *(pi a, int c) {
    return {a.fi * c, a.se * c};
}

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

pi sgn2(pi a) {
    return {sgn(a.fi), sgn(a.se)};
}

vector<pi> dir_order = {
    {1,0},
    {0,1},
    {-1,0},
    {0,-1}
};
pi rotate_left_dir(pi dir) {
    for(int i = 0; i < 4; i++) {
        if(dir_order[i] == dir) {
            return dir_order[(i+1)%4]; // replace with &3
        }
    }
    assert(false);
}
pi rotate_right_dir(pi dir) {
    for(int i = 0; i < 4; i++) {
        if(dir_order[i] == dir) {
            return dir_order[(i-1+4)%4]; // replace with &3
        }
    }
    assert(false);
}

const int OUTSIDE = 0;
const int NOTCOVERED = 1;
const int COVERED = 2;

Compress::Compress(vector<FT> vals) {
    set<FT> s;
    foe(e, vals) s.insert(e);
    int idx = 10;
    foe(e, s) {
        mp[e] = idx;
        revmp[idx] = e;
        idx++;
    }
    mx = idx - 1;
}

Compress::Compress(){}

FT Compress::rev(int val) {
    return revmp[val];
}
int Compress::get(FT val) {
    return mp[val];
}
int Compress::get_mx() {
    return mx + 10;
}
Point Compress::compress_point(Point p) {
    int x = get(p.x());
    int y = get(p.y());
    return Point(x,y);
}
Point Compress::rev_compress_point(Point p) {
    return Point(rev(p.x().to_double()), rev(p.y().to_double()));
}
Polygon Compress::compress_polygon(Polygon pol) {
    Polygon res;
    foe(p, pol) res.push_back(compress_point(p));
    return res;
}
Polygon Compress::rev_compress_polygon(Polygon pol) {
    Polygon res;
    foe(p, pol) res.push_back(rev_compress_point(p));
    return res;
}

Polygon_with_holes Compress::compress_polygon_with_holes(Polygon_with_holes pol) {
    vector<Polygon> holes;
    foe(hole, pol.holes()) holes.push_back(compress_polygon(hole));
    return Polygon_with_holes(compress_polygon(pol.outer_boundary()), all(holes));
}

GridSolver::GridSolver(Polygon_with_holes pol) : polygon(pol) {
    vector<FT> vals;
    {
        auto points = get_points(polygon);
        foe(p, points) {
            vals.push_back(p.x());
            vals.push_back(p.y());
        }
    }
    compress = Compress(vals);
    n = compress.get_mx();
}

// vector<Segment> allsegments;

void GridSolver::init_grid() {
    assert(n <= 10000);
    vector<Polygon> polygons;
    fon(i, n) {
        fon(j, n) {
            Polygon pol;
            pol.push_back(Point(i,j));
            pol.push_back(Point(i+1,j));
            pol.push_back(Point(i+1,j+1));
            pol.push_back(Point(i,j+1));
            polygons.push_back(pol);
        }
    }
    dualgraph = DualGraph(polygons);

    // must be parallel to an axis
    auto divided_into_unit_segments = [&](Segment seg) {
        pi _dir = {
            (seg.max().x() - seg.min().x()).to_double() > 0 ? 1 : 0,
            (seg.max().y() - seg.min().y()).to_double() > 0 ? 1 : 0
        };
        assert(_dir.fi + _dir.se == 1);
        Vector dir (_dir.fi, _dir.se);
        int len = max(
            (seg.max().x() - seg.min().x()).to_double(),
            (seg.max().y() - seg.min().y()).to_double()
        );
        assert(len * len == seg.squared_length());
        vector<Segment> res;
        for(int i = 0; i < len; i++) {
            auto p1 = seg.min() + dir * i;
            auto p2 = seg.min() + dir * (i + 1);
            res.push_back(Segment(p1,p2));
        }
        return res;
    };
    vector<int> candiate_nodes;
    auto remove_edges = [&](vector<Point> points) {
        for(int i = 0; i < points.size(); i++) {
            auto p1 = compress.compress_point(points[i]);
            auto p2 = compress.compress_point(points[(i+1)%points.size()]);
            vector<Segment> segments = divided_into_unit_segments(Segment(p1,p2));
            foe(seg, segments) {
                assert(seg.squared_length() == 1);
                // allsegments.push_back(seg);
                auto nodepair = dualgraph.get_nodes_sharing_edge(seg);
                candiate_nodes = {nodepair.fi, nodepair.se};
                dualgraph.remove_edge(nodepair.fi, nodepair.se);
            }
        }
    };
    remove_edges(polygon.outer_boundary().vertices());
    foe(hole, polygon.holes()) {
        remove_edges(hole.vertices());
    }
    dualgraph.get_nodes();
    Polygon_with_holes compressed_polygon = compress.compress_polygon_with_holes(polygon);
    IntersectionPredicates pred (compressed_polygon);
    int start = -1;
    foe(node, candiate_nodes) {
        Polygon pol = dualgraph.get_polygon(node);
        if(pred.is_completely_inside_slow(pol)) {
            start = node;
            break;
        }
    }
    assert(start != -1);
    vector<int> inside_nodes = dualgraph.get_component(start);
    auto get_square = [&](Polygon square) -> pi {
        FT mnx = 1e9, mny = 1e9;
        foe(p, square.vertices()) {
            mnx = min(mnx, p.x());
            mny = min(mny, p.y());
        }
        return {(int)mnx.to_double(), (int)mny.to_double()}; // good right?
    };
    grid = vector<vector<int>> (n, vector<int> (n, OUTSIDE));
    nodesingrid = vector<vector<int>> (n, vector<int> (n, -1));
    foe(node, inside_nodes) {
        pair<int,int> square = get_square(dualgraph.get_polygon(node));
        grid[square.fi][square.se] = NOTCOVERED;
        nodesingrid[square.fi][square.se] = node;
    }
}

vector<Polygon> GridSolver::get_convex_cover() {
    init_grid();
    /*{
        vector<Polygon> polygons;
        foe(seg, allsegments) {
            Polygon p; p.push_back(seg.min()); p.push_back(seg.max());
            polygons.push_back(p);
        }
        return polygons;
    }*/
    int n = grid.size();
    /*vector<int> nodes;
    fon(i, n) fon(j, n) if(grid[i][j] == 1) nodes.push_back(nodesingrid[i][j]);
    return dualgraph.get_polygons(nodes);*/
    auto get_furthest_uncovered = [&](pi pos, pi dir) -> pi {
        assert(grid[pos.fi][pos.se] == NOTCOVERED);
        pi res;
        while(grid[pos.fi][pos.se] != OUTSIDE) {
            if(grid[pos.fi][pos.se] == NOTCOVERED) {
                res = pos;
            }
            pos = pos + dir;
        }
        return res;
    };
    vector<pair<int,int>> dirs = {
        {0,1},
        {1,0},
        {0,-1},
        {-1,0}
    };
    auto n_outside_nbs = [&](pi pos) -> int {
        int res = 0;
        foe(dir, dirs) {
            pi nb = pos + dir;
            if(grid[nb.fi][nb.se] == OUTSIDE) res++;
        }
        return res;
    };
    auto n_covered_nbs = [&](pi pos) -> int {
        int res = 0;
        foe(dir, dirs) {
            pi nb = pos + dir;
            if(grid[nb.fi][nb.se] == COVERED) res++;
        }
        return res;
    };
    auto n_not_not_covered_nbs = [&](pi pos) -> int {
        int res = 0;
        foe(dir, dirs) {
            pi nb = pos + dir;
            if(grid[nb.fi][nb.se] != NOTCOVERED) res++;
        }
        return res;
    };
    /*auto get_score = [&](pi pos) -> int {
        return n_outside_nbs(pos) * 10 + n_covered_nbs(pos);
    };*/
    /*auto comp_score = [&](ppi a, ppi b) {
        if(a.fi.fi > b.fi.fi)
    };*/

    auto must_have_own_polygon = [&](pi pos) {
        if(n_outside_nbs(pos) >= 3) return true;
        map<pi,int> status;
        foe(dir, dirs) {
            auto nb = pos + dir;
            status[dir] = grid[nb.fi][nb.se];
        }
        if(status[{0,1}] == OUTSIDE && status[{0,-1}] == OUTSIDE) return true;
        if(status[{1,0}] == OUTSIDE && status[{-1,0}] == OUTSIDE) return true;
        return false;
    };
    auto is_good_pair = [&](pi p1, pi p2) {
        if(must_have_own_polygon(p1) || must_have_own_polygon(p2)) return true;
        if(n_outside_nbs(p1) >= 2 && n_outside_nbs(p2) >= 2) return true;
        return false;
    };
    auto can_be_taken = [&](pi pos) {
        if(n_not_not_covered_nbs(pos) >= 3) return true;
        map<pi,int> status;
        foe(dir, dirs) {
            auto nb = pos + dir;
            status[dir] = grid[nb.fi][nb.se];
        }
        if(status[{0,1}] != NOTCOVERED && status[{0,-1}] != NOTCOVERED) return true;
        if(status[{1,0}] != NOTCOVERED && status[{-1,0}] != NOTCOVERED) return true;
        return false;
    };
    auto are_two_corners_on_a_side = [&](pi p1, pi p2) {
        assert(p1 != p2);
        pi dir = sgn2(p2 - p1);

        int confidence = 0;
        pi behind = p1 + rotate_left_dir(rotate_left_dir(dir));
        pi infront = p2 + dir;
        if(grid[behind.fi][behind.se] == NOTCOVERED ||
           grid[infront.fi][infront.se] == NOTCOVERED) return 0;
        confidence += (grid[behind.fi][behind.se] == OUTSIDE);
        confidence += (grid[infront.fi][infront.se] == OUTSIDE);

        int cnt_left = 0;
        int cnt_right = 0;
        int cnt = 0;
        int confidence_left = 0;
        int confidence_right = 0;
        for(pi p = p1; p != p2 + dir; p = p + dir) {
            cnt++;
            pi left = p + rotate_left_dir(dir);
            if(grid[left.fi][left.se] != NOTCOVERED) {
                if(grid[left.fi][left.se] == OUTSIDE) confidence_left++;
                cnt_left++;
            }
            pi right = p + rotate_right_dir(dir);
            if(grid[right.fi][right.se] != NOTCOVERED) {
                if(grid[right.fi][right.se] == OUTSIDE) confidence_right++;
                cnt_right++;
            }
        }

        if(cnt == cnt_left) {
            return confidence_left + confidence;
        } else if(cnt == cnt_right) {
            return confidence_right + confidence;
        } else return 0;
    };
    // maybe some are covered by others?
    auto get_next_extension_pair = [&]() -> ppi {
        int best_score = -1; 
        ppi best_pair = {{-1,-1},{-1,-1}};
        fon(i, n) fon(j, n) {
            if(grid[i][j] != NOTCOVERED) continue;
            pi pos = {i,j};
            foe(dir, dirs) {
                auto nb = get_furthest_uncovered(pos,dir);
                //int s1 = get_score(pos);
                //int s2 = get_score(nb);
                int score = -1;
                if(pos == nb) {
                    if(must_have_own_polygon(pos)) {
                        score = 100000000;
                    } else {
                        score = 1;
                    }
                } else {
                    if(must_have_own_polygon(pos) || must_have_own_polygon(nb)) {
                        score = 0; // this is really bad!
                    } else {
                        score = are_two_corners_on_a_side(pos, nb) + 1;
                    }
                }
                if(score > best_score) {
                    best_pair = {pos,nb};
                    best_score = score;
                }
            }
        }
        debug(best_score);
        return best_pair;
        assert(best_score > -1);
        return best_pair;
    };
    vector<ppi> bounding_boxes;
    auto extend = [&](ppi p) {
        auto p1 = p.fi;
        auto p2 = p.se;
        pi dir1 = {
            p1.fi == p2.fi,
            p1.se == p2.se,
        };
        if(p1 == p2) {
            bool found = false;
            foe(dir, dirs) {
                pi nb = p1 + dir;
                if(grid[nb.fi][nb.se] != OUTSIDE) {
                    // assert(!found);
                    found = true;
                    dir1 = sgn2(nb - p1);
                }
            }
            assert(found);
            /*bounding_boxes.push_back({p1,p1});
            grid[p1.fi][p1.se] = COVERED;
            return;*/
        }
        pi dir2 = dir1 * -1;
        vector<pi> relevant_points = {p1, p2};
        auto process_dir = [&](pi dir) {
            int len = -1;
            while(true) {
                len++;
                pi start = p1 + dir * len;
                pi end = p2 + dir * len;
                pi a2b = sgn2(end - start);
                if(a2b == ZERO) a2b = {1,1}; // or something else
                bool ok = true;
                for(pi p = start; p != end + a2b; p = p + a2b) {
                    if(grid[p.fi][p.se] == OUTSIDE) {
                        ok = false;
                        break;
                    }
                }
                if(ok) {
                    for(pi p = start; p != end + a2b; p = p + a2b) {
                        grid[p.fi][p.se] = COVERED;
                    }
                } else {
                    len--;
                    break;
                }
            }
            assert(len >= 0);
            relevant_points.push_back(p1 + dir * len);
            relevant_points.push_back(p2 + dir * len);
        };
        process_dir(dir1);
        process_dir(dir2);
        ppi bbox = {{1e9, 1e9},{-1e9,-1e9}};
        foe(p, relevant_points) {
            bbox.fi.fi = min(bbox.fi.fi, p.fi);
            bbox.fi.se = min(bbox.fi.se, p.se);
            bbox.se.fi = max(bbox.se.fi, p.fi);
            bbox.se.se = max(bbox.se.se, p.se);
        }
        bounding_boxes.push_back(bbox);
    };
    bool done = false;
    while(!done) {
        auto p = get_next_extension_pair();
        if(p.fi.fi == -1) break;
        extend(p);
        done = true;
        fon(i, n) fon(j, n) {
            if(grid[i][j] == NOTCOVERED) {
                done = false;
            }
        }
    }
    vector<Polygon> polygons;
    foe(bbox, bounding_boxes) {
        vector<Polygon> corners;
        corners.push_back(dualgraph.get_polygon(nodesingrid[bbox.fi.fi][bbox.fi.se]));
        corners.push_back(dualgraph.get_polygon(nodesingrid[bbox.fi.fi][bbox.se.se]));
        corners.push_back(dualgraph.get_polygon(nodesingrid[bbox.se.fi][bbox.fi.se]));
        corners.push_back(dualgraph.get_polygon(nodesingrid[bbox.se.fi][bbox.se.se]));
        polygons.push_back(compress.rev_compress_polygon(get_convex_hull_of_polygons(corners)));
    }
    return polygons;
}
