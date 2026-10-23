#include "main.hpp"

void InvertedIndex::saveToCache(const std::string &path) {
    std::ofstream out(path, std::ios::binary);
    if (!out.is_open()) return;

    // total_words: filename -> count
    out << total_words.size() << "\n";
    for (auto &[filename, count] : total_words) {
        out << filename << " " << count << "\n";
    }

    // position_index: word -> filename -> positions
    out << position_index.size() << "\n";
    for (auto &[word, fileMap] : position_index) {
        out << word << " " << fileMap.size() << "\n";
        for (auto &[filename, positions] : fileMap) {
            out << filename << " " << positions.size();
            for (int p : positions) out << " " << p;
            out << "\n";
        }
    }
}

bool InvertedIndex::loadFromCache(const std::string &path) {
    std::ifstream in(path, std::ios::binary);
    if (!in.is_open()) return false;

    //building from the cache file
    size_t totalWordsSize;
    if ((!in >> totalWordsSize)) return false;
    for (size_t i = 0; i < totalWordsSize; i++) {
        std::string filename;
        int count;
        in >> filename >> count;
        total_words[filename] = count;
    }

    size_t posIndexSize;
    if (!(in >> posIndexSize)) return false;
    for (size_t i = 0; i < posIndexSize; i++) {
        std::string word;
        size_t fileCount;
        in >> word >> fileCount;
        for (size_t j = 0; j < fileCount; j++) {
            std::string filename;
            size_t posCount;
            in >> filename >> posCount;
            std::vector<int> positions(posCount);
            for (size_t k = 0; k < posCount; k++) in >> positions[k];
            position_index[word][filename] = positions;
        }
    }

    return true;
}

void InvertedIndex::insertIndex(std::string &s, std::string &filename, int idx) {
    total_words[filename]++;
    position_index[s][filename].push_back(idx);
}

double InvertedIndex::tf(int termCountInDoc, int totalWordsInDoc) {
    return static_cast<double>(termCountInDoc) / totalWordsInDoc;
}

double InvertedIndex::idf(int total_files, int docsContainingTerm) {
    return log(static_cast<double>(total_files) / (1 + docsContainingTerm));
}

double InvertedIndex::tfidf(int termCount, int docLength, int docsWithTerm, int total_files) {
    return tf(termCount, docLength) * idf(total_files, docsWithTerm);
}

// for position search we check two files at a time
std::vector<int> intersectAdjacent(std::vector<int> &pos1, std::vector<int> &pos2) {
    std::vector<int> matches;
    int i = 0, j = 0;
    while (i < pos1.size() && j < pos2.size()) {
        if (pos1[i] + 1 == pos2[j]) {
            matches.push_back(pos2[j]); 
            i++; j++;
        } else if (pos1[i] + 1 < pos2[j]) {
            i++;
        } else {
            j++;
        }
    }
    return matches;
}

// finds if the exact phrase is present or not
bool InvertedIndex::phraseSearch(std::vector<std::string> &s) {
    if (s.empty()) {
        std::cout << "No phrase to search" << std::endl;
        return false;
    }

    // Every word must exist in the index
    for (auto &word : s) {
        if (position_index.find(word) == position_index.end()) {
            return false;
        }
    }

    std::unordered_map<std::string, std::vector<int>> candidates = position_index[s[0]];
    for (size_t i = 1; i < s.size(); i++) {
        std::unordered_map<std::string, std::vector<int>> next;
        auto &nextWordPositions = position_index[s[i]];
        for (auto &entry : candidates) {
            const std::string &filename = entry.first;
            auto it = nextWordPositions.find(filename);
            if (it == nextWordPositions.end()) continue;
            std::vector<int> pos1 = entry.second;
            std::vector<int> pos2 = it->second;
            std::vector<int> matched = intersectAdjacent(pos1, pos2);
            if (!matched.empty()) {
                next[filename] = matched;
            }
        }
        candidates = std::move(next);
        if (candidates.empty()) break; 
    }

    if (candidates.empty()) {
        return false;
    }

    std::vector<std::pair<std::string, int>> result;
    for (auto &entry : candidates) {
        result.push_back({entry.first, (int)entry.second.size()});
    }
    sort(result.begin(), result.end(), [](auto &a, auto &b) {
        return a.second > b.second;
    });
    std::cout << "Phrase search results (" << result.size() << " matches):\n";
    int num = 1;
    for (auto &r : result) {
        std::cout << num << ". " << r.first
                  << " (" << r.second << " occurrence(s))\n";
        num++;
    }
    return true;
}


// if the phrase isn't present we try to check if all the words of the query are present in a file
void InvertedIndex::findIndex(std::vector<std::string> &s, int total_files) {
    std::unordered_map<std::string, double> scores;      
    std::unordered_map<std::string, int> matchedTerms;
    for (int i = 0; i < s.size(); i++) {
        if (position_index.find(s[i]) == position_index.end()) continue;
        int docs = position_index[s[i]].size();
        for (auto it = position_index[s[i]].begin(); it != position_index[s[i]].end(); ++it) {
            double score = tfidf((it -> second).size(), total_words[it -> first], docs,total_files);
            scores[it -> first] += score;
            matchedTerms[it -> first]++;
        }
    }
    if (scores.empty()) {
        std::cout << "No Results" << std::endl;
        return;
    }
   
    std::vector<std::pair<std::string, double>> result;
    for (auto &entry : scores) {
        if (matchedTerms[entry.first] == (int)s.size()) {
            result.push_back(entry);
        }
    }

    if (result.empty()) {
        std::cout << "No Results" << std::endl;
        return;
    }

    sort(result.begin(), result.end(), [](auto &a, auto &b) {
        return a.second > b.second;
    });

    //std::cout << "Search results (" << result.size() << " matches):\n";
    for (size_t i = 0; i < result.size(); i++) {
        std::cout << (i + 1) << ". " << result[i].first
                   << " (score: " << std::fixed << std::setprecision(4) << result[i].second << ")\n";
    }
}

const std::unordered_map<std::string, int>&
InvertedIndex::getTotalWords() const {
    return total_words;
}