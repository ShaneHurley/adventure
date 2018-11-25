//
// Created by Shane Hurley on 11/24/18.
//

#include "game.h"
#include "room.h"
#include<iostream>
#include<fstream>
#include<string>

Game::Game() {
    std::ifstream rooms("../data/rooms.txt");
    std::string line;

    while (std::getline(rooms, line)) {
        Room r(line);
    }
}

void Game::play() {
    std::cout << "Hello, word!" << std::endl;
}
