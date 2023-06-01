#define FMT_HEADER_ONLY
#include "C:\Users\Chris\Desktop\repos\windows-bot-template\fmt\include\fmt\core.h"
#include "C:\Users\Chris\Desktop\repos\windows-bot-template\json.hpp"
#include "MyBot.h"
#include <dpp/dpp.h>
#include <iostream>
#include <set>

// ******************************************************************************************************
const dpp::snowflake M_MESSAGE_ID = dpp::snowflake(1113501811664506970);
const dpp::snowflake M_CHANNEL_ID = dpp::snowflake(975842789420048445);
const std::string M_ADDON_DUMP = "C:\\Program Files (x86)\\World of Warcraft\\_classic_\\WTF\\Account\\ANGELITEWEAPON\\Pyrewood Village\\Chronkz\\SavedVariables\\ChronkzBankAddon.lua";
// ******************************************************************************************************

// Returns sanitized bank info.
auto bankCleanse(std::string dump) {

	const std::set<std::string> KEEPSET = {
		// Enchanting Mats.
		"Abyss Crystal",
		"Dream Shard",
		"Greater Cosmic Essence",
		"Infinite Dust",
		// Fish Feasts and its mats.
		"Fish Feast",
		"Nettlefish",
		"Musselback Sculpin",
		"Glacial Salmon",
		"Northern Spices",
		// Gems
		"Autumn's Glow",
		"Monarch Topaz",
		"Forest Emerald",
		"Scarlet Ruby",
		"Sky Sapphire",
		"Twilight Opal",
		// Power Crystals
		"Red Power Crystal",
		"Blue Power Crystal"
	};

	std::string line;
	std::vector<std::string> myLines;
	std::ifstream myfile(dump);

	while (std::getline(myfile, line)) {
		myLines.push_back(line);
	}
	myLines.pop_back(); // Removes blank space at the end.

	std::string gold = myLines[2];
	std::string lastScanned = myLines[1];
	gold = gold.substr(23, 5);
	lastScanned = lastScanned.substr(22, 35);

	// Removes all lines beginning with { or }.
	std::vector<std::string> ree = {};
	for (int i = 4; i < myLines.size(); i++) {
		if (myLines[i][1] != '{' && myLines[i][1] != '}') {
			ree.push_back(myLines[i]);
		}
	}

	// Removes all lines that contains "No Item".
	std::vector<std::string> ree2 = {};
	for (int i = 0; i < ree.size(); i++) {
		if (ree[i].find("No Item") != std::string::npos) {
			continue;
		}
		ree2.push_back(ree[i]);
	}

	// Removes the tabs of all of it (move up TODO)
	for (int i = 0; i < ree2.size(); i++) {
		ree2[i].erase(std::remove(ree2[i].begin(), ree2[i].end(), '\t'), ree2[i].end()); // Removes the tabs
	}

	// Iterates over vector and collects the names of the items and then the count.
	std::vector<std::string> ree4 = {};
	for (int i = 0; i < ree2.size(); i++) {
		if (i % 2 == 0) {
			std::string str2 = ree2[i].substr(ree2[i].find("[") + 1);
			std::size_t posSecond = str2.find("]");
			std::string mystr = str2.substr(0, str2.find("]", 0));
			ree4.push_back(mystr);
		}
		else {
			std::string mystr = ree2[i].substr(0, ree2[i].find(",", 0));
			ree4.push_back(mystr);
		}
	}

	// Creates a Map of all items and their combined counts.
	std::map<std::string, int> my_map = {};
	for (int i = 0; i < ree4.size(); i += 2) {
		if (!my_map[ree4[i]]) {
			my_map[ree4[i]] = stoi(ree4[i + 1]);
		}
		else {
			my_map[ree4[i]] = my_map[ree4[i]] + stoi(ree4[i + 1]);
		}
	}

	json jsonthing3 = { my_map };
	json jsonthing4 = jsonthing3[0];

	// Checks against existing list of items we want to keep.
	std::vector<std::string> thingsToRemove = {};
	for (auto items : my_map) {
		if (KEEPSET.contains(items.first) == false) {
			thingsToRemove.push_back(items.first);
		}
	}
	// Removes the items we don't want from the map.
	for (auto remove : thingsToRemove) {
		my_map.erase(remove);
	}
	// Converts the int values to strings.
	std::map<std::string, std::string> my_map_2 = {};
	for (auto item : my_map) {
		my_map_2[item.first] = std::to_string(item.second);
	}

	json jsonthing = { my_map_2 };
	json jsonthing2 = jsonthing[0];

	struct returnedStuff {
		json a;
		json b;
		std::string c;
		std::string d;
	};

	return returnedStuff{ jsonthing2, jsonthing4, gold, lastScanned };
}

// Reads a location and returns its token.
auto getToken(std::string path) {
	std::string botToken;
	std::ifstream myBotToken(path);
	if (myBotToken.is_open()) {
		myBotToken >> botToken;
	}

	return botToken;
}
