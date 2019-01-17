//
// Created by Shane Hurley on 11/24/18.
//

#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <chrono>
#include <thread>
#include <string>

#include "csv.h"

#include "game.h"
#include "room.h"
void teletype(const std::string& msg);
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
    //

    io::CSVReader<6,io::trim_chars<' ', '\t'>, io::double_quote_escape<',','"'>> in_item("../data/items.csv");
    in_item.read_header(io::ignore_extra_column, "item id", "item name", "description", "powers", "classification", "location");
    std::string powers, classification, location;
    while(in_item.read_row(id, name, description, powers, classification, location)) {
        items[id] = std::make_unique<Item>(id, name, description, classification);
        if (rooms.count(location))
        {
            rooms[location]->addItem(id);
        } else if (location.size() > 0) {
             teletype ( "Unable to find room " + location + " for item " + id + "\n");
        }
    }

}//log

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
             teletype ( "You have a " + item + "\n");
        } else {
             teletype ( "You do not see a " + item + " here." );
        }
    }
}//drop items

void Game::drop(string cw, vector<string> cmd) {
    cmd = remove_words(cmd, { cw });
    for (auto item : cmd) {
        if (erase(inventory, item)) {
            currentRoom().addItem(item);
             teletype ( "You drop the " + item +"\n");
        } else {
            teletype("You do not have a " + item + "\n");
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
    //aliases
    cmd_list["north"] = [&]() { location = currentRoom().go("north"); };
    cmd_list["south"] = [&]() { location = currentRoom().go("south"); };
    cmd_list["east"] = [&]() { location = currentRoom().go("east"); };
    cmd_list["west"] = [&]() { location = currentRoom().go("west"); };
    cmd_list["pickup"] = [&]() { pickup("pickup", cmd); };
    cmd_list["drop"] = [&]() { drop("drop", cmd); };
    cmd_list["use"] = [&]() { system("attack")/* mac only */;  teletype ( "use << <<!!!\n"); };
    cmd_list["open"] = [&]() { system("say open")/* mac only */;  teletype ( "OPEN!!!\n"); };
    cmd_list["quit"] = [&]() { play = false; };
    //camands
    room_cmd_list["start"]["jump"] = [&]() {  teletype ( "Jump jump\n"); };
    room_cmd_list["start"]["north"] = [&]() {
        if (contains(inventory, "butter knife")) {
            location = currentRoom().go("north");
        } else {
             teletype ( "For some reason you cannot go north right now \n");
        }
    };//keys
    room_cmd_list["city drug cartel"]["south"] = [&]() {
        if (contains(inventory, "final boss key")) {
            location = currentRoom().go("south");
        } else {
            teletype ( "For some reason you cannot go south right now looks like you need a key \n");
        }
    };//keys
    room_cmd_list["south mine"]["south"] = [&]() {
        if (contains(inventory, "lantern")) {
            location = currentRoom().go("south");
        } else {
            teletype ( "For some reason you cannot go south right now looks like you need a lantern to see and get through it \n");
        }
    };//keys
    room_cmd_list["sand dune"]["attack"] = [&]() {
        if (contains(cmd, "shane")) {
            if (contains(inventory, "butter knife")) {
                system("say attack");
                 teletype ( "Shane is vanquished\n");
                currentRoom().setDescription("A barren sand dune, that seems a little worse for not having Shane here.");
            } else {
                system("say attack");
                 teletype ( "You attack Shane, but without a butter knife you were destine to fail\n");
                play = false;
            }

        } else {
            system("say attack");
             teletype ( "Swing into the air with great force hitting... nothing.\n");
        }
    };//attack
    room_cmd_list["final boss"]["attack"] = [&]() {
        if (contains(cmd, "boss")) {
            if (contains(inventory, "great glaive")) {
                system("attack");
                 teletype ( "boss is vanquished\n") ;
                currentRoom().setDescription("Taking a deep breath the danger is gone.\n");
            } else {
                system("attack");
                 teletype ( "You attack boss, but without a great glaive you were destine to die.\n") ;
                play = false;
            }
            //
        } else {
            system("attack");
             teletype ( "Swing into the air with great force hitting... nothing.\n");
        }
    };//attack
    location = "start";
    int a=0;
    string command;
    string line;
    cout<< "\033[2J\033[H" << flush;
    system("say -r 160 Shall we play a game? &");
    teletype("Shall we play a game?\n\n");

    cout <<"***********************************************************************************************\n*********************************** want to play **********************************************\n1= nice game of adventure\n2= global thermonuclear war\n\n0= not to play at all\n";
    cin>>a;
    //start screen
    if (a==1)
    {
        play = true;
         cout<< "\033[2J\033[H";
        cout << "You wake up in a land, unfamiliar to you, your ears are ringing, you don’t remember anything that happened. You look around, you see that you are in some kind of “desert”, it looks like a bomb went off. To the North, there are giant sand dunes, you can see nothing past them. To the East, there is a rundown and abandoned city, some of the buildings have small fires in them. To the South, there is a old mine, with a rail leading into it. To the West, there is a smoldering forest, with a dried up pond.\n";
    }

    getline(cin,line);  // absorb the enter from the cin above
    while (play)
    {
         teletype ( "You are in " + currentRoom().getDescription() + " what do you want to do?\n") ;
         //description
        getline(cin,line);
        //camands



        bool cmd_found = false;
        cmd = parse_command(line);
        for (auto cmd_word : room_cmd_list[location]) {
            if (contains(cmd, cmd_word.first)) {
                cmd_word.second();
                cmd_found = true;
            }
        }
        cout << "\033[2J\033[H";
        if (!cmd_found) {
            for (auto cmd_word : cmd_list) {
                if (contains(cmd, cmd_word.first)) {
                    cmd_word.second();
                    break;

                }
            }
        }


    }
    //main game loop
     cout <<"\033[2J\033[H";

    if (a != 0)
    {
        for (int b = 0; b < 100; b++)
        {
            cout << " Bye loser ";
            //teletype ( " Bye loser ");
        }
    } else {
        system("say -r 170 A strange game the only winning move is to not play.&");
        teletype("A STRANGE GAME\nTHE ONLY WINNING MOVE IS\nNOT TO PLAY.\n\n");
    }
        //end screen
}

Item &Game::getItem(string id)
{
    return *items[id];
}
//getting items

void teletype(const std::string& msg) {
    for (auto ch : msg) {
        std:: cout <<  ch << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(60));
        }
}
//delay text
