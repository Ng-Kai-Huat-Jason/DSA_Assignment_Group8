#include "Dictionary.h"
#include "Actor.h"
#include "Movie.h"
#include "AVLTree.h"

#include <cctype>
#include <sstream>
#include <fstream>
#include <stdexcept>

using namespace std;

namespace {
    // Helper function to trim whitespace from a string.
    string trim(const string& str) {
        size_t start = str.find_first_not_of(" \t");
        size_t end = str.find_last_not_of(" \t");
        return (start == string::npos || end == string::npos) ? "" : str.substr(start, end - start + 1);
    }

    // Helper function to parse a CSV line while preserving quotes.
    // This function toggles an inQuotes flag so that commas inside quotes are not treated as delimiters.
    vector<string> parseCSVLinePreserveQuotes(const string& line) {
        vector<string> fields;
        string current;
        bool inQuotes = false;
        for (size_t i = 0; i < line.size(); i++) {
            char c = line[i];
            if (c == '"') {
                // If inside quotes and next char is also a quote, add a literal quote.
                if (inQuotes && i + 1 < line.size() && line[i + 1] == '"') {
                    current.push_back('"');
                    i++; // skip escaped quote
                }
                else {
                    inQuotes = !inQuotes;
                    current.push_back(c); // Keep the quote so we know the field was quoted
                }
            }
            else if (c == ',' && !inQuotes) {
                fields.push_back(trim(current));
                current.clear();
            }
            else {
                current.push_back(c);
            }
        }
        fields.push_back(trim(current));
        return fields;
    }
} // end anonymous namespace

// --------------------
// Existing implementations
// --------------------

// Constructor for Dictionary
template <typename KeyType, typename ValueType>
Dictionary<KeyType, ValueType>::Dictionary() : size(0) {
    for (int i = 0; i < MAX_SIZE; ++i) {
        table[i] = nullptr;
    }
}

// Virtual Destructor for Dictionary
template <typename KeyType, typename ValueType>
Dictionary<KeyType, ValueType>::~Dictionary() {
    for (int i = 0; i < MAX_SIZE; ++i) {
        if (table[i] != nullptr) {
            delete table[i];
        }  
    }
}

// Custom hash function for string keys
template <typename KeyType, typename ValueType>
unsigned long Dictionary<KeyType, ValueType>::hash(const KeyType& key) const {
    unsigned long hashValue = 0;
    for (char c : key) {
        hashValue = (hashValue * 31 + c) % MAX_SIZE;
    }
    return hashValue;
}

// Inserts a new key value pair into the dictionary
// if the bucket's avl tree does not exist, it creates a new one
// checks for duplicates and inserts new kvp
template <typename KeyType, typename ValueType>
bool Dictionary<KeyType, ValueType>::add(const KeyType& key, ValueType* value) {
    if (!value) {
        cerr << "Error: Trying to add a null value for key: " << key << endl;
        return false;
    }

    unsigned long index = hash(key);  // Compute the hash bucket index

    // If the AVL tree for the bucket doesn't exist, create it
    if (table[index] == nullptr) {
        table[index] = new AVLTree<KeyValuePair<KeyType, ValueType>>();
    }

    // Create a KeyValuePair object
    KeyValuePair<KeyType, ValueType> kvp(key, value);

    // Check if the KeyValuePair already exists in the AVL tree
    auto found = table[index]->find(kvp);
    if (found != nullptr) {
        cerr << "Error: Duplicate key detected: " << key << endl;
        return false;  // Duplicate key, insertion fails
    }

    // Insert the KeyValuePair into the AVL tree
    table[index]->insert(kvp);
    size++;  // Increment the size of the dictionary
    return true;  // Successfully added
}


// Removes the key–value pair identified by key from the dictionary.
// It returns true if the removal was successful.
template <typename KeyType, typename ValueType>
bool Dictionary<KeyType, ValueType>::remove(const KeyType& key) {
    unsigned long index = hash(key);
    if (table[index] == nullptr)
        return false;

    KeyValuePair<KeyType, ValueType> kvp(key, nullptr);

    if (table[index]->find(kvp) == nullptr) {
        return false;  
    }

    table[index]->remove(kvp);
    size--;
    return true;
}

// Returns the ValueType* associated with the given key by searching in the AVL tree of that bucket
template <typename KeyType, typename ValueType>
ValueType* Dictionary<KeyType, ValueType>::get(const KeyType& key) const {
    unsigned long index = hash(key);
    if (table[index] == nullptr) {
        return nullptr;
    }

    KeyValuePair<KeyType, ValueType> kvp(key, nullptr);
    auto found = table[index]->find(kvp);  // Find the KeyValuePair in the AVL tree
    if (found != nullptr) {
        return found->value;  // Return the value (ValueType*)
    }

    return nullptr;  // Return nullptr if the key is not found
}


// Check if the dictionary is empty
template <typename KeyType, typename ValueType>
bool Dictionary<KeyType, ValueType>::isEmpty() const {
    return size == 0;
}

// Get the size of the dictionary
template <typename KeyType, typename ValueType>
int Dictionary<KeyType, ValueType>::getSize() const {
    return size;
}

// Print the dictionary (for debugging)
template <typename KeyType, typename ValueType>
void Dictionary<KeyType, ValueType>::print() const {
    for (int i = 0; i < MAX_SIZE; ++i) {
        if (table[i] != nullptr) {
            cout << "Bucket " << i << ":\n";
            table[i]->displayInOrder();
        }
    }
}

// Returns a vector of all values in the dictionary.
template <typename KeyType, typename ValueType>
vector<ValueType*> Dictionary<KeyType, ValueType>::getAllItems() const {
    vector<ValueType*> items;
    for (int i = 0; i < MAX_SIZE; ++i) {
        if (table[i] != nullptr) {
            vector<KeyValuePair<KeyType, ValueType>> bucketItems = table[i]->getAllItems();
            for (const auto& kvp : bucketItems) {
                items.push_back(kvp.value);
            }
        }
    }
    return items;
}

// Returns a vector of pointers to all nodes.
template <typename KeyType, typename ValueType>
vector<KeyValuePair<KeyType, ValueType>*> Dictionary<KeyType, ValueType>::getAllNodes() const {
    vector<KeyValuePair<KeyType, ValueType>*> nodes;
    for (int i = 0; i < MAX_SIZE; ++i) {
        if (table[i] != nullptr) {
            vector<KeyValuePair<KeyType, ValueType>> bucketItems = table[i]->getAllItems();
            for (const auto& kvp : bucketItems) {
                nodes.push_back(new KeyValuePair<KeyType, ValueType>(kvp.key, kvp.value));
            }
        }
    }
    return nodes;
}

// --------------------
// New CSV-related methods with improved movie title handling
// --------------------
// parseCSVLine: Splits a CSV line at commas and trims whitespace from each field.
// Note: This version does NOT handle commas within quotes.
static vector<string> parseCSVLine(const string& line) {
    vector<string> fields;
    string current;
    for (char c : line) {
        if (c == ',') {
            fields.push_back(trim(current));
            current.clear();
        }
        else {
            current.push_back(c);
        }
    }
    fields.push_back(trim(current));
    return fields;
}

template<>
bool Dictionary<string, Movie>::loadFromCSV(const string& fileName, bool isActor) {
    ifstream file(fileName);
    if (!file.is_open()) {
        cerr << "[Error] Failed to open " << fileName << " for reading." << endl;
        return false;
    }
    string line;
    getline(file, line); // Skip header
    while (getline(file, line)) {
        if (line.empty()) continue;

        vector<string> fields = parseCSVLinePreserveQuotes(line);
        if (fields.size() < 4) {
            cerr << "[Warning] Skipping invalid movie record: " << line << endl;
            continue;
        }

        string id = fields[0];
        string title = fields[1];
        bool titleWasQuoted = false;
        if (title.size() >= 2 && title.front() == '"' && title.back() == '"') {
            titleWasQuoted = true;
            title = title.substr(1, title.size() - 2);
        }
        string plot = fields[2];
        string year = fields[3];
        double rating = (fields.size() >= 5) ? stod(fields[4]) : 0.0;
        int noOfTimesRated = (fields.size() >= 6) ? stoi(fields[5]) : 0;

        Movie* newMovie = new Movie(id, title, plot, year);
        newMovie->rating = rating;
        newMovie->noOfTimesRated = noOfTimesRated;
        newMovie->titleWasQuoted = titleWasQuoted;
        if (!this->add(id, newMovie)) {
            cerr << "[Error] Duplicate movie ID: " << id << endl;
            delete newMovie;
        }
    }
    file.close();
    cout << "[Info] Data loaded successfully from " << fileName << endl;
    return true;
}

template<>
bool Dictionary<string, Actor>::loadFromCSV(const string& fileName, bool isActor) {
    ifstream file(fileName);
    if (!file.is_open()) {
        cerr << "[Error] Failed to open " << fileName << " for reading." << endl;
        return false;
    }
    string line;
    getline(file, line); // Skip header
    while (getline(file, line)) {
        if (line.empty()) continue;

        vector<string> fields = parseCSVLine(line);
        if (fields.size() < 3) {
            cerr << "[Warning] Skipping invalid actor record: " << line << endl;
            continue;
        }

        string id = fields[0];
        string name = fields[1];
        int birthYear = stoi(fields[2]);
        double rating = (fields.size() >= 4) ? stod(fields[3]) : 0.0;
        int noOfTimesRated = (fields.size() >= 5) ? stoi(fields[4]) : 0;

        Actor* newActor = new Actor(id, name, birthYear);
        newActor->rating = rating;
        newActor->noOfTimesRated = noOfTimesRated;
        if (!this->add(id, newActor)) {
            cerr << "[Error] Duplicate actor ID: " << id << endl;
            delete newActor;
        }
    }
    file.close();
    cout << "[Info] Data loaded successfully from " << fileName << endl;
    return true;
}
// Patches the CSV file based on the current dictionary state.
// For each record in the CSV, if the key exists in the dictionary,
// it rebuilds the record from the stored object's current data.
template <typename KeyType, typename ValueType>
bool Dictionary<KeyType, ValueType>::patchCSV(const string& fileName, bool isActor) {
    ifstream inFile(fileName);
    if (!inFile.is_open()) {
        cerr << "[Error] Unable to open " << fileName << " for patching." << endl;
        return false;
    }
    vector<string> lines;
    string header;
    getline(inFile, header); // Read header
    lines.push_back(header);
    string line;
    while (getline(inFile, line)) {
        if (line.empty())
            continue;
        vector<string> fields = parseCSVLine(line);
        if (fields.empty())
            continue;
        string key = fields[0];
        ValueType* obj = this->get(key);
        if (obj) {
            stringstream updatedLine;
            updatedLine << key << ",";
            if (isActor) {
                Actor* actorObj = dynamic_cast<Actor*>(obj);
                if (!actorObj) {
                    cerr << "[Error] Object with key " << key << " is not an Actor." << endl;
                    continue;
                }
                updatedLine << actorObj->name << ",";
                updatedLine << actorObj->birthYear << ",";
                updatedLine << actorObj->rating << ",";
                updatedLine << actorObj->noOfTimesRated;
            }
            else {
                Movie* movieObj = dynamic_cast<Movie*>(obj);
                if (!movieObj) {
                    cerr << "[Error] Object with key " << key << " is not a Movie." << endl;
                    continue;
                }
                // Ensure the movie title is written with quotes if it was originally quoted.
                string title = movieObj->title;
                if (movieObj->titleWasQuoted) {
                    if (!(title.front() == '"' && title.back() == '"')) {
                        title = "\"" + title + "\"";
                    }
                }
                updatedLine << title << ",";
                updatedLine << movieObj->plot << ",";
                updatedLine << movieObj->year << ",";
                updatedLine << movieObj->rating << ",";
                updatedLine << movieObj->noOfTimesRated;
            }
            lines.push_back(updatedLine.str());
        }
        else {
            lines.push_back(line);
        }
    }
    inFile.close();
    // Write patched data back to file.
    ofstream outFile(fileName, ios::out);
    if (!outFile.is_open()) {
        cerr << "[Error] Unable to open " << fileName << " for writing patched data." << endl;
        return false;
    }
    for (const string& l : lines) {
        outFile << l << "\n";
    }
    outFile.close();
    cout << "[Info] CSV file " << fileName << " patched successfully." << endl;
    return true;
}

// Explicit template instantiation
template class Dictionary<string, Actor>;
template class Dictionary<string, Movie>;
