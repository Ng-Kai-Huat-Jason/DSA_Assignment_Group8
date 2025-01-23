#pragma once
#include <string>
#include "Actor.h"
#include "DoublyLinkedList.h"

using namespace std;

struct Movie {
    string title;
    string plot;
    int year;
    DoublyLinkedList<Actor*> actors;  // list of actors related to this movie

    Movie(string title, string plot, int year) : title(title), plot(plot), year(year) {}
};
