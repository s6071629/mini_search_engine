#include "main.hpp"

std::string ReadFiles::computeFingerprint() {
    // Combine filename + last write time + size for every file in the directory
    std::vector<std::string> entries;
    for (const auto &entry : fs::directory_iterator(path)) {
        if (!entry.is_regular_file()) continue;
        auto ftime = fs::last_write_time(entry);
        auto size = fs::file_size(entry);
        entries.push_back(entry.path().filename().string() + ":" +
                           std::to_string(ftime.time_since_epoch().count()) + ":" +
                           std::to_string(size));
    }
    sort(entries.begin(), entries.end()); // order-independent

    std::string combined;
    for (auto &e : entries) combined += e + "|";
    return combined;
}


void ReadFiles::countFilesOnly() {
    total_files = 0;
    for (const auto &entry : fs::directory_iterator(path)) {
        if (!entry.is_regular_file()) continue;
        total_files++;
    }
}

int ReadFiles::getTotalFiles() {return total_files;}

void ReadFiles::readFile(InvertedIndex &index) {
    for (const auto &entry : fs::directory_iterator(path)) {

        std::ifstream file(entry.path());

        if(!file.is_open()) continue;
        total_files++;
        std::string line;
        std::string filename = entry.path().filename().string();
        int idx = 0;
        while (getline(file, line)) {
            std::stringstream ss(line);
            std::string words;
            while (ss >> words) {
                for (char &c : words) {
                    c = tolower(c);
                }
                words.erase(remove_if(words.begin(), words.end(),[](unsigned char c){ return ispunct(c); }), words.end());
                if(words.empty()) continue;
                if (ignore.count(words)) { idx++; continue;}
                index.insertIndex(words, filename, idx);
                idx++;
            }
        }
    }
}
