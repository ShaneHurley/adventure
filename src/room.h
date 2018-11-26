//
// Created by Shane Hurley on 11/24/18.
//

#ifndef ADVENTURE_ROOM_H
#define ADVENTURE_ROOM_H

#include <string>
#include <map>
#include <vector>
#include <memory>
#include "item.h"

class Room
{
public:
    Room(std::string def);

protected:
    std::string id;
    std::string description;
    std::vector<Item> items;
    std::map<std::string, std::shared_ptr<Room>> map;
};


#endif //ADVENTURE_ROOM_H
