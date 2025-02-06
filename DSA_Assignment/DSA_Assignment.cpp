// Student 1: Ng Kai Huat Jason(S10262552)
// Student 2: Law Ming Qi (S10257808)

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <ctime>         // For getting the current year dynamically
#include <limits>        // For numeric_limits
#include <sys/stat.h>    // For fileExists
#include <vector>

#include "Actor.h"
#include "Movie.h"

#include "Dictionary.h"
#include "AVLTree.h"
#include "Graph.h"


using namespace std;

// Global vectors to track new entries
vector<Actor*> newActors;
vector<Movie*> newMovies;
vector<pair<string, string>> newCasts; // Each pair holds (ActorID, MovieID)

// Global data structures
Dictionary<string, Actor> actorDictionary;
Dictionary<string, Movie> movieDictionary;
AVLTree<Actor> actorAVLTree;
AVLTree<Movie> movieAVLTree;
Graph<string> actorMovieGraph;

// Menu Functions
//======================//

// Displays the main menu.
void mainMenu() {
    cout << "=====================================" << endl;
    cout << "        Movie Information Helper     " << endl;
    cout << "=====================================" << endl;
    cout << "(1) Enter as Admin" << endl;
    cout << "(2) Enter as User" << endl;
    cout << "(3) Store updates to CSVs" << endl;
    cout << "=====================================" << endl;
    cout << "Enter your choice: ";
}

// Displays the admin menu.
void adminMenu() {
    cout << "-------------------------------------" << endl;
    cout << "             Admin Menu              " << endl;
    cout << "-------------------------------------" << endl;
    cout << "(1) Add new actor" << endl;
    cout << "(2) Add new movie" << endl;
    cout << "(3) Add an actor to a movie" << endl;
    cout << "(4) Update actor/movie details" << endl;
    cout << "(5) Go back to Main Menu" << endl;
    cout << "-------------------------------------" << endl;
    cout << "Enter your choice: ";
}

// Displays the user menu.
void userMenu() {
    cout << "=====================================" << endl;
    cout << "             User Menu                " << endl;
    cout << "=====================================" << endl;
    cout << "(1) Display actors between a certain age in ascending order" << endl;
    cout << "(2) Display movies made within the past 3 years in ascending order" << endl;
    cout << "(3) Display all movies an actor starred in (in alphabetical order)" << endl;
    cout << "(4) Display all the actors in a particular movie (in alphabetical order)" << endl;
    cout << "(5) Display a list of all actors that a particular actor knows" << endl;
	cout << "(6) Rate a Movie or Actor" << endl;
	cout << "(7) Display Top 10 rating for Actor or Movie" << endl;
    cout << "(8) Go back to Main Menu" << endl;
    cout << "=====================================" << endl;
    cout << "Enter your choice: ";
}

//Helper Functions
//======================//

// Trim leading and trailing whitespace.
string trim(const string& str) {
    size_t start = str.find_first_not_of(" \t");
    size_t end = str.find_last_not_of(" \t");
    return (start == string::npos || end == string::npos) ? "" : str.substr(start, end - start + 1);
}

// Check if a file exists.
bool fileExists(const string& filename) {
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}

// Prompt the user until a non-empty, trimmed input is provided.
string getNonEmptyInput(const string& prompt) {
    string input;
    while (true) {
        cout << prompt;
        getline(cin, input);
        size_t start = input.find_first_not_of(" \t");
        size_t end = input.find_last_not_of(" \t");
        if (start != string::npos && end != string::npos)
            input = input.substr(start, end - start + 1);
        else
            input = "";
        if (!input.empty()) {
            return input;
        }
        else {
            cout << "[Error] Input cannot be empty. Please try again.\n";
        }
    }
}

// Helper function that parses a CSV line into fields while respecting quotes.
vector<string> parseCSVLine(const string& line) {
    vector<string> fields;
    string current;
    bool inQuotes = false;
    for (size_t i = 0; i < line.size(); i++) {
        char c = line[i];
        if (c == '"') {
            // If inside quotes and next character is also a quote, add a single quote.
            if (inQuotes && i + 1 < line.size() && line[i + 1] == '"') {
                current.push_back('"');
                i++; // skip the next quote
            }
            else {
                inQuotes = !inQuotes;
            }
        }
        else if (c == ',' && !inQuotes) {
            // Field delimiter found (and not inside quotes)
            fields.push_back(current);
            current.clear();
        }
        else {
            current.push_back(c);
        }
    }
    // Push the last field.
    fields.push_back(current);

    // Trim each field and remove surrounding quotes if present.
    for (auto& field : fields) {
        field = trim(field);
        if (field.size() >= 2 && field.front() == '"' && field.back() == '"') {
            field = field.substr(1, field.size() - 2);
        }
    }
    return fields;
}

// Parse a CSV line into fields but DO NOT remove surrounding quotes.
vector<string> parseCSVLinePreserveQuotes(const string& line) {
    vector<string> fields;
    string current;
    bool inQuotes = false;
    for (size_t i = 0; i < line.size(); i++) {
        char c = line[i];
        if (c == '"') {
            // If inside quotes and next char is also a quote, add one literal quote.
            if (inQuotes && i + 1 < line.size() && line[i + 1] == '"') {
                current.push_back('"');
                i++; // skip the escaped quote
            }
            else {
                inQuotes = !inQuotes;
                // Instead of discarding the quote, we keep it.
                current.push_back(c);
            }
        }
        else if (c == ',' && !inQuotes) {
            fields.push_back(current);
            current.clear();
        }
        else {
            current.push_back(c);
        }
    }
    fields.push_back(current);
    // Trim whitespace from each field (but do not remove the surrounding quotes).
    for (auto& field : fields) {
        field = trim(field);
    }
    return fields;
}

// Generalized partition function that accepts a comparison function.
template <typename T, typename Compare>
int partition(vector<T>& arr, int low, int high, Compare comp) {
    T pivot = arr[high];
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (comp(arr[j], pivot)) { // if arr[j] should come before pivot
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);
    return i + 1;
}

// Generalized quick sort that accepts a comparison function.
template <typename T, typename Compare>
void quickSort(vector<T>& arr, int low, int high, Compare comp) {
    if (low < high) {
        int pi = partition(arr, low, high, comp);
        quickSort(arr, low, pi - 1, comp);
        quickSort(arr, pi + 1, high, comp);
    }
}


// Removes duplicate strings from a sorted vector.
void removeDuplicates(vector<string>& arr) {
    // If the vector is empty, there's nothing to do.
    if (arr.empty())
        return;

    // Create a temporary vector to hold unique items.
    vector<string> temp;

    // Always include the first element.
    temp.push_back(arr[0]);

    // Iterate through the vector starting from the second element.
    for (size_t i = 1; i < arr.size(); ++i) {
        // If the current element is different from the last unique element added,
        // add it to the temporary vector.
        if (arr[i] != temp.back()) {
            temp.push_back(arr[i]);
        }
    }

    // Replace the original vector with the vector of unique elements.
    arr = temp;
}

// For Rating Feature
// ======================//
// Quick sort helper for Actor* based on rating (descending)
int partitionActors(vector<Actor*>& arr, int low, int high) {
    Actor* pivot = arr[high]; // pivot actor
    int i = low - 1;
    for (int j = low; j < high; j++) {
        // If the current actor's rating is greater than or equal to the pivot's rating
        if (arr[j]->rating >= pivot->rating) {
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);
    return i + 1;
}

void quickSortActors(vector<Actor*>& arr, int low, int high) {
    if (low < high) {
        int pi = partitionActors(arr, low, high);
        quickSortActors(arr, low, pi - 1);
        quickSortActors(arr, pi + 1, high);
    }
}

// Quick sort helper for Movie* based on rating (descending)
int partitionMoviesRating(vector<Movie*>& arr, int low, int high) {
    // Use the rating of the movie at index high as the pivot.
    double pivot = arr[high]->rating;
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (arr[j]->rating >= pivot) { // sort in descending order
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);
    return i + 1;
}

void quickSortMoviesRating(vector<Movie*>& arr, int low, int high) {
    if (low < high) {
        int pi = partitionMoviesRating(arr, low, high);
        quickSortMoviesRating(arr, low, pi - 1);
        quickSortMoviesRating(arr, pi + 1, high);
    }
}



//CSV Functions to load from csv
//======================//

// Load actors from CSV
void loadActorsFromCSV(const string& fileName) {
    ifstream file(fileName);
    if (!file.is_open()) {
        cerr << "[Error] Failed to open " << fileName << endl;
        return;
    }
    string line;
    getline(file, line); // Skip header
    while (getline(file, line)) {
        // Use a stringstream to parse comma-separated fields.
        stringstream ss(line);
        string id, name, birthYearStr, ratingStr, timesRatedStr;
        getline(ss, id, ',');
        getline(ss, name, ',');
        getline(ss, birthYearStr, ',');
        // If there are extra fields, try to get rating and noOfTimesRated.
        if (!getline(ss, ratingStr, ',')) {
            ratingStr = "0";  // default rating if missing
        }
        if (!getline(ss, timesRatedStr, ',')) {
            timesRatedStr = "0";  // default noOfTimesRated if missing
        }
        // Remove surrounding quotes from name if they exist.
        if (!name.empty() && name.front() == '"') name.erase(0, 1);
        if (!name.empty() && name.back() == '"') name.erase(name.size() - 1);
        try {
            int birthYear = stoi(birthYearStr);
            double rating = stod(ratingStr);
            int noOfTimesRated = stoi(timesRatedStr);
            Actor* newActor = new Actor(id, name, birthYear);
            // Set new fields.
            newActor->rating = rating;
            newActor->noOfTimesRated = noOfTimesRated;
            if (!id.empty() && !name.empty() && birthYear > 0) {
                if (!actorDictionary.add(id, newActor)) {
                    cerr << "[Error] Duplicate actor ID: " << id << endl;
                    delete newActor;
                }
            }
            else {
                cerr << "[Error] Invalid data for actor: " << line << endl;
                delete newActor;
            }
        }
        catch (const exception& e) {
            cerr << "[Error] Parsing actor data: " << line << " (" << e.what() << ")" << endl;
        }
    }
    file.close();
    cout << "[Info] Actors loaded successfully from " << fileName << endl;
}

// Load movies from CSV without removing quotation marks for the Title.
void loadMoviesFromCSV(const string& fileName) {
    ifstream file(fileName);
    if (!file.is_open()) {
        cout << "[Error] Failed to open " << fileName << endl;
        return;
    }
    string line;
    getline(file, line); // Skip header line

    while (getline(file, line)) {
        if (line.empty())
            continue;

        // Use our custom parser that preserves quotes.
        vector<string> fields = parseCSVLinePreserveQuotes(line);
        // Expect at least four fields: id, title, plot, and year.
        if (fields.size() < 4) {
            cout << "[Warning] Skipping invalid movie record: " << line << endl;
            continue;
        }

        string id = trim(fields[0]);
        // Do NOT remove the quotes from the title field.
        string title = fields[1];  // leave as-is (with quotes if present)
        string plot = trim(fields[2]);
        string yearStr = trim(fields[3]);

        // Default values if not provided.
        string ratingStr = "0";
        string timesRatedStr = "0";
        if (fields.size() >= 5)
            ratingStr = trim(fields[4]);
        if (fields.size() >= 6)
            timesRatedStr = trim(fields[5]);

        if (id.empty() || title.empty() || yearStr.empty()) {
            cout << "[Warning] Skipping invalid movie record: " << line << endl;
            continue;
        }
        Movie* newMovie = new Movie(id, title, plot, yearStr);
        try {
            newMovie->rating = stod(ratingStr);
            newMovie->noOfTimesRated = stoi(timesRatedStr);
        }
        catch (...) {
            newMovie->rating = 0;
            newMovie->noOfTimesRated = 0;
        }
        if (!movieDictionary.add(id, newMovie)) {
            delete newMovie; // Prevent memory leak if already exists.
        }
        else {
            movieAVLTree.insert(newMovie);
        }
    }
    file.close();
    cout << "[Info] Movies loaded successfully from " << fileName << endl;
}


// Load cast relationships from CSV.
void loadCastsFromCSV(const string& fileName) {
    ifstream file(fileName);
    if (!file.is_open()) {
        cout << "[Error] Failed to open " << fileName << endl;
        return;
    }
    string line;
    getline(file, line); // Skip header
    while (getline(file, line)) {
        stringstream ss(line);
        string actorId, movieId;
        getline(ss, actorId, ',');
        getline(ss, movieId, ',');
        if (actorId.empty() || movieId.empty()) {
            cout << "[Warning] Skipping invalid cast record: " << line << endl;
            continue;
        }
        Actor* actor = actorDictionary.get(actorId);
        Movie* movie = movieDictionary.get(movieId);
        if (!actor || !movie) {
            cout << "[Warning] Actor or Movie not found for IDs (" << actorId << ", " << movieId << "). Skipping record.\n";
            continue;
        }
        actor->addMovie(movie);
        movie->addActor(actor);
        if (!actorMovieGraph.nodeExists(actor->name))
            actorMovieGraph.addNode(actor->name);
        if (!actorMovieGraph.nodeExists(movie->title))
            actorMovieGraph.addNode(movie->title);
        actorMovieGraph.addEdge(actor->name, movie->title);
    }
    file.close();
    cout << "[Info] Casts loaded successfully from " << fileName << endl;
}


// CSV Functons to store data and update
//------------------------------//

// Append new actor, movie, and cast records to their CSV files.
void appendNewDataToCsv() {
    // Append new actors
    {
        string filename = "../actors.csv";
        ofstream outFile;
        // If the file doesn't exist, create it and write a header.
        if (!fileExists(filename)) {
            outFile.open(filename, ios::out);
            if (outFile.is_open()) {
                outFile << "ActorID,Name,BirthYear,Rating,NoOfTimesRated\n";
                outFile.close();
            }
        }
        outFile.open(filename, ios::app);
        if (outFile.is_open()) {
            for (const Actor* actor : newActors) {
                outFile << actor->id << "," << actor->name << "," << actor->birthYear << ","
                    << actor->rating << "," << actor->noOfTimesRated << "\n";
            }
            outFile.close();
            cout << "[Info] New actors appended successfully.\n";
        }
        else {
            cout << "[Error] Unable to open " << filename << " for appending new actors.\n";
        }
    }

    // Append new movies
    {
        string filename = "../movies.csv";
        ofstream outFile;
        if (!fileExists(filename)) {
            outFile.open(filename, ios::out);
            if (outFile.is_open()) {
                outFile << "MovieID,Title,Plot,Year,Rating,NoOfTimesRated\n";
                outFile.close();
            }
        }
        outFile.open(filename, ios::app);
        if (outFile.is_open()) {
            for (const Movie* movie : newMovies) {
                // Note: No extra quotes added.
                outFile << movie->id << "," << movie->title << "," << movie->plot << ","
                    << movie->year << "," << movie->rating << "," << movie->noOfTimesRated << "\n";
            }
            outFile.close();
            cout << "[Info] New movies appended successfully.\n";
        }
        else {
            cout << "[Error] Unable to open " << filename << " for appending new movies.\n";
        }
    }

    // Append new cast relationships (unchanged).
    {
        string filename = "../cast.csv";
        ofstream outFile;
        if (!fileExists(filename)) {
            outFile.open(filename, ios::out);
            if (outFile.is_open()) {
                outFile << "ActorID,MovieID\n";
                outFile.close();
            }
        }
        outFile.open(filename, ios::app);
        if (outFile.is_open()) {
            for (const auto& castPair : newCasts) {
                outFile << castPair.first << "," << castPair.second << "\n";
            }
            outFile.close();
            cout << "[Info] New cast relationships appended successfully.\n";
        }
        else {
            cout << "[Error] Unable to open " << filename << " for appending new casts.\n";
        }
    }
}

// Patch (update) the actors CSV: update only changed records.
void patchActorsCSV() {
    string filename = "../actors.csv";
    ifstream inFile(filename);
    if (!inFile.is_open()) {
        cout << "[Error] Unable to open " << filename << " for patching actors.\n";
        return;
    }
    vector<string> lines;
    string header;
    getline(inFile, header); // Read header
    lines.push_back(header);
    string line;
    while (getline(inFile, line)) {
        stringstream ss(line);
        string id;
        getline(ss, id, ',');
        id = trim(id);
        Actor* actor = actorDictionary.get(id);
        if (actor) {
            // Rebuild record line using current dictionary data.
            string updatedLine = actor->id + "," + actor->name + "," + to_string(actor->birthYear) +
                "," + to_string(actor->rating) + "," + to_string(actor->noOfTimesRated);
            lines.push_back(updatedLine);
        }
        else {
            lines.push_back(line);
        }
    }
    inFile.close();
    ofstream outFile(filename, ios::out);
    if (outFile.is_open()) {
        for (const string& l : lines)
            outFile << l << "\n";
        outFile.close();
        cout << "[Info] Actors CSV patched successfully.\n";
    }
    else {
        cout << "[Error] Unable to open " << filename << " for writing.\n";
    }
}

// Patch (update) the movies CSV: update only changed records.
void patchMoviesCSV() {
    string filename = "../movies.csv";
    ifstream inFile(filename);
    if (!inFile.is_open()) {
        cout << "[Error] Unable to open " << filename << " for patching movies.\n";
        return;
    }
    vector<string> lines;
    string header;
    getline(inFile, header); // Read header
    lines.push_back(header);
    string line;
    while (getline(inFile, line)) {
        stringstream ss(line);
        string id;
        getline(ss, id, ',');
        id = trim(id);
        Movie* movie = movieDictionary.get(id);
        if (movie) {
            // Rebuild record line using current dictionary data.
            // Use movie->year as is (without trim) so the year digits are preserved.
            string updatedLine = movie->id + "," + movie->title + "," + movie->plot + "," + movie->year +
                "," + to_string(movie->rating) + "," + to_string(movie->noOfTimesRated);
            lines.push_back(updatedLine);
        }
        else {
            lines.push_back(line);
        }
    }
    inFile.close();
    ofstream outFile(filename, ios::out);
    if (outFile.is_open()) {
        for (const string& l : lines)
            outFile << l << "\n";
        outFile.close();
        cout << "[Info] Movies CSV patched successfully.\n";
    }
    else {
        cout << "[Error] Unable to open " << filename << " for writing.\n";
    }
}

// Combined function to store (patch and append) all CSV data.
void storeDataToCsv() {
    cout << "[Info] Storing data to CSV files...\n";
	appendNewDataToCsv();
    patchActorsCSV();
    patchMoviesCSV();
    cout << "[Info] Data storage to CSV files completed.\n";
}


// Feature Functions
//------------------------------//

// Add a new actor.
void addActor() {
	// Get actor details from user input.
    string id, name;
    int birthYear;
    id = getNonEmptyInput("Enter Actor ID: ");
    name = getNonEmptyInput("Enter Actor Name: ");
    while (true) {
        cout << "Enter Actor Birth Year: ";
        cin >> birthYear;
        if (cin.fail() || birthYear <= 0) {
			// Clear error flags and ignore the rest of the input buffer.
            cout << "[Error] Invalid birth year. Please enter a positive integer.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        else {
			// Clear the rest of the input buffer.
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            break;
        }
    }
	// Create a new Actor and add it to dictionary, AVL tree, and graph.

    Actor* newActor = new Actor(id, name, birthYear);
    if (actorDictionary.add(id, newActor)) {
        cout << "[Success] Actor \"" << name << "\" (ID: " << id << ") added to Dictionary successfully!\n";
        newActors.push_back(newActor);
    }
    else {
        cout << "[Error] Actor with ID \"" << id << "\" already exists in Dictionary.\n";
        delete newActor;
        return;
    }
    actorAVLTree.insert(newActor);
    cout << "[Success] Actor \"" << name << "\" added to AVL Tree successfully!\n";
    actorMovieGraph.addNode(name);
    cout << "[Success] Actor \"" << name << "\" added to graph successfully!\n";
}

// Add a new movie.
void addMovie() {
    string id, title, plot, year;
    id = getNonEmptyInput("Enter Movie ID: ");
    title = getNonEmptyInput("Enter Movie Title: ");
    plot = getNonEmptyInput("Enter Movie Plot: ");
    while (true) {
        year = getNonEmptyInput("Enter Movie Year: ");
        bool isValidYear = true;
        for (char ch : year) {
            if (!isdigit(ch)) {
                isValidYear = false;
                break;
            }
        }
        if (isValidYear)
            break;
        else
            cout << "[Error] Invalid year. Please enter a year containing only digits.\n";
    }
    Movie* newMovie = new Movie(id, title, plot, year);
    if (movieDictionary.add(id, newMovie)) {
        cout << "[Success] Movie \"" << title << "\" (ID: " << id << ") added to Dictionary successfully!\n";
        newMovies.push_back(newMovie);
    }
    else {
        cout << "[Error] Movie with ID \"" << id << "\" already exists in Dictionary.\n";
        delete newMovie;
        return;
    }
    movieAVLTree.insert(newMovie);
    cout << "[Success] Movie \"" << title << "\" added to AVL Tree successfully!\n";
    actorMovieGraph.addNode(title);
    cout << "[Success] Movie \"" << title << "\" added to graph successfully!\n";
}

// Add an actor to a movie.
void addActorToMovie() {
    string actorId, movieId;
    actorId = getNonEmptyInput("Enter Actor ID: ");
    movieId = getNonEmptyInput("Enter Movie ID: ");
    Actor* actor = actorDictionary.get(actorId);
    Movie* movie = movieDictionary.get(movieId);
    if (actor && movie) {
        bool isAlreadyCast = false;
        for (const Movie* m : actor->movies) {
            if (m->id == movieId) {
                isAlreadyCast = true;
                break;
            }
        }
        if (isAlreadyCast) {
            cout << "[Error] Actor \"" << actor->name << "\" is already part of the cast for movie \"" << movie->title << "\".\n";
            return;
        }
        if (!actorMovieGraph.nodeExists(actor->name))
            actorMovieGraph.addNode(actor->name);
        if (!actorMovieGraph.nodeExists(movie->title))
            actorMovieGraph.addNode(movie->title);
        actorMovieGraph.addEdge(actor->name, movie->title);
        actor->addMovie(movie);
        movie->addActor(actor);
        newCasts.emplace_back(actorId, movieId);
        cout << "[Success] Actor \"" << actor->name << "\" has been added to movie \"" << movie->title << "\" as part of the cast.\n";
    }
    else {
        cout << "[Error] Invalid Actor ID or Movie ID.\n";
    }
}

// Update details of an actor or a movie.h
void updateDetails() {
    int updateChoice;
    cout << "Update: (1) Actor Details, (2) Movie Details: ";
    cin >> updateChoice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    if (updateChoice == 1) {
        string actorId = getNonEmptyInput("Enter Actor ID to update: ");
        Actor* actor = actorDictionary.get(actorId);
        if (actor) {
            string oldName = actor->name;
            string newName = getNonEmptyInput("Enter new Name (current: " + actor->name + "): ");
            int newBirthYear;
            while (true) {
                cout << "Enter new Birth Year (current: " << actor->birthYear << "): ";
                cin >> newBirthYear;
                if (cin.fail() || newBirthYear <= 0) {
                    cout << "[Error] Invalid birth year. Please enter a positive integer.\n";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
                else {
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    break;
                }
            }
            actor->name = newName;
            actor->birthYear = newBirthYear;
            cout << "[Success] Actor details updated successfully!\n";
            actorAVLTree.insert(actor);
            cout << "[Success] Actor \"" << newName << "\" added to AVL Tree successfully!\n";
            if (oldName != actor->name) {
                actorMovieGraph.removeNode(oldName);
                actorMovieGraph.addNode(actor->name);
                for (Movie* movie : actor->movies) {
                    actorMovieGraph.addEdge(actor->name, movie->title);
                }
                cout << "[Success] Graph updated: Actor \"" << oldName << "\" is now \"" << actor->name << "\".\n";
            }
        }
        else {
            cout << "[Error] Actor not found.\n";
        }
    }
    else if (updateChoice == 2) {
        string movieId = getNonEmptyInput("Enter Movie ID to update: ");
        Movie* movie = movieDictionary.get(movieId);
        if (movie) {
            string oldTitle = movie->title;
            string newTitle = getNonEmptyInput("Enter new Title (current: " + movie->title + "): ");
            string newPlot = getNonEmptyInput("Enter new Plot (current: " + movie->plot + "): ");
            int newYear;
            while (true) {
                cout << "Enter new Year (current: " << movie->year << "): ";
                cin >> newYear;
                if (cin.fail() || newYear <= 0) {
                    cout << "[Error] Invalid year. Please enter a positive integer.\n";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
                else {
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    break;
                }
            }
            movie->title = newTitle;
            movie->plot = newPlot;
            movie->year = to_string(newYear);
            cout << "[Success] Movie details updated successfully!\n";
            movieAVLTree.insert(movie);
            cout << "[Success] Movie \"" << newTitle << "\" added to AVL Tree successfully!\n";
            if (oldTitle != movie->title) {
                actorMovieGraph.removeNode(oldTitle);
                actorMovieGraph.addNode(movie->title);
                for (Actor* actor : movie->actors) {
                    actorMovieGraph.addEdge(actor->name, movie->title);
                }
                cout << "[Success] Graph updated: Movie \"" << oldTitle << "\" is now \"" << movie->title << "\".\n";
            }
        }
        else {
            cout << "[Error] Movie not found.\n";
        }
    }
    else {
        cout << "[Error] Invalid choice.\n";
    }
}

// Display movies from a user-specified year covering the past 3 years.
void displayRecentMovies() {
    // Prompt the user to enter a base year.
    int baseYear;
    while (true) {
        cout << "Enter a year : ";
        cin >> baseYear;
        if (cin.fail() || baseYear <= 0) {
            cout << "[Error] Invalid year. Please enter a valid positive integer.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            break;
        }
    }

    // Calculate the lower bound (baseYear - 3) for filtering.
    int lowerBound = baseYear - 3;

    // Retrieve all movies from the dictionary.
    vector<Movie*> allMovies = movieDictionary.getAllItems();
    vector<Movie*> recentMovies;

    // Filter movies that were made between lowerBound and Year inputted (inclusive).
    for (Movie* movie : allMovies) {
        int movieYear = movie->getYearAsInt();
        if (movieYear > 0 && movieYear >= lowerBound && movieYear <= baseYear) {
            recentMovies.push_back(movie);
        }
    }

    // Sort the filtered movies using quick sort.
    if (!recentMovies.empty()) {
        quickSort(recentMovies, 0, recentMovies.size() - 1,
            [](Movie* a, Movie* b) {
                return a->getYearAsInt() <= b->getYearAsInt();
            }
        );
    }

    // Display the filtered and sorted movies.
    cout << "\n=====================================" << endl;
    cout << "Recent Movies (From " << lowerBound << " to " << baseYear << ")" << endl;
    cout << "=====================================" << endl;
    if (recentMovies.empty()) {
        cout << "[Info] No movies found in the specified range (" << lowerBound << " to " << baseYear << ").\n";
    }
    else {
        for (const Movie* movie : recentMovies) {
            cout << "Title: " << movie->title << "\nYear: " << movie->year
                << "\nPlot: " << movie->plot << "\n-------------------------\n";
        }
    }
}


// Display actors within a specified age range.
void displayActorsByAgeRange() {
    int x, y;
    time_t t = time(nullptr);
    tm currentTime;
    localtime_s(&currentTime, &t);
    int currentYear = 1900 + currentTime.tm_year;
    while (true) {
        cout << "Enter the lower age limit (x): ";
        cin >> x;
        if (cin.fail() || x < 0) {
            cout << "[Error] Invalid age. Please enter a non-negative integer.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            break;
        }
    }
    while (true) {
        cout << "Enter the upper age limit (y): ";
        cin >> y;
        if (cin.fail() || y < x) {
            cout << "[Error] Invalid age. Please enter an integer greater than or equal to " << x << ".\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            break;
        }
    }
    vector<Actor*> filteredActors;
    vector<Actor*> allActors = actorDictionary.getAllItems();
    for (Actor* actor : allActors) {
        int age = currentYear - actor->birthYear;
        if (age >= x && age <= y) {
            filteredActors.push_back(actor);
        }
    }
    for (size_t i = 0; i < filteredActors.size(); ++i) {
        for (size_t j = i + 1; j < filteredActors.size(); ++j) {
            int ageI = currentYear - filteredActors[i]->birthYear;
            int ageJ = currentYear - filteredActors[j]->birthYear;
            if (ageI > ageJ) {
                Actor* temp = filteredActors[i];
                filteredActors[i] = filteredActors[j];
                filteredActors[j] = temp;
            }
        }
    }
    cout << "\n=====================================" << endl;
    cout << "Actors Aged Between " << x << " and " << y << endl;
    cout << "=====================================" << endl;
    if (filteredActors.empty()) {
        cout << "[Info] No actors found in the specified age range.\n";
    }
    else {
        for (const Actor* actor : filteredActors) {
            cout << "Name: " << actor->name << ", Age: " << (currentYear - actor->birthYear) << "\n-------------------------\n";
        }
    }
}

// Display all movies an actor has starred in.
void getMoviesByActor() {
    string actorName;
    cout << "Enter actor name: ";
    getline(cin, actorName);
    actorName = trim(actorName);
    int actorIndex = actorMovieGraph.getNodeIndex(actorName);
    if (actorIndex == -1) {
        cout << "\n[Error] Actor \"" << actorName << "\" is not in any movie.\n";
        return;
    }
    vector<string> movies = actorMovieGraph.getNeighbors(actorName);
    if (movies.empty()) {
        cout << "\n[Info] Actor \"" << actorName << "\" exists but has not starred in any movie.\n";
    }
    else {
        // Use quick sort to sort movie titles alphabetically.
        quickSort(movies, 0, movies.size() - 1,
            [](const string& a, const string& b) {
                return a <= b;
            }
        );
        cout << "\n=====================================" << endl;
        cout << "Movies Starred by " << actorName << endl;
        cout << "=====================================" << endl;
        for (const auto& movie : movies) {
            cout << " - " << movie << "\n";
        }
        cout << "=====================================\n" << endl;
    }
}


// Display all actors in a given movie.
void getActorsByMovie() {
    string movieTitle;
    cout << "Enter movie title: ";
    getline(cin, movieTitle);
    movieTitle = trim(movieTitle);

    // Try to find the movie as entered.
    int movieIndex = actorMovieGraph.getNodeIndex(movieTitle);
    // If not found, try adding surrounding quotes.
    if (movieIndex == -1) {
        string quotedTitle = "\"" + movieTitle + "\"";
        movieIndex = actorMovieGraph.getNodeIndex(quotedTitle);
        if (movieIndex != -1)
            movieTitle = quotedTitle;  // use the quoted version if found
    }

    if (movieIndex == -1) {
        cout << "\n[Error] Movie \"" << movieTitle << "\" not found in the graph.\n";
        return;
    }

    vector<string> actors = actorMovieGraph.getNeighbors(movieTitle);
    if (actors.empty()) {
        cout << "\n[Info] No actors found for the movie \"" << movieTitle << "\".\n";
    }
    else {
        // Use quick sort to sort actor names alphabetically.
        quickSort(actors, 0, actors.size() - 1,
            [](const string& a, const string& b) {
                return a < b; // strict less-than comparison
            }
        );

        cout << "\n=====================================" << endl;
        cout << "Actors in " << movieTitle << endl;
        cout << "=====================================" << endl;
        for (const auto& actor : actors) {
            cout << " - " << actor << "\n";
        }
        cout << "=====================================\n" << endl;
    }
}


// Use quick sort to display known actors (via graph traversal)
void displayKnownActors() {
    string actorName;
    cout << "Enter actor name: ";
    getline(cin, actorName);

    if (!actorMovieGraph.nodeExists(actorName)) {
        cout << "[Error] Actor \"" << actorName << "\" not found.\n";
        return;
    }

	// This finds the actors who have worked with the actor name inputted
    vector<string> level1;
    vector<string> movies = actorMovieGraph.getNeighbors(actorName);
    for (const string& movie : movies) {
        vector<string> coActors = actorMovieGraph.getNeighbors(movie);
        for (const string& coActor : coActors) {
            if (coActor != actorName) {
                level1.push_back(coActor);
            }
        }
    }

	// This finds the actors who have worked with the actors found in the above loop
    vector<string> level2;
    for (const string& level1Actor : level1) {
        vector<string> moviesOfLevel1 = actorMovieGraph.getNeighbors(level1Actor);
        for (const string& movie : moviesOfLevel1) {
            vector<string> coActors = actorMovieGraph.getNeighbors(movie);
            for (const string& coActor : coActors) {
                if (coActor != actorName) { // Exclude the original actor
                    level2.push_back(coActor);
                }
            }
        }
    }

	// Combine the actors found in both levels
    vector<string> knownActors = level1;
    knownActors.insert(knownActors.end(), level2.begin(), level2.end());

    // Use Quick Sort to sort the names alphabetically.
    if (!knownActors.empty()) {
        quickSort(knownActors, 0, knownActors.size() - 1,
            [](const string& a, const string& b) {
                return a < b;  
            }
        );
    }

    // Remove duplicates from the sorted list.
    removeDuplicates(knownActors);

    // Display the sorted list of known actors.
    cout << "\n=====================================" << endl;
    cout << "Actors known by " << actorName << " (sorted alphabetically):" << endl;
    cout << "=====================================" << endl;
    if (knownActors.empty()) {
        cout << "[Info] No known actors found for \"" << actorName << "\".\n";
    }
    else {
        for (const string& actor : knownActors) {
            cout << " - " << actor << "\n";
        }
    }
    cout << "=====================================\n" << endl;
}

// Rate a Movie or Actor
void rateMovieOrActor() {
    // Ask the user whether they wish to rate an actor or a movie.
    cout << "\nDo you want to rate an Actor or a Movie? (A/M): ";
    char choice;
    cin >> choice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // clear the input buffer

    // Normalize the input.
    choice = toupper(choice);

    if (choice == 'A') {
        // Rate an actor.
        string actorID = getNonEmptyInput("Enter the Actor ID to rate: ");
        Actor* actor = actorDictionary.get(actorID);
        if (!actor) {
            cout << "[Error] Actor with ID \"" << actorID << "\" not found.\n";
            return;
        }
        int stars;
        while (true) {
            cout << "Enter rating (1-5 stars): ";
            cin >> stars;
            if (cin.fail() || stars < 1 || stars > 5) {
                cout << "[Error] Please enter a number between 1 and 5.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            else {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                break;
            }
        }
        // Update the rating as a running average.
        // newRating = (oldRating * noOfTimesRated + stars) / (noOfTimesRated + 1)
        actor->rating = (actor->rating * actor->noOfTimesRated + stars) / (actor->noOfTimesRated + 1);
        actor->noOfTimesRated++;
        cout << "[Success] Actor \"" << actor->name << "\" now has an average rating of "
            << actor->rating << " (" << actor->noOfTimesRated << " ratings).\n";
    }
    else if (choice == 'M') {
        // Rate a movie.
        string movieID = getNonEmptyInput("Enter the Movie ID to rate: ");
        Movie* movie = movieDictionary.get(movieID);
        if (!movie) {
            cout << "[Error] Movie with ID \"" << movieID << "\" not found.\n";
            return;
        }
        int stars;
        while (true) {
            cout << "Enter rating (1-5 stars): ";
            cin >> stars;
            if (cin.fail() || stars < 1 || stars > 5) {
                cout << "[Error] Please enter a number between 1 and 5.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            else {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                break;
            }
        }
        // Update the rating as a running average.
        movie->rating = (movie->rating * movie->noOfTimesRated + stars) / (movie->noOfTimesRated + 1);
        movie->noOfTimesRated++;
        cout << "[Success] Movie \"" << movie->title << "\" now has an average rating of "
            << movie->rating << " (" << movie->noOfTimesRated << " ratings).\n";
    }
    else {
        cout << "[Error] Invalid choice. Please select either 'A' for actor or 'M' for movie.\n";
    }
}

// Display top 10 rating for Actor or Movie
void displayRating() {

}





//Main Loop
//======================//

int main() {
    // Load initial data from CSV files
    loadActorsFromCSV("../actors.csv");
    loadMoviesFromCSV("../movies.csv");
    loadCastsFromCSV("../cast.csv");

    // Main program loop
    while (true) {
        mainMenu();

        int choice;
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the input buffer

        if (choice == 1) { // Admin features
            while (true) {
                adminMenu();

                int adminChoice;
                cin >> adminChoice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the input buffer

                if (adminChoice == 5) {
                    cout << "[Info] Returning to Main Menu...\n";
                    break;
                }

                switch (adminChoice) {
                case 1:
                    // Feature A: Add new actor
                    addActor();
                    break;
                case 2:
                    // Feature B: Add new movie
                    addMovie();
                    break;
                case 3:
                    // Feature C: Add an actor to a movie
                    addActorToMovie();
                    break;
                case 4:
                    // Feature D: Update actor/movie details
                    updateDetails();
                    break;
                default:
                    cout << "[Error] Invalid input, please try again.\n";
                }
            }
        }
        else if (choice == 2) { // User features
            while (true) {
                userMenu();

                int userChoice;
                cin >> userChoice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the input buffer

                if (userChoice == 8) {
                    cout << "[Info] Returning to Main Menu...\n";
                    break;
                }

                switch (userChoice) {
                case 1:
                    // Feature E: Display actors between a certain age
                    displayActorsByAgeRange();
                    break;
                case 2:
                    // Feature F: Display movies made within the past 3 years
                    displayRecentMovies();
                    break;
                case 3:
                    // Feature G: Display all movies an actor starred in
                    getMoviesByActor();
                    break;
                case 4:
                    // Feature H: Display all the actors in a particular movie
                    getActorsByMovie();
                    break;
                case 5:
                    // Feature I: Display list of actors a particular actor knows
                    displayKnownActors();
                    break;
                case 6:
                    // Advance 1: Rate a movie or actor
					rateMovieOrActor();
                    break;
                case 7:
					// Advance 1.1 Display top 10 rating for Actor or Movie
					displayRating();
					break;
                default:
                    cout << "[Error] Invalid input, please try again.\n";
                }
            }
        }
        else if (choice == 3) {
            // Store updates to CSVs (patch & append)
            storeDataToCsv();
            cout << "[Info] Data stored to CSV files successfully.\n";
        }
        else {
            cout << "[Error] Invalid input, please try again.\n";
        }
    }
}
