#include<bits/stdc++.h>

class VCC {

private:

    std::string cache_directory = "tmp/vcc_cache";
    std::string graph_filename;
    long long hash;

    std::string solver_time_limit = "10";
    std::string improve_time_limit = "10";

    // order does not matter!
    long long get_hash(std::map<int,std::set<int>>& adj, int n, int m);

    std::string construct_command(std::string run_type, std::string clique_partition_filename, bool); 

    void run_vcc_command(std::string command); 

    std::vector<std::vector<int>> run_algorithm(std::string run_type); 

    // zero indexed nodes
    void write_graph(std::map<int,std::set<int>>& adj, int n, long long m); 

    // zero indexed nodes
    std::vector<std::vector<int>> read_clique_partition(std::string clique_partition_filename); 

public:

    int seed = 1;

    VCC(std::map<int,std::set<int>>& adj, int n, int m); 

    std::vector<std::vector<int>> run_chalupa(); 

    std::vector<std::vector<int>> run_redu(); 

    std::vector<std::vector<int>> run_reduvcc(); 

    std::vector<std::vector<int>> run_bnr(); 

    std::vector<std::vector<int>> run_edgebnr(); 

    // deterministic, but uses randomness under the hood
    std::vector<std::vector<int>> get_cliques(); 

    void remove_graphfile(); 

};
