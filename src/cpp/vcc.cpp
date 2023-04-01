#include<bits/stdc++.h>
#include<boost/process.hpp>

#include"rng.h"
#include"vcc.h"

#include"common.h"
#include"debug.h"

namespace fs = boost::filesystem;
namespace bp = boost::process;

using namespace std;

// order does not matter!
ll VCC::get_hash(map<int,set<int>>& adj, int n, int m) {
    ll res = 0;
    ll M = 1000000007ll;
    ll A = 349734811ll;
    ll B = 748139483ll;
    vector<ll> vals;
    vals.push_back(((ll)n) % M);
    vals.push_back(m % M);
    foe(e, adj) {
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

VCC::VCC(map<int,set<int>>& adj, int n, int m) {
    // cout << "VCC(adj, " << n << ", " << m << ")" << endl;
    if(getenv("SOLVER_TIME_LIMIT")) {
        solver_time_limit = getenv("SOLVER_TIME_LIMIT");
    }
    if(getenv("IMPROVE_TIME_LIMIT")) {
        improve_time_limit = getenv("IMPROVE_TIME_LIMIT");
    }
    hash = get_hash(adj,n,m);
    auto get_graph_filename = [&]() {
        fs::create_directories(cache_directory);
        fs::path file ("graph_" + to_string(hash) + ".txt");
        return (cache_directory / file).string();
    };
    graph_filename = get_graph_filename();
    write_graph(adj,n,m);
}

string VCC::construct_command(string run_type, string clique_partition_filename, bool use_total_time_limit) {
    if(use_total_time_limit) {
        return "./bin/vcc --solver_time_limit=" + solver_time_limit + " --seed=" + to_string(seed) + " --output_cover_file=" + clique_partition_filename + " --run_type=" + run_type + " " +  graph_filename;
    } else {
        return "./bin/vcc --solver_time_limit=" + solver_time_limit + " --improve_time_limit=" + improve_time_limit + " --seed=" + to_string(seed) + " --output_cover_file=" + clique_partition_filename + " --run_type=" + run_type + " " +  graph_filename;
    }
}

void VCC::run_vcc_command(string command) {
    cout << "running VCC using command: " << command << endl;
    cout << "=================================" << endl;
    int ret = bp::system(command, bp::std_out > stdout, bp::std_err > stderr);
    cout << "=================================" << endl;
    assert(ret == 0);
}

vector<vector<int>> VCC::run_algorithm(string run_type) {
    auto get_clique_partition_filename = [&]() {
        fs::create_directories(cache_directory);
        fs::path file ("clique_partition_" + run_type + "_" + to_string(hash) + ".txt");
        return (cache_directory / file).string();
    };
    string clique_partition_filename = get_clique_partition_filename();
    // fs::remove(clique_partition_filename);
    assert(fs::exists(graph_filename));
    string command = construct_command(run_type, clique_partition_filename, !(run_type == "Chalupa" || run_type == "ReduVCC"));
    run_vcc_command(command);
    if(fs::exists(clique_partition_filename)) {
        auto cliques = read_clique_partition(clique_partition_filename); // found a solution
        // fs::remove(clique_partition_filename);
        return cliques;
    } else {
        return {}; // did not find a solution
    }
}

vector<vector<int>> VCC::run_chalupa() {
    return run_algorithm("Chalupa");
}

vector<vector<int>> VCC::run_redu() {
    return run_algorithm("Redu");
}

vector<vector<int>> VCC::run_reduvcc() {
    return run_algorithm("ReduVCC");
}

vector<vector<int>> VCC::run_bnr() {
    return run_algorithm("bnr");
}

vector<vector<int>> VCC::run_edgebnr() {
    return run_algorithm("edge_bnr");
}

// zero indexed nodes
void VCC::write_graph(map<int,set<int>>& adj, int n, ll m) {
    std::ofstream file(graph_filename);
    file << n << " " << m << " 0" << '\n';
    for(int node = 0; node <= n - 1; node++) {
        if(adj.count(node)) { // not doing this could modify adj
            foe(nb, adj[node]) {
                assert(nb != node);
                file << nb + 1 << " ";
            }
        }
        file << "\n";
    }
    file.close();
}

// zero indexed nodes
vector<vector<int>> VCC::read_clique_partition(string clique_partition_filename) {
    assert(fs::exists(clique_partition_filename));
    std::ifstream _file(clique_partition_filename);
    int number_of_lines = std::count(std::istreambuf_iterator<char>(_file), std::istreambuf_iterator<char>(), '\n');
    _file.close();
    std::ifstream file(clique_partition_filename);
    vector<vector<int>> cliques;
    for(int i = 0; i < number_of_lines; i++) {
        string line; getline(file, line);
        stringstream ss (line);
        vector<int> clique;
        int node;
        while(ss >> node) {
            clique.push_back(node - 1);
        }
        cliques.push_back(clique);
    }
    file.close();
    return cliques;
}

// deterministic, but uses randomness under the hood
vector<vector<int>> VCC::get_cliques() {
    RNG rng (1, 1000000, 42);
    string best_type = "none";
    vector<vector<int>> best_cliques;
    bool is_optimal = false;
    auto pick_best = [&](string type, vector<vector<int>> cliques, bool _is_optimal) {
        if(cliques.size() == 0) return;
        if(best_cliques.size() == 0 || cliques.size() < best_cliques.size()) {
            assert(!is_optimal);
            best_cliques = cliques;
            best_type = type;
            is_optimal = _is_optimal;
        } else if(cliques.size() == best_cliques.size() && _is_optimal) { // it is better since we know it is optimal
            best_cliques = cliques;
            best_type = type;
            is_optimal = true;
        }
    };
    rep(5) {
        seed = rng.get();
        pick_best("Chalupa", run_chalupa(), false);
        // pick_best("bnr", run_bnr(), false);
        // pick_best("edge_bnr", run_edgebnr(), false);
        // pick_best("Redu", run_redu(), true);
        // pick_best("ReduVCC", run_reduvcc(), false);
    }
    assert(best_type != "none");
    assert(best_cliques.size() > 0);
    // cout << "=================" << endl;
    // cout << "found solution with " << best_cliques.size() << " cliques using " << best_type << endl;
    // if(is_optimal) cout << "the solution is known to be optimal" << endl;
    // cout << "=================" << endl;
    return best_cliques;
}

void VCC::remove_graphfile() {
    // fs::remove(graph_filename);
}
