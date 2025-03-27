#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <mutex>
#include <queue>
#include <thread>
#include <random>
#include <map>

#include "Dungeon.h"

using namespace std;

bool isValidNumber(const string& str) {
    if (str.empty()) return false;

    for (size_t i = 0; i < str.size(); i++) {
        if (!isdigit(str[i])) return false;
    }
    return true;
}

static void readFile(uint32_t& dungeonCount, uint32_t& tankCount, uint32_t& healerCount, uint32_t& DPSCount, uint16_t& minTime, uint16_t& maxTime) {
    bool end = false;

    ifstream inputFile("config.txt");
    if (!inputFile) {
        cerr << "Error Reading File." << endl;
        end = true;
    }

    string key, strValue;
    int64_t value;

    while (inputFile >> key >> strValue) {
        if (!isValidNumber(strValue)) {
            cerr << "Error: Invalid number format (only positive integers allowed) for key: " << key << endl;
            end = true;
            break;
        }

        try {
            value = stoull(strValue);
        }
        catch (const out_of_range&) {
            cerr << "Error: Number out of range for key: " << key << endl;
            end = true;
            break;
        }

        if (value < 0) {
            cerr << "Error: '" << key << "' cannot be negative!\n";
            end = true;
            break;
        }

        if (key != "n" && key != "t" && key != "h" && key != "d" && key != "t1" && key != "t2") {
            cerr << "Invalid key: " << key << endl;
            end = true;
            break;
        }

        if ((key == "n" || key == "t" || key == "h" || key == "d") &&
            value > numeric_limits<uint32_t>::max())
        {
            cerr << "Error: '" << key << "' exceeds uint32_t limit!\n";
            end = true;
            break;
        }
        else if ((key == "t1" || key == "t2") &&
            value > 15)
        {
            cerr << "Error: '" << key << "' exceeds max Time Value (15)!\n";
            end = true;
            break;
        }
        else if ((key == "t1" || key == "t2") &&
            value < 1)
        {
            cerr << "Error: '" << key << "' must be greater than 0!\n";
            end = true;
            break;
        }

        if (key == "n")      dungeonCount = static_cast<uint32_t>(value);
        else if (key == "t") tankCount = static_cast<uint32_t>(value);
        else if (key == "h") healerCount = static_cast<uint32_t>(value);
        else if (key == "d") DPSCount = static_cast<uint32_t>(value);
        else if (key == "t1") minTime = static_cast<uint16_t>(value);
        else if (key == "t2") maxTime = static_cast<uint16_t>(value);
    }

    inputFile.close();

    if (minTime > maxTime) {
		maxTime = minTime;
		cout << "t2 cannot be less than t1. Setting t2 = t1.\n\n";
    }

    if (end)
    {
        cout << "\nPress enter to exit...";
        cin.get();

		exit(1);
    }
}

// Output for logging.
ofstream outputFile("output.txt");

void logOutput(const string& message) {
    cout << message;
    outputFile << message;
}

void logDungeonStatus(map<int, int>& dungeonStatus) {
    string output = "Dungeon Status\n";
    for (const auto& dungeon : dungeonStatus) {
        int dungeonId = dungeon.first;
        int status = dungeon.second;
        output += "Dungeon " + to_string(dungeonId) + ": ";
        output += (status == 0 ? "Empty" : "Active") + string("\n");
    }
    output += "\n";
    logOutput(output);
}

int main()
{
    uint32_t dungeonCount;
    uint32_t tankCount;
    uint32_t healerCount;
    uint32_t DPSCount;
    uint16_t minTime;
    uint16_t maxTime;

    readFile(dungeonCount, tankCount, healerCount, DPSCount, minTime, maxTime);

    atomic<uint32_t> currCount(0);

	// Party Count
    mutex partyMutex;
    uint32_t partyCount = min({ tankCount, healerCount, DPSCount / 3 });

	tankCount -= partyCount;
	healerCount -= partyCount;
	DPSCount -= partyCount * 3;

    // Random Number generator 
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(minTime, maxTime);

    // Dungeon Status
    mutex statusMutex;
    map<int, int> dungeonStatus;

	// Create Output File
    ofstream outputFile("output.txt");

    // Create Dungeon Instances
    vector<Dungeon> dungeons;

    for (uint32_t i = 0; i < dungeonCount; i++) {
        dungeons.emplace_back(i + 1);
        dungeonStatus[i + 1] = 0;
    }

    // Threading
    vector<thread> instances;
    
    for (uint32_t i = 0; i < dungeonCount; i++) {
        instances.push_back(thread([&, i]() {
            while (true) {
                {
                    lock_guard<mutex> lock(partyMutex);
                    if (partyCount == 0) {
                        break;
                    }
                    if (currCount != i) {
                        continue;
                    }

                    partyCount--;
                    currCount = (currCount + 1) % dungeonCount; 
                }

                {
                    lock_guard<mutex> lock(statusMutex);
                    dungeonStatus[dungeons[i].getId()] = 1;
                    logOutput("Party entered Dungeon " + to_string(dungeons[i].getId()) + "\n\n");
                    logDungeonStatus(dungeonStatus);
                }

                int time = dis(gen);
                dungeons[i].RunDungeon(time);

                {
                    lock_guard<mutex> lock(statusMutex);
                    dungeonStatus[dungeons[i].getId()] = 0;
                    logOutput("Party finished Dungeon " + to_string(dungeons[i].getId()) + "\n\n");
                    logDungeonStatus(dungeonStatus);
                }
            }
        }));
    }

	for (auto& instance : instances) {
		instance.join();
	}

    // Print Summary.
    string summary = "Summary\n";
    for (uint32_t i = 0; i < dungeonCount; i++) {
        summary += "Dungeon " + to_string(dungeons[i].getId()) + ": " +
            "Parties Served: " + to_string(dungeons[i].getPartiesServed()) + " " +
            "Total Time: " + to_string(dungeons[i].getTimeElapsed()) + " seconds\n";;
    }
	summary += "\n";
	summary += "Remaining Troops: \n";
    summary += "Tank: " + to_string(tankCount) + "\n";
    summary += "Healer: " + to_string(healerCount) + "\n";
    summary += "DPS: " + to_string(DPSCount) + "\n\n";
    logOutput(summary);

    outputFile.close();
    
	cout << "Press enter to exit...";
	cin.get();

	return 0;
}