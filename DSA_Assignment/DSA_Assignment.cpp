// DSA_Assignment.cpp : This file contains the 'main' function. Program execution begins and ends there.
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
#include <ctime>     // For getting the current year dynamically
#include <limits>    // For numeric_limits

using namespace std;

// Vectors to track new entries
vector<Actor*> newActors;
vector<Movie*> newMovies;
vector<pair<string, string>> newCasts; // Pair of ActorID and MovieID


// Data structures for actors and movies
Dictionary<string, Actor> actorDictionary;
Dictionary<string, Movie> movieDictionary;
AVLTree<Actor> actorAVLTree;
AVLTree<Movie> movieAVLTree;
Graph<string> actorMovieGraph;

// Menu Functions
void mainMenu() {
    cout << "=====================================" << endl;
    cout << "        Movie Information Helper     " << endl;
    cout << "=====================================" << endl;
    cout << "(1) Enter as Admin" << endl;
    cout << "(2) Enter as User" << endl;
    cout << "=====================================" << endl;
    cout << "Enter your choice: ";
}

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

// Function to get non-empty string input
string getNonEmptyInput(const string& prompt) {
    string input;
    while (true) {
        cout << prompt;
        getline(cin, input);
        // Remove leading/trailing whitespace
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

// CSV Functions 
// Function to load actors from CSV
void loadActorsFromCSV(const std::string& fileName) {
    std::ifstream file(fileName);
    if (!file.is_open()) {
        std::cerr << "[Error] Failed to open " << fileName << std::endl;
        return;
    }

    std::string line;
    std::getline(file, line); // Skip header row

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string id, name, birthYearStr;

        // Read values
        std::getline(ss, id, ',');
        std::getline(ss, name, ',');
        std::getline(ss, birthYearStr, ',');

        // Remove surrounding quotes, if any
        if (!name.empty() && name.front() == '"') name.erase(0, 1);
        if (!name.empty() && name.back() == '"') name.erase(name.size() - 1);

        try {
            // Validate and parse birthYear
            int birthYear = std::stoi(birthYearStr);

            // Create new Actor
            Actor* newActor = new Actor(id, name, birthYear);

            // Validate and add to Dictionary
            if (!id.empty() && !name.empty() && birthYear > 0) {
                if (!actorDictionary.add(id, newActor)) {
                    std::cerr << "[Error] Duplicate actor ID: " << id << std::endl;
                    delete newActor;
                }
            }
            else {
                std::cerr << "[Error] Invalid data for actor: " << line << std::endl;
                delete newActor;
            }
        }
        catch (const std::exception& e) {
            std::cerr << "[Error] Parsing actor data: " << line << " (" << e.what() << ")" << std::endl;
        }
    }

    file.close();
    std::cout << "[Info] Actors loaded successfully from " << fileName << std::endl;
}

// Function to load movies from CSV
void loadMoviesFromCSV(const string& fileName) {
    ifstream file(fileName);
    if (!file.is_open()) {
        cout << "[Error] Failed to open " << fileName << endl;
        return;
    }

    string line;

    // Skip the first line (header)
    getline(file, line);

    while (getline(file, line)) {
        stringstream ss(line);
        string id, title, plot, yearStr;
        int year;

        getline(ss, id, ',');
        getline(ss, title, ',');
        getline(ss, plot, ',');
        getline(ss, yearStr, ',');

        // Remove leading/trailing spaces or quotes
        if (!title.empty() && title.front() == '"') title.erase(0, 1);
        if (!title.empty() && title.back() == '"') title.erase(title.size() - 1);
        if (!plot.empty() && plot.front() == '"') plot.erase(0, 1);
        if (!plot.empty() && plot.back() == '"') plot.erase(plot.size() - 1);

        // Validate input
        if (id.empty() || title.empty() || yearStr.empty()) {
            cout << "[Warning] Skipping invalid movie record: " << line << endl;
            continue;
        }

        try {
            year = stoi(yearStr);
        }
        catch (const invalid_argument&) {
            cout << "[Warning] Invalid year for movie ID " << id << ". Skipping record.\n";
            continue;
        }

        Movie* newMovie = new Movie(id, title, plot, year);

        if (!movieDictionary.add(id, newMovie)) {
            cout << "[Warning] Movie with ID " << id << " already exists. Skipping addition.\n";
            delete newMovie; // Prevent memory leak if already exists
        }
        else {
            movieAVLTree.insert(newMovie);
            cout << "[Success] Movie \"" << title << "\" added to AVL Tree successfully!\n";
        }
    }

    file.close();
    cout << "[Info] Movies loaded successfully from " << fileName << endl;
}

// Function to load cast relationships from CSV
void loadCastsFromCSV(const string& fileName) {
    ifstream file(fileName);
    if (!file.is_open()) {
        cout << "[Error] Failed to open " << fileName << endl;
        return;
    }

    string line;

    // Skip the first line (header)
    getline(file, line);

    while (getline(file, line)) {
        stringstream ss(line);
        string actorId, movieId;

        getline(ss, actorId, ',');
        getline(ss, movieId, ',');

        // Validate input
        if (actorId.empty() || movieId.empty()) {
            cout << "[Warning] Skipping invalid cast record: " << line << endl;
            continue;
        }

        // Check if actor and movie exist in the dictionaries
        Actor* actor = actorDictionary.get(actorId);
        Movie* movie = movieDictionary.get(movieId);

        if (!actor || !movie) {
            cout << "[Warning] Actor or Movie not found for IDs (" << actorId << ", " << movieId << "). Skipping record.\n";
            continue;
        }

        // Add actor-movie relationship
        actor->addMovie(movie);
        movie->addActor(actor);

        // Ensure both actor and movie exist in the graph
        if (!actorMovieGraph.nodeExists(actor->name)) {
            actorMovieGraph.addNode(actor->name);
        }
        if (!actorMovieGraph.nodeExists(movie->title)) {
            actorMovieGraph.addNode(movie->title);
        }

        // Add edge to the graph
        actorMovieGraph.addEdge(actor->name, movie->title);
    }

    file.close();
    cout << "[Info] Casts loaded successfully from " << fileName << endl;
}

// Function to write new entries to CSV files upon program termination
void writeToCSV() {
    // Append new actors to actors.csv
    if (!newActors.empty()) {
        ofstream actorsFile("../actors.csv", ios::app);
        if (actorsFile.is_open()) {
            for (const Actor* actor : newActors) {
                actorsFile << actor->id << ",\"" << actor->name << "\"," << actor->birthYear << "\n";
            }
            actorsFile.close();
            cout << "[Info] New actors appended to actors.csv successfully.\n";
        }
        else {
            cout << "[Error] Unable to open actors.csv for appending.\n";
        }
    }

    // Append new movies to movies.csv
    if (!newMovies.empty()) {
        ofstream moviesFile("../movies.csv", ios::app);
        if (moviesFile.is_open()) {
            for (const Movie* movie : newMovies) {
                // Handle quotes in plot by replacing any existing quotes with double quotes
                string sanitizedPlot = movie->plot;
                size_t pos = 0;
                while ((pos = sanitizedPlot.find("\"", pos)) != string::npos) {
                    sanitizedPlot.replace(pos, 1, "\"\"");
                    pos += 2; // Move past the escaped quotes
                }

                moviesFile << movie->id << ",\"" << movie->title << "\",\"" << sanitizedPlot << "\"," << movie->year << "\n";
            }
            moviesFile.close();
            cout << "[Info] New movies appended to movies.csv successfully.\n";
        }
        else {
            cout << "[Error] Unable to open movies.csv for appending.\n";
        }
    }

    // Append new casts to cast.csv
    if (!newCasts.empty()) {
        ofstream castFile("../cast.csv", ios::app);
        if (castFile.is_open()) {
            for (const auto& cast : newCasts) {
                castFile << cast.first << "," << cast.second << "\n";
            }
            castFile.close();
            cout << "[Info] New cast relationships appended to cast.csv successfully.\n";
        }
        else {
            cout << "[Error] Unable to open cast.csv for appending.\n";
        }
    }
}

// Assignemnt Features
// Function to handle adding a new actor 
void addActor() {
    string id, name;
    int birthYear;

    // Get non-empty Actor ID
    id = getNonEmptyInput("Enter Actor ID: ");

    // Get non-empty Actor Name
    name = getNonEmptyInput("Enter Actor Name: ");

    // Get valid Birth Year
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

    // Add to Dictionary
    if (actorDictionary.add(id, newActor)) {
        cout << "[Success] Actor \"" << name << "\" (ID: " << id << ") added to Dictionary successfully!\n";
        // Track the new actor for appending to CSV
        newActors.push_back(newActor);
    }
    else {
        cout << "[Error] Actor with ID \"" << id << "\" already exists in Dictionary.\n";
        delete newActor; // Prevent memory leak if not added
        return;
    }

    // Add to AVL Tree
    actorAVLTree.insert(newActor);
    cout << "[Success] Actor \"" << name << "\" added to AVL Tree successfully!\n";

    // Add actor to graph
    actorMovieGraph.addNode(name);
    cout << "[Success] Actor \"" << name << "\" added to graph successfully!\n";
}


// Function to handle adding a new movie 
void addMovie() {
    string id, title, plot;
    int year;

    // Get non-empty Movie ID
    id = getNonEmptyInput("Enter Movie ID: ");

    // Get non-empty Movie Title
    title = getNonEmptyInput("Enter Movie Title: ");

    // Get non-empty Movie Plot
    plot = getNonEmptyInput("Enter Movie Plot: ");

    // Get valid Year
    while (true) {
        cout << "Enter Movie Year: ";
        cin >> year;
        if (cin.fail() || year <= 0) {
            cout << "[Error] Invalid year. Please enter a positive integer.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            break;
        }
    }

    Movie* newMovie = new Movie(id, title, plot, year);

    // Add to Dictionary
    if (movieDictionary.add(id, newMovie)) {
        cout << "[Success] Movie \"" << title << "\" (ID: " << id << ") added to Dictionary successfully!\n";
        // Track the new movie for appending to CSV
        newMovies.push_back(newMovie);
    }
    else {
        cout << "[Error] Movie with ID \"" << id << "\" already exists in Dictionary.\n";
        delete newMovie; // Prevent memory leak if not added
        return;
    }

    // Add to AVL Tree
    movieAVLTree.insert(newMovie);
    cout << "[Success] Movie \"" << title << "\" added to AVL Tree successfully!\n";

    // Add movie to graph
    actorMovieGraph.addNode(title);
    cout << "[Success] Movie \"" << title << "\" added to graph successfully!\n";
}


// Function to handle adding an actor to a movie 
void addActorToMovie() {
    string actorId, movieId;

    // Get non-empty Actor ID
    actorId = getNonEmptyInput("Enter Actor ID: ");

    // Get non-empty Movie ID
    movieId = getNonEmptyInput("Enter Movie ID: ");

    Actor* actor = actorDictionary.get(actorId);
    Movie* movie = movieDictionary.get(movieId);

    if (actor && movie) {
        // Check if the actor is already associated with the movie
        bool alreadyCast = false;
        for (Movie* m : actor->movies) {
            if (m->id == movieId) {
                alreadyCast = true;
                break;
            }
        }

        if (alreadyCast) {
            cout << "[Info] Actor \"" << actor->name << "\" is already cast in movie \"" << movie->title << "\".\n";
            return;
        }

        actor->addMovie(movie);
        movie->addActor(actor);

        // Update Graph
        actorMovieGraph.addEdge(actor->name, movie->title);

        // Track the new cast relationship for appending to CSV
        newCasts.emplace_back(actorId, movieId);

        // Inform the user about the addition
        cout << "[Success] Actor \"" << actor->name << "\" has been added to movie \"" << movie->title << "\" as part of the cast.\n";
    }
    else {
        cout << "[Error] Invalid Actor ID or Movie ID.\n";
    }
}



// Function to update an actor or movie details
void updateDetails() {
    int updateChoice;

    cout << "Update: (1) Actor Details, (2) Movie Details: ";
    cin >> updateChoice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the input buffer

    if (updateChoice == 1) {
        string actorId;

        // Get non-empty Actor ID
        actorId = getNonEmptyInput("Enter Actor ID to update: ");

        Actor* actor = actorDictionary.get(actorId);
        if (actor) {
            string oldName = actor->name; // Store old name

            // Get new Name
            string newName = getNonEmptyInput("Enter new Name (current: " + actor->name + "): ");
            // Get new Birth Year
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

            // Update details
            actor->name = newName;
            actor->birthYear = newBirthYear;

            cout << "[Success] Actor details updated successfully!\n";

            // Update AVL Tree
            actorAVLTree.insert(actor);
            cout << "[Success] Actor \"" << newName << "\" added to AVL Tree successfully!\n";

            // Update Graph if name changed
            if (oldName != actor->name) {
                actorMovieGraph.removeNode(oldName);      // Remove old node
                actorMovieGraph.addNode(actor->name);     // Add new node

                // Reconnect edges
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
        string movieId;

        // Get non-empty Movie ID
        movieId = getNonEmptyInput("Enter Movie ID to update: ");

        Movie* movie = movieDictionary.get(movieId);
        if (movie) {
            string oldTitle = movie->title; // Store old title

            // Get new Title
            string newTitle = getNonEmptyInput("Enter new Title (current: " + movie->title + "): ");
            // Get new Plot
            string newPlot = getNonEmptyInput("Enter new Plot (current: " + movie->plot + "): ");
            // Get new Year
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

            // Update details
            movie->title = newTitle;
            movie->plot = newPlot;
            movie->year = newYear;

            cout << "[Success] Movie details updated successfully!\n";

            // Update AVL Tree
            movieAVLTree.insert(movie);
            cout << "[Success] Movie \"" << newTitle << "\" added to AVL Tree successfully!\n";

            // Update Graph if title changed
            if (oldTitle != movie->title) {
                actorMovieGraph.removeNode(oldTitle);      // Remove old node
                actorMovieGraph.addNode(movie->title);     // Add new node

                // Reconnect edges
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

void displayRecentMovies() {
    // Get the current year
    time_t t = time(nullptr);
    tm currentTime;
    localtime_s(&currentTime, &t);
    int currentYear = 1900 + currentTime.tm_year;

    // Get all movies from the dictionary
    vector<Movie*> allMovies = movieDictionary.getAllItems();

    // Filter movies made within the past 3 years
    vector<Movie*> recentMovies;
    for (Movie* movie : allMovies) {
        if (currentYear - movie->year <= 3) {
            recentMovies.push_back(movie);
        }
    }

    // Sort movies by ascending year
    for (size_t i = 0; i < recentMovies.size(); ++i) {
        for (size_t j = i + 1; j < recentMovies.size(); ++j) {
            if (recentMovies[i]->year > recentMovies[j]->year) {
                Movie* temp = recentMovies[i];
                recentMovies[i] = recentMovies[j];
                recentMovies[j] = temp;
            }
        }
    }

    // Display the movies with neat formatting
    cout << "\n=====================================" << endl;
    cout << "Recent Movies (Last 3 Years)" << endl;
    cout << "=====================================" << endl;
    if (recentMovies.empty()) {
        cout << "[Info] No recent movies found within the past 3 years.\n";
    }
    else {
        for (const Movie* movie : recentMovies) {
            cout << "Title: " << movie->title << "\nYear: " << movie->year << "\nPlot: " << movie->plot << "\n-------------------------\n";
        }
    }
}

void displayActorsByAgeRange() {
    int x, y;

    // Get the current year
    time_t t = time(nullptr);
    tm currentTime;
    localtime_s(&currentTime, &t);
    int currentYear = 1900 + currentTime.tm_year;

    // Get age range from the user
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

    // Store actors in a vector
    vector<Actor*> filteredActors;
    vector<Actor*> allActors = actorDictionary.getAllItems();

    // Filter actors by age range
    for (Actor* actor : allActors) {
        int age = currentYear - actor->birthYear;
        if (age >= x && age <= y) {
            filteredActors.push_back(actor);
        }
    }

    // Sort actors by ascending age
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

    // Display actors with neat formatting
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

void getMoviesByActor() {
    string actorName;
    cout << "Enter actor name: ";
    getline(cin, actorName);

    // Check if the actor exists in the graph
    int actorIndex = actorMovieGraph.getNodeIndex(actorName);
    if (actorIndex == -1) {
        cout << "\n[Error] Actor \"" << actorName << "\" not found.\n";
        return;
    }

    // Retrieve the movies for the actor 
    vector<string> movies = actorMovieGraph.getNeighbors(actorName);

    if (movies.empty()) {
        cout << "\n[Info] No movies found for the actor \"" << actorName << "\".\n";
    }
    else {
        // Sort movies alphabetically
        for (size_t i = 0; i < movies.size(); ++i) {
            for (size_t j = i + 1; j < movies.size(); ++j) {
                if (movies[i] > movies[j]) {
                    swap(movies[i], movies[j]);
                }
            }
        }

        // Display movies with neat formatting
        cout << "\n=====================================" << endl;
        cout << "Movies Starred by " << actorName << endl;
        cout << "=====================================" << endl;
        for (const auto& movie : movies) {
            cout << " - " << movie << "\n";
        }
        cout << "=====================================\n" << endl;
    }
}

void getActorsByMovie() {
    string movieTitle;
    cout << "Enter movie title: ";
    getline(cin, movieTitle);

    // Check if the movie exists in the graph
    int movieIndex = actorMovieGraph.getNodeIndex(movieTitle);
    if (movieIndex == -1) {
        cout << "\n[Error] Movie \"" << movieTitle << "\" not found in the graph.\n";
        return;
    }

    // Retrieve the actors for the movie (actors are the neighbors of the movie node)
    vector<string> actors = actorMovieGraph.getNeighbors(movieTitle);

    if (actors.empty()) {
        cout << "\n[Info] No actors found for the movie \"" << movieTitle << "\".\n";
    }
    else {
        // Sort actors alphabetically
        for (size_t i = 0; i < actors.size(); ++i) {
            for (size_t j = i + 1; j < actors.size(); ++j) {
                if (actors[i] > actors[j]) {
                    swap(actors[i], actors[j]);
                }
            }
        }

        // Display actors with neat formatting
        cout << "\n=====================================" << endl;
        cout << "Actors in \"" << movieTitle << "\"" << endl;
        cout << "=====================================" << endl;
        for (const auto& actor : actors) {
            cout << " - " << actor << "\n";
        }
        cout << "=====================================\n" << endl;
    }
}

void displayKnownActors() {
    string actorName;
    cout << "Enter actor name: ";
    getline(cin, actorName);

    // Check if actor exists in the graph
    if (!actorMovieGraph.nodeExists(actorName)) {
        cout << "[Error] Actor \"" << actorName << "\" not found.\n";
        return;
    }

    // Call BFS with the actor's name
    actorMovieGraph.bfs(actorName);
}

int main() {

    // Load data from CSV files
    loadActorsFromCSV("../actors.csv");
    loadMoviesFromCSV("../movies.csv");
    loadCastsFromCSV("../cast.csv");

    while (true) {
        mainMenu();

        int choice;
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the input buffer

        if (choice == 1) {
            // Admin Features
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
                case 1: {
                    // Feature A : Add new actor
                    addActor();
                    break;
                }
                case 2: {
                    // Feature B : Add new movie
                    addMovie();
                    break;
                }
                case 3: {
                    // Feature C : Add an actor to a movie
                    addActorToMovie();
                    break;
                }
                case 4: {
                    // Feature D : Update actor/movie details
                    updateDetails();
                    break;
                }
                default:
                    cout << "[Error] Invalid input, please try again.\n";
                }
            }
        }
        else if (choice == 2) {
            // User Features
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
                case 1: {
                    // Feature E: Display actors between a certain age
                    displayActorsByAgeRange();
                    break;
                }
                case 2: {
                    // Feature F: Display movies made within the past 3 years
                    displayRecentMovies();
                    break;
                }

                case 3: {
                    // Feature G: Display all movies an actor starred in
                    getMoviesByActor();
                    break;
                }

                case 4: {
                    // Feature H: Display all the actors in a particular movie
                    getActorsByMovie();
                    break;
                }

                case 5: {
                    // Feature I: Display list of actors a particular actor knows
                    displayKnownActors();
                    break;
                }

                default:
                    cout << "[Error] Invalid input, please try again.\n";
                }
            }
        }
        else {
            cout << "[Error] Invalid input, please try again.\n";
        }
    }
}
