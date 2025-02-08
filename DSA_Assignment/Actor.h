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

    /*
    Adds a movie to the actor's list of movies

    This function adds a pointer to a Movie object to the actor's list of movies, creating a relationship between the actor and the movie

    Parameter - movie: A pointer to the Movie object to be added
    Return - None (updates the actor's movie list)

    */
    void addMovie(Movie* movie) {
        movies.push_back(movie);
    }

    /*
    Updates the actor's average rating with a new rating

	This function calculates the average rating of the actor based on the new rating and the number of times the actor has been rated

    Parameter - newRating: An integer (1 to 5) representing the new rating
    Return - None (modifies the actor's rating and increments the rating count)
    */
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

    virtual ~Actor() {}  

};
