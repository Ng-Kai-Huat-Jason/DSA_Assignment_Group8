#pragma once
#include <string>

using namespace std;

struct Actor {
private:
    string name;
    int birthYear;

public:
    Actor(string name, int birthYear) : name(name), birthYear(birthYear) {}

    string getName() { return name; }
    int getBirthYear() { return birthYear; }
};
