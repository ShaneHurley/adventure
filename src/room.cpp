//
// Created by Shane Hurley on 11/24/18.
//
#include <regex>
#include <iostream>
#include "room.h"
#include "util.h"
#include "game.h"

using namespace std;

extern Game game;

Room::Room(std::string line)
{

    regex roomPattern(" *(.*) *::: *(.*) *::: *(.*) *");
    smatch sm1;
    if (regex_search(line, sm1, roomPattern))
    {
        id = sm1[1];
        description = sm1[2];
        //TODO fix items
    }

}

Room::Room(std::string id, std::string name, std::string description, std::string ld)
{
    this->id = id;
    this->name = name;
    this->description = description;
    this->long_des = ld;
    // TODO do something with items
}

const string &Room::getId() const
{
    return id;
}

string Room::getDescription() const
{
    std::string result;

    if (long_des.size())
    {
        result = long_des;
    } else if (description.size())
    {
        result = description;
    } else
    {
        result = name;
    }

    if (items.size() > 0) {
        result += "\nand you can see the following item";
        if (items.size() > 1) {
            result += "s";
        }
        result += ":";
    }

    result += "\n\n";
    for (auto item : items) {
        result += game.getItem(item).getDescription() + "\n";
    }

    return result;
}

void Room::addExit(std::string direction, std::string id)
{
    map[direction] = id;
}

const std::string Room::go(std::string dir)
{
    if (map.count(dir))
    {
        return map[dir];
    } else
    {
        return id;
    }
}

void Room::addItem(std::string item) {
    items.push_back(item);
}

bool Room::removeItem(std::string item) {
    auto before = items.size();
    items.erase(std::remove(begin(items), end(items), item), end(items));
    return before != items.size();
}
