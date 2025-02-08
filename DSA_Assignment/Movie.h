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

    /*
    Adds an actor to the movie's list of actors

    This function adds a pointer to an Actor object to the movie's list of actors, creating a relationship between the movie and the actor

    Parameter - actor: A pointer to the Actor object to be added
    Return - None (updates the movie's actor list)
    */
    void addActor(Actor* actor) {
        actors.push_back(actor);
    }

    /*
    Converts the movie's year from string to an integer

	This function converts the year to an integer to allow for comparison operations based on the movie's release year.

    Parameter - None
    Return - The movie's year as an integer, or -1 if
	*/
    int getYearAsInt() const {
        try {
            return stoi(year);
        }
        catch (...) {
            return -1;
        }
    }

    /*
    Updates the movie's average rating with a new rating

	This function calculates the average rating of the movie based on the new rating and the number of times the movie has been rated

    Parameter - newRating: An integer (1 to 5) representing the new rating
    Return - None (updates the movie's rating and increments the rating count)
    */
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
