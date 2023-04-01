#include<bits/stdc++.h>
#include<stdlib.h>
#include"clique.h"
#include"chgraph.h"
#include"debug.h"
#include"rng.h"
#include"clique.h"
#include"common.h"
#include"geometry_utils.h"
#include"dsu.h"
#include"vcc.h"

using namespace std;

CliqueCover::CliqueCover(CHGraph& _chgraph) : chgraph(_chgraph) {
    chgraph.add_edges_to_make_undirected();
    // rng = RNG(0,n-1,42);
}

int CliqueCover::n() {
    return chgraph.dualgraph.n();
}

ll CliqueCover::m() {
    ll _m = chgraph.m();
    assert(_m % 2 == 0);
    _m /= 2;
    return _m;
}

vector<vector<int>> CliqueCover::_get_cliques_largeadj_naive() {
    auto comp = [&](const int& node1, const int& node2) -> bool {
        if(chgraph.adj[node1].size() == chgraph.adj[node2].size()) return node1 < node2;
        return chgraph.adj[node1].size() < chgraph.adj[node2].size();
    };
    return _get_cliques_sorter_naive(comp);
}

vector<vector<int>> CliqueCover::_get_cliques_smalladj_naive() {
    auto comp = [&](const int& node1, const int& node2) -> bool {
        if(chgraph.adj[node1].size() == chgraph.adj[node2].size()) return node1 < node2;
        return chgraph.adj[node1].size() > chgraph.adj[node2].size();
    };
    return _get_cliques_sorter_naive(comp);
}

// order does not matter!
ll CliqueCover::get_hash() {
    ll res = 0;
    ll M = 1000000007ll;
    ll A = 349734811ll;
    ll B = 748139483ll;
    vector<ll> vals;
    vals.push_back(((ll)n()) % M);
    vals.push_back(m() % M);
    foe(e, chgraph.adj) {
        ll node = e.fi;
        foe(_nb, e.se) {
            ll nb = _nb;
            ll val = (node * A % M + B) % M * (nb * A % M + B) % M;
            vals.pb(val);
        }
    }
    foe(v, vals) {
        res += A * v + B;
        res %= M;
    }
    return res;
}

vector<vector<int>> CliqueCover::_get_cliques_sorter_naive(function<bool(const int&, const int&)> comp) {
    // cout << "FINDING CLIQUES" << endl;
    vector<vector<int>> cliques;
    set<int> vis;
    vector<int> nodes;
    for(auto& p : chgraph.adj) {
        int node = p.first;
        nodes.push_back(node);
    }
    sort(nodes.rbegin(), nodes.rend(), comp);
    for(int node : nodes) {
        if(vis.count(node)) continue;
        priority_queue<int, vector<int>, decltype(comp)> q (comp);
        q.push(node);
        vector<int> clique;
        while(!q.empty()) {
            int node = q.top(); q.pop();
            if(vis.count(node)) continue;
            bool ok = 1;
            for(auto& nb : clique) {
                if(chgraph.adj[node].count(nb) == 0) {
                    ok = 0;
                    break;
                }
            }
            if(!ok) continue;
            vis.insert(node);
            clique.push_back(node);
            for(auto& nb : chgraph.adj[node]) {
                if(vis.count(nb)) continue;
                q.push(nb);
            }
        }
        cliques.push_back(clique);
    }
    return cliques;
}

vector<vector<int>> CliqueCover::_get_cliques_vcc() {
    VCC vcc (chgraph.adj, n(), m());
    auto cliques = vcc.get_cliques();
    vcc.remove_graphfile();
    return cliques;
}

vector<vector<int>> CliqueCover::_get_cliques_local() {
    assert(false);
}

// using triangles or using components
// merge neighbouring triangles into existing cliques
bool CliqueCover::fix_chgraph_clique_remove_all_edges(vector<int>& clique) {
    auto pol = chgraph.get_convex_hull_of_nodes(clique);
    if(!chgraph.pred.is_completely_inside_for_convex_pol(pol)) {
        debug("invalid clique - fast");
        debug(clique.size());
        DSU dsu;
        set<int> nodes;
        for(auto& node : clique) {
            dsu.make_set(node);
            nodes.insert(node);
        }
        for(auto& node : clique) {
            for(auto& nb : chgraph.dualgraph.adj[node]) {
                if(!nodes.count(nb)) continue;
                dsu.union_sets(node, nb);
            }
        }
        map<int,vi> components;
        for(auto& node : clique) {
            components[dsu.find_set(node)].pb(node);
        }
        vector<vi> comps;
        for(auto& p : components) {
            auto& component = p.se;
            comps.pb(component);
        }
        debug(comps.size());
        assert(comps.size() >= 3);
        int n_comps = comps.size();
        int total_removed = 0;
        fon(i, n_comps) foab(j, i + 1, n_comps) {
            foe(a, comps[i]) foe(b, comps[j]) {
                assert(a != b);
                total_removed++;
                chgraph.remove_edge(a,b);
            }
        }
        debug(total_removed);
        return true;
    }
    return false;
}

// returns false since it should not run clique algorithm again
vector<vector<int>> CliqueCover::fix_chgraph_clique_split(vector<int>& clique) {
    auto pol = chgraph.get_convex_hull_of_nodes(clique);
    if(chgraph.pred.is_completely_inside_for_convex_pol(pol)) {
        return {clique};
    }
    // cout << "clique is invalid" << endl;
    // cout << "fixing clique of size: " << clique.size() << endl;
    auto hole = chgraph.pred.get_hole_in_interior(pol); // this should be fine in terms of actual performance, even though there is no guarantee
    auto centroid = get_centroid(hole); // not actually necessary
    auto point = pol[0];
    Line line (centroid, point);
    vector<int> clique1, clique2;
    foe(node, clique) {
        auto triangle = chgraph.triangulation[node];
        auto centroid = get_centroid(triangle);
        if(line.has_on(centroid) || line.has_on_negative_side(centroid)) {
            clique1.push_back(node);
        } else {
            assert(line.has_on_positive_side(centroid));
            clique2.push_back(node);
        }
    }
    vector<vector<int>> cliques1 = fix_chgraph_clique_split(clique1);
    vector<vector<int>> cliques2 = fix_chgraph_clique_split(clique2);
    vector<vector<int>> cliques;
    cliques.insert(cliques.end(), all(cliques1));
    cliques.insert(cliques.end(), all(cliques2));
    // cout << "number of fix-cliques: " << cliques.size() << endl;
    return cliques;
}

bool CliqueCover::fix_chgraph_clique_remove_single_edge(vector<int>& clique) {
    auto pol = chgraph.get_convex_hull_of_nodes(clique);
    if(!chgraph.pred.is_completely_inside_for_convex_pol(pol)) {
        debug("invalid clique");
        DSU dsu;
        set<int> nodes;
        for(auto& node : clique) {
            dsu.make_set(node);
            nodes.insert(node);
        }
        for(auto& node : clique) {
            for(auto& nb : chgraph.dualgraph.adj[node]) {
                if(!nodes.count(nb)) continue;
                dsu.union_sets(node, nb);
            }
        }
        map<int,vi> components;
        for(auto& node : clique) {
            components[dsu.find_set(node)].pb(node);
        }
        vector<vi> comps;
        for(auto& p : components) {
            auto& component = p.se;
            comps.pb(component);
        }
        assert(comps.size() >= 3);
        int a = comps[0][0];
        int b = comps[1][0];
        chgraph.remove_edge(a,b);
        return true;
    }
    return false;
}

bool CliqueCover::fix_chgraph_cliques(vector<vector<int>>& cliques) {
    bool change = false;
    for(int i = 0; i < cliques.size(); i++) {
        auto& clique = cliques[i];
        change |= fix_chgraph_clique_remove_all_edges(clique);
    }
    return change;
}

vector<vector<int>> CliqueCover::find_valid_cliques(GetCliquesLambda get_cliques, int fix_type) {
    if(fix_type == 1) {
        CliqueCover cliquecover (chgraph);
        TIME start_cliquefinding = NOW();
        vector<vector<int>> cliques = get_cliques();
        TIME end_cliquefinding = NOW();
        TIME start_fixing = NOW();
        vector<vector<int>> fixed_cliques;
        map<int,int> invalid_cliques;
        int total = 0;
        foe(clique, cliques) {
            vector<vector<int>> fix = fix_chgraph_clique_split(clique);
            invalid_cliques[fix.size()]++;
            if(fix.size() > 1) total++;
            foe(fixed_clique, fix) {
                fixed_cliques.push_back(fixed_clique);
            }
        }
        TIME end_fixing = NOW();
        cout << "\n\n\nCliqueCover stats" << endl;
        cout << "  total number of invalid cliques: " << total << endl;
        foe(e, invalid_cliques) {
            if(e.fi <= 1) continue;
            cout << "    number of invalid cliques that turned into " << e.fi << " new cliques: " << e.se << endl;
        }
        cout << "  initial solution size: " << cliques.size() << endl;
        cout << "  fixed solution size: " << fixed_cliques.size() << endl;
        cout << "  difference in solution size: " << (sz(fixed_cliques) - sz(cliques)) << endl;
        cout << "  time to compute initial cliques: " << COUT_TIME(TO_MICRO(end_cliquefinding, start_cliquefinding)) << endl;
        cout << "  time to fix cliques: " << COUT_TIME(TO_MICRO(end_fixing, start_fixing)) << endl;
        cout << "\n\n\n";
        return fixed_cliques;
    } else if(fix_type == 2) {
        vector<vector<int>> cliques;
        while(true) {
            CliqueCover cliquecover (chgraph);
            cliques = get_cliques();
            // cout << "number of cliques found: " << cliques.size() << endl;
            // cout << "size of graph: n=" << n() << " m=" << m() << endl;
            vector<Polygon> clique_polygons = chgraph.get_convex_hull_of_decomposition(cliques);
            if(!fix_chgraph_cliques(cliques)) {
                break;
            }
        }
        return cliques;
    } else {
        assert(false);
    }
}

vector<Polygon> CliqueCover::find_valid_clique_polygons(GetCliquesLambda get_cliques, int fix_type) {
    auto cliques = find_valid_cliques(get_cliques, fix_type);
    return chgraph.get_convex_hull_of_decomposition(cliques);
}

std::vector<Polygon> CliqueCover::get_cliques_smalladj_naive() {
    return find_valid_clique_polygons([&](){
        return _get_cliques_smalladj_naive();
    }, 1);
}

std::vector<Polygon> CliqueCover::get_cliques_largeadj_naive() {
    return find_valid_clique_polygons([&](){
        return _get_cliques_largeadj_naive();
    }, 1);
}

std::vector<Polygon> CliqueCover::get_cliques_local() {
    return find_valid_clique_polygons([&](){
        return _get_cliques_local();
    }, 1);
}

std::vector<Polygon> CliqueCover::get_cliques_vcc() {
    return find_valid_clique_polygons([&](){
        return _get_cliques_vcc();
    }, 1);
}
