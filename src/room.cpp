//
// Created by Shane Hurley on 11/24/18.
//

#include <iostream>

#include "room.h"
#include "util.h"

Room::Room(std::string def) {
    for (auto r : split(def,":::")) {
        std::cout << "Room: " << r << std::endl;
    }
}