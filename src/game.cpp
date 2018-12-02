//
// Created by Shane Hurley on 11/24/18.
//

#include <iostream>
#include <fstream>
#include <string>
#include <regex>

#include "game.h"
#include "room.h"

using namespace std;

Game::Game() {
    ifstream roomFile("../data/rooms.txt");
    string line;


    while (getline(roomFile, line)) {
        Room newRoom(line);
        rooms.push_back(newRoom);
    }
}

void Game::play() {

    for(auto room : rooms)
    {
        cout << room.getId() << "\t" ;
    }
    cout << "\nHello, word!" << endl;
}
