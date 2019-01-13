//
// Created by Shane Hurley on 11/24/18.
//

#include "item.h"

Item::Item(std::string id, std::string name, std::string des, std::string cls) {
    this->id = id;
    this->name = name;
    this->des = des;
    this->cls = cls;
    // TODO do something with items
}

std::string Item::getDescription() {
    return des;
}