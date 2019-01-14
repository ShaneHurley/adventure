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
    Room(std::string id, std::string name, std::string description, std::string long_description);

    const std::string &getId() const;
    std::string getDescription() const;
    void setDescription(std::string d) { long_des = d; }

    void addExit(std::string direction, std::string id);
    const std::vector<std::string>& getItems();
    bool removeItem(std::string item);

    const std::string go(std::string dir);

    void addItem(std::string item);
protected:
    std::string id;
    std::string description;
    std::string name;
    std::string long_des;
    std::vector<std::string> items;
    std::map<std::string, std::string> map;
};


#endif //ADVENTURE_ROOM_H
