#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <algorithm>
#include <ctime>
#include <omp.h>

using namespace std;

std::unordered_map<std::string, std::vector<int>> build_inverted_index(const vector<string>& files)
{
    std::unordered_map<string, std::vector<int>> invertedIndex;
    const int size = files.size();

    #pragma omp parallel for
    for (int i = 0; i < size; i++)
    {
        std::unordered_set<std::string> local_words;
        ifstream inFile(files[i].c_str());

        string word;
        while (inFile >> word) {
            local_words.insert(word);
        }

        inFile.close();
        std::cout << "Archivo " << to_string(i + 1) << " procesado exitosamente\n";

        #pragma omp critical
        {
            for (const auto& it: local_words) 
            {
                invertedIndex[it].push_back(i + 1);
            }
        }
        std::cout << "Archivo " << to_string(i + 1) << " procesado a map global\n";
        
    }

    return invertedIndex;
}

void search(const unordered_map<string, vector<int>>& invertedIndex, const string& query) 
{
    auto it = invertedIndex.find(query);
    if (it != invertedIndex.end()) {
        cout << "Documentos que contienen \"" << query << "\":" << endl;
        for (const auto& doc : it->second) {
            cout << " - " << doc << endl;
        }
    } else {
        cout << "Ningun documento tiene la palabra \"" << query << "\"" << endl;
    }
}

int main() {
    vector<string> files;
    for (int i = 0; i < 24; i++)
    {
        files.push_back("xdoc" + to_string(i + 1) + ".txt");
    }

    std::clock_t start = std::clock();
    std::unordered_map<std::string, std::vector<int>> invertedIndex = build_inverted_index(files);
    std::clock_t end = std::clock();
    std::cout << "Indice invertido completado en: " << (end - start) / (double)CLOCKS_PER_SEC << " seconds\n";
    string query;
    cout << "Enter a word to search for in the documents: ";
    while (cin >> query)
    {
        search(invertedIndex, query);
    }

    return 0;
}