#include "main.hpp"
#include "queries.hpp"
#include <chrono>

long long timedSearch(InvertedIndex &index, ReadFiles &read,std::vector<std::string> &words) {
    std::stringstream discard;
    std::streambuf *oldBuf = std::cout.rdbuf(discard.rdbuf());

    auto start = std::chrono::high_resolution_clock::now();
    if (!index.phraseSearch(words)) {
        index.findIndex(words, read.getTotalFiles());
    }
    auto end = std::chrono::high_resolution_clock::now();

    std::cout.rdbuf(oldBuf);
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

int main() {
    InvertedIndex index;
    ReadFiles read;

    const int NUM_QUERIES = 1000;
    long long latencies[NUM_QUERIES];

    for (int i = 0; i < NUM_QUERIES; i++) {
        std::stringstream ss(benchmarkQueries[i]);
        std::string w;
        std::vector<std::string> words;
        while (ss >> w) {
            for (char &c : w) c = tolower(static_cast<unsigned char>(c));
            w.erase(remove_if(w.begin(), w.end(),
                    [](unsigned char c) { return ispunct(c); }), w.end());
            if (!w.empty()) words.push_back(w);
        }
        latencies[i] = words.empty() ? 0 : timedSearch(index, read, words);
    }

    long long total = 0, minLat = latencies[0], maxLat = latencies[0];
    for (int i = 0; i < NUM_QUERIES; i++) {
        total += latencies[i];
        minLat = std::min(minLat, latencies[i]);
        maxLat = std::max(maxLat, latencies[i]);
    }

    std::cout << "Ran " << NUM_QUERIES << " searches.\n";
    std::cout << "Total time: " << total << " us\n";
    std::cout << "Average latency: " << (total / NUM_QUERIES) << " us\n";
    std::cout << "Min latency: " << minLat << " us\n";
    std::cout << "Max latency: " << maxLat << " us\n";

    return 0;
}