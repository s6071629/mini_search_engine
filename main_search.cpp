#include "main.hpp"

#include "main.hpp"

int main() {
    InvertedIndex index;
    ReadFiles read;

    const std::string cacheFile = "index_cache.dat";
    const std::string fingerprintFile = "index_fingerprint.txt";

    std::string currentFingerprint = read.computeFingerprint();
    std::string savedFingerprint;

    std::ifstream fpIn(fingerprintFile);
    if (fpIn.is_open()) {
        std::getline(fpIn, savedFingerprint);
        fpIn.close();
    }

    bool cacheValid = (currentFingerprint == savedFingerprint) &&
                       index.loadFromCache(cacheFile);

    if (cacheValid) {
       // std::cout << "Loaded index from cache." << std::endl;
        read.countFilesOnly();          // just counts files, doesn't reparse
    } else {
        //std::cout << "Rebuilding index..." << std::endl;
        read.readFile(index);           // parses files ONCE
        index.saveToCache(cacheFile);

        std::ofstream fpOut(fingerprintFile);
        fpOut << currentFingerprint;
    }

    std::string word;
    std::cout << "Search: ";
    std::getline(std::cin, word);

    std::stringstream ss(word);
    std::string w;
    std::vector<std::string> words;
    while (ss >> w) {
        for (char &c : w) c = tolower(static_cast<unsigned char>(c));
        w.erase(remove_if(w.begin(), w.end(),
                [](unsigned char c) { return ispunct(c); }), w.end());
        if (!w.empty()) words.push_back(w);
    }

    if (words.empty()) {
        std::cout << "No word to search" << std::endl;
    } else {
        if (!index.phraseSearch(words)) {
            std::cout << "The exact phrase was not found" << std::endl;
            index.findIndex(words, read.getTotalFiles());
        }
    }
    return 0;
}