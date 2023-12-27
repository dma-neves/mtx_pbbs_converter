#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <algorithm>

#define ZERO_BASED_INDEXING

struct edge {
    int source;
    int destination;
};

bool compare_edges(const edge& e1, const edge& e2) {
    return e1.source < e2.source || (e1.source == e2.source && e1.destination < e2.destination);
}

void mtx_to_pbbs(const std::string& input_filename, const std::string& output_filename) {

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

    output_file << "AdjacencyGraph" << std::endl;
    output_file << n << std::endl;
    output_file << m << std::endl;

    std::vector<edge> edges;
    edges.reserve(m);

    int source, dest;
    for (int i = 0; i < m; i++) {

        input_file >> source >> dest;
        edges.push_back({source, dest});
    }

    std::sort(edges.begin(), edges.end(), compare_edges);

    std::vector<int> degrees(n, 0);
    for (int i = 0; i < m; ++i) {
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

    for (int i = 0; i < m; i++) {
        
#ifdef ZERO_BASED_INDEXING
        output_file << edges[i].destination - 1 << std::endl;
#else
        output_file << edges[i].destination << std::endl;
#endif
    }

    input_file.close();
    output_file.close();
}

int main(int argc, char* argv[]) {
    
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_filename> <output_filename>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string input_filename = argv[1];
    std::string output_filename = argv[2];

    mtx_to_pbbs(input_filename, output_filename);

    std::cout << "Conversion completed successfully." << std::endl;

    return EXIT_SUCCESS;
}
