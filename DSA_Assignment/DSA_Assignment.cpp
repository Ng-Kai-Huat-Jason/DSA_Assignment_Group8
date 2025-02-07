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

// ==================== Global Variables ====================
// Global vectors to track new entries
vector<Actor*> newActors;               // Stores newly added actors
vector<Movie*> newMovies;               // Stores newly added movies
vector<pair<string, string>> newCasts;  // Stores new cast relationships (ActorID, MovieID)

// Global data structures
Dictionary<string, Actor> actorDictionary;  // Dictionary to store actors
Dictionary<string, Movie> movieDictionary;  // Dictionary to store movies
AVLTree<Actor> actorAVLTree;                // AVL Tree to store actors
AVLTree<Movie> movieAVLTree;                // AVL Tree to store movies
Graph<string> actorMovieGraph;              // Graph to represent actor-movie relationships

// ==================== Menu Functions ====================

// Displays menu for the main menu
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

// Displays menu for the admin menu
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

// Displays menu for the user menu
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

// ==================== Helper Functions ====================
// Trims leading and trailing whitespace from a string.
string trim(const string& str) {
    size_t start = str.find_first_not_of(" \t");
    size_t end = str.find_last_not_of(" \t");
    return (start == string::npos || end == string::npos) ? "" : str.substr(start, end - start + 1);
}

// Check if the files exist 
bool fileExists(const string& filename) {
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}

// Validator to check if input is empty
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

// Parses CSV Line into fields
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

// Parses CSV Line into fields while preserving quotes
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

// ==================== Sorting Functions ====================

// Function to partition a vector for quicksort
// This function selects a pivot element and partitions the array such that
// elements less than the pivot are on the left, and elements greater are on the right.
// It returns the index of the pivot element after partitioning.
template <typename T, typename Compare>
int partition(vector<T>& arr, int low, int high, Compare comp) {
    T pivot = arr[high];  // Choose the last element as the pivot
    int i = low - 1;      // Index of the smaller element

    // Iterate through the array and rearrange elements based on the comparison function
    for (int j = low; j < high; j++) {
        if (comp(arr[j], pivot)) {  // If the current element is less than the pivot
            i++;  // Increment the index of the smaller element
            swap(arr[i], arr[j]);  // Swap the current element with the element at index i
        }
    }
    swap(arr[i + 1], arr[high]);  // Swap the pivot element with the element at index i+1
    return i + 1;  // Return the partitioning index
}

// Function to perform quicksort on a vector
// This function recursively sorts the vector by partitioning it around a pivot
// and then sorting the sub-arrays before and after the pivot.
template <typename T, typename Compare>
void quickSort(vector<T>& arr, int low, int high, Compare comp) {
    if (low < high) {
        int pi = partition(arr, low, high, comp);  // Partition the array and get the pivot index
        quickSort(arr, low, pi - 1, comp);  // Recursively sort the left sub-array
        quickSort(arr, pi + 1, high, comp);  // Recursively sort the right sub-array
    }
}

// Function to merge two halves of a vector for merge sort (actors by rating)
// This function merges two sorted halves of a vector of Actor pointers based on their ratings.
void mergeActorsByRating(vector<Actor*>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;  // Size of the left sub-array
    int n2 = right - mid;     // Size of the right sub-array

    // Create temporary vectors to store the left and right sub-arrays
    vector<Actor*> L(n1), R(n2);
    for (int i = 0; i < n1; i++)
        L[i] = arr[left + i];  // Copy elements to the left sub-array
    for (int i = 0; i < n2; i++)
        R[i] = arr[mid + 1 + i];  // Copy elements to the right sub-array

    int i = 0, j = 0, k = left;  // Initialize indices for merging

    // Merge the two sub-arrays back into the original array in sorted order
    while (i < n1 && j < n2) {
        if (L[i]->rating >= R[j]->rating) {  // Compare ratings
            arr[k] = L[i];  // Take the element from the left sub-array
            i++;
        }
        else {
            arr[k] = R[j];  // Take the element from the right sub-array
            j++;
        }
        k++;
    }

    // Copy any remaining elements from the left sub-array
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    // Copy any remaining elements from the right sub-array
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

// Function to perform merge sort on a vector of Actor pointers by rating
// This function recursively divides the vector into two halves, sorts them, and then merges them.
void mergeSortActorsByRating(vector<Actor*>& arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;  // Find the middle point
        mergeSortActorsByRating(arr, left, mid);  // Sort the left half
        mergeSortActorsByRating(arr, mid + 1, right);  // Sort the right half
        mergeActorsByRating(arr, left, mid, right);  // Merge the sorted halves
    }
}

// Function to merge two halves of a vector for merge sort (movies by rating)
// This function merges two sorted halves of a vector of Movie pointers based on their ratings.
void mergeMoviesByRating(vector<Movie*>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;  // Size of the left sub-array
    int n2 = right - mid;     // Size of the right sub-array

    // Create temporary vectors to store the left and right sub-arrays
    vector<Movie*> L(n1), R(n2);
    for (int i = 0; i < n1; i++)
        L[i] = arr[left + i];  // Copy elements to the left sub-array
    for (int i = 0; i < n2; i++)
        R[i] = arr[mid + 1 + i];  // Copy elements to the right sub-array

    int i = 0, j = 0, k = left;  // Initialize indices for merging

    // Merge the two sub-arrays back into the original array in sorted order
    while (i < n1 && j < n2) {
        if (L[i]->rating >= R[j]->rating) {  // Compare ratings
            arr[k] = L[i];  // Take the element from the left sub-array
            i++;
        }
        else {
            arr[k] = R[j];  // Take the element from the right sub-array
            j++;
        }
        k++;
    }

    // Copy any remaining elements from the left sub-array
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    // Copy any remaining elements from the right sub-array
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

// Function to perform merge sort on a vector of Movie pointers by rating
// This function recursively divides the vector into two halves, sorts them, and then merges them.
void mergeSortMoviesByRating(vector<Movie*>& arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;  // Find the middle point
        mergeSortMoviesByRating(arr, left, mid);  // Sort the left half
        mergeSortMoviesByRating(arr, mid + 1, right);  // Sort the right half
        mergeMoviesByRating(arr, left, mid, right);  // Merge the sorted halves
    }
}


// ==================== Display Functions for Top 10 Actor or Movie  ====================

// Display top 10 actors by rating 
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

// Display top 10 movies by rating
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

// ==================== Data Loading Functions ====================

// load Cast Relationships from CSV 
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

// ==================== Data Storage Functions ====================

// Use to append new actors to CSV
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

// Use to append new movies to CSV
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

// Use to append new cast relationships to CSV
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

// Calls are storing CSV functions into 1 function
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

// ==================== Feature Functions ====================

// Function to add a new actor to the system
void addActor() {

    // Prompt user to enter a ID for new actor
    string id = getNonEmptyInput("Enter Actor ID: ");

    // Prompt user to enter name for new actor
    string name = getNonEmptyInput("Enter Actor Name: ");

	// Create a variable to store the birth year
    int birthYear;

	// Loop to ensure the user enters a valid birth year
    while (true) {
        cout << "Enter Actor Birth Year: ";
        cin >> birthYear;

        // Check if the input is invalid (not a positive integer)
        if (cin.fail() || birthYear <= 0) {
            cout << "[Error] Invalid birth year. Please enter a positive integer.\n";

            // Clear the error flag on cin
            cin.clear();

            // Ignore the rest of the input line to prevent further errors
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        else {
            // If the input is valid, ignore any extra input and break the loop
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            break;
        }
    }

    // Create a new Actor object with the provided ID, name, and birth year
    Actor* newActor = new Actor(id, name, birthYear);

    // Attempt to add the new actor to the actorDictionary
    if (actorDictionary.add(id, newActor)) {

        // If successful, print a success message and add the actor to the newActors list
        cout << "[Success] Actor \"" << name << "\" (ID: " << id << ") added successfully!\n";
        newActors.push_back(newActor);
    }
    else {
        // If the actor ID already exists, print an error message and clean up the new actor object
        cout << "[Error] Actor with ID \"" << id << "\" already exists.\n";
        delete newActor;
        return;  // Exit the function as the actor cannot be added
    }

    // Add the new actor to the AVL tree
    actorAVLTree.insert(newActor);

    // Add the actor's name as a node in the actor-movie graph
    actorMovieGraph.addNode(name);

    // Print a success message indicating the actor was added to the AVL tree and graph
    cout << "[Success] Actor added to AVL Tree and Graph.\n";
}

// Function to add a new movie to system
void addMovie() {

	// Prompt user to enter a ID for new movie
    string id = getNonEmptyInput("Enter Movie ID: ");

	// Prompt user to enter title for new movie
    string title = getNonEmptyInput("Enter Movie Title: ");

	// Prompt user to enter plot for new movie
    string plot = getNonEmptyInput("Enter Movie Plot: ");
    
	// Prompt user to enter year for new movie , reason for string is that it is stored as string for CSV compatibility
    string year;

	// Loop to ensure the user enters a valid year
    while (true) {
        year = getNonEmptyInput("Enter Movie Year: ");
        if (all_of(year.begin(), year.end(), ::isdigit))
            break;
        else
            cout << "[Error] Invalid year. Please enter a year containing only digits.\n";
    }

	// Create a new Movie object with the provided ID, title, plot, and year
    Movie* newMovie = new Movie(id, title, plot, year);

	// Attempt to add the new movie to the movieDictionary
    if (movieDictionary.add(id, newMovie)) {
        cout << "[Success] Movie \"" << title << "\" (ID: " << id << ") added successfully!\n";
        newMovies.push_back(newMovie);
    }
    else {
		// If the movie ID already exists, print an error message and clean up the new movie object
        cout << "[Error] Movie with ID \"" << id << "\" already exists.\n";
        delete newMovie;
        return;
    }
	// Add the new movie to the AVL tree
    movieAVLTree.insert(newMovie);

	// Add the movie's title as a node in the actor-movie graph
    actorMovieGraph.addNode(title);

	// Print a success message indicating the movie was added to the AVL tree and graph
    cout << "[Success] Movie added to AVL Tree and Graph.\n";
}

// Function to add Actor to Movie as a Cast
void addActorToMovie() {
	// Prompt user to enter Actor ID
    string actorId = getNonEmptyInput("Enter Actor ID: ");

	// Prompt user to enter Movie ID
    string movieId = getNonEmptyInput("Enter Movie ID: ");

	// Check if the actor and movie exist in the dictionaries
    Actor* actor = actorDictionary.get(actorId);
    Movie* movie = movieDictionary.get(movieId);

	// If both the actor and movie exist, check if the actor is already in the movie
    if (actor && movie) {
        bool alreadyCast = false;
        for (const Movie* m : actor->movies) {
            if (m->id == movieId) {

				// If the actor is already in the movie, print an error message and return
                alreadyCast = true;
                break;
            }
        }
        if (alreadyCast) {
            cout << "[Error] Actor \"" << actor->name << "\" is already in movie \"" << movie->title << "\".\n";
            return;
        }
        
		// Add a new edge to the actor-movie graph and update the actor and movie objects
        actorMovieGraph.addEdge(actor->name, movie->title);

		// Add the actor to the movie and the movie to the actor
        actor->addMovie(movie);

		// Add the movie to the actor and the actor to the movie
        movie->addActor(actor);
        
		// Add the cast relationship to the newCasts vector
        newCasts.emplace_back(actorId, movieId);

		// Print a success message indicating the actor was added to the movie
        cout << "[Success] Actor \"" << actor->name << "\" added to movie \"" << movie->title << "\".\n";
    }
    else {
		// If the actor or movie does not exist, print an error message
        cout << "[Error] Invalid Actor ID or Movie ID.\n";
    }
}

// Function to update Actor details
void updateActorDetails() {
    
	// Prompt user to enter Actor ID to update
    string actorId = getNonEmptyInput("Enter Actor ID to update: ");

	// Check if the actor exists in the dictionary
    Actor* actor = actorDictionary.get(actorId);
    if (!actor) {
        cout << "[Error] Actor not found.\n";
        return;
    }

	// Display options to update actor details
    cout << "\nUpdate Actor Details:\n";
    cout << "1. Update Name\n";
    cout << "2. Update Birth Year\n";
    cout << "3. Update Both\n";

	// Get user choice
    int choice;
    cin >> choice;

	// Clear the input buffer
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

	// Update the actor's name
    if (choice == 1 || choice == 3) {
        string oldName = actor->name;
        string newName = getNonEmptyInput("Enter new Name (current: " + actor->name + "): ");
        actor->name = newName;
        if (oldName != newName)
            actorMovieGraph.updateNode(oldName, newName);
    }

	// Update the actor's birth year
    if (choice == 2 || choice == 3) {
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
                actor->birthYear = newBirthYear;
                break;
            }
        }
    }

	// Print a success message indicating the actor details were updated
    cout << "[Success] Actor details updated.\n";

	// Add the updated actor to the AVL tree
    actorAVLTree.insert(actor);
}

// Function to update Movie details
void updateMovieDetails() {

    // Prompt user to enter Movie ID to update
    string movieId = getNonEmptyInput("Enter Movie ID to update: ");

    // Check if the movie exists in the dictionary
    Movie* movie = movieDictionary.get(movieId);
    if (!movie) {
        cout << "[Error] Movie not found.\n";
        return;
    }

    // Display options to update movie details
    cout << "\nUpdate Movie Details:\n";
    cout << "1. Update Title\n";
    cout << "2. Update Plot\n";
    cout << "3. Update Year\n";
    cout << "4. Update All (Title, Plot, and Year)\n";
    cout << "Enter your choice: ";

    // Get user choice
    int choice;
    cin >> choice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    // Update the movie's title
    if (choice == 1 || choice == 4) {
        string oldTitle = movie->title;
        string newTitle = getNonEmptyInput("Enter new Title (current: " + movie->title + "): ");
        movie->title = newTitle;

        // Ask if the title should be quoted.
        char quoteChoice;
        cout << "Should the title be quoted? (Y/N): ";
        cin >> quoteChoice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        movie->titleWasQuoted = (toupper(quoteChoice) == 'Y');

        if (oldTitle != newTitle)
            actorMovieGraph.updateNode(oldTitle, newTitle);
    }

    // Update the movie's plot
    if (choice == 2 || choice == 4) {
        string newPlot = getNonEmptyInput("Enter new Plot (current: " + movie->plot + "): ");
        movie->plot = newPlot;
    }

    // Update the movie's year
    if (choice == 3 || choice == 4) {
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
                movie->year = to_string(newYear);
                break;
            }
        }
    }

	// Print a success message indicating the movie details were updated
    cout << "[Success] Movie details updated.\n";

	// Add the updated movie to the AVL tree
    movieAVLTree.insert(movie);
}

// Function to combine both update functions
void updateDetails() {
    int updateChoice;
    cout << "Update: (1) Actor Details, (2) Movie Details: ";
    cin >> updateChoice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (updateChoice == 1) {
        updateActorDetails();
    }
    else if (updateChoice == 2) {
        updateMovieDetails();
    }
    else {
        cout << "[Error] Invalid choice.\n";
    }
}

// Function to display recent movies ( past 3 years ) of prompted year 
void displayRecentMovies() {

	// Prompt user to enter a year
    int baseYear;

	// Loop to ensure the user enters a valid year
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
    // Get Movies 3 years past the year prompted
    int lowerBound = baseYear - 3;
    vector<Movie*> allMovies = movieDictionary.getAllItems();
    vector<Movie*> recentMovies;

	// Loop through all movies and add to recentMovies if the year is within the range
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

	// Display the recent movies
    cout << "\nRecent Movies (From " << lowerBound << " to " << baseYear << "):\n";
    for (const Movie* movie : recentMovies) {
        cout << "Title: " << movie->title << "\nYear: " << movie->year
            << "\nPlot: " << movie->plot << "\n-------------------------\n";
    }
}

// Function to display actors within a specified age range.
void displayActorsByAgeRange() {

    // Create variables to store the lower and upper age limits
    int x, y;

    // Get the current year dynamically
    time_t t = time(nullptr);
    tm currentTime;
    localtime_s(&currentTime, &t);

    // Calculate the current year based on system time
    int currentYear = 1900 + currentTime.tm_year;

    // Loop to prompt for the lower age limit (x)
    while (true) {
        cout << "Enter the lower age limit (x): ";
        cin >> x;

        // Validate input: Must be a non-negative integer
        if (cin.fail() || x < 0) {
            cout << "[Error] Invalid age. Please enter a non-negative integer.\n";
            cin.clear(); // Clear error flag
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore invalid input
        }
        else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear remaining input
            break; // Exit loop when valid input is provided
        }
    }

    // Loop to prompt for the upper age limit (y)
    while (true) {
        cout << "Enter the upper age limit (y): ";
        cin >> y;

        // Validate input: Must be greater than or equal to the lower limit (x)
        if (cin.fail() || y < x) {
            cout << "[Error] Please enter an integer >= " << x << ".\n";
            cin.clear(); // Clear error flag
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore invalid input
        }
        else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear remaining input
            break; // Exit loop when valid input is provided
        }
    }

    // Retrieve all actors from the dictionary
    vector<Actor*> allActors = actorDictionary.getAllItems();
	vector<Actor*> filteredActors; // Vector to store actors within the specified age range

    // Filter actors based on the specified age range (x to y)
    for (Actor* actor : allActors) {
        int age = currentYear - actor->birthYear;
        if (age >= x && age <= y) {
            filteredActors.push_back(actor);
        }
    }

    // Sort filtered actors by age in ascending order
    sort(filteredActors.begin(), filteredActors.end(), [currentYear](Actor* a, Actor* b) {
        return (currentYear - a->birthYear) < (currentYear - b->birthYear);
        });

    // Display the sorted list of actors in the specified age range
    cout << "\n=====================================" << endl;
    cout << "Actors Aged Between " << x << " and " << y << ":" << endl;
    cout << "=====================================" << endl;

    // If no actors match the criteria, display an informative message
    if (filteredActors.empty()) {
        cout << "[Info] No actors found in the specified age range.\n";
    }
    else {
        // Display each actor's name and calculated age
        for (const Actor* actor : filteredActors) {
            cout << "Name: " << actor->name
                << " - Age: " << (currentYear - actor->birthYear)
                << "\n-------------------------\n";
        }
    }
}


// Function to get movies by actor
void getMoviesByActor() {

	// Prompt user to enter actor name
    string actorName = getNonEmptyInput("Enter actor name: ");

	// Check if the actor exists in the graph
    if (!actorMovieGraph.nodeExists(actorName)) {
        cout << "[Error] Actor \"" << actorName << "\" is not found in the graph.\n";
        return;
    }

	// Get all movies for the specified actor
    vector<string> movies = actorMovieGraph.listMoviesForActor(actorName);

	// Display the movies starred by the actor
    cout << "\nMovies Starred by " << actorName << ":\n";
    for (const string& movie : movies) {
        cout << " - " << movie << "\n";
    }
}

// Function to get actors by movie
void getActorsByMovie() {
    // Prompt the user for the movie title.
    string movieTitle = getNonEmptyInput("Enter movie title: ");

    // First, check if the movie node exists exactly as entered.
    if (!actorMovieGraph.nodeExists(movieTitle)) {
        // Prepare a variant with quotes.
        string quotedTitle = "\"" + movieTitle + "\"";
        if (actorMovieGraph.nodeExists(quotedTitle)) {
            movieTitle = quotedTitle;
        }
        else {
            // Movie node not found in the graph.
            // Instead of using movieDictionary.get (which uses the ID as key),
            // search through all movies for a matching title.
            Movie* movieObj = nullptr;
            vector<Movie*> allMovies = movieDictionary.getAllItems();
            for (Movie* m : allMovies) {
                // Compare the stored title with the input title.
                if (m->title == movieTitle) {
                    movieObj = m;
                    break;
                }
            }
            if (movieObj) {
                cout << "[Info] Movie \"" << movieTitle
                    << "\" exists but has no cast recorded.\n";
            }
            else {
                cout << "[Error] Movie \"" << movieTitle << "\" not found.\n";
            }
            return;
        }
    }

    // Retrieve the list of actors from the graph.
    vector<string> actors = actorMovieGraph.listActorsForMovie(movieTitle);
    if (actors.empty()) {
        cout << "[Info] There are no actors in the movie \"" << movieTitle << "\".\n";
    }
    else {
        cout << "\nActors in " << movieTitle << ":\n";
        for (const string& actor : actors) {
            cout << " - " << actor << "\n";
        }
    }
}





// Function to display actors known by a specified actor and vice versa
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

// Function to allow the user to rate an Actor or a Movie
void rateMovieOrActor() {

    // Prompt the user to choose between rating an Actor or a Movie
    cout << "\nDo you want to rate an Actor or a Movie? (A/M): ";
    char choice;
    cin >> choice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear input buffer

    // Convert choice to uppercase for case insensitivity
    choice = toupper(choice);

	// Function to rate an Actor
    if (choice == 'A') {

        // Get actor name from user input
        string actorName = getNonEmptyInput("Enter the Actor Name to rate: ");

        // Retrieve all actors from the dictionary
        vector<Actor*> allActors = actorDictionary.getAllItems();
        Actor* actor = nullptr; // Pointer to store the matched actor

        // Search for the actor by name
        for (Actor* a : allActors) {
            if (a->name == actorName) {
                actor = a;
                break; // Stop search when found
            }
        }

        // If the actor is not found, display an error message
        if (!actor) {
            cout << "[Error] Actor \"" << actorName << "\" not found.\n";
            return;
        }

        // Prompt user for a rating between 1 and 5 stars
        int stars;
        while (true) {
            cout << "Enter rating (1-5 stars): ";
            cin >> stars;

            // Validate rating input
            if (cin.fail() || stars < 1 || stars > 5) {
                cout << "[Error] Please enter a number between 1 and 5.\n";
                cin.clear(); // Clear error flag
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore invalid input
            }
            else {
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear input buffer
                break; // Exit loop when valid input is received
            }
        }

        // Update the actor's rating using the `updateRating()` function
        actor->updateRating(stars);

        // Display success message with the updated rating
        cout << "[Success] Actor \"" << actor->name << "\" now has an average rating of "
            << actor->rating << " (" << actor->noOfTimesRated << " ratings).\n";
    }

	// Function to rate a Movie
    else if (choice == 'M') {

        // Get movie title from user input
        string movieTitle = getNonEmptyInput("Enter the Movie Title to rate: ");

        // Retrieve all movies from the dictionary
        vector<Movie*> allMovies = movieDictionary.getAllItems();
        Movie* movie = nullptr; // Pointer to store the matched movie

        // Search for the movie by title
        for (Movie* m : allMovies) {
            if (m->title == movieTitle) {
                movie = m;
                break; // Stop search when found
            }
        }

        // If the movie is not found, display an error message
        if (!movie) {
            cout << "[Error] Movie \"" << movieTitle << "\" not found.\n";
            return;
        }

        // Prompt user for a rating between 1 and 5 stars
        int stars;
        while (true) {
            cout << "Enter rating (1-5 stars): ";
            cin >> stars;

            // Validate rating input
            if (cin.fail() || stars < 1 || stars > 5) {
                cout << "[Error] Please enter a number between 1 and 5.\n";
                cin.clear(); // Clear error flag
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore invalid input
            }
            else {
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear input buffer
                break; // Exit loop when valid input is received
            }
        }

        // Update the movie's rating using the `updateRating()` function
        movie->updateRating(stars);

        // Display success message with the updated rating
        cout << "[Success] Movie \"" << movie->title << "\" now has an average rating of "
            << movie->rating << " (" << movie->noOfTimesRated << " ratings).\n";
    }

    // If they didnt enter A or M , it prompts an error
    else {
        cout << "[Error] Invalid choice. Please select 'A' for actor or 'M' for movie.\n";
    }
}

// Function to display either top 10 actor or movie ratings
void displayRating() {

    // Prompt for either view top 10 actor or movie 
    cout << "\nDo you want to view Top 10 Actors or Movies? (A/M): ";
    char choice;
    cin >> choice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    
    // To upper so it doesnt break if its not captialize
    choice = toupper(choice);

	// If the user chooses to view top 10 actors
    if (choice == 'A') {
        vector<Actor*> actors = actorDictionary.getAllItems();
        if (!actors.empty())
            mergeSortActorsByRating(actors, 0, actors.size() - 1);
        displayTopActors(actors);
    }

	// If the user chooses to view top 10 movies
    else if (choice == 'M') {
        vector<Movie*> movies = movieDictionary.getAllItems();
        if (!movies.empty())
            mergeSortMoviesByRating(movies, 0, movies.size() - 1);
        displayTopMovies(movies);
    }
    
	// If they didnt enter A or M , it prompts an error
    else {
        cout << "[Error] Invalid choice. Please select 'A' for actors or 'M' for movies.\n";
    }
}

// ==================== Main Function ====================
int main() {

    // Load initial data using Dictionary methods.
    actorDictionary.loadFromCSV("../actors.csv", true);
    movieDictionary.loadFromCSV("../movies.csv", false);

    // Load cast relationships
    loadCastsFromCSV("../cast.csv");

    while (true) {

		// Display the main menu and prompt the user for a choice
        mainMenu();
        int choice;
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        // If Choice is 1 , opens admin menu
        if (choice == 1) { 
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

		// If Choice is 2 , opens user menu
        else if (choice == 2) { 
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

		// If Choice is 3 , calls storeDataToCsv function which calls all functions needed to append and save to CSV
        else if (choice == 3) {
            storeDataToCsv();
            cout << "[Info] Data stored to CSV files successfully.\n";
        }
        else {
            cout << "[Error] Invalid input, please try again.\n";
        }
    }
}