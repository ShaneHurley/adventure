//
// Created by Shane Hurley on 11/24/18.
//

#ifndef ADVENTURE_ITEM_H
#define ADVENTURE_ITEM_H

#include <string>

class Item {
public:
    Item(std::string id, std::string name, std::string des, std::string cls);

    std::string getDescription();
private:
    // item id,item name,description,powers,classification,location
    std::string id;
    std::string name;
    std::string des;
    std::string cls;

};


#endif //ADVENTURE_ITEM_H
