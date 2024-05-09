#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_set>
using namespace std;

// Trie Node
struct TrieNode 
{
    TrieNode* children[26];
    unordered_set<int> docIDs;

    TrieNode() {
        for (int i = 0; i < 26; i++) {
            children[i] = NULL;
        }
    } // Initialize children with null pointers
};

// Function to tokenize a string into words
vector<string> tokenize(const string& text) {
    vector<string> tokens;
    stringstream ss(text);
    string token;
    while (ss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

// Function to insert a word into the trie
void insertWord(TrieNode* root, const string& word, int docID) {
    TrieNode* curr = root;
    for (char c : word) {
        int index = c - 'a';
        if (curr->children[index] == NULL) 
        {
            curr->children[index] = new TrieNode(); // Create new node if it doesn't exist
        }
        curr = curr->children[index];
    }
    curr->docIDs.insert(docID);
}

// Function to build the inverted index using trie
void buildInvertedIndex(const string& filename, TrieNode* root, int& docID) {
    ifstream inFile(filename);
    if (!inFile) {
        cerr << "Unable to open file: " << filename << endl;
        return;
    }

    string word;
    while (inFile >> word) {
        insertWord(root, word, docID);
    }
    inFile.close();
}

int main() {
    // Build the inverted index
    TrieNode* root = new TrieNode();

    int docID = 0;
    string filename;

    cout << "Enter the filename(s) (separated by space) of the text file(s): ";
    getline(cin, filename);

    istringstream iss(filename);

    string file;
    while (iss >> file) {
        cout << file;
        buildInvertedIndex(file, root, docID);
        docID++;
    }

    // Example usage: print inverted index for a word
    string wordToSearch = "population";
    TrieNode* curr = root;
    for (char c : wordToSearch) {
        int index = c - 'a';
        if (!curr->children[index]) {
            cout << "Word not found in any document." << endl;
            return 0;
        }
        curr = curr->children[index];
    }

    cout << "Documents containing '" << wordToSearch << "':" << endl;
    for (int docID : curr->docIDs) {
        cout << "Document " << docID << endl;
    }

    return 0;
}