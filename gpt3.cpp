#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <omp.h>
#include <unordered_map>
#include <unordered_set>
#include <ctime>
using namespace std;

std::unordered_map<std::string, std::unordered_set<int>> build_inverted_index(const std::vector<string>& files)
{
    const int numThreads = omp_get_max_threads();

    std::unordered_map<std::string, std::unordered_set<int>> inverted_index;

    const int size = files.size();
    for (int i = 0; i < size; i++)
    {
        ifstream file(files[i], ios::ate); // Open the file to determine its size
        if (!file.is_open()) {
            cerr << "Error opening file." << endl;
            continue;
        }

        const long long fileSize = file.tellg();
        file.close();

        const long long chunkSize = fileSize / numThreads;
        const string filename = files[i];

        // Parallel region
        #pragma omp parallel num_threads(numThreads)
        {
            int tid = omp_get_thread_num();
            long long start = tid * chunkSize;
            long long end_ = (tid == numThreads - 1) ? fileSize : (tid + 1) * chunkSize;
            long long chunkSize = end_ - start;

            ifstream file(filename);
            std::unordered_set<std::string> local_inverted_index;

            file.seekg(start);
            string word;

            long long end = start + chunkSize;
            
            if (end > 0) {
                file.ignore(end, '\n'); // Move to the beginning of the next line
            }

            while (file.tellg() < end && file >> word) 
            {
                local_inverted_index.insert(word);
            }
            file.close();
            # pragma omp critical
            for (const auto& a: local_inverted_index)
            {
                inverted_index[a].insert(i + 1);
            }
        }
        std::cout << "Archivo " << to_string(i + 1) << " procesado exitosamente\n";

    }

    return inverted_index;
}

void search(std::unordered_map<std::string, unordered_set<int>> inverted_index, std::string query)
{
    auto it = inverted_index.find(query);
    if (it != inverted_index.end()) {
        cout << "Documentos que contienen \"" << query << "\":" << endl;
        for (const auto& doc : it->second) {
            cout << " - " << doc << endl;
        }
    } else {
        cout << "Ningun documento tiene la palabra \"" << query << "\"" << endl;
    }
}

int main() {
    vector<string> files; // Add your file names here
    for (int i = 0; i < 10; i++)
    {
        files.push_back("zdoc" + to_string(i + 1) + ".txt");
    }
    std::clock_t start = std::clock();
    unordered_map<string, std::unordered_set<int>> invertedIndex = build_inverted_index(files);
    std::clock_t end = std::clock();
    std::cout << "Word count completed in " << (end - start) / (double)CLOCKS_PER_SEC << " seconds\n";
    std::cout << invertedIndex.size() << '\n';
    string query;
    cout << "Enter a word to search for in the documents: ";
    while (cin >> query)
    {
        search(invertedIndex, query);
    }
    
    return 0;
}
