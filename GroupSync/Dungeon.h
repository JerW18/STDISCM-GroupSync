#pragma once

using namespace std;

class Dungeon
{
	public:
		Dungeon(int id);
		void RunDungeon(int time);
		int getPartiesServed();
		int getTimeElapsed();
		int getId();

	private:
		int id;
		int partiesServed;
		int timeElapsed;
};

