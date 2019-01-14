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
        rooms[id] = std::make_unique<Room>(id, name, description, long_description);
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

bool erase(vector<string>& list, string word) {
    auto before = list.size();
    list.erase(std::remove(begin(list), end(list), word), end(list));
    return before != list.size();
}

vector<string> remove_words(vector<string> cmd, vector<string> words) {
    vector<string> result;
    for (auto word : cmd) {
        if (!contains(words, word)) {
            result.push_back(word);
        }
    }
    return result;
}

vector<string> Game::parse_command(std::string cmd) {
    static const vector<string> ignore_words { "a", "of", "the", "by", "carefully", "go" };
    static const vector<string> join_words { "red", "blue", "rusty", "broken", "glowing", "wooden", "butter"};


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

    // apply aliases
    for (auto& word : result) {
        for (auto alias : aliases) {
            if (word == alias.second) {
                word = alias.first;
            }
        }
    }
    return result;
}

void Game::pickup(string cw, vector<string> cmd) {
    cmd = remove_words(cmd, { cw });

    for (auto item : cmd) {
        if (currentRoom().removeItem(item)) {
            inventory.push_back(item);
            cout << "You have a " << item << endl;
        } else {
            cout << "You do not see a " << item << " here." << endl;
        }
    }
}

void Game::drop(string cw, vector<string> cmd) {
    cmd = remove_words(cmd, { cw });
    for (auto item : cmd) {
        if (erase(inventory, item)) {
            currentRoom().addItem(item);
            cout << "You drop the " << item << endl;
        } else {
            cout << "You do not have a " << item << endl;
        }
    }
}

void Game::play() {
    bool play = false;
    vector<string> cmd;

    aliases.push_back({ "north", "n" });
    aliases.push_back({ "south", "s" });
    aliases.push_back({ "east", "e" });
    aliases.push_back({ "west", "w" });
    aliases.push_back({ "pickup", "grab" });
    aliases.push_back({ "pickup", "take" });

    cmd_list["north"] = [&]() { location = currentRoom().go("north"); };
    cmd_list["south"] = [&]() { location = currentRoom().go("south"); };
    cmd_list["east"] = [&]() { location = currentRoom().go("east"); };
    cmd_list["west"] = [&]() { location = currentRoom().go("west"); };
    cmd_list["pickup"] = [&]() { pickup("pickup", cmd); };
    cmd_list["drop"] = [&]() { drop("drop", cmd); };
    cmd_list["use"] = [&]() { system("attack")/* mac only */; cout << "use << <<!!!\n"; };
    cmd_list["open"] = [&]() { system("say open")/* mac only */; cout << "OPEN!!!\n"; };
    cmd_list["quit"] = [&]() { play = false; };

    room_cmd_list["start"]["jump"] = [&]() { cout << "Jump jump\n"; };
    room_cmd_list["start"]["north"] = [&]() {
        if (contains(inventory, "butter knife")) {
            location = currentRoom().go("north");
        } else {
            cout << "For some reason you cannot go north right now" << endl;
        }
    };

    room_cmd_list["sand dune"]["attack"] = [&]() {
        if (contains(cmd, "shane")) {
            if (contains(inventory, "butter knife")) {
                cout << "Shane is vanquished" << endl;
                currentRoom().setDescription("A barren sand dune, that seems a little worse for not having Shane here.");
            } else {
                cout << "You attack Shane, but without a butter knife you were destine to fail." << endl;
                play = false;
            }

        } else {
            cout << "Swing into the air with great force hitting... nothing.\n";
        }
    };
    room_cmd_list["final boss"]["attack"] = [&]() {
        if (contains(cmd, "boss")) {
            if (contains(inventory, "great  glaive")) {
                cout << "boss is vanquished" << endl;
                currentRoom().setDescription("Taking a deep breath the danger is gone.");
            } else {
                cout << "You attack boss, but without a great  glaive you were destine to die." << endl;
                play = false;
            }

        } else {
            cout << "Swing into the air with great force hitting... nothing.\n";
        }
    };
    location = "start";
    int a=0;
    string command;
    string line;
    cout<<"***********************************************************************************************\n*********************************** want to play **********************************************\n\t\t\t\t\t\t\t\t    1= yes 2= no\n";
    cin>>a;
    if (a==1)
    {
        play = true;
        cout << "\033[2J\033[H";
    }
    while (play)
    {
        cout << "You are in " << currentRoom().getDescription() << " what do you want to do?" << endl;
        getline(cin,line);
        cout << "\033[2J\033[H";

        bool cmd_found = false;
        cmd = parse_command(line);
        for (auto cmd_word : room_cmd_list[location]) {
            if (contains(cmd, cmd_word.first)) {
                cmd_word.second();
                cmd_found = true;
            }
        }

        if (!cmd_found) {
            for (auto cmd_word : cmd_list) {
                if (contains(cmd, cmd_word.first)) {
                    cmd_word.second();
                    break;
                }
            }
        }


    }
    cout << "\033[2J\033[H";
    for (int b=0; b<100; b++)
    {
        cout << " Bye loser ";
    }

}

Item &Game::getItem(string id)
{
    return *items[id];
}
