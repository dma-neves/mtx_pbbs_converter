#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <sstream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define ZERO_BASED_INDEXING
#define PADDING 256

struct edge {
    int source;
    int destination;
};

bool compare_edges(const edge& e1, const edge& e2) {
    return e1.source < e2.source || (e1.source == e2.source && e1.destination < e2.destination);
}

char* read_file_to_buffer(const std::string& file_path) {

    std::ifstream file(file_path, std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Error opening input file: " << file_path << std::endl;
        return nullptr;
    }

    long end = file.tellg();
    file.seekg (0, std::ios::beg);
    long n = end - file.tellg();

    char* buffer = new char[n+1];
    file.read(buffer, n);
    file.close();
    return buffer;
}

void write_buffer_to_file(const char* buffer, const std::string& file_path) {

}


void mtx_to_pbbs_adjacency_graph(const std::string& input_file_path, const std::string& output_file_path, bool weighted = false) {

    char* input_file_buffer = read_file_to_buffer(input_file_path);
    if(input_file_buffer == nullptr) {
        return;
    }
    std::ifstream input_file(input_file_path);

    std::string line;
    int max_id, n, m;

    // skip comments
    while (getline(input_file, line) && line[0] == '%');

    // read header
    sscanf(line.c_str(), "%d %d %d", &max_id, &n, &m);

    std::string header = weighted ? "WeightedAdjacencyGraph\n" : "AdjacencyGraph\n";
    std::string offset_line = std::to_string(m) + "\n";
    std::string adjacency_line = std::to_string(max_id) + "\n";
    std::size_t output_file_size = header.size() * n * offset_line.size() + m * adjacency_line.size() + PADDING;

    char* output_file_buffer = new char[output_file_size];
    std::ostringstream output_stream(std::string(output_file_buffer, output_file_size));

    output_stream << (weighted ? "WeightedAdjacencyGraph" : "AdjacencyGraph") << std::endl;
    output_stream << n << std::endl;
    output_stream << m << std::endl;

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
        output_stream << offset_sum << std::endl;
        offset_sum += degree;
    }

    for (int i = 0; i < m; i++) {
        
#ifdef ZERO_BASED_INDEXING
        if(weighted) {
            output_stream << edges[i].destination - 1 << " 1.0" << std::endl;
        }
        else {
            output_stream << edges[i].destination - 1 << std::endl;
        }
#else
        if(weighted) {
            output_stream << edges[i].destination << " 1.0" << std::endl;
        }
        else {
            output_stream << edges[i].destination << std::endl;
        }
#endif
    }

    input_file.close();

    std::ofstream output_file(output_file_path);
    output_file.write(output_file_buffer, output_file_size);

    output_file.close();

    delete [] input_file_buffer;
    delete [] output_file_buffer;
}

void mtx_to_pbbs_adjacency_graph_old(const std::string& input_filename, const std::string& output_filename, bool weighted = false) {

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

    output_file << (weighted ? "WeightedAdjacencyGraph" : "AdjacencyGraph") << std::endl;
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

void mtx_to_pbbs_edge_graph(const std::string& input_file_path, const std::string& output_file_path, bool weighted = false) {

    std::ifstream input_file(input_file_path);
    if (!input_file.is_open()) {
        std::cerr << "Error opening input file: " << input_file_path << std::endl;
        return;
    }

    std::ofstream output_file(output_file_path);
    if (!output_file.is_open()) {
        std::cerr << "Error opening output file: " << output_file_path << std::endl;
        input_file.close();
        return;
    }

    std::string line;
    int max_id, n, m;

    // skip comments
    while (getline(input_file, line) && line[0] == '%');

    // read header
    sscanf(line.c_str(), "%d %d %d", &max_id, &n, &m);

    output_file << (weighted ? "WeightedEdgeGraph" : "EdgeGraph") << std::endl;
    output_file << n << std::endl;
    output_file << m << std::endl;

    for (int i = 0; i < m; i++) {
        
        edge e;
        input_file >> e.source >> e.destination;

#ifdef ZERO_BASED_INDEXING
        if(weighted) {
            output_file << e.source << e.destination - 1 << " 1.0" << std::endl;
        }
        else {
            output_file << e.source << e.destination - 1 << std::endl;
        }

#else
        // TODO: just append the rest of the file instead ?
        if(weighted) {
            output_file << e.source << e.destination << " 1.0" << std::endl;
        }
        else {
            output_file << e.source << e.destination << std::endl;
        }
#endif
    }

    input_file.close();
    output_file.close();
}

int main(int argc, char* argv[]) {
    
    if (argc != 4) {
        std::cerr << "   Usage: " << argv[0] << " <type> <input_file_path> <output_file_path>" << std::endl;
        std::cerr << "  <type>: Specify graph type. Options are:" << std::endl;
        std::cerr << "      -a: Adjacency graph" << std::endl;
        std::cerr << "      -e: Edge graph" << std::endl;
        std::cerr << "     -we: Weighted edge graph" << std::endl;
        std::cerr << "     -wa: Weighted adjacency graph" << std::endl;
        return EXIT_FAILURE;
    }


    std::string type = argv[1];
    std::string input_file_path = argv[2];
    std::string output_file_path = argv[3];

    if (type == "-a") {
        mtx_to_pbbs_adjacency_graph(input_file_path, output_file_path);
    } 
    else if (type == "-e") {
        mtx_to_pbbs_edge_graph(input_file_path, output_file_path);
    } 
    else if (type == "-we") {
        mtx_to_pbbs_edge_graph(input_file_path, output_file_path, true);
    } 
    else if (type == "-wa") {
        mtx_to_pbbs_adjacency_graph(input_file_path, output_file_path, true);
    } 
    else {
        std::cerr << "Invalid graph type specified." << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Conversion completed successfully." << std::endl;

    return EXIT_SUCCESS;
}
