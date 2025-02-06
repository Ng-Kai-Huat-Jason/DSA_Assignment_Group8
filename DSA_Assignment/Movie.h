#pragma once
#include <string>
#include <iostream>
#include <vector>

using namespace std;

class Actor; 

class Movie {
public:
    string id;
    string title;
    string plot;
    string year; 
    vector<Actor*> actors;

    Movie(string id, string title, string plot, string year)
        : id(id), title(title), plot(plot), year(year) {
    }

    void addActor(Actor* actor) {
        actors.push_back(actor);
    }

    // Helper function to convert year to int for numeric operations
    int getYearAsInt() const {
        try {
            return stoi(year);
        }
        catch (...) {
            return -1; // Return -1 if conversion fails
        }
    }

    // Overload < operator
    bool operator<(const Movie& other) const {
        return this->getYearAsInt() < other.getYearAsInt();
    }

    // Overload > operator
    bool operator>(const Movie& other) const {
        return this->getYearAsInt() > other.getYearAsInt();
    }

    // Overload << operator
    friend ostream& operator<<(ostream& os, const Movie& movie) {
        os << "Movie(ID: " << movie.id << ", Title: " << movie.title
            << ", Year: " << movie.year << ", Plot: " << movie.plot << ")";
        return os;
    }
};

