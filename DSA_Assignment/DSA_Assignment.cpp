// DSA_Assignment.cpp : This file contains the 'main' function. Program execution begins and ends there.
// Student 1: Ng Kai Huat Jason(S10262552)
// Student 2: Law Ming Qi

using namespace std;

#include <iostream>
#include <string>

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

int main()
{
	while (true) {
		// Call Main Menu
		mainMenu();

		int choice;
		cin >> choice;

		if (choice == 1) {

			// Admin Menu Options
			while (true) {
				// Calling Admin Menu 
				cout << "\n" << endl;
				adminMenu();

				int adminChoice;
				cin >> adminChoice;

				if (adminChoice == 1) {

				}
				else if (adminChoice == 2) {

				}
				else if (adminChoice == 3) {

				}
				else if (adminChoice == 4) {

				}
				else if (adminChoice == 5) {
					// Go back to Main Menu
					cout << "Returning to Main Menu..." << endl;
					cout << "\n" << endl;
					break; // Exit the Admin menu loop
				}
				else {
					cout << "Invalid input, please try again." << endl;
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


