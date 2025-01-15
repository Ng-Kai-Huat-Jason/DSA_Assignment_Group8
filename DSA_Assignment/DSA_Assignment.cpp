// DSA_Assignment.cpp : This file contains the 'main' function. Program execution begins and ends there.
// Student 1: Ng Kai Huat Jason(S10262552)
// Student 2: Law Ming Qi

using namespace std;

#include <iostream>
#include <string>

#include "Dictionary.h"

// Create dictionaries for actors, movies, and casts
Dictionary<string, Actor> actorDictionary;
Dictionary<string, Movie> movieDictionary;
Dictionary<string, Cast> castDictionary;

// Functions 
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
	cout << "(4) Add update actor/movie details" << endl;
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
	cout << "(3) Display all movies and actors starred in alphabetical order" << endl;
	cout << "(4) Display all the actors in a particular movie in alphabetical order" << endl;
	cout << "(5) Display list of actors that a particular actor knows" << endl;
	cout << "(6) Go back to Main Menu" << endl;
	cout << "-------------------------" << endl;
	cout << "Enter your choice: ";
}

// Function to handle adding a new actor
void addActor() {
	Actor newActor;
	cout << "Enter Actor ID: ";
	cin >> newActor.id;
	cout << "Enter Actor Name: ";
	cin.ignore();
	getline(cin, newActor.name);
	cout << "Enter Actor Birth Year: ";
	cin >> newActor.birth;

	if (actorDictionary.add(newActor.id, newActor)) {
		cout << "Actor added successfully!\n";
	}
	else {
		cout << "Actor with this ID already exists.\n";
	}
}

// Function to handle adding a new movie
void addMovie() {
	Movie newMovie;
	cout << "Enter Movie ID: ";
	cin >> newMovie.id;
	cout << "Enter Movie Title: ";
	cin.ignore();
	getline(cin, newMovie.title);
	cout << "Enter Movie Plot: ";
	getline(cin, newMovie.plot);
	cout << "Enter Movie Year: ";
	cin >> newMovie.year;

	if (movieDictionary.add(newMovie.id, newMovie)) {
		cout << "Movie added successfully!\n";
	}
	else {
		cout << "Movie with this ID already exists.\n";
	}
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
		Cast newCast = { actorId, movieId };
		string castKey = actorId + ":" + movieId;

		if (castDictionary.add(castKey, newCast)) {
			cout << "Actor added to movie successfully!\n";
		}
		else {
			cout << "This actor is already part of the movie.\n";
		}
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
			cout << "Enter new Birth Year (current: " << actor->birth << "): ";
			cin >> actor->birth;

			cout << "Actor details updated successfully!\n";
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
		}
		else {
			cout << "Movie not found.\n";
		}
	}
	else {
		cout << "Invalid choice.\n";
	}
}

void addSampleData() {
	// Add sample actors
	Actor actor1 = { "111", "Tom Hanks", 1956 };
	Actor actor2 = { "222", "Leonardo DiCaprio", 1974 };
	actorDictionary.add(actor1.id, actor1);
	actorDictionary.add(actor2.id, actor2);

	// Add sample movies
	Movie movie1 = { "333", "Forrest Gump", "The story of a man running", 1994 };
	Movie movie2 = { "444", "Inception", "Dream within a dream in a dream", 2010 };
	movieDictionary.add(movie1.id, movie1);
	movieDictionary.add(movie2.id, movie2);

	// Add sample casts
	Cast cast1 = { actor1.id, movie1.id };
	Cast cast2 = { actor2.id, movie2.id };
	castDictionary.add(cast1.person_id + ":" + cast1.movie_id, cast1);
	castDictionary.add(cast2.person_id + ":" + cast2.movie_id, cast2);

}

int main()
{
	while (true) {
		// Call Main Menu
		mainMenu();

		int choice;
		cin >> choice;

		if (choice == 1) {
			while (true) {
				// Calling Admin Menu 
				cout << "\n";
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
					cout << "\n";
					break; // Exit the Admin menu loop
				}
				else {
					cout << "Invalid input, please try again.\n";
				}
			}
		}

		else if (choice == 2) {
			// User Menu Options
			cout << "\n" << endl;
			userMenu();

			int userChoice;
			cin >> userChoice;

			if (userChoice == 1) {
			}
			else if (userChoice == 2) {
			}
			else if (userChoice == 3) {
			}
			else if (userChoice == 4) {
			}
			else if (userChoice == 5) {
			}
			else if (userChoice == 6) {
				// Go back to Main Menu
				cout << "Returning to Main Menu..." << endl;
				cout << "\n" << endl;
			}
			else {
				cout << "Invalid input, please try again." << endl;
			}

		}
		else {
			cout << "Invalid input, please try again." << endl;
		}
	}
}


