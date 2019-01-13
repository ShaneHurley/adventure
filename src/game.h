//
// Created by Shane Hurley on 11/24/18.
//

#ifndef ADVENTURE_GAME_H
#define ADVENTURE_GAME_H

#include <map>
#include <string>
#include "room.h"

class Game {

public:
    Game();
    void play();

    Item &getItem(std::string basic_string);

private:
    Room& currentRoom()
    {
     return *rooms[location];
    }


    std::map<std::string,std::function<void()>> cmd_list;
    std::string location;

    std::map<std::string, std::unique_ptr<Room>> rooms;
    std::map<std::string, std::unique_ptr<Item>> items;

    std::vector <std::string> storage;
    double hP = 100;
};


#endif //ADVENTURE_GAME_H
