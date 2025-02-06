#pragma once
#include <string>
#include <iostream>
#include <vector>

using namespace std;

class Movie; 

class Actor {
public:
    string id;
    string name;
    int birthYear;
    vector<Movie*> movies;

    Actor(string id, string name, int birthYear)
        : id(id), name(name), birthYear(birthYear) {
    }

    void addMovie(Movie* movie) {
        movies.push_back(movie);
    }

    // Overload < operator
    bool operator<(const Actor& other) const {
        return this->birthYear < other.birthYear;
    }

    // Overload > operator
    bool operator>(const Actor& other) const {
        return this->birthYear > other.birthYear;
    }

    // Overload << operator
    friend ostream& operator<<(ostream& os, const Actor& actor) {
        os << "Actor(ID: " << actor.id << ", Name: " << actor.name
            << ", Birth Year: " << actor.birthYear << ")";
        return os;
    }
};
