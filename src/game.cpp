//
// Created by Shane Hurley on 11/24/18.
//

#include <iostream>
#include <fstream>
#include <string>
#include <regex>

#include "csv.h"

#include "game.h"
#include "room.h"

using namespace std;

Game::Game() {
    io::CSVReader<5,io::trim_chars<' ', '\t'>, io::double_quote_escape<',','"'>> in("../data/rooms.csv");
    in.read_header(io::ignore_extra_column, "Id", "Name", "Short Description", "Long Description", "Items");
    std::string id, name, description, long_description, unused;
    while(in.read_row(id, name, description, long_description, unused)) {
        rooms[id] = std::make_unique<Room>(id, name, description, unused);
    }

    io::CSVReader<5,io::trim_chars<' ', '\t'>, io::double_quote_escape<',','"'>> in_map("../data/map.csv");
    in_map.read_header(io::ignore_extra_column, "Id", "North", "South", "East", "West");
    std::string north, south, east, west;
    while(in_map.read_row(id, north, south, east, west)) {
        if (north.size() > 0) {
            rooms[id]->addExit("north", north);
        }

        if (south.size() > 0) {
            rooms[id]->addExit("south", south);
        }

        if (east.size() > 0) {
            rooms[id]->addExit("east", east);
        }

        if (west.size() > 0) {
            rooms[id]->addExit("west", west);
        }
    }

    // item id,item name,description,powers,classification,location
    io::CSVReader<6,io::trim_chars<' ', '\t'>, io::double_quote_escape<',','"'>> in_item("../data/items.csv");
    in_item.read_header(io::ignore_extra_column, "item id", "item name", "description", "powers", "classification", "location");
    std::string powers, classification, location;
    while(in_item.read_row(id, name, description, powers, classification, location)) {
        items[id] = std::make_unique<Item>(id, name, description, classification);
        if (rooms.count(location))
        {
            rooms[location]->addItem(id);
        } else if (location.size() > 0) {
            cout << "Unable to find room " << location << " for item " << id << endl;
        }
    }

}

vector<string> split(const string& string_to_split, const string regex="\\s+") {
    std::regex rgx(regex);
    std::sregex_token_iterator iter(string_to_split.begin(), string_to_split.end(), rgx, -1);
    return vector<string>(iter, sregex_token_iterator());
}

vector<string>& filter(vector<string>& source, vector<string> zaplist) {
    source.erase( std::remove_if( source.begin(), source.end(),
                                  [zaplist](string w){
                                      return find(zaplist.cbegin(), zaplist.cend(), w) != zaplist.cend();
                                  }),
                  source.end());

    return source;
}

bool contains(const vector<string>& list, string word) {
    return find(list.cbegin(), list.cend(), word) != list.cend();
}

vector<string> parse_command(std::string cmd) {
    static const vector<string> ignore_words { "a", "of", "the", "by", "carefully", "go" };
    static const vector<string> join_words { "red", "blue", "rusty", "broken", "glowing", "wooden" };

    bool joining = false;
    vector<string> result;
    for (auto word : split(cmd)) {
        if (!contains(ignore_words, word)) {
            if (joining) {
                auto index = result.size() - 1;
                result[index] = result[index] + " " + word;
            } else {
                result.push_back(word);
            }
            joining = contains(join_words, word);
        }
    }

    return result;
}

void Game::play() {
    bool play = true;
    vector<string> cmd;

    cmd_list["north"] = [&]() { location = currentRoom().go("north"); };
    cmd_list["south"] = [&]() { location = currentRoom().go("south"); };
    cmd_list["east"] = [&]() { location = currentRoom().go("east"); };
    cmd_list["west"] = [&]() { location = currentRoom().go("west"); };
    cmd_list["n"] = [&]() { location = currentRoom().go("north"); };
    cmd_list["s"] = [&]() { location = currentRoom().go("south"); };
    cmd_list["e"] = [&]() { location = currentRoom().go("east"); };
    cmd_list["w"] = [&]() { location = currentRoom().go("west"); };
    cmd_list["use"] = [&]() { system("attack")/* mac only */; cout << "use << <<!!!\n"; };
    cmd_list["open"] = [&]() { system("say open")/* mac only */; cout << "OPEN!!!\n"; };
    cmd_list["quit"] = [&]() { play = false; };

    location = "start";

    string command;
    string line;

    while (play)
    {
        cout << "You are in " << currentRoom().getDescription() << " what do you want to do?" << endl;
        getline(cin,line);

        cmd = parse_command(line);
        for (auto cmd_word : cmd_list) {
            if (contains(cmd, cmd_word.first)) {
                cmd_word.second();
                break;
            }
        }
    }
}

Item &Game::getItem(string id)
{
    return *items[id];
}
