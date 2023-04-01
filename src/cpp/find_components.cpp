#include<bits/stdc++.h>
#include"find_components.h"
#include"debug.h"
#include"common.h"
#include"rng.h"

using namespace std;

ComponentFinder::ComponentFinder(std::map<int,std::set<int>> _adj, function<bool(vector<int>)> _valid) : valid(_valid) {
    adj = _adj;
}
vector<vector<int>> ComponentFinder::find_components() {
    return find_components_multi_source();
}

vector<vector<int>> ComponentFinder::find_components_single_source() {
    vector<vector<int>> components;
    set<int> done;
    _find_components_single_source(components, done);
    return components;
}

void ComponentFinder::_find_components_single_source(vector<vi>& components, set<int>& done) {
    auto compare = [&](const int& node1, const int& node2) -> bool {
        if(adj[node1].size() == adj[node2].size()) return node1 < node2;
        return adj[node1].size() > adj[node2].size();
    };
    vi nodes; foe(e, adj) nodes.pb(e.fi);
    // sort(all(nodes), compare);
    for(auto& node : nodes) {
        if(done.count(node)) continue;
        set<int> vis;
        //priority_queue<int, vector<int>, decltype(compare)> q (compare); q.push(node);
        queue<int> q;
        q.push(node);
        vector<int> comp;
        while(comp.size() < MXCOMPSIZE && !q.empty()) {
            int cur = q.front(); q.pop();
            if(done.count(cur) || vis.count(cur)) continue;
            vis.insert(cur);
            comp.push_back(cur);
            if(!valid(comp)) {
                comp.pop_back();
                continue;
            }
            done.insert(cur);
            for(auto& nb : adj[cur]) {
                if(done.count(nb) == 0 && vis.count(nb) == 0) {
                    q.push(nb);
                }
            }
        }
        components.push_back(comp);
    }
}

vi ComponentFinder::get_random_sources() {
    vi nodes; foe(e, adj) nodes.pb(e.fi);
    int n = nodes.size();
    int tospawn = 2 * ((n + MXCOMPSIZE - 1) / MXCOMPSIZE);
    RNG rng (0,n-1,RAND_SEED);
    set<int> chosen;
    vi res;
    rep(tospawn) {
        if(chosen.size() == n) break;
        int idx = rng.get();
        while(chosen.count(idx)) idx = rng.get();
        chosen.insert(idx);
        res.pb(nodes[idx]);
    }
    return res;
}

vi ComponentFinder::get_good_sources() {
    vi sources = get_random_sources();
    vi nodes; foe(e, adj) nodes.pb(e.fi);
    vector<vector<int>> components;
    set<int> done;
    _find_components_multi_source(components, done, sources);
    _find_components_single_source(components, done);
    int m = sz(components);
    for(int i = 0; i < m; i++) {
        if(components[i].size() < MNCOMPSIZE) {
            sources.pb(components[i].front());
        }
    }
    return sources;
}

void ComponentFinder::_find_components_multi_source(vector<vi>& comps, set<int>& done, vi& sources) {
    queue<pair<int,int>> q;
    foe(node, sources) {
        q.push({node, sz(comps)});
        comps.pb({});
    }
    while(!q.empty()) {
        int node = q.front().fi;
        int comp = q.front().se;
        q.pop();
        if(done.count(node)) continue;
        comps[comp].pb(node);
        if(!valid(comps[comp])) {
            comps[comp].pop_back();
            continue;
        }
        done.insert(node);
        foe(nb, adj[node]) {
            q.push({nb, comp});
        }
    }
} 

vector<vi> ComponentFinder::find_components_multi_source() {
    vector<vi> components;
    set<int> done;
    vi sources = get_good_sources();
    _find_components_multi_source(components, done, sources);
    _find_components_single_source(components, done);
    return components;
} 

/*struct DSU {
    vector<vector<int>> components;
};

vector<vi> ComponentFinder::find_components_all_sources() {
}*/
