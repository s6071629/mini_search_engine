#pragma once

#include<iostream>
#include<filesystem>
#include<unordered_map>
#include<unordered_set>
#include<vector>
#include<fstream>
#include<sstream>
#include<algorithm>
#include<cctype>
#include<cmath>
#include<iomanip>

//using namespace std;
namespace fs = std::filesystem;

class InvertedIndex{
private:
    std::unordered_map<std::string, std::unordered_map<std::string, int>> index;
    std::unordered_map<std::string, int> total_words;
    std::unordered_map<std::string, std::unordered_map<std::string, std::vector<int>>> position_index;

public:
    void insertIndex(std::string &s, std::string &filename, int idx);
    double tf(int termCountInDox, int totalWordsInDoc);
    double idf(int total_files, int docsContainingTerms);
    double tfidf(int termCount, int docLength, int docsWitTerm, int total_files);
    void findIndex(std::vector<std::string> &s, int total_files);
    bool phraseSearch(std::vector<std::string> &s);
    const std::unordered_map<std::string, std::unordered_map<std::string, int>>& getIndex() const;
    const std::unordered_map<std::string, int>& getTotalWords() const;

    void saveToCache(const std::string &path);
    bool loadFromCache(const std::string &path);
};

class ReadFiles {
private:
    std::string path = "documents";
    std::unordered_set<std::string> ignore = {"the", "is", "a", "an", "and", "or", "was", "do", "does",
    "on", "in", "at", "under", "after", "before"};
    int total_files = 0;

public:
    int getTotalFiles();
    void readFile(InvertedIndex &index);
    std::string computeFingerprint();
    void countFilesOnly();
};