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
    double rating;    
    int noOfTimesRated; 
    vector<Movie*> movies;

    Actor(string id, string name, int birthYear, double rating = 0.0, int noOfTimesRated = 0)
        : id(id), name(name), birthYear(birthYear), rating(rating), noOfTimesRated(noOfTimesRated) {
    }


    void addMovie(Movie* movie) {
        movies.push_back(movie);
    }

    // Update the average rating with a new rating (1 to 5 stars).
    void updateRating(int newRating) {
        // new average = ((old average * count) + newRating) / (count + 1)
        rating = ((rating * noOfTimesRated) + newRating) / (noOfTimesRated + 1);
        noOfTimesRated++;
    }

    // Overload < operator based on birthYear.
    bool operator<(const Actor& other) const {
        return this->birthYear < other.birthYear;
    }

    // Overload > operator based on birthYear.
    bool operator>(const Actor& other) const {
        return this->birthYear > other.birthYear;
    }


    friend ostream& operator<<(ostream& os, const Actor& actor) {
        os << "Actor(ID: " << actor.id
            << ", Name: " << actor.name
            << ", Birth Year: " << actor.birthYear
            << ", Rating: " << actor.rating
            << ", Rated: " << actor.noOfTimesRated << ")";
        return os;
    }
};
