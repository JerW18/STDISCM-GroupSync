#include <iostream>
#include <fstream>
#include <string>
#include <cctype>

//a) There are only n instances that can be concurrently active.Thus, there can only be a maximum n number of parties that are currently in a dungeon.
//b) A standard party of 5 is 1 tank, 1 healer, 3 DPS.
//c) The solution should not result in a deadlock.
//d) The solution should not result in starvation.
//e) It is assumed that the input values arrived at the same time.
//f) A time value(in seconds) t is randomly selected between t1 and t2.Where t1 represents the fastest clear time of a dungeon instance and t2 is the slowest clear time of a dungeon instance.For ease of testing t2 <= 15.

using namespace std;

bool isValidNumber(const string& str) {
    if (str.empty()) return false;

    size_t start = 0;
    if (str[0] == '-') start = 1;  // Allow leading negative sign

    for (size_t i = start; i < str.size(); i++) {
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
            cerr << "Error: Invalid number format (only digits allowed) for key: " << key << endl;
            end = true;
            break;
        }

        value = stoll(strValue);  // Convert to signed integer

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

int main()
{
    uint32_t dungeonCount;
    uint32_t tankCount;
    uint32_t healerCount;
    uint32_t DPSCount;
    uint16_t minTime;
    uint16_t maxTime;

    readFile(dungeonCount, tankCount, healerCount, DPSCount, minTime, maxTime);

}