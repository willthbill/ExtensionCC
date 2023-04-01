#include "geometry_utils.h"
#include "partition_constructor.h"
#include "intersection_predicates.h"
#include "reduce.h"
#include "tree.h"

#include "debug.h"
#include "common.h"

using namespace std;

Reducer::Reducer(
    Polygon_with_holes _polygon,
    std::vector<Polygon> _cover
) : cover(_cover), polygon(_polygon) {}

std::vector<Polygon> Reducer::reduce() {
    int n_cover = cover.size();
    PartitionConstructor partition (polygon);
    partition.add_segments_from_polygons(cover);
    auto triangulation = partition.get_constrained_delaunay_triangulation();
    int n_tri = triangulation.size();
    vector<Point> centroids;
    map<Point,int> centroid2idx;
    fon(i, n_tri) {
        auto cent = get_centroid(triangulation[i]);
        centroids.push_back(cent);
        centroid2idx[cent] = i;
    }
    SegmentQueryUtil segutil;
    segutil.init_pointset(centroids);
    vector<set<int>> cov2tri (n_cover);
    vector<set<int>> tri2cov (n_tri);
    fon(i, n_cover) {
        // if(i % 1000 == 0) cout << "processing cover " << (i+1) << endl;
        auto& pol = cover[i];
        auto bbox = pol.bbox();
        Point bottomleft (bbox.xmin() - 1, bbox.ymin() - 1), upperright(bbox.xmax() + 1, bbox.ymax() + 1);
        auto triangles_in_bbox = segutil.query_pointset(bottomleft, upperright);
        IntersectionPredicates pred (pol, false); // TODO: optimize using intersection predicates! Maybe only do if it is large
        pred.setup_star_shaped(get_centroid(pol));
        foe(p, triangles_in_bbox) {
            //if(pol.has_on_bounded_side(p)) {
            if(pred.is_point_inside_star_shaped_polygon(p)) {
                //assert(!pol.has_on_unbounded_side(p));
                int tri_idx = centroid2idx[p];
                cov2tri[i].insert(tri_idx);
                tri2cov[tri_idx].insert(i);
            }
        }
        // pred.delete_datastructures();
    }
    fon(i, n_tri) assert(tri2cov[i].size() >= 1);

    vector<int> q;
    {
        vector<pair<pair<FT,FT>,int>> pols;
        fon(i, n_tri) {
            if(tri2cov[i].size() == 1) {
                int cov_src_idx = *tri2cov[i].begin();
                // pols.push_back({cover[cov_src_idx].size(), cov_src_idx});
                pols.push_back({{cover[cov_src_idx].size(), abs(cover[cov_src_idx].area())}, cov_src_idx});
            }
        }
        sort(rall(pols));
        foe(pol, pols) q.push_back(pol.se);
    }
    // assert(q.size() == 0);
    {
        vector<pair<pair<FT,FT>,int>> pols;
        // fon(i, n_cover) pols.push_back({abs(cover[i].area()), i});
        //sort(rall(pols));
        //fon(i, n_cover) pols.push_back({cover[i].size(), i});
        //sort(rall(pols));
        fon(i, n_cover) pols.push_back({{cover[i].size(), abs(cover[i].area())}, i});
        sort(rall(pols));
        foe(pol, pols) q.push_back(pol.se);
    }

    int removed = 0;
    vector<Polygon> solution;
    int cnt = 0;
    foe(cov_src_idx, q) {
        if(cov2tri[cov_src_idx].size() == 0) continue;
        // if(cnt % 1000 == 0) cout << "adding cover " << cov_src_idx << " to solution" << endl;
        cnt++;
        foe(tri_idx, cov2tri[cov_src_idx]) {
            tri2cov[tri_idx].erase(cov_src_idx);
            foe(cov_idx, tri2cov[tri_idx]) {
                cov2tri[cov_idx].erase(tri_idx);
                if(cov2tri[cov_idx].size() == 0) {
                    removed++;
                }
            }
            tri2cov[tri_idx] = {};
        }
        cov2tri[cov_src_idx] = {};
        solution.push_back(cover[cov_src_idx]);
    }
    // cout << "removed " << removed << " polygons from solution" << endl;
    segutil.delete_datastructures();
    return solution;
}

std::vector<Polygon> Reducer::reduce_repeated() {
    // cout << "original cover size: " << cover.size() << endl;
    rep(5) {
        int size = cover.size();
        cover = reduce();
        // cout << "found new cover size: " << cover.size() << endl;
        if(cover.size() == size) break;
    }
    // cout << "stopping since size did not change or at most 5 iterations" << endl;
    return cover;
}
