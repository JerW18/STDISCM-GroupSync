#include <iostream>
#include <thread>
#include <chrono>

#include "Dungeon.h"

using namespace std;

Dungeon::Dungeon(int id) : id(id), partiesServed(0), timeElapsed(0) {}

void Dungeon::RunDungeon(int time)
{
	timeElapsed += time;
	partiesServed++;
	this_thread::sleep_for(chrono::seconds(time));
}

int Dungeon::getPartiesServed()
{
	return partiesServed;
}

int Dungeon::getTimeElapsed()
{
	return timeElapsed;
}

int Dungeon::getId()
{
	return id;
}

