# Looking for Group Synchronization
This program utilizes multiple threads to assign players to a limited number of dungeons.

## How to Run
1. Double click on the provided `.sln` file and open the project in Visual Studio.
2. Build the project by clicking **Build → Build Solution**.
3. Run the executable by clicking **Debug → Start Debugging**.
4. Enter the desired parameters in config file.
   - Open `config.txt` file to input the desired dungeon instance, number of tanks, healers, and DPS, and minimum and maximum dungeon run time.
     - Current configuration is `10` dungeons, `100` each of tanks, healers, and DPS, `1` minimum time, and `15` maximum time.
5. Make sure that Visual Studio is in `x64 Debug` mode and `ISO C++20 Standard (/std:c++20)` is being used. 
