//
// Created by Shane Hurley on 11/24/18.
//

#include <iostream>
#include <fstream>
#include <string>
#include <regex>

#include "csv.h"

#include "game.h"
#include "room.h"

using namespace std;

Game::Game() {
    io::CSVReader<5> in("../data/rooms.csv");
    in.read_header(io::ignore_extra_column, "Id", "Name", "Short Description", "Long Description", "Items");
    std::string id, name, description, long_description, items;
    while(in.read_row(id, name, description, long_description, items)) {
        rooms[id] = std::make_unique<Room>(id, name, description, items);
    }

    io::CSVReader<5> in_map("../data/map.csv");
    in_map.read_header(io::ignore_extra_column, "Id", "North", "South", "East", "West");
    std::string north, south, east, west;
    while(in_map.read_row(id, north, south, east, west)) {
        if (north.size() > 0) {
            rooms[id]->addExit("north", north);
        }

        if (south.size() > 0) {
            rooms[id]->addExit("south", south);
        }

        if (east.size() > 0) {
            rooms[id]->addExit("east", east);
        }

        if (west.size() > 0) {
            rooms[id]->addExit("west", west);
        }
    }
}

void Game::play() {
    string location = "start";
    bool play = true;

    string command;

    while (play)
    {
        Room& room = *rooms[location];
        cout << room.getDescription()<<endl;
        cin >> command;
        std::string newRoomId = room.go(command);
        if (newRoomId.size()>0)
        {
            location=newRoomId;
        }

    }
}
