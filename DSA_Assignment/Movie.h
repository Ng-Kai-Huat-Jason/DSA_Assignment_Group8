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
    std::string year; // Change from int to string
    std::vector<Actor*> actors;

    Movie(std::string id, std::string title, std::string plot, std::string year)
        : id(id), title(title), plot(plot), year(year) {
    }

    void addActor(Actor* actor) {
        actors.push_back(actor);
    }

    // Helper function to convert year to int for numeric operations
    int getYearAsInt() const {
        try {
            return std::stoi(year);
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
    friend std::ostream& operator<<(std::ostream& os, const Movie& movie) {
        os << "Movie(ID: " << movie.id << ", Title: " << movie.title
            << ", Year: " << movie.year << ", Plot: " << movie.plot << ")";
        return os;
    }
};

