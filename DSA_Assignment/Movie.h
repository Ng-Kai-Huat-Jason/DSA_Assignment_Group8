#pragma once
#include <iostream>
#include <string>
#include <vector>
using namespace std;

class Actor; // forward declaration

class Movie {
public:
    string id;
    string title;
    string plot;
    string year;  // Stored as string for CSV compatibility.
    double rating;      // Average rating (e.g., 4.2)
    int noOfTimesRated; // Number of ratings received.
    vector<Actor*> actors;

    Movie(string id, string title, string plot, string year, double rating = 0.0, int noOfTimesRated = 0)
        : id(id), title(title), plot(plot), year(year), rating(rating), noOfTimesRated(noOfTimesRated) {
    }

    // Add an actor to this movie.
    void addActor(Actor* actor) {
        actors.push_back(actor);
    }

    // Helper function to convert year to int.
    int getYearAsInt() const {
        try {
            return stoi(year);
        }
        catch (...) {
            return -1; // Return -1 if conversion fails.
        }
    }

    // Update the movie's rating with a new rating (1 to 5 stars).
    void updateRating(int newRating) {
        rating = ((rating * noOfTimesRated) + newRating) / (noOfTimesRated + 1);
        noOfTimesRated++;
    }

    // Overload < operator based on the movie's year.
    bool operator<(const Movie& other) const {
        return this->getYearAsInt() < other.getYearAsInt();
    }

    // Overload > operator based on the movie's year.
    bool operator>(const Movie& other) const {
        return this->getYearAsInt() > other.getYearAsInt();
    }

    friend ostream& operator<<(ostream& os, const Movie& movie) {
        os << "Movie(ID: " << movie.id
            << ", Title: " << movie.title
            << ", Year: " << movie.year
            << ", Plot: " << movie.plot
            << ", Rating: " << movie.rating
            << ", Rated: " << movie.noOfTimesRated << ")";
        return os;
    }
};
