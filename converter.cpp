#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <algorithm>

#define ZERO_BASED_INDEXING
#define DIRECTED_TO_UNDIRECTED

struct edge {
    int source;
    int destination;
};

bool compare_edges(const edge& e1, const edge& e2) {
    return e1.source < e2.source || (e1.source == e2.source && e1.destination < e2.destination);
}

void mtx_to_pbbs_adjacency_graph(const std::string& input_filename, const std::string& output_filename, bool weighted = false) {

    std::ifstream input_file(input_filename);
    if (!input_file.is_open()) {
        std::cerr << "Error opening input file: " << input_filename << std::endl;
        return;
    }

    std::ofstream output_file(output_filename);
    if (!output_file.is_open()) {
        std::cerr << "Error opening output file: " << output_filename << std::endl;
        input_file.close();
        return;
    }

    std::string line;
    int max_id, n, m;

    // skip comments
    while (getline(input_file, line) && line[0] == '%');

    // read header
    sscanf(line.c_str(), "%d %d %d", &max_id, &n, &m);

#ifdef DIRECTED_TO_UNDIRECTED
    int output_m = m*2;
#else
    int output_m = m*2;
#endif

    output_file << (weighted ? "WeightedAdjacencyGraph" : "AdjacencyGraph") << std::endl;
    output_file << n << std::endl;
    output_file << output_m << std::endl;

    std::vector<edge> edges;
    edges.reserve(output_m);

    int source, dest;
    for (int i = 0; i < m; i++) {

        input_file >> source >> dest;
        edges.push_back({source, dest});
#ifdef DIRECTED_TO_UNDIRECTED
        edges.push_back({dest, source});
#endif
    }

    std::sort(edges.begin(), edges.end(), compare_edges);

    std::vector<int> degrees(n, 0);
    for (int i = 0; i < output_m; ++i) {
#ifdef ZERO_BASED_INDEXING
        degrees[edges[i].source - 1]++;
#else
        degrees[edges[i].source]++;
#endif
    }

    int offset_sum = 0;
    for (int i = 0; i < n; ++i) {
        int degree = degrees[i];
        output_file << offset_sum << std::endl;
        offset_sum += degree;
    }

    for (int i = 0; i < output_m; i++) {
        
#ifdef ZERO_BASED_INDEXING
        if(weighted) {
            output_file << edges[i].destination - 1 << " 1.0" << std::endl;
        }
        else {
            output_file << edges[i].destination - 1 << std::endl;
        }
#else
        if(weighted) {
            output_file << edges[i].destination << " 1.0" << std::endl;
        }
        else {
            output_file << edges[i].destination << std::endl;
        }
#endif
    }

    input_file.close();
    output_file.close();
}

void mtx_to_pbbs_edge_graph(const std::string& input_filename, const std::string& output_filename, bool weighted = false) {

    std::ifstream input_file(input_filename);
    if (!input_file.is_open()) {
        std::cerr << "Error opening input file: " << input_filename << std::endl;
        return;
    }

    std::ofstream output_file(output_filename);
    if (!output_file.is_open()) {
        std::cerr << "Error opening output file: " << output_filename << std::endl;
        input_file.close();
        return;
    }

    std::string line;
    int max_id, n, m;

    // skip comments
    while (getline(input_file, line) && line[0] == '%');

    // read header
    sscanf(line.c_str(), "%d %d %d", &max_id, &n, &m);

#ifdef DIRECTED_TO_UNDIRECTED
    int output_m = m*2;
#else
    int output_m = m*2;
#endif

    output_file << (weighted ? "WeightedEdgeGraph" : "EdgeGraph") << std::endl;
    output_file << n << std::endl;
    output_file << output_m << std::endl;

    for (int i = 0; i < m; i++) {
        
        edge e;
        input_file >> e.source >> e.destination;

#ifdef ZERO_BASED_INDEXING
        if(weighted) {
            output_file << e.source - 1 << e.destination - 1 << " 1.0" << std::endl;
#ifdef DIRECTED_TO_UNDIRECTED
            output_file << e.destination - 1 << e.source - 1 << " 1.0" << std::endl;
#endif
        }
        else {
            output_file << e.source - 1 << e.destination - 1 << std::endl;
#ifdef DIRECTED_TO_UNDIRECTED
            output_file << e.destination - 1 << e.source - 1 << std::endl;
#endif
        }
#else
        // TODO: just append the rest of the file instead ?
        if(weighted) {
            output_file << e.source << e.destination << " 1.0" << std::endl;
#ifdef DIRECTED_TO_UNDIRECTED
            output_file << e.destination << e.source << " 1.0" << std::endl;
#endif
        }
        else {
            output_file << e.source << e.destination << std::endl;
#ifdef DIRECTED_TO_UNDIRECTED
            output_file << e.destination << e.source << std::endl;
#endif
        }
#endif
    }

    input_file.close();
    output_file.close();
}

int main(int argc, char* argv[]) {
    
    if (argc != 4) {
        std::cerr << "   Usage: " << argv[0] << " <type> <input_filename> <output_filename>" << std::endl;
        std::cerr << "  <type>: Specify graph type. Options are:" << std::endl;
        std::cerr << "      -a: Adjacency graph" << std::endl;
        std::cerr << "      -e: Edge graph" << std::endl;
        std::cerr << "     -we: Weighted edge graph" << std::endl;
        std::cerr << "     -wa: Weighted adjacency graph" << std::endl;
        return EXIT_FAILURE;
    }


    std::string type = argv[1];
    std::string input_filename = argv[2];
    std::string output_filename = argv[3];

    if (type == "-a") {
        mtx_to_pbbs_adjacency_graph(input_filename, output_filename);
    } 
    else if (type == "-e") {
        mtx_to_pbbs_edge_graph(input_filename, output_filename);
    } 
    else if (type == "-we") {
        mtx_to_pbbs_edge_graph(input_filename, output_filename, true);
    } 
    else if (type == "-wa") {
        mtx_to_pbbs_adjacency_graph(input_filename, output_filename, true);
    } 
    else {
        std::cerr << "Invalid graph type specified." << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Conversion completed successfully." << std::endl;

    return EXIT_SUCCESS;
}
