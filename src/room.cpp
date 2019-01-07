//
// Created by Shane Hurley on 11/24/18.
//
#include <regex>
#include <iostream>
#include "room.h"
#include "util.h"


using namespace std;

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

Room::Room(std::string id, std::string name, std::string description, std::string items)
{
    this->id = id;
    this->name = name;
    this->description = description;
    // TODO do something with items
}

const string &Room::getId() const
{
    return id;
}

const string &Room::getDescription() const
{
    if (description.size())
    {
        return description;
    } else
    {
        return name;
    }
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
