// DSA_Assignment.cpp : This file contains the 'main' function. Program execution begins and ends there.
// Student 1: Ng Kai Huat Jason(S10262552)
// Student 2: Law Ming Qi

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

// CSV Functions 
// Function to load actors from CSV
void loadActorsFromCSV(const string& fileName) {
    ifstream file(fileName);
    if (!file.is_open()) {
        cout << "Failed to open " << fileName << endl;
        return;
    }

    string line;

    // Skip the header row
    getline(file, line);

    while (getline(file, line)) {
        stringstream ss(line);
        string id, name, birthYearStr;
        int birthYear;

        getline(ss, id, ',');
        getline(ss, name, ',');
        getline(ss, birthYearStr, ',');

        // Validate and parse birthYear
        try {
            birthYear = stoi(birthYearStr);
        }
        catch (const invalid_argument& e) {
            cout << "Invalid birth year for actor ID: " << id << endl;
            continue; // Skip this row
        }

        Actor* newActor = new Actor(id, name, birthYear);

        // Add actor to Dictionary and AVL Tree
        actorDictionary.add(id, newActor);
        actorAVLTree.insert(newActor);
    }

    file.close();
    cout << "Actors loaded successfully from " << fileName << endl;
}


// Function to load movies from CSV
void loadMoviesFromCSV(const string& fileName) {
    ifstream file(fileName);
    if (!file.is_open()) {
        cout << "Failed to open " << fileName << endl;
        return;
    }

    string line;

    // Skip the header row
    getline(file, line);

    while (getline(file, line)) {
        stringstream ss(line);
        string id, title, plot, yearStr;
        int year;

        getline(ss, id, ',');
        getline(ss, title, ',');
        getline(ss, plot, ',');
        getline(ss, yearStr, ',');

        // Validate and parse year
        try {
            year = stoi(yearStr);
        }
        catch (const invalid_argument& e) {
            cout << "Invalid year for movie ID: " << id << endl;
            continue; // Skip this row
        }

        Movie* newMovie = new Movie(id, title, plot, year);

        // Add movie to Dictionary and AVL Tree
        movieDictionary.add(id, newMovie);
        movieAVLTree.insert(newMovie);
    }

    file.close();
    cout << "Movies loaded successfully from " << fileName << endl;
}


// Function to load cast relationships from CSV
void loadCastsFromCSV(const string& fileName) {
    ifstream file(fileName);
    if (!file.is_open()) {
        cout << "Failed to open " << fileName << endl;
        return;
    }

    string line;

    // Skip the header row
    getline(file, line);

    while (getline(file, line)) {
        stringstream ss(line);
        string actorId, movieId;

        getline(ss, actorId, ',');
        getline(ss, movieId, ',');

        // Ensure actor exists in the graph
        if (actorDictionary.get(actorId)) {
            actorMovieGraph.addNode(actorId);
        }

        // Ensure movie exists in the graph
        if (movieDictionary.get(movieId)) {
            actorMovieGraph.addNode(movieId);
        }

        // Add edge between actor and movie
        if (actorDictionary.get(actorId) && movieDictionary.get(movieId)) {
            actorMovieGraph.addEdge(actorId, movieId);
        }
        else {
            cout << "Error: One or both nodes not found in the graph! (" << actorId << ", " << movieId << ")" << endl;
        }
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
        actorMovieGraph.addEdge(actorId, movieId);

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
            cout << "Enter new Name (current: " << actor->name << "): ";
            cin.ignore();
            getline(cin, actor->name);
            cout << "Enter new Birth Year (current: " << actor->birthYear << "): ";
            cin >> actor->birthYear;

            cout << "Actor details updated successfully!\n";

            // Re-insert into AVL Tree to maintain order
            actorAVLTree.insert(actor);
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
            cout << "Enter new Title (current: " << movie->title << "): ";
            cin.ignore();
            getline(cin, movie->title);
            cout << "Enter new Plot (current: " << movie->plot << "): ";
            getline(cin, movie->plot);
            cout << "Enter new Year (current: " << movie->year << "): ";
            cin >> movie->year;

            cout << "Movie details updated successfully!\n";

            // Re-insert into AVL Tree to maintain order
            movieAVLTree.insert(movie);
        }
        else {
            cout << "Movie not found.\n";
        }
    }
    else {
        cout << "Invalid choice.\n";
    }
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
            while (true) {
                adminMenu();

                int adminChoice;
                cin >> adminChoice;

                if (adminChoice == 1) {
                    addActor();
                }
                else if (adminChoice == 2) {
                    addMovie();
                }
                else if (adminChoice == 3) {
                    addActorToMovie();
                }
                else if (adminChoice == 4) {
                    updateDetails();
                }
                else if (adminChoice == 5) {
                    cout << "Returning to Main Menu...\n";
                    break;
                }
                else {
                    cout << "Invalid input, please try again.\n";
                }
            }
        }
        else if (choice == 2) {
            cout << "User features are under development.\n";
        }
        else {
            cout << "Invalid input, please try again.\n";
        }
    }
}

