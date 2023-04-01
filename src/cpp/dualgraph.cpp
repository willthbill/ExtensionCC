#include<bits/stdc++.h>
#include"cgal.h"
#include<CGAL/Boolean_set_operations_2.h>
#include"dualgraph.h"
#include"common.h"
#include"geometry_utils.h"
#include"debug.h"

using namespace std;

#define fi first
#define se second 

DualGraph::DualGraph(){}

// no multiple edges, no selfloops, undirected
DualGraph::DualGraph(vector<Polygon> polygons) {
    for(int i = 0; i < polygons.size(); i++) {
        node2pol[i] = polygons[i];
    }
    vector<pair<int,int>> edges;
    for(int node = 0; node < polygons.size(); node++) {
        for(const Segment& e : polygons[node].edges()){
            pair<Point, Point> key = {e.min(), e.max()};
            for(int& nb : edgemap[key]) {
                edges.emplace_back(node, nb);
            }
            edgemap[key].push_back(node);
        }
    }
    for(auto& e : edges) {
        add_edge(e.fi, e.se);
    }
    next_node_id = polygons.size();
}

int DualGraph::get_mx_node_id() {
    return next_node_id - 1;
}

int DualGraph::new_node() {
    int node = next_node_id++;
    return node;
}

void DualGraph::add_edge(int a, int b) {
    adj[a].insert(b);
    adj[b].insert(a);
}

void DualGraph::remove_edge(int a, int b) {
    if(adj[a].count(b)) adj[a].erase(b);
    if(adj[b].count(a)) adj[b].erase(a);
}

int DualGraph::merge_nodes(int a, int b) {
    assert(check_union_all(a,b));
    int node = new_node();
    auto edges = adj[a];
    for(auto& nb : edges) {
        add_edge(node, nb);
        remove_edge(a, nb);
    }
    edges = adj[b];
    for(auto& nb : edges) {
        add_edge(node, nb);
        remove_edge(b, nb);
    }
    Polygon_with_holes p;
    CGAL::join(node2pol[a], node2pol[b], p);
    node2pol[node] = p.outer_boundary();
    remove_edge(node, a);
    remove_edge(node, b);
    remove_edge(node, node);
    adj.erase(a);
    adj.erase(b);
    node2pol.erase(a);
    node2pol.erase(b);
    assert(!does_node_exist(a) && !does_node_exist(b));
    return node;
}

array<bool,4> DualGraph::check_union(int a, int b) {
    assert(does_node_exist(a) && does_node_exist(b));
    Polygon_with_holes p;
    bool intersects = CGAL::join (node2pol[a], node2pol[b], p);
    bool has_no_holes = !p.has_holes();
    bool is_convex = p.outer_boundary().is_convex();
    bool is_simple = p.outer_boundary().is_simple();
    return {intersects, has_no_holes, is_convex, is_simple};
}

bool DualGraph::check_union_all(int a, int b) {
    auto t = check_union(a,b);
    return t[0] && t[1] && t[2] && t[3];
}

vector<int> DualGraph::get_nodes() {
    vector<int> nodes;
    for(auto& p : node2pol) {
        nodes.push_back(p.fi);
    }
    return nodes;
}

int DualGraph::size() {
    return adj.size();
}

bool DualGraph::does_node_exist(int node) {
    bool res = node2pol.count(node) > 0;
    assert((adj.count(node) > 0) == res);
    return res;
}

Polygon DualGraph::get_polygon(int node) {
    return node2pol[node];
}

vector<Polygon> DualGraph::get_polygons() {
    vector<Polygon> res;
    for(auto& p : node2pol) res.push_back(p.se);
    return res;
}

vector<Polygon> DualGraph::get_polygons(vector<int> nodes) {
    vector<Polygon> res;
    for(auto& node : nodes) res.push_back(get_polygon(node));
    return res;
}

vector<Point> DualGraph::get_points() {
    vector<Point> points;
    for(auto& pol : get_polygons()) {
        for(auto& p : pol) {
            points.pb(p);
        }
    }
    mkunique(points);
    return points;
}

vector<pair<Point,Point>> DualGraph::get_sides() {
    vector<pair<Point,Point>> sides;
    for(auto& pol : get_polygons()) {
        for(auto e : pol.edges()) {
            pair<Point, Point> key = {e.min(), e.max()};
            sides.pb(key);
        }
    }
    mkunique(sides);
    return sides;
}

int DualGraph::n() {
    return adj.size();
}

vector<int> DualGraph::get_dfs_order() {
    set<int> visited;
    vector<int> order;
    int cnt = 0;
    foe(node, get_nodes()) {
        if(visited.count(node)) continue;
        cnt++;
        stack<int> s;
        s.push(node);
        while(!s.empty()) {
            int node = s.top(); s.pop();
            if(!visited.count(node)) {
                order.push_back(node);
            }
            visited.insert(node);
            bool pushed = false;
            s.push(node);
            foe(nb, adj[node]) {
                if(visited.count(nb)) continue;
                pushed = true;
                s.push(nb);
                break;
            }
            if(!pushed) s.pop();
        }
    }
    assert(cnt == 1);
    assert(visited.size() == get_nodes().size());
    return order;
}

std::pair<int,int> DualGraph::get_nodes_sharing_edge(Segment seg) {
    assert(edgemap.count(convert2pair(seg)));
    auto nodes = edgemap[convert2pair(seg)];
    assert(nodes.size() == 2);
    return {nodes[0], nodes[1]};
}

std::vector<int> DualGraph::get_component(int start) {
    vector<int> nodes;
    queue<int> q;
    q.push(start);
    set<int> visited;
    while(!q.empty()) {
        int node = q.front(); q.pop();
        if(visited.count(node)) continue;
        visited.insert(node);
        nodes.push_back(node);
        foe(e, adj[node]) q.push(e);
    }
    return nodes;
}
