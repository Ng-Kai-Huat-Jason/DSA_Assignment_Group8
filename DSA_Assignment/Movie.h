// Movie.h
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
    double rating;
    int noOfTimesRated;
    vector<Actor*> actors;

    // Used to make sure if title is quoted it stays quoted from storing back to CSV
    bool titleWasQuoted;

    Movie(string id, string title, string plot, string year, double rating = 0.0, int noOfTimesRated = 0)
        : id(id), title(title), plot(plot), year(year), rating(rating), noOfTimesRated(noOfTimesRated),
        titleWasQuoted(false)
    {
    }

    void addActor(Actor* actor) {
        actors.push_back(actor);
    }

    int getYearAsInt() const {
        try {
            return stoi(year);
        }
        catch (...) {
            return -1;
        }
    }

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

    virtual ~Movie() {}  

};
