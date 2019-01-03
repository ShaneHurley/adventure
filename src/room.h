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
    Room(std::string id, std::string name, std::string description, std::string items);

    const std::string &getId() const;
    const std::string &getDescription() const;

    void addExit(std::string direction, std::string id);

    const std::string go(std::string dir);
protected:
    std::string id;
    std::string description;
    std::string name;
    std::vector<Item> items;
    std::map<std::string, std::string> map;
};


#endif //ADVENTURE_ROOM_H
