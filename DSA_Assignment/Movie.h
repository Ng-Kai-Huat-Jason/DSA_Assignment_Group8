#pragma once
#include <string>
#include <iostream>
#include <vector>

class Actor; // Forward declaration

class Movie {
public:
    std::string id;
    std::string title;
    std::string plot;
    int year;
    std::vector<Actor*> actors;

    Movie(std::string id, std::string title, std::string plot, int year)
        : id(id), title(title), plot(plot), year(year) {
    }

    void addActor(Actor* actor) {
        actors.push_back(actor);
    }

    // Overload < operator
    bool operator<(const Movie& other) const {
        return this->year < other.year;
    }

    // Overload > operator
    bool operator>(const Movie& other) const {
        return this->year > other.year;
    }

    // Overload << operator
    friend std::ostream& operator<<(std::ostream& os, const Movie& movie) {
        os << "Movie(ID: " << movie.id << ", Title: " << movie.title
            << ", Year: " << movie.year << ", Plot: " << movie.plot << ")";
        return os;
    }
};
