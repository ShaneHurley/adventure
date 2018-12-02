//
// Created by Shane Hurley on 11/24/18.
//
#include <regex>
#include <iostream>
#include "room.h"
#include "util.h"


using namespace std;

Room::Room(std::string line) {

    regex roomPattern(" *(.*) *::: *(.*) *::: *(.*) *");
    smatch sm1;
        if(regex_search(line, sm1, roomPattern))
        {
            id=sm1[1];
            description=sm1[2];
            //TODO fix items
        }

    }

const string &Room::getId() const {
    return id;
}
