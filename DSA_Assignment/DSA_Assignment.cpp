// DSA_Assignment.cpp : This file contains the 'main' function. 
// Program execution begins and ends here.
// Student 1: Ng Kai Huat Jason(S10262552)
// Student 2: Law Ming Qi (S10257808)

#include <iostream>
#include <string>
#include "Dictionary.h"
#include "Actor.h"
#include "Movie.h"
#include "AVLTree.h"
#include "Graph.h"
#include <fstream>
#include <sstream>
#include <ctime>         // For getting the current year dynamically
#include <limits>        // For numeric_limits
#include <sys/stat.h>    // For fileExists
#include <vector>

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
    cout << "(6) Go back to Main Menu" << endl;
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
        stringstream ss(line);
        string id, name, birthYearStr;
        getline(ss, id, ',');
        getline(ss, name, ',');
        getline(ss, birthYearStr, ',');
        if (!name.empty() && name.front() == '"') name.erase(0, 1);
        if (!name.empty() && name.back() == '"') name.erase(name.size() - 1);
        try {
            int birthYear = stoi(birthYearStr);
            Actor* newActor = new Actor(id, name, birthYear);
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

// Load movies from CSV 
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

        // Use our custom CSV parser to split the line.
        vector<string> fields = parseCSVLine(line);
        // Expect at least four fields: id, title, plot, and year.
        if (fields.size() < 4) {
            cout << "[Warning] Skipping invalid movie record: " << line << endl;
            continue;
        }

        string id = trim(fields[0]);
        string title = trim(fields[1]);
        string plot = trim(fields[2]);
        string yearStr = trim(fields[3]);

        if (id.empty() || title.empty() || yearStr.empty()) {
            cout << "[Warning] Skipping invalid movie record: " << line << endl;
            continue;
        }

        // Create the new Movie object.
        Movie* newMovie = new Movie(id, title, plot, yearStr);
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
                outFile << "ActorID,Name,BirthYear\n";
                outFile.close();
            }
        }
        outFile.open(filename, ios::app);
        if (outFile.is_open()) {
            for (const Actor* actor : newActors) {
                outFile << actor->id << "," << actor->name << "," << actor->birthYear << "\n";
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
                outFile << "MovieID,Title,Plot,Year\n";
                outFile.close();
            }
        }
        outFile.open(filename, ios::app);
        if (outFile.is_open()) {
            for (const Movie* movie : newMovies) {
                // Note: No extra quotes added.
                outFile << movie->id << "," << movie->title << "," << movie->plot << "," << movie->year << "\n";
            }
            outFile.close();
            cout << "[Info] New movies appended successfully.\n";
        }
        else {
            cout << "[Error] Unable to open " << filename << " for appending new movies.\n";
        }
    }

    // Append new cast relationships
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
            string updatedLine = actor->id + "," + actor->name + "," + to_string(actor->birthYear);
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
            string updatedLine = movie->id + "," + movie->title + "," + movie->plot + "," + movie->year;
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
    string id, name;
    int birthYear;
    id = getNonEmptyInput("Enter Actor ID: ");
    name = getNonEmptyInput("Enter Actor Name: ");
    while (true) {
        cout << "Enter Actor Birth Year: ";
        cin >> birthYear;
        if (cin.fail() || birthYear <= 0) {
            cout << "[Error] Invalid birth year. Please enter a positive integer.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            break;
        }
    }
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

// Update details of an actor or a movie.
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

// Display recent movies (from the past 3 years) in ascending order.
void displayRecentMovies() {
    time_t t = time(nullptr);
    tm currentTime;
    localtime_s(&currentTime, &t);
    int currentYear = 1900 + currentTime.tm_year;
    vector<Movie*> allMovies = movieDictionary.getAllItems();
    vector<Movie*> recentMovies;
    for (Movie* movie : allMovies) {
        int movieYear = movie->getYearAsInt();
        if (movieYear > 0 && currentYear - movieYear <= 3) {
            recentMovies.push_back(movie);
        }
    }
    // Bubble sort by ascending year.
    for (size_t i = 0; i < recentMovies.size(); ++i) {
        for (size_t j = 0; j < recentMovies.size() - i - 1; ++j) {
            if (recentMovies[j]->getYearAsInt() > recentMovies[j + 1]->getYearAsInt()) {
                Movie* temp = recentMovies[j];
                recentMovies[j] = recentMovies[j + 1];
                recentMovies[j + 1] = temp;
            }
        }
    }
    cout << "\n=====================================" << endl;
    cout << "Recent Movies (Last 3 Years)" << endl;
    cout << "=====================================" << endl;
    if (recentMovies.empty()) {
        cout << "[Info] No recent movies found within the past 3 years.\n";
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
        // Bubble sort alphabetically.
        for (size_t i = 0; i < movies.size(); ++i) {
            for (size_t j = i + 1; j < movies.size(); ++j) {
                if (movies[i] > movies[j])
                    swap(movies[i], movies[j]);
            }
        }
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
    int movieIndex = actorMovieGraph.getNodeIndex(movieTitle);
    if (movieIndex == -1) {
        cout << "\n[Error] Movie \"" << movieTitle << "\" not found in the graph.\n";
        return;
    }
    vector<string> actors = actorMovieGraph.getNeighbors(movieTitle);
    if (actors.empty()) {
        cout << "\n[Info] No actors found for the movie \"" << movieTitle << "\".\n";
    }
    else {
        // Bubble sort alphabetically.
        for (size_t i = 0; i < actors.size(); ++i) {
            for (size_t j = i + 1; j < actors.size(); ++j) {
                if (actors[i] > actors[j])
                    swap(actors[i], actors[j]);
            }
        }
        cout << "\n=====================================" << endl;
        cout << "Actors in \"" << movieTitle << "\"" << endl;
        cout << "=====================================" << endl;
        for (const auto& actor : actors) {
            cout << " - " << actor << "\n";
        }
        cout << "=====================================\n" << endl;
    }
}

// Use bubble sort to display known actors (via graph traversal).
void displayKnownActors() {
    string actorName;
    cout << "Enter actor name: ";
    getline(cin, actorName);
    if (!actorMovieGraph.nodeExists(actorName)) {
        cout << "[Error] Actor \"" << actorName << "\" not found.\n";
        return;
    }
    actorMovieGraph.bfs(actorName);
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

                if (userChoice == 6) {
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
