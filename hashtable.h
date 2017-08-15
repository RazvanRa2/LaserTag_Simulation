// Copyright 2017 Razvan Radoi, first of his name
#ifndef _HOME_STUDENT__RESOURCES__HASHTABLE_H_
#define _HOME_STUDENT__RESOURCES__HASHTABLE_H_
#include <iostream>
#include <string>
#include <fstream>
#include <list>
#include "./player.h"
class Hashtable {
 private:
	int HMAX;		// nr de sets
	std::list<struct Player> * H;
	int(*hashingFunction) (std::string);  // functia de hashing

 public:
	// constructor
	Hashtable(int hmax, int (*h)(std::string)){
		HMAX = hmax;
		hashingFunction = h;
		H = new std::list<struct Player>[HMAX];
	}
	// deconstructor
	~Hashtable(){
		delete[] H;
	}
	// adaugarea unui nou jucator
	void addNewMember(std::string key, int teamNr, int index) {
		int nameSum = hashingFunction(key) % HMAX;
		// creez un jucator temporar cu datele joului jucator
		struct Player dummyPlayer;
		dummyPlayer.name = key;
		dummyPlayer.team = teamNr;
		dummyPlayer.playerIndex = index;
		dummyPlayer.health = 2;
		int error = 0;
		// si il adaug acolo unde ii e locul, daca acesta nu exista deja
		// daca exista deja e sigur o eroare, pentru ca nu se adauaga
		// acelasi jucator de doua ori
		// totusi, daca se adauga, ii updateaza datele.
		std::list<struct Player>::iterator i;
		for(i = H[nameSum].begin(); i != H[nameSum].end(); i++) {
			if(i->name == key) {
				i->team = teamNr;
				i->playerIndex = index;
				i->health = 2;
				error = 1;
			}
		}
		if(!error)
			H[nameSum].insert(i, dummyPlayer);
	}
	// verifica daca un jucator e membru al unei echipe, dupa nume
	bool checkIfMember(std::string key) {
		bool isMember = false;
		// calculeaza cu functia de hash unde sa il caute, apoi il cauta
		int nameSum = hashingFunction(key) % HMAX;
		std::list<struct Player>::iterator i;
		for( i = H[nameSum].begin(); i != H[nameSum].end(); i++ ) {
			if( i->name == key ) {
				isMember = true;
			}
		}
		return isMember;
	}
	// returneaza ultimul senzor pe langa care a trecut un player
	int getLastSenzor(std::string key) {
		int nameSum = hashingFunction(key) % HMAX;
		std::list<struct Player>::iterator i;
	 	for(i = H[nameSum].begin(); i != H[nameSum].end(); i++){
			if( i-> name == key )
				return i->lastSenzor;
		}
		// pentru siguranta daca nu e gasit niciun senzor, returneaza -1
		// adica ca acesta nu a trecut pe langa niciun senzor inca
		return -1;
	}
	// adauga informatii despre senzori pentru un player
	void addSenzorInfo(std::string key, int senzor, int distance) {
		int nameSum = hashingFunction(key) % HMAX;
		std::list<struct Player>::iterator i;
		for(i = H[nameSum].begin(); i != H[nameSum].end(); i++){
			if( i->name == key ) {
				// daca nu a mai trecut pe langa vreun senzor
				// doar adauga informatiile noi
				if( i->lastSenzor == -1 ) {
					i->triggeredSenzors[senzor] = 1;
					i->lastSenzor = senzor;
				} else {
					// daca a mai trecut, face update si la
					// explorer score
					i->explorerScore += distance;
					i->triggeredSenzors[senzor] = 1;
					i->lastSenzor = senzor;
				}
			}
		}
	}
	// returneaza explorer score
	int getExplorerScore(std::string key) {
		int nameSum = hashingFunction(key) % HMAX;
		std::list<struct Player>::iterator i;
	 	for(i = H[nameSum].begin(); i != H[nameSum].end(); i++){
			if(i->name == key)
				return i->explorerScore;
		}
		return 0;
	}
	// returneaza indexul unui jucator
	int getPlayerIndex(std::string key) {
		int nameSum = hashingFunction(key) % HMAX;
		std::list<struct Player>::iterator i;
		for(i = H[nameSum].begin(); i != H[nameSum].end(); i++){
			if(i->name == key){
				return i->playerIndex;
			}
		}
		// daca nu exista jucatorul, returneaza -1
		return -1;
	}
	// modifica shooting score
	void changeShootingScore(std::string key, bool ok) {
		int nameSum = hashingFunction(key) % HMAX;
		std::list<struct Player>::iterator i;
		for(i = H[nameSum].begin(); i != H[nameSum].end(); i++){
			if(i->name == key) {
				// daca a tras in adversar adauga 2 puncte
				if(ok == true) {
					i->shootingScore += 2;
				} else {
				// daca a tras in coechipier scade 5 puncte
					i->shootingScore -= 5;
				}
			}
		}
	}
	// reduce viata unui jucator
	void reduceHealth(std::string key) {
		int nameSum = hashingFunction(key) % HMAX;
		std::list<struct Player>::iterator i;
		for(i = H[nameSum].begin(); i != H[nameSum].end(); i++){
			if(i->name == key) {
				i->health = i->health - 1;
			}
		}
	}
	// returneaza shootingScore pentru un jucator
	int getShootingScore(std::string key) {
		int nameSum = hashingFunction(key) % HMAX;
		std::list<struct Player>::iterator i;
		for(i = H[nameSum].begin(); i != H[nameSum].end(); i++){
			if(i->name == key) {
				return i->shootingScore;
			}
		}
		return -1;
	}
	// dupa un index dat, spune numele jucatorului cu acel index
	std::string getNameByIndex(int index) {
		for(int j = 0; j < HMAX; j++) {
			std::list<struct Player>::iterator i;
			for(i = H[j].begin(); i != H[j].end(); i++){
				if(i->playerIndex == index) {
					return i->name;
				}
			}
		}
		// daca dupa index nu a fost gasit un jucator, returneaza un string global
		// care inseamna ca jucatorul nu apartine echipei in care a fost cautat
		std::string junk = "";
		return junk;
	}

	// verifica daca mai exista jucatori in viata, iar dupa asta spune
	// care echipa a castigat runda
	int searchAnyAlivePlayers() {
		int j;
		for( j = 0; j < HMAX; j++ ){
			std::list<struct Player>::iterator i;
			for(i = H[j].begin(); i != H[j].end(); i++){
				if(i->health > 0)
				// daca in prima echipa mai sunt jucatori vii, ea a castigat
					return 1;
			}
		}
		// altfel, a doua echipa a castigat
		return 2;
	}
	// afla sansa de castig a unei echipe
	float getChance(int tss, int tes) {
		float chance = 0;  // default sansa de castig e 0
		int j;
		for( j = 0; j < HMAX; j++ ) {  // pentru fiecare jucator
			std::list<struct Player>::iterator i;
			for(i = H[j].begin(); i != H[j].end(); i++){
				// daca e in viata
				if(i->health > 0) {
					if(tss != 0)  // daca top score e mai mare ca 0
					// adauga la chance
					chance += (float)i->shootingScore / (float)tss;
					if(tes != 0)  // daca top explorer e mai mare ca 0
					// adauga la chance
					chance += (float)i->explorerScore / (float)tes;
				}
			}
		}

		return chance;
	}
	// reinvie toti jucatorii la inceput de joc si le adauga scorul
	// pentru unique senzors
	void revivePlayers(){
		int j, k;
		for( k = 0; k < HMAX; k++ ) {
			std::list<struct Player>::iterator i;
			for(i = H[k].begin(); i != H[k].end(); i++){
				if( i-> name != "" ) {
					i->health = 2;  // reset health la fiecare

					// activatedSenzors = senzori unici activati de jucator
					int activatedSenzors = 0;
					for( j = 0; j < 30; j++ ) {
						if( i->triggeredSenzors[j] == 1 ) {
							activatedSenzors++;
						}
					}
					// adauga scorul aferent
					i->explorerScore += (3 * activatedSenzors);
					i->lastSenzor = -1;
					// reset la vectorul de frecventa al senzorilor activati
					for( j = 0; j < 30; j++ ) {
						i->triggeredSenzors[j] = 0;
					}
				}
			}
		}
	}
	// la fel ca revivePlayers, adauga scorul pentru top explorers, dar
	// nu si reinvie jucatorii, pentru ca s-a ajuns la finalul campionatului
	void setFinalData(){
		int j, k;
		for( k = 0; k < HMAX; k++ ) {
			std::list<struct Player>::iterator i;
			for(i = H[k].begin(); i != H[k].end(); i++){
				if( i-> name != "" ) {
					int activatedSenzors = 0;
					for( j = 0; j < 30; j++ ) {
						if( i->triggeredSenzors[j] == 1 ) {
							activatedSenzors++;
						}
					}
					i->explorerScore += (3 * activatedSenzors);
					i->lastSenzor = -1;

					for( j = 0; j < 30; j++ ) {
						i->triggeredSenzors[j] = 0;
					}
				}
			}
		}
	}
};
#endif  // _HOME_STUDENT__RESOURCES__HASHTABLE_H_
