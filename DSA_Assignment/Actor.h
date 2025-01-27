#pragma once
#include <string>
#include <iostream>
#include <vector>

class Movie; // Forward declaration

class Actor {
public:
    std::string id;
    std::string name;
    int birthYear;
    std::vector<Movie*> movies;

    Actor(std::string id, std::string name, int birthYear)
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
    friend std::ostream& operator<<(std::ostream& os, const Actor& actor) {
        os << "Actor(ID: " << actor.id << ", Name: " << actor.name
            << ", Birth Year: " << actor.birthYear << ")";
        return os;
    }
};
