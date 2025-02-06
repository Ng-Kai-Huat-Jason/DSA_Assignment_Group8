// Student 1: Ng Kai Huat Jason (S10262552)
// Student 2: Law Ming Qi (S10257808)

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <ctime>         // For getting the current year dynamically
#include <limits>        // For numeric_limits
#include <sys/stat.h>    // For fileExists
#include <vector>
#include <algorithm>

#include "Actor.h"
#include "Movie.h"
#include "Dictionary.h"
#include "AVLTree.h"
#include "Graph.h"

using namespace std;

// Global vectors to track new entries
vector<Actor*> newActors;               // Stores newly added actors
vector<Movie*> newMovies;               // Stores newly added movies
vector<pair<string, string>> newCasts;  // Stores new cast relationships (ActorID, MovieID)

// Global data structures
Dictionary<string, Actor> actorDictionary;  // Dictionary to store actors
Dictionary<string, Movie> movieDictionary;    // Dictionary to store movies
AVLTree<Actor> actorAVLTree;                  // AVL Tree to store actors
AVLTree<Movie> movieAVLTree;                  // AVL Tree to store movies
Graph<string> actorMovieGraph;                // Graph to represent actor-movie relationships

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

// Helper Functions
//======================//

// Trim leading and trailing whitespace from a string.
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
        input = trim(input);
        if (!input.empty()) {
            return input;
        }
        else {
            cout << "[Error] Input cannot be empty. Please try again.\n";
        }
    }
}

// Parse a CSV line into fields while respecting quotes.
vector<string> parseCSVLine(const string& line) {
    vector<string> fields;
    string current;
    bool inQuotes = false;
    for (size_t i = 0; i < line.size(); i++) {
        char c = line[i];
        if (c == '"') {
            if (inQuotes && i + 1 < line.size() && line[i + 1] == '"') {
                current.push_back('"');
                i++;
            }
            else {
                inQuotes = !inQuotes;
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
            if (inQuotes && i + 1 < line.size() && line[i + 1] == '"') {
                current.push_back('"');
                i++;
            }
            else {
                inQuotes = !inQuotes;
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
        if (comp(arr[j], pivot)) {
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

// Merge function for sorting actors by rating (descending order).
void mergeActorsByRating(vector<Actor*>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    vector<Actor*> L(n1), R(n2);
    for (int i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (int i = 0; i < n2; i++)
        R[i] = arr[mid + 1 + i];
    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i]->rating >= R[j]->rating) {
            arr[k] = L[i];
            i++;
        }
        else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void mergeSortActorsByRating(vector<Actor*>& arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSortActorsByRating(arr, left, mid);
        mergeSortActorsByRating(arr, mid + 1, right);
        mergeActorsByRating(arr, left, mid, right);
    }
}

// Merge function for sorting movies by rating (descending order).
void mergeMoviesByRating(vector<Movie*>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    vector<Movie*> L(n1), R(n2);
    for (int i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (int i = 0; i < n2; i++)
        R[i] = arr[mid + 1 + i];
    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i]->rating >= R[j]->rating) {
            arr[k] = L[i];
            i++;
        }
        else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void mergeSortMoviesByRating(vector<Movie*>& arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSortMoviesByRating(arr, left, mid);
        mergeSortMoviesByRating(arr, mid + 1, right);
        mergeMoviesByRating(arr, left, mid, right);
    }
}

// Display the top 10 rated actors.
void displayTopActors(const vector<Actor*>& actors) {
    cout << "\n=====================================" << endl;
    cout << "Top 10 Rated Actors" << endl;
    cout << "=====================================" << endl;
    if (actors.empty()) {
        cout << "[Info] No rated actors found.\n";
        return;
    }
    int displayCount = min(10, static_cast<int>(actors.size()));
    for (int i = 0; i < displayCount; i++) {
        string ratingStr = to_string(actors[i]->rating);
        size_t decimalPos = ratingStr.find('.');
        if (decimalPos != string::npos && ratingStr.length() > decimalPos + 2) {
            ratingStr = ratingStr.substr(0, decimalPos + 3);
        }
        cout << i + 1 << ". " << actors[i]->name << " - " << ratingStr
            << " - " << actors[i]->noOfTimesRated << " ratings" << endl;
    }
}

// Display the top 10 rated movies.
void displayTopMovies(const vector<Movie*>& movies) {
    cout << "\n=====================================" << endl;
    cout << "Top 10 Rated Movies" << endl;
    cout << "=====================================" << endl;
    if (movies.empty()) {
        cout << "[Info] No rated movies found.\n";
        return;
    }
    int displayCount = min(10, static_cast<int>(movies.size()));
    for (int i = 0; i < displayCount; i++) {
        string ratingStr = to_string(movies[i]->rating);
        size_t decimalPos = ratingStr.find('.');
        if (decimalPos != string::npos && ratingStr.length() > decimalPos + 2) {
            ratingStr = ratingStr.substr(0, decimalPos + 3);
        }
        cout << i + 1 << ". " << movies[i]->title << " - " << ratingStr
            << " - " << movies[i]->noOfTimesRated << " ratings" << endl;
    }
}


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

// Append new actor records to the actors CSV file.
void appendNewActorsToCsv() {
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
            outFile << actor->id << ","
                << actor->name << ","
                << actor->birthYear << ","
                << actor->rating << ","
                << actor->noOfTimesRated << "\n";
        }
        outFile.close();
        cout << "[Info] New actors appended successfully.\n";
    }
    else {
        cout << "[Error] Unable to open " << filename << " for appending new actors.\n";
    }
}

// Append new movie records to the movies CSV file.
void appendNewMoviesToCsv() {
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
            outFile << movie->id << ","
                << movie->title << ","
                << movie->plot << ","
                << movie->year << ","
                << movie->rating << ","
                << movie->noOfTimesRated << "\n";
        }
        outFile.close();
        cout << "[Info] New movies appended successfully.\n";
    }
    else {
        cout << "[Error] Unable to open " << filename << " for appending new movies.\n";
    }
}

// Append new cast records to the cast CSV file.
void appendNewCastsToCsv() {
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

// Combined function to store (append new data and patch existing records) CSV files.
void storeDataToCsv() {
    cout << "[Info] Storing data to CSV files...\n";
    appendNewActorsToCsv();
    appendNewMoviesToCsv();
    appendNewCastsToCsv();
    // Call the patch functions provided by Dictionary:
    actorDictionary.patchCSV("../actors.csv", true);
    movieDictionary.patchCSV("../movies.csv", false);
    cout << "[Info] Data storage to CSV files completed.\n";
}


// Feature Functions
//==============================//

void addActor() {
    string id = getNonEmptyInput("Enter Actor ID: ");
    string name = getNonEmptyInput("Enter Actor Name: ");
    int birthYear;
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
        cout << "[Success] Actor \"" << name << "\" (ID: " << id << ") added successfully!\n";
        newActors.push_back(newActor);
    }
    else {
        cout << "[Error] Actor with ID \"" << id << "\" already exists.\n";
        delete newActor;
        return;
    }
    actorAVLTree.insert(newActor);
    actorMovieGraph.addNode(name);
    cout << "[Success] Actor added to AVL Tree and Graph.\n";
}

void addMovie() {
    string id = getNonEmptyInput("Enter Movie ID: ");
    string title = getNonEmptyInput("Enter Movie Title: ");
    string plot = getNonEmptyInput("Enter Movie Plot: ");
    string year;
    while (true) {
        year = getNonEmptyInput("Enter Movie Year: ");
        if (all_of(year.begin(), year.end(), ::isdigit))
            break;
        else
            cout << "[Error] Invalid year. Please enter a year containing only digits.\n";
    }
    Movie* newMovie = new Movie(id, title, plot, year);
    if (movieDictionary.add(id, newMovie)) {
        cout << "[Success] Movie \"" << title << "\" (ID: " << id << ") added successfully!\n";
        newMovies.push_back(newMovie);
    }
    else {
        cout << "[Error] Movie with ID \"" << id << "\" already exists.\n";
        delete newMovie;
        return;
    }
    movieAVLTree.insert(newMovie);
    actorMovieGraph.addNode(title);
    cout << "[Success] Movie added to AVL Tree and Graph.\n";
}

void addActorToMovie() {
    string actorId = getNonEmptyInput("Enter Actor ID: ");
    string movieId = getNonEmptyInput("Enter Movie ID: ");
    Actor* actor = actorDictionary.get(actorId);
    Movie* movie = movieDictionary.get(movieId);
    if (actor && movie) {
        bool alreadyCast = false;
        for (const Movie* m : actor->movies) {
            if (m->id == movieId) {
                alreadyCast = true;
                break;
            }
        }
        if (alreadyCast) {
            cout << "[Error] Actor \"" << actor->name << "\" is already in movie \"" << movie->title << "\".\n";
            return;
        }
        actorMovieGraph.addEdge(actor->name, movie->title);
        actor->addMovie(movie);
        movie->addActor(actor);
        newCasts.emplace_back(actorId, movieId);
        cout << "[Success] Actor \"" << actor->name << "\" added to movie \"" << movie->title << "\".\n";
    }
    else {
        cout << "[Error] Invalid Actor ID or Movie ID.\n";
    }
}

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
                    cout << "[Error] Invalid birth year.\n";
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
            cout << "[Success] Actor details updated.\n";
            actorAVLTree.insert(actor);
            if (oldName != newName)
                actorMovieGraph.updateNode(oldName, newName);
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
                    cout << "[Error] Invalid year.\n";
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
            cout << "[Success] Movie details updated.\n";
            movieAVLTree.insert(movie);
            if (oldTitle != newTitle)
                actorMovieGraph.updateNode(oldTitle, newTitle);
        }
        else {
            cout << "[Error] Movie not found.\n";
        }
    }
    else {
        cout << "[Error] Invalid choice.\n";
    }
}

void displayRecentMovies() {
    int baseYear;
    while (true) {
        cout << "Enter a year: ";
        cin >> baseYear;
        if (cin.fail() || baseYear <= 0) {
            cout << "[Error] Invalid year.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            break;
        }
    }
    int lowerBound = baseYear - 3;
    vector<Movie*> allMovies = movieDictionary.getAllItems();
    vector<Movie*> recentMovies;
    for (Movie* movie : allMovies) {
        int movieYear = movie->getYearAsInt();
        if (movieYear > 0 && movieYear >= lowerBound && movieYear <= baseYear)
            recentMovies.push_back(movie);
    }
    if (!recentMovies.empty()) {
        quickSort(recentMovies, 0, recentMovies.size() - 1, [](Movie* a, Movie* b) {
            return a->getYearAsInt() <= b->getYearAsInt();
            });
    }
    cout << "\nRecent Movies (From " << lowerBound << " to " << baseYear << "):\n";
    for (const Movie* movie : recentMovies) {
        cout << "Title: " << movie->title << "\nYear: " << movie->year
            << "\nPlot: " << movie->plot << "\n-------------------------\n";
    }
}

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
            cout << "[Error] Invalid age.\n";
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
            cout << "[Error] Please enter an integer >= " << x << ".\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            break;
        }
    }
    vector<Actor*> allActors = actorDictionary.getAllItems();
    vector<Actor*> filteredActors;
    for (Actor* actor : allActors) {
        int age = currentYear - actor->birthYear;
        if (age >= x && age <= y)
            filteredActors.push_back(actor);
    }
    sort(filteredActors.begin(), filteredActors.end(), [currentYear](Actor* a, Actor* b) {
        return (currentYear - a->birthYear) < (currentYear - b->birthYear);
        });
    cout << "\nActors Aged Between " << x << " and " << y << ":\n";
    for (const Actor* actor : filteredActors) {
        cout << "Name: " << actor->name << ", Age: " << (currentYear - actor->birthYear)
            << "\n-------------------------\n";
    }
}

void getMoviesByActor() {
    string actorName = getNonEmptyInput("Enter actor name: ");
    if (!actorMovieGraph.nodeExists(actorName)) {
        cout << "[Error] Actor \"" << actorName << "\" is not found in the graph.\n";
        return;
    }
    vector<string> movies = actorMovieGraph.listMoviesForActor(actorName);
    cout << "\nMovies Starred by " << actorName << ":\n";
    for (const string& movie : movies) {
        cout << " - " << movie << "\n";
    }
}

void getActorsByMovie() {
    string movieTitle = getNonEmptyInput("Enter movie title: ");
    if (!actorMovieGraph.nodeExists(movieTitle)) {
        // Try with quotes
        string quotedTitle = "\"" + movieTitle + "\"";
        if (!actorMovieGraph.nodeExists(quotedTitle)) {
            cout << "[Error] Movie \"" << movieTitle << "\" not found.\n";
            return;
        }
        movieTitle = quotedTitle;
    }
    vector<string> actors = actorMovieGraph.listActorsForMovie(movieTitle);
    cout << "\nActors in " << movieTitle << ":\n";
    for (const string& actor : actors) {
        cout << " - " << actor << "\n";
    }
}

void displayKnownActors() {
    string actorName = getNonEmptyInput("Enter actor name: ");
    if (!actorMovieGraph.nodeExists(actorName)) {
        cout << "[Error] Actor \"" << actorName << "\" not found.\n";
        return;
    }
    vector<string> knownActors = actorMovieGraph.getKnownActors(actorName);
    cout << "\nActors known by " << actorName << ":\n";
    for (const string& actor : knownActors) {
        cout << " - " << actor << "\n";
    }
}

void rateMovieOrActor() {
    cout << "\nDo you want to rate an Actor or a Movie? (A/M): ";
    char choice;
    cin >> choice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    choice = toupper(choice);
    if (choice == 'A') {
        string actorName = getNonEmptyInput("Enter the Actor Name to rate: ");
        vector<Actor*> allActors = actorDictionary.getAllItems();
        Actor* actor = nullptr;
        for (Actor* a : allActors) {
            if (a->name == actorName) {
                actor = a;
                break;
            }
        }
        if (!actor) {
            cout << "[Error] Actor \"" << actorName << "\" not found.\n";
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
        actor->updateRating(stars);
        cout << "[Success] Actor \"" << actor->name << "\" now has an average rating of "
            << actor->rating << " (" << actor->noOfTimesRated << " ratings).\n";
    }
    else if (choice == 'M') {
        string movieTitle = getNonEmptyInput("Enter the Movie Title to rate: ");
        vector<Movie*> allMovies = movieDictionary.getAllItems();
        Movie* movie = nullptr;
        for (Movie* m : allMovies) {
            if (m->title == movieTitle) {
                movie = m;
                break;
            }
        }
        if (!movie) {
            cout << "[Error] Movie \"" << movieTitle << "\" not found.\n";
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
        movie->updateRating(stars);
        cout << "[Success] Movie \"" << movie->title << "\" now has an average rating of "
            << movie->rating << " (" << movie->noOfTimesRated << " ratings).\n";
    }
    else {
        cout << "[Error] Invalid choice. Please select 'A' for actor or 'M' for movie.\n";
    }
}

void displayRating() {
    cout << "\nDo you want to view Top 10 Actors or Movies? (A/M): ";
    char choice;
    cin >> choice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    choice = toupper(choice);
    if (choice == 'A') {
        vector<Actor*> actors = actorDictionary.getAllItems();
        if (!actors.empty())
            mergeSortActorsByRating(actors, 0, actors.size() - 1);
        displayTopActors(actors);
    }
    else if (choice == 'M') {
        vector<Movie*> movies = movieDictionary.getAllItems();
        if (!movies.empty())
            mergeSortMoviesByRating(movies, 0, movies.size() - 1);
        displayTopMovies(movies);
    }
    else {
        cout << "[Error] Invalid choice. Please select 'A' for actors or 'M' for movies.\n";
    }
}

int main() {
    // Load initial data using the new Dictionary methods.
    actorDictionary.loadFromCSV("../actors.csv", true);
    movieDictionary.loadFromCSV("../movies.csv", false);
    // Load cast relationships (remains in main because it involves multiple data structures).
    loadCastsFromCSV("../cast.csv");

    while (true) {
        mainMenu();
        int choice;
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (choice == 1) { // Admin
            while (true) {
                adminMenu();
                int adminChoice;
                cin >> adminChoice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                if (adminChoice == 5) {
                    cout << "[Info] Returning to Main Menu...\n";
                    break;
                }
                switch (adminChoice) {
                case 1: addActor(); break;
                case 2: addMovie(); break;
                case 3: addActorToMovie(); break;
                case 4: updateDetails(); break;
                default: cout << "[Error] Invalid input, please try again.\n";
                }
            }
        }
        else if (choice == 2) { // User
            while (true) {
                userMenu();
                int userChoice;
                cin >> userChoice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                if (userChoice == 8) {
                    cout << "[Info] Returning to Main Menu...\n";
                    break;
                }
                switch (userChoice) {
                case 1: displayActorsByAgeRange(); break;
                case 2: displayRecentMovies(); break;
                case 3: getMoviesByActor(); break;
                case 4: getActorsByMovie(); break;
                case 5: displayKnownActors(); break;
                case 6: rateMovieOrActor(); break;
                case 7: displayRating(); break;
                default: cout << "[Error] Invalid input, please try again.\n";
                }
            }
        }
        else if (choice == 3) {
            // Store updates to CSV files: first append new records, then patch updated records.
            storeDataToCsv();
            cout << "[Info] Data stored to CSV files successfully.\n";
        }

        else {
            cout << "[Error] Invalid input, please try again.\n";
        }
    }
}
