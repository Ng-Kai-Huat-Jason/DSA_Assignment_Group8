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
    /*
        Trims leading and trailing whitespace from a string

        This function removes any leading and trailing spaces or tab characters
        from the given string

        Parameter - str: The string to trim
        Return - The trimmed string without leading or trailing whitespace
    */
    string trim(const string& str) {
        size_t start = str.find_first_not_of(" \t");
        size_t end = str.find_last_not_of(" \t");
        return (start == string::npos || end == string::npos) ? "" : str.substr(start, end - start + 1);
    }

    /*
        Parses a CSV line into fields while preserving quotes

        This function extracts individual fields from a CSV-formatted line,
        handling quoted values correctly. Quotes inside quoted fields are preserved

        Parameter - line: The CSV line to parse
        Return - A vector containing the extracted fields as strings
    */
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
}   

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

/*
    Computes the hash value for a given key

	This function calculates the hash value for a given key using a polynomial rolling hash function.

    Parameter - key: The key to be hashed
    Return - An unsigned long representing the hash index within the table
*/
template <typename KeyType, typename ValueType>
unsigned long Dictionary<KeyType, ValueType>::hash(const KeyType& key) const {
    unsigned long hashValue = 0;
    for (char c : key) {
        hashValue = (hashValue * 31 + c) % MAX_SIZE;
    }
    return hashValue;
}

/*
    Inserts a new key-value pair into the dictionary

    This function inserts a key-value pair into the dictionary.
    If the bucket's AVL tree does not exist, it creates a new one.
    It also checks for duplicate keys before insertion to prevent overwriting.

    Parameter - key: The key to be inserted
    Parameter - value: A pointer to the value associated with the key
    Return - True if insertion is successful, false if the key already exists
*/
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


/*
    Removes a key-value pair from the dictionary


    This function removes the KVP from the dictionary by finding the respective AVL tree bucket based on the hash value,
	and then removing the KVP from the AVL tree. The size of the dictionary is also decremented

    Parameter - key: The key associated with the value to be removed
    Return - True if the key was successfully removed, false if the key was not found
*/
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

/*
    Retrieves the value associated with a given key

    This function searches the given key in the respective AVL tree bucket in the dictionary.
    If the key is found, the value is returned , if not it returns a nullptr

    Parameter - key: The key whose associated value is to be retrieved
    Return - A pointer to the associated value (ValueType*) if found, otherwise nullptr
*/
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


/*
    Checks if the dictionary is empty

	This functions checks if the dictionary is empty by checking the size of the dictionary is equal to 0

    Return - True if the dictionary is empty, false otherwise
*/
template <typename KeyType, typename ValueType>
bool Dictionary<KeyType, ValueType>::isEmpty() const {
    return size == 0;
}

/*
    Retrieves the number of KVPs in the dictionary

    This function returns the total number of KVPs currently stored

    Return - The number of elements in the dictionary
*/

template <typename KeyType, typename ValueType>
int Dictionary<KeyType, ValueType>::getSize() const {
    return size;
}

/*
    Prints the contents of the dictionary

    This function iterates through all hash table buckets and prints the key-value
    pairs stored in each AVL tree. It displays only non-empty buckets.

    Return - Void (Outputs dictionary contents to the console)
*/
template <typename KeyType, typename ValueType>
void Dictionary<KeyType, ValueType>::print() const {
    for (int i = 0; i < MAX_SIZE; ++i) {
        if (table[i] != nullptr) {
            cout << "Bucket " << i << ":\n";
            table[i]->displayInOrder();
        }
    }
}

/*
    Retrieves all values stored in the dictionary

    This function iterates through all the hash table buckets and collects all values from the AVL trees into a vector

    Return - A vector containing pointers to all values stored in the dictionary
*/
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

/*
    Retrieves all key-value pairs stored in the dictionary

    This function iterates through all the hash table buckets and collects all key-value pairs from the AVL trees into a vector

    Return - A vector containing pointers to all key-value pairs in the dictionary
*/
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


/*
    Parses a CSV line into individual fields

    This functions takes a CSV line and splits it into seperate fields while trimming whitespaces

    Parameter - line: The input CSV line as a string
    Return - A vector of strings containing the parsed fields
*/
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

/*
    Loads movie data from a CSV file into the dictionary

	This function reads the movie.CSV and adds it to the dictionary, validating duplicate entries and missing fields

    Parameter - fileName: The name of the CSV file to load data from
    Parameter - isActor: A boolean flag indicating whether the file contains actor data (unused in this function)
    Return - True if the file is successfully loaded, false otherwise
*/
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


/*
    Loads actor data from a CSV file into the dictionary

    This function reads the actors.CSV and adds it to the dictionary, validating duplicate entries and missing fields

    Parameter - fileName: The name of the CSV file to load data from
    Parameter - isActor: A boolean flag indicating whether the file contains actor data (unused in this function)
    Return - True if the file is successfully loaded, false otherwise
*/
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

/*
    Updates (patches) the CSV file with the latest data from the dictionary

    This functions reads the CSV files and update the records based on the current dictionary. 
	It updated the data instead appending new one and make sure the data is correctly formatted

    Parameter - fileName: The name of the CSV file to be patched
    Parameter - isActor: A boolean flag indicating whether the file contains actor data
    Return - True if the file is successfully updated, false otherwise
*/
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
