// DSA_Assignment.cpp : This file contains the 'main' function. Program execution begins and ends there.
// Student 1: Ng Kai Huat Jason(S10262552)
// Student 2: Law Ming Qi (S10257808)

using namespace std;

// Imports
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

// Data structures for actors and movies
Dictionary<string, Actor> actorDictionary;
Dictionary<string, Movie> movieDictionary;
AVLTree<Actor> actorAVLTree;
AVLTree<Movie> movieAVLTree;
Graph<string> actorMovieGraph;

// Menu Functions
void mainMenu() {
    cout << "-------------------------" << endl;
    cout << "Movie Information Helper" << endl;
    cout << "-------------------------" << endl;
    cout << "(1) Enter as Admin" << endl;
    cout << "(2) Enter as User" << endl;
    cout << "-------------------------" << endl;
    cout << "Enter your choice: ";
}

void adminMenu() {
    cout << "-------------------------" << endl;
    cout << "      Admin Menu" << endl;
    cout << "-------------------------" << endl;
    cout << "(1) Add new actor" << endl;
    cout << "(2) Add new movie" << endl;
    cout << "(3) Add an actor to a movie" << endl;
    cout << "(4) Update actor/movie details" << endl;
    cout << "(5) Go back to Main Menu" << endl;
    cout << "-------------------------" << endl;
    cout << "Enter your choice: ";
}

void userMenu() {
    cout << "-------------------------" << endl;
    cout << "      User Menu" << endl;
    cout << "-------------------------" << endl;
    cout << "(1) Display actors between a certain age in ascending order" << endl;
    cout << "(2) Display movies made within the past 3 years in ascending order" << endl;
    cout << "(3) Display all movies an actor starred in (in alphabetical order)" << endl;
    cout << "(4) Display all the actors in a particular movie (in alphabetical order)" << endl;
    cout << "(5) Display a list of all actors that a particular actor knows" << endl;
    cout << "(6) Go back to Main Menu" << endl;
    cout << "-------------------------" << endl;
    cout << "Enter your choice: ";
}


// CSV Functions 
// Function to load actors from CSV
void loadActorsFromCSV(const std::string& fileName) {
    std::ifstream file(fileName);
    if (!file.is_open()) {
        std::cerr << "Failed to open " << fileName << std::endl;
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
                    std::cerr << "Duplicate actor ID: " << id << std::endl;
                    delete newActor;
                }
            }
            else {
                std::cerr << "Invalid data for actor: " << line << std::endl;
                delete newActor;
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Error parsing actor data: " << line << " (" << e.what() << ")" << std::endl;
        }
    }

    file.close();
    std::cout << "Actors loaded successfully from " << fileName << std::endl;
}





// Function to load movies from CSV
void loadMoviesFromCSV(const string& fileName) {
    ifstream file(fileName);
    if (!file.is_open()) {
        cout << "Error: Failed to open " << fileName << endl;
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
            cout << "Warning: Skipping invalid movie record: " << line << endl;
            continue;
        }

        try {
            year = stoi(yearStr);
        }
        catch (const invalid_argument&) {
            cout << "Warning: Invalid year for movie ID " << id << ". Skipping record.\n";
            continue;
        }

        Movie* newMovie = new Movie(id, title, plot, year);

        if (!movieDictionary.add(id, newMovie)) {
            delete newMovie; // Prevent memory leak if already exists
        }
        else {
            movieAVLTree.insert(newMovie);
        }
    }

    file.close();
    cout << "Movies loaded successfully from " << fileName << endl;
}




// Function to load cast relationships from CSV
void loadCastsFromCSV(const string& fileName) {
    ifstream file(fileName);
    if (!file.is_open()) {
        cout << "Error: Failed to open " << fileName << endl;
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
            cout << "Warning: Skipping invalid cast record: " << line << endl;
            continue;
        }

        // Check if actor and movie exist in the dictionaries
        Actor* actor = actorDictionary.get(actorId);
        Movie* movie = movieDictionary.get(movieId);

        if (!actor || !movie) {
            cout << "Warning: Actor or Movie not found for IDs (" << actorId << ", " << movieId << "). Skipping record.\n";
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
    cout << "Casts loaded successfully from " << fileName << endl;
}



// Function to handle adding a new actor
void addActor() {
    string id, name;
    int birthYear;

    cout << "Enter Actor ID: ";
    cin >> id;
    cout << "Enter Actor Name: ";
    cin.ignore();
    getline(cin, name);
    cout << "Enter Actor Birth Year: ";
    cin >> birthYear;

    Actor* newActor = new Actor(id, name, birthYear);

    // Add to Dictionary
    if (actorDictionary.add(id, newActor)) {
        cout << "Actor added to Dictionary successfully!\n";
    }
    else {
        cout << "Actor with this ID already exists in Dictionary.\n";
    }

    // Add to AVL Tree
    actorAVLTree.insert(newActor);
    cout << "Actor added to AVL Tree successfully!\n";

    // Add actor to graph
    actorMovieGraph.addNode(name);
    cout << "Actor added to graph successfully!\n";
}



// Function to handle adding a new movie
void addMovie() {
    string id, title, plot;
    int year;

    cout << "Enter Movie ID: ";
    cin >> id;
    cout << "Enter Movie Title: ";
    cin.ignore();
    getline(cin, title);
    cout << "Enter Movie Plot: ";
    getline(cin, plot);
    cout << "Enter Movie Year: ";
    cin >> year;

    Movie* newMovie = new Movie(id, title, plot, year);

    // Add to Dictionary
    if (movieDictionary.add(id, newMovie)) {
        cout << "Movie added to Dictionary successfully!\n";
    }
    else {
        cout << "Movie with this ID already exists in Dictionary.\n";
    }

    // Add to AVL Tree
    movieAVLTree.insert(newMovie);
    cout << "Movie added to AVL Tree successfully!\n";

    // Add movie to graph
    actorMovieGraph.addNode(title);
    cout << "Movie added to graph successfully!\n";
}


// Function to handle adding an actor to a movie
void addActorToMovie() {
    string actorId, movieId;

    cout << "Enter Actor ID: ";
    cin >> actorId;
    cout << "Enter Movie ID: ";
    cin >> movieId;

    Actor* actor = actorDictionary.get(actorId);
    Movie* movie = movieDictionary.get(movieId);

    if (actor && movie) {
        actor->addMovie(movie);
        movie->addActor(actor);

        // Update Graph
        actorMovieGraph.addEdge(actor->name, movie->title);

        cout << "Actor added to Movie and Graph successfully!\n";
    }
    else {
        cout << "Invalid Actor ID or Movie ID.\n";
    }
}


// Function to update an actor or movie details
void updateDetails() {
    int updateChoice;

    cout << "Update: (1) Actor Details, (2) Movie Details: ";
    cin >> updateChoice;

    if (updateChoice == 1) {
        string actorId;

        cout << "Enter Actor ID to update: ";
        cin >> actorId;

        Actor* actor = actorDictionary.get(actorId);
        if (actor) {
            string oldName = actor->name; // Store old name

            cout << "Enter new Name (current: " << actor->name << "): ";
            cin.ignore();
            getline(cin, actor->name);
            cout << "Enter new Birth Year (current: " << actor->birthYear << "): ";
            cin >> actor->birthYear;

            cout << "Actor details updated successfully!\n";

            // Update AVL Tree
            actorAVLTree.insert(actor);

            // Update Graph if name changed
            if (oldName != actor->name) {
                actorMovieGraph.removeNode(oldName);      // Remove old node
                actorMovieGraph.addNode(actor->name);     // Add new node

                // Reconnect edges
                for (Movie* movie : actor->movies) {
                    actorMovieGraph.addEdge(actor->name, movie->title);
                }
            }
        }
        else {
            cout << "Actor not found.\n";
        }
    }
    else if (updateChoice == 2) {
        string movieId;

        cout << "Enter Movie ID to update: ";
        cin >> movieId;

        Movie* movie = movieDictionary.get(movieId);
        if (movie) {
            string oldTitle = movie->title; // Store old title

            cout << "Enter new Title (current: " << movie->title << "): ";
            cin.ignore();
            getline(cin, movie->title);
            cout << "Enter new Plot (current: " << movie->plot << "): ";
            getline(cin, movie->plot);
            cout << "Enter new Year (current: " << movie->year << "): ";
            cin >> movie->year;

            cout << "Movie details updated successfully!\n";

            // Update AVL Tree
            movieAVLTree.insert(movie);

            // Update Graph if title changed
            if (oldTitle != movie->title) {
                actorMovieGraph.removeNode(oldTitle);      // Remove old node
                actorMovieGraph.addNode(movie->title);     // Add new node

                // Reconnect edges
                for (Actor* actor : movie->actors) {
                    actorMovieGraph.addEdge(actor->name, movie->title);
                }
            }
        }
        else {
            cout << "Movie not found.\n";
        }
    }
    else {
        cout << "Invalid choice.\n";
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

    for (size_t i = 0; i < recentMovies.size(); ++i) {
        for (size_t j = i + 1; j < recentMovies.size(); ++j) {
            if (recentMovies[i]->year > recentMovies[j]->year) {
                Movie* temp = recentMovies[i];
                recentMovies[i] = recentMovies[j];
                recentMovies[j] = temp;
            }
        }
    }

    // Display the movies
    cout << "Movies made within the past 3 years (in ascending order):" << endl;
    for (const Movie* movie : recentMovies) {
        cout << "Title: " << movie->title << ", Year: " << movie->year << ", Plot: " << movie->plot << endl;
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
    cout << "Enter the lower age limit (x): ";
    cin >> x;
    cout << "Enter the upper age limit (y): ";
    cin >> y;

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

    // Display actors
    cout << "Actors aged between " << x << " and " << y << " (in ascending order of age):" << endl;
    for (const Actor* actor : filteredActors) {
        cout << "Name: " << actor->name << ", Age: " << (currentYear - actor->birthYear) << endl;
    }
}

void getMoviesByActor() {
    string actorName;
    cout << "Enter actor name: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, actorName);

    // Check if the actor exists in the graph
    int actorIndex = actorMovieGraph.getNodeIndex(actorName);
    if (actorIndex == -1) {
        cout << "\nActor \"" << actorName << "\" not found." << endl;
        return;
    }

    // Retrieve the movies for the actor 
    vector<string> movies = actorMovieGraph.getNeighbors(actorName);

    if (movies.empty()) {
        cout << "\nNo movies found for the actor \"" << actorName << "\"." << endl;
    }
    else {
        cout << "\nMovies played by " << actorName << ":" << endl;
        for (const auto& movie : movies) {
            cout << " - " << movie << endl;
        }
    }
}

void getActorsByMovie() {
    string movieTitle;
    cout << "Enter movie title: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear any leftover input
    getline(cin, movieTitle);

    // Check if the movie exists in the graph
    int movieIndex = actorMovieGraph.getNodeIndex(movieTitle);
    if (movieIndex == -1) {
        cout << "\nMovie \"" << movieTitle << "\" not found in the graph." << endl;
        return;
    }

    // Retrieve the actors for the movie (actors are the neighbors of the movie node)
    vector<string> actors = actorMovieGraph.getNeighbors(movieTitle);

    if (actors.empty()) {
        cout << "\nNo actors found for the movie \"" << movieTitle << "\"." << endl;
    }
    else {
        for (size_t i = 0; i < actors.size(); ++i) {
            for (size_t j = i + 1; j < actors.size(); ++j) {
                if (actors[i] > actors[j]) {
                    swap(actors[i], actors[j]);
                }
            }
        }

        cout << "\nActors in \"" << movieTitle << "\":" << endl;
        for (const auto& actor : actors) {
            cout << " - " << actor << endl;
        }
    }
}


void displayKnownActors() {
    string actorName;
    cout << "Enter actor name: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear any leftover input
    getline(cin, actorName);

    // Check if actor exists in the graph
    if (!actorMovieGraph.nodeExists(actorName)) {
        cout << "Actor not found" << endl;
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

        if (choice == 1) {
            // Admin Features
            while (true) {
                adminMenu();

                int adminChoice;
                cin >> adminChoice;

                if (adminChoice == 5) {
                    cout << "Returning to Main Menu...\n";
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
                default : 
                    cout << "Invalid input, please try again.\n";
                }
            }
        }
        else if (choice == 2) {
            // User Features
            while (true) {
                userMenu();

                int userChoice;
                cin >> userChoice;

                if (userChoice == 6) {
                    cout << "Returning to Main Menu...\n";
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

                case 4:{
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
                    cout << "Invalid input, please try again.\n";
                }
            }
        }
        else {
            cout << "Invalid input, please try again.\n";
        }
    }
}


