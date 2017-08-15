// Copyright 2017 Razvan Radoi, first of his name
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include "./hashtable.h"

// structura folosita pentru datele minim necesare pentru top fire exchange
struct Shots {
	std::string name1;
	std::string name2;
	int firedShots;

	Shots(){
		name1 = "";
		name2 = "";
		firedShots = 0;
	}
};
// strucutra cu datele minim necesare pentru top shooters si top explorers
struct tinyData {
	std::string name;
	int explorerScore;
	int shootingScore;

	tinyData() {
		name = "";
		explorerScore = -999;
		shootingScore = -999;
	}
};

// regula pentru sortarea top explorers
bool explorerRule(tinyData a, tinyData b) {
	// daca scorul e mai mic, interschimba
	if( a.explorerScore > b.explorerScore )
		return 1;
	if( a.explorerScore < b.explorerScore )
		return 0;
	// daca scorul e egal, iar numele sunt invers alfabetic schimba
	if( a.explorerScore == b.explorerScore ) {
		if( a.name < b.name )
			return 1;
		else
			return 0;
	}
	// daca e acelasi nume si acelasi scor, return 0 ( nu se intampla )
	return 0;
}

// regula de sortare pentru top shooters
bool shooterRule(tinyData a, tinyData b) {
	// daca scorul e mai mic, interschimba
	if( a.shootingScore > b.shootingScore )
		return 1;
	if( a.shootingScore < b.shootingScore )
		return 0;
	// daca scorul e egal dar numele nu sunt in ordinea corecta, interschimba
	if( a.shootingScore == b.shootingScore ) {
		if( a.name < b.name )
			return 1;
		else
			return 0;
	}
	// daca e acelasi nume si acelasi scor, return 0 ( nu se intampla )
	return 0;
}

// regula de sortare pentru fire exchange
bool feRule(Shots a, Shots b) {
	// interschimba daca:
	// 1. scorul e mai mic
	if ( a.firedShots > b.firedShots )
		return 1;
	if ( a.firedShots < b.firedShots )
		return 0;
	// 2. scorul e egal, dar primul nume e "alfabetic mai mare"
	if( a.firedShots == b.firedShots ) {
		if( a.name1 < b.name1 ) {
			return 1;
		}
		if( a.name1 > b.name1 ) {
			return 0;
		}
		// 3. scoruri egale, primul nume egal, dar al doilea nume nu apare
		// in ordinea corecta
		if( a.name1 == b.name1 ) {
			if( a.name2 < b.name2 ) {
				return 1;
			} else {
				return 0;
			}
		}
	}

	return 0;
}

// o functie simpla de hashing pentru stringuri
int hashingFunction(std::string key) {
	int sum = 0;
	for( unsigned int i = 0; i < key.length(); i++ )
		sum += key[i];

	return (sum >> 1) % 5381;
}

// afla dintrun string de tipul "x: name" care este x
int getSenzor(std::string input) {
	int result = 0, i;
	int end = input.find(":");  // se deplaseaza pana la :
	// pana la : calculeaza cat este senzorul, pentru cazul in care indicele
	// e mai mare decat 10
	for( i = 0; i < end; i++ ) {
		result = result * 10 + (input[i] - 48);
	}
	return result;
}
// dintr-un string de tipul X: player selecteaza player
std::string getMovingPlayer(std::string input) {
	// deplasare pana la inceputul numelui
	int start = input.find(":");
	start = start + 2;
	// selecteaza tot de la start pana la sfarsitul stringului
	return input.substr(start);
}
// dintr-un string de tipul name1 -> name2 selecteaza name1
std::string getShooter(std::string input) {
	// deplasare pana la sfarsitul numelui
	int end = input.find("-");
	// selecteaza substringul de la inceputul inputului pana la finalul name1
	return input.substr(0, end - 1);
}
// dintr-un string de tipul name1 -> name2 selecteaza name2
std::string getShot(std::string input) {
	// deplasare pana la inceputul numelui
	int start = input.find(">");
	start = start + 2;
	// selecteaza substringul de la pozitia de start pana la sfarsitul input
	return input.substr(start);
}
// afla in ce echipa se afla un jucator
int getTeam(std::string playerName, Hashtable& team1) {
	// verifica daca apartine primei echipe
	bool inTeam1 = team1.checkIfMember(playerName);
	if ( inTeam1 == true ) {
		return 1;
	}
	// daca nu, inseamna ca apartine echipei nr 2 ( doar pentru ca stim ca
	// nu apar nume care sa nu fie in nicio echipa )
	return 2;
}

int main() {
	// arenaSize este numarul de noduri din graful arenei de joc
	// arena e matricea de adiacenta
	int arenaSize, i, arena[105][105] = {0};
	// introducerea datelor despre arena de joc
	std::cin >> arenaSize;
	for( i = 0; i < arenaSize; i++ ) {
		for( int j = 0; j < arenaSize; j++ ) {
			if( i != j ) {
				std::cin >> arena[i][j];
			}
		}
	}
	// introducerea datelor despre cele doua echipe
	int nrTeam1, nrTeam2;
	std::string tempName;

	std::cin >> nrTeam1 >> nrTeam2;
	// cele doua echipe au fiecare cate un hashtable
	// mi-am dat seama ca as fi putut folosi unul singur mult prea tarziu
	Hashtable team1(250, hashingFunction);
	Hashtable team2(250, hashingFunction);
	// introducerea datelor despre prima echipa
	for( i = 0; i < nrTeam1; i++ ) {
		std::cin >> tempName;
		// 1 este indecele echipei, i este indicele jucatorului in joc
		team1.addNewMember(tempName, 1, i);
	}
	// introducerea datelor despre a doua echipa
	for( i = 0; i < nrTeam2; i++ ) {
		std::cin >> tempName;
		// 2 este indicele echipei, i + nrTeam1 este indicele jucatorului
		team2.addNewMember(tempName, 2, i + nrTeam1);
	}
	// shGraph e matricea de adiacenta a unui graf ce retine cine in cine a tras
	int shGraph[250][250] = {0};
	// fire exchange e un vector de structuri in care se retin datele minim
	// necesare pentru construirea top fire exchange
	struct Shots fireExchange[12900];

	int score1 = 0;  // score1 -> scorul in championship pentru team1
	int score2 = 0;  // score2 -> scorull in championship pentru team2

	// incepe turneul
	std::string temp;  // in temp citesc fiecare linie
	while(getline(std::cin, temp)) {
		// daca linia e de tipul JOC_X
		if( temp.find("JOC_") != std::string::npos ) {
			int winner;
			// determina care echipa mai are jucatori in viata si
			// aceleia ii creste scorul
			winner = team1.searchAnyAlivePlayers();
			if (winner == 1)
				score1++;
			else
				score2++;
			// reseteaza datele despre jucatori pentru urmatorul joc
			// in plus, revivePlayers adauga 3 * unique senzors la scorul
			// de la top explorers
			team1.revivePlayers();
			team2.revivePlayers();
		}
		// daca linia e de tipul senzor: jucator
		if( temp.find(":") != std::string::npos ) {
			int senzor = getSenzor(temp);  // afla care e senzorul
			std::string movingPlayer = getMovingPlayer(temp);  // afla jucatorul
			int playersTeam = getTeam(movingPlayer, team1);  // si echipa sa
			if( playersTeam == 1 ) {  // daca e din prima echipa
				// calculeaza noul scor pentru top explorers
				int lastSenzor = team1.getLastSenzor(movingPlayer);
				int distance = 0;
				if (lastSenzor != -1)  // doar daca nu e primul senzor activat
					distance = arena[lastSenzor][senzor];
				team1.addSenzorInfo(movingPlayer, senzor, distance);
			}
			if( playersTeam == 2 ) {  // la fel daca jucatorul e din team2
				int lastSenzor = team2.getLastSenzor(movingPlayer);
				int distance = arena[lastSenzor][senzor];
				team2.addSenzorInfo(movingPlayer, senzor, distance);
			}
		}
		// daca linia e de tipul jucator1 -> jucator2
		if( temp.find("->") != std::string::npos ) {
			// afla cine e shooter
			std::string shooter = getShooter(temp);
			int shooterTeam = getTeam(shooter, team1);
			// afla cine a fost impuscat
			std::string shot = getShot(temp);
			int shotTeam = getTeam(shot, team1);
			// daca a fost impuscat un jucator din echipa adversa
			if( shooterTeam != shotTeam ) {
				// se adauga doua puncte lui shooter
				if( shooterTeam == 1 ) {
					team1.changeShootingScore(shooter, true);
					team2.reduceHealth(shot);

					int shooterIndex = team1.getPlayerIndex(shooter);
					int shotIndex = team2.getPlayerIndex(shot);
					shGraph[shooterIndex][shotIndex]++;
				} else {
					team2.changeShootingScore(shooter, true);
					team1.reduceHealth(shot);

					int shooterIndex = team2.getPlayerIndex(shooter);
					int shotIndex = team1.getPlayerIndex(shot);
					shGraph[shooterIndex][shotIndex]++;
				}
			} else {  // daca a fost impuscat un jucator din aceeasi echipa
				// se scad 5 puncte lui shooter
				if( shooterTeam == 1 ) {
					team1.changeShootingScore(shooter, false);
					team1.reduceHealth(shot);

					int shooterIndex = team1.getPlayerIndex(shooter);
					int shotIndex = team1.getPlayerIndex(shot);
					shGraph[shooterIndex][shotIndex]++;
				} else {
					team2.changeShootingScore(shooter, false);
					team2.reduceHealth(shot);
					int shooterIndex = team2.getPlayerIndex(shooter);
					int shotIndex = team2.getPlayerIndex(shot);
					shGraph[shooterIndex][shotIndex]++;
				}
			}
		}
		// daca s-a ajuns la finalul campionatului
		if( temp.find("END_") != std::string::npos ) {
			// set final data adauga la explorerScore 3 * unique senzors
			team1.setFinalData();
			team2.setFinalData();

			// creeaza vectorul jucatorilor la final, pentru a fi
			// sortati pentru top shooters si top explorers
			struct tinyData sortedPlayers[1500];
			for( i = 0; i < (nrTeam1 + nrTeam2); i++ ) {
				if (i < nrTeam1) {
					// daca e din prima echipa
					// adauga nume si cele doua scoruri
					sortedPlayers[i].name = team1.getNameByIndex(i);
					sortedPlayers[i].explorerScore =
					team1.getExplorerScore(sortedPlayers[i].name);
					sortedPlayers[i].shootingScore =
					team1.getShootingScore(sortedPlayers[i].name);
				} else {
					// la fel daca e din a doua echipa
					sortedPlayers[i].name = team2.getNameByIndex(i);
					sortedPlayers[i].explorerScore =
					team2.getExplorerScore(sortedPlayers[i].name);
					sortedPlayers[i].shootingScore =
					team2.getShootingScore(sortedPlayers[i].name);
				}
			}
			// sorteaza dupa regula pentru top shooters
			std::sort(sortedPlayers, sortedPlayers + nrTeam1 + nrTeam2, shooterRule);
			std::cout << "I. Top shooters" << '\n';

			// afisare top shooters
			int cnt = 0;
			while( cnt < 5 ) {  // cat timp nu s-au afisat inca top 5 players
				// daca numele sau scorul nu sunt nevalide
				if( sortedPlayers[cnt].name != "" &&
				sortedPlayers[cnt].shootingScore > -900 )
				// le afiseaza
					std::cout << cnt + 1 << ". " << sortedPlayers[cnt].name << " ";
					std::cout << sortedPlayers[cnt].shootingScore << "p" << '\n';
				// daca nu s-a terminat top 5 sau trebuie afisati mai mult
				// de 5 jucatori
				if ( sortedPlayers[cnt + 1].shootingScore !=
					sortedPlayers[cnt].shootingScore || cnt < 5 ) {
					cnt++;  // incrementare counter
					// cand s-a ajuns la 5 jucatori verifica cati mai
					// sunt cu scorul celui de pe locul 5 si ii afiseaza
					// si pe aceia
					if( cnt == 5 ) {
						while( sortedPlayers[cnt].shootingScore ==
							sortedPlayers[cnt - 1].shootingScore ){
							std::cout << cnt + 1 << ". " << sortedPlayers[cnt].name;
							std::cout << " " << sortedPlayers[cnt].shootingScore << "p" << '\n';
							cnt++;
						}
					}
				}
				// daca s-a ajuns la final cu afisarea (nu mai exista datele
			    // valide de afisat => iese din bucla while de afisare)
				if (sortedPlayers[cnt].name == "" ||
				sortedPlayers[cnt].shootingScore < -900)
					cnt = cnt + 6;
			}
			// topShooterScore global folosit pentru winning chance
			int topShooterScore = sortedPlayers[0].shootingScore;
			std::cout << '\n';

			// sorteaza dupa regula de la top explorers
			std::sort(sortedPlayers, sortedPlayers + nrTeam1 + nrTeam2, explorerRule);
			std::cout << "II. Top explorers" << '\n';

			// afisarea top explorers dupa exact aceleasi reguli ca mai substr
			// la top shooters
			cnt = 0;
			while( cnt < 5 ) {
				if( sortedPlayers[cnt].name != "" &&
				sortedPlayers[cnt].explorerScore > -900 )
					std::cout << cnt + 1 << ". " << sortedPlayers[cnt].name << " ";
					std::cout << sortedPlayers[cnt].explorerScore << "p" << '\n';

				if ( sortedPlayers[cnt + 1].explorerScore !=
					sortedPlayers[cnt].explorerScore || cnt < 5 ) {
					cnt++;
					if( cnt == 5 ) {
						while( sortedPlayers[cnt].explorerScore ==
							sortedPlayers[cnt - 1].explorerScore ){
							std::cout << cnt + 1 << ". " << sortedPlayers[cnt].name;
							std::cout << " " << sortedPlayers[cnt].explorerScore << "p" << '\n';
							cnt++;
						}
					}
				}

				if (sortedPlayers[cnt].name == "" ||
				    sortedPlayers[cnt].explorerScore < -900)
					cnt = cnt + 6;
			}
			std::cout << '\n';
			// scor top explorer global, folosit la winning chance
			int topExplorerScore = sortedPlayers[0].explorerScore;

			// formez array-ul fireExchange care condine datele minim necesare
			// pentru formarea top fire exchange
			int fcnt = 0;
			std::string player1 = "";
			std::string player2 = "";
			int j;
			// parcurge o zona din matricea de adiacenta a grafului
			// ce retine cine in cine a tras astfel incat sa nu apara
			// cazurile playerX - playerX sau duplicate precum
			// playerX - playerY ; playerY - playerX
			for( i = 1; i < nrTeam1 + nrTeam2; i++ ) {
				for( j = 0; j <=i; j++ ) {
					// afla cine sunt jucatorii si din ce echipa sunt ei
					player1 = team1.getNameByIndex(i);
					if(player1 == "")
						player1 = team2.getNameByIndex(i);
					player2 = team1.getNameByIndex(j);
					if(player2 == "")
					player2 = team2.getNameByIndex(j);

					// ii pune in ordine alfabetica
					std::string aux;
					if( player1 > player2 ){
						aux = player1;
						player1 = player2;
						player2 = aux;
					}
					// adauga datele despre acestia in array
					fireExchange[fcnt].firedShots = 0;
					fireExchange[fcnt].name1 = player1;
					fireExchange[fcnt].name2 = player2;
					// scorul este dat de #x->y + #y->x retinut in graf
					// la pozitiile [indexX][indexY] si [indexY][indexX]
					fireExchange[fcnt].firedShots = shGraph[i][j] + shGraph[j][i];
					fcnt++;
				}
			}
			// sortarea topului
			std::sort(fireExchange, fireExchange + fcnt, feRule);

			// afisarea topului
			std::cout << "III. Top fire exchange" << '\n';
			// daca sunt mai putin de 5 in top, afiseaza doar cati sunt
			int linesToDisplay = 5;
			if( nrTeam1 + nrTeam2 < 5 ) {  // adica nrTeam1 + nrTeam2
				linesToDisplay = nrTeam1 + nrTeam2;
			}
			i = 0;
			// cat timp nu s-au afisat toti
			while( i < linesToDisplay ) {
				// cout doar datele valide
				if( fireExchange[i].name1 != "" && fireExchange[i].name2 != "")
				if( fireExchange[i].name1 != fireExchange[i].name2)
				if( fireExchange[i].firedShots > -900 ) {
					std::cout << i+1 << ". " << fireExchange[i].name1 << " - ";
					std::cout << fireExchange[i].name2 << " ";
					std::cout << fireExchange[i].firedShots << '\n';
				}
				i++;
			}
			// daca s-a afisat top5, dar urmeaza persoane cu acelasi score
			// ca persoanele de pe locul 5
			if ( linesToDisplay >= 5 ) {
				j = 5;
				// ii afiseaza si pe urmatorii, atata timp cat datele sunt valide
				while( fireExchange[i].firedShots == fireExchange[j].firedShots ) {
					if( fireExchange[j].name1 != "" && fireExchange[j].name2 != "")
					if( fireExchange[j].name1 != fireExchange[j].name2)
					if( fireExchange[j].firedShots > -900 ) {
					std::cout << j+1 << ". " << fireExchange[j].name1 << " - ";
					std::cout << fireExchange[j].name2 << " ";
					std::cout << fireExchange[j].firedShots << '\n';
				}
					j++;
				}
			}
			std::cout << '\n';

			// calculeaza sansa de castig a fiecarei echipe
			float team1Chance = team1.getChance(topShooterScore, topExplorerScore);
			float team2Chance = team2.getChance(topShooterScore, topExplorerScore);
			if( team1Chance >= team2Chance ) {
				score1++;
			} else {
				score2++;
			}
			// se scade 1 din score1 pentru ca team1 primeste inca 1 punct
			// cand incepe primul joc
			score1 = score1 - 1;
			// display final score
			std::cout << "IV. Final score" << '\n';
			std::cout << score1 << " - " << score2 << '\n';

			break;  // iese din gigantica bucla while la terminarea campionatului
		}
	}

	return 0;
}
