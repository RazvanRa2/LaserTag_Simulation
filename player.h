// Copyright 2017 Razvan Radoi, first of his name
#ifndef _HOME_STUDENT__RESOURCES__PLAYER_H_
#define _HOME_STUDENT__RESOURCES__PLAYER_H_
#include <string>
// structura Player retine toate informatiile necesare despre un jucator
struct Player {
	std::string name;
	int playerIndex;  // indexul "global", de la 0 la nrTeam1 + nrTeam2 - 1
	int team;  // in care echipa se afla => 1 sau 2

	int health;  // 2 - de cate ori a fost impsucat intr-un joc
	int shootingScore;  // scorul pentru top shooters

	int explorerScore;  // scorul pentru top explorers
	int lastSenzor;  // ultimul senzor, folosit pentru top explorers
	int triggeredSenzors[30];  // senzorii activati intr-un joc

	Player() {  // initializari
		name = "";
		playerIndex = -1;
		team = 0;

		health = 0;
		shootingScore = 0;

		lastSenzor = -1;
		explorerScore = 0;
		for( int i = 0; i < 30; i++ )
			triggeredSenzors[i] = 0;
	}
};
#endif  // _HOME_STUDENT__RESOURCES__PLAYER_H_
