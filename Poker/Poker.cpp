#include "stdafx.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <time.h>
#include <list>
#include <iostream>
#include <algorithm>
#include <functional>
#include "Table.h"
#include "Object.h"
#include "Animation.h"
#include "Player.h"
#include "Opponent.h"
#include "Cards.h"
#include "Util.h"

void mixCards(int*, Player &, Opponent &);
void playSound(std::string);
int ai(sf::RenderWindow &, Player &, Opponent &, Table *, Cards *cards, Animation *, int*, int, sf::String&);
void drawAll(sf::RenderWindow&, std::list<Object*>, Table *);
void checkCards(Player &, Opponent &, std::string, std::string&, int&, int&, int&, int*);
void delay(int);

using namespace sf;

const int ScreenWidth = 1207;
const int ScreenHeight = 831;
const int AI_ThrowCards = -10;
const int AI_ReplaceCards = -20;
const int AI_increasesRate = -30;
const int Draw_Win = -10;
const int Draw_Defeat= -20;
const int Draw_Score = -30;


int main()
{
	srand(time(0));
	std::list<Object*> objects;


	RenderWindow app(VideoMode(ScreenWidth, ScreenHeight), "SFML JNXEngine v1.0");
	app.setFramerateLimit(60);

	Table *table = new Table();
	table->setFont("BornAddict.ttf", 24, sf::Color::White);

	Texture t2, tCards, tCardBack;
	t2.loadFromFile("images/opponent.png");
	tCards.loadFromFile("images/playingCards.png");
	tCardBack.loadFromFile("images/playingCardBacks.png");

	Animation sOpponent(t2, 170, 0, 79, 110, 3, 0, 0.05);
	Animation sOpponentWin(t2, 0, 120, 140, 132, 8, 4, 0.05);
	Animation sOpponentDefeat(t2, 0, 390, 176, 110, 3, 0, 0.02);

	int cardsPosition[120] = {		// Sprite position on file
		700 ,380,280 ,950,280 ,760,280 ,570,280 ,380,280 ,190,280 ,0,140 ,1710,140 ,1520,140 ,1140,140 ,760,140 ,950,140 ,1330,420 ,1710,420 ,1520,420 ,1330,420 ,1140,420 ,950,420 ,760,420 ,570,420 ,380,420 ,190,280 ,1710,280 ,1330,280 ,1520,420 ,0,140 ,380,140 ,190,140 ,0,0 ,1710,0 ,1520,0 ,1330,0 ,1140,0 ,950,0 ,760,0 ,380,0 ,0,0 ,190,0 ,570,280 ,1140,700 ,190,700 ,0,560 ,1710,560 ,1520,560 ,1330,560 ,1140,560 ,950,560 ,760,560 ,380,560 ,0,560 ,190,560 ,570
	};
	Animation sCard[60];
	for (int nr = 0; nr < 52; nr++) {
		Animation tempAnim(tCards, cardsPosition[nr * 2], cardsPosition[nr * 2 + 1], 140, 190, 1, 0, 0);
		tempAnim.reduceSprite();
		sCard[nr] = tempAnim;
	}
	Animation tempAnim(tCardBack, 0, 0, 140, 190, 1, 0, 0);		// back cards
	tempAnim.reduceSprite();
	sCard[52] = tempAnim;


	Opponent opponent;
	opponent.settings(sOpponent, 480, 100);
	objects.push_back(&opponent);

	Player player;

	Cards cards[20];		// cards to display, 0-4 player, 5-9 ai,
	int deckOfcards[52];			// deck with card numbers, 0-4 player, 5-9 and, 10+ to choose 

	Util myUtil;		// mouse click zones
	myUtil.setZone(1, 265, 645, 395, 695);			// rzuæ cards
	myUtil.setZone(2, 405, 645, 620, 695);			// check / pass
	myUtil.setZone(3, 630, 645, 780, 695);			// exchange cards
	myUtil.setZone(4, 790, 645, 930, 695);			// new cards deal

	myUtil.setZone(8, 410, 320, 475, 410);		// cards
	myUtil.setZone(9, 490, 320, 560, 410);
	myUtil.setZone(10, 570, 320, 640, 410);
	myUtil.setZone(11, 650, 320, 720, 410);
	myUtil.setZone(12, 730, 320, 800, 410);

	myUtil.setZone(13, 450, 500, 515, 565);		// chips
	myUtil.setZone(14, 530, 500, 595, 565);
	myUtil.setZone(15, 610, 500, 675, 565);
	myUtil.setZone(16, 690, 500, 755, 565);

	int selectedZone;	// which zone was selected (clicked)
	int round = 0;
	// =0 - start of the game, distribution of cards
	// =1 - you can throw cards, raise the bid, check (drop, rise, pass)
	// =11 (ai) - ai see also drop/rise/pass 
	// =2 - you can throw cards, swap cards, check, you have to raise the rate (drop, rise, change, pass)
	// =22 - ai: player raises and can no longer bet now
	// =2a cards are being exchanged now
	// =3 - throw cards, raise your bid, check (drop, rise, check)
	// =33 - ai: drop/pass/check

	int AI_reaction = 0;
	sf::String message = "";

	while (app.isOpen())
	{
		Event event;
		while (app.pollEvent(event))
		{
			if (event.type == Event::Closed)		app.close();
		}

		if (Keyboard::isKeyPressed(Keyboard::Escape))		app.close();
		if (Keyboard::isKeyPressed(Keyboard::P))			table->spause(app);
		if (Keyboard::isKeyPressed(Keyboard::G))			table->sgameOver(app);

		selectedZone = myUtil.checkZones(app);
		if (selectedZone == 1 && round != 5) {	// drop cards
			// opponent cards
			for (int i = 0; i < 5; i++) {
				cards[i + 5].settings(sCard[opponent.getCard(i)], 445 + i * 80, 220);
				objects.push_back(&cards[i + 5]);
				drawAll(app, objects, table);
			}
			playSound("images/you_lose.ogg");
			table->dropCards(app);
			opponent.settings(sOpponentWin, 480, 100);
			objects.push_back(&opponent);
			drawAll(app, objects, table);
			myUtil.delay(200);
			round = 5;
		}
		if (selectedZone == 2 && round == 1) {		// first card check
			AI_reaction = ai(app, player, opponent, table, cards, sCard, deckOfcards, round, message);
			if (AI_reaction == AI_ThrowCards) {
				// opponent cards
				for (int i = 0; i < 5; i++) {
					cards[i + 5].settings(sCard[opponent.getCard(i)], 445 + i * 80, 220);
					objects.push_back(&cards[i + 5]);
					drawAll(app, objects, table);
				}
				opponent.settings(sOpponentDefeat, 480, 100);
				objects.push_back(&opponent);
				round = 5;
				playSound("images/you_win.ogg");
				for (int i = 0; i < 25; i++)
					drawAll(app, objects, table);
				table->AI_dropCards(app);
			}
			else
			{
				round = 2;
			}
		}

		//check opponents, next round
		if (selectedZone == 2 && AI_reaction != AI_increasesRate && (round == 2 || round == 3)) {
			AI_reaction = ai(app, player, opponent, table, cards, sCard, deckOfcards, 2, message);
			if (AI_reaction == AI_ThrowCards) {
				// opponent cards
				for (int i = 0; i < 5; i++) {
					cards[i + 5].settings(sCard[opponent.getCard(i)], 445 + i * 80, 220);
					objects.push_back(&cards[i + 5]);
					drawAll(app, objects, table);
				}
				opponent.settings(sOpponentDefeat, 480, 100);
				objects.push_back(&opponent);
				round = 5;
				playSound("images/you_win.ogg");
				for (int i = 0; i < 25; i++)
					drawAll(app, objects, table);
				table->AI_dropCards(app);
			}
			else
			{
				round = 4;
			}
		}

		if (message.getSize() > 5 && round == 2 && AI_reaction != AI_increasesRate) {
			table->write(app, sf::Vector2f(370, 700), message);
			app.display();
			delay(2200);
			message = "";
		}

		if (selectedZone == 2 && AI_reaction != AI_increasesRate && round == 4) {		// last card check

			// opponent cards
			for (int i = 0; i < 5; i++) {
				cards[i + 5].settings(sCard[opponent.getCard(i)], 445 + i * 80, 220);
				objects.push_back(&cards[i + 5]);
				drawAll(app, objects, table);
			}
			AI_reaction = ai(app, player, opponent, table, cards, sCard, deckOfcards, 3, message);
			if (AI_reaction == Draw_Win) {
				playSound("images/you_win.ogg");
				opponent.settings(sOpponentDefeat, 480, 100);
				objects.push_back(&opponent);
				drawAll(app, objects, table);
				table->youwin(app);
			}
			else if (AI_reaction == Draw_Score) {
				table->draw(app);
			}
			else if (AI_reaction == Draw_Defeat) {
				playSound("images/you_lose.ogg");
				opponent.settings(sOpponentWin, 480, 100);
				objects.push_back(&opponent);
				drawAll(app, objects, table);
				table->youlose(app);
			}

			round = 5;
		}

		if (selectedZone == 3 && round == 2 && AI_reaction != AI_increasesRate) {		// exchange cards
			for (int i = 0; i < 5; i++) {
				if (player.CheckIsRaised(i) == true) {
					player.setCard(i, deckOfcards[10 + i]);
					cards[i].settings(sCard[player.getCard(i)], 445 + i * 80, 364);
					player.resetCard(i);
				}
			}
			round = 3;
		}
		if ((selectedZone == 4 && round == 5) || round == 0) {		// new deal
			system("cls");
			opponent.settings(sOpponent, 480, 100);
			objects.push_back(&opponent);
			mixCards(deckOfcards, player, opponent);
			for (int i = 0; i < 5; i++) {
				cards[i].settings(sCard[player.getCard(i)], 445 + i * 80, 364);
				//				cards[i+5].settings(sCard[opponent.getCard(i)], 445 + i * 80, 220);
				cards[i + 5].settings(sCard[52], 445 + i * 80, 220);
				objects.push_back(&cards[i]);
				objects.push_back(&cards[i + 5]);
			}
			table->newDealCards();
			round = 1;
		}
		if (selectedZone >= 8 && selectedZone <= 12 && round == 2) {
			player.changeRaised(selectedZone - 8);
			if (player.FourSelectedCards() == true) {
				player.changeRaised(selectedZone - 8);
			}
			else
			{
				playSound("images/cardSlide1.wav");
				if (player.CheckIsRaised(selectedZone - 8) == true) {
					cards[selectedZone - 8].setY(350);
				}
				else if (player.CheckIsRaised(selectedZone - 8) == false)
				{
					cards[selectedZone - 8].setY(364);
				}
			}
		}
		if (selectedZone == 13 && (round == 2 || round == 1 || round == 3))	table->addToRate(5);
		if (selectedZone == 14 && (round == 2 || round == 1 || round == 3))	table->addToRate(10);
		if (selectedZone == 15 && (round == 2 || round == 1 || round == 3))	table->addToRate(20);
		if (selectedZone == 16 && (round == 2 || round == 1 || round == 3))	table->addToRate(50);
		if (selectedZone == 13 && round == 4)	table->addedToRate(5);
		if (selectedZone == 14 && round == 4)	table->addedToRate(10);
		if (selectedZone == 15 && round == 4)	table->addedToRate(20);
		if (selectedZone == 16 && round == 4)	table->addedToRate(50);
		if (AI_reaction == AI_increasesRate) {
			if (table->getRateAI() == table->getPlayerRate()) {
				AI_reaction = 0;
			}
			if (table->getRateAI() < table->getPlayerRate()) {
				AI_reaction = 0;
				table->toCall();
			}
		}
		drawAll(app, objects, table);
		myUtil.delay(40);
	}
	return 0;
}
void drawAll(sf::RenderWindow& app, std::list<Object*> objects, Table *table) {
	for (auto i = objects.begin(); i != objects.end();)
	{
		Object* e = *i;
		e->update();
		e->anim.update();
		if (e->getLife() == false) 		i = objects.erase(i);
		else	i++;

	}

	app.clear();
	table->drawTable(app);
	for (auto i : objects)
		i->draw(app);
	app.display();
}
int ai(sf::RenderWindow& app, Player& player, Opponent& opponent, Table *table, Cards *cards, Animation *sCard, int* deckOfcards, int round, sf::String& message) {
	int cardsToExchange[5]; // which cards to exchange = 0
	std::string coMasz = "nothing";			// here: a couple, two couples, three, three and a couple
	std::string czyStrit = "nothing";		// here: straight, almost straight (4 cards same)
	int figura1, figura2;		// the numbers you have, the higher the number, the higher the figure
	figura1 = figura2 = -1;

	int MOC_KART = 0;
	int MOC_FIGUR = 0;
	//	message = "nothing";

	for (int i = 0; i < 5; i++)	cardsToExchange[i] = -1;		// wyczyœæ (ai) cards do wymiany

	// check which cards your opponent has
	checkCards(player, opponent, "AI", coMasz, figura1, figura2, MOC_KART, cardsToExchange);

	// now you know what the computer has, so it's time to react ai
	std::cout << coMasz << std::endl;

	int ileZagrac = ((rand() % 9) * 5) * MOC_KART;
	if (MOC_KART == 0)
		ileZagrac = (rand() % 7) * 5;

	// Round 1
	if (round == 1) {
		int tempk = 0;

		std::cout << "ROUND 1" << std::endl;
		if (coMasz == "nothing" && (rand() % 3) == 1)
			return AI_ThrowCards;	// nothing, so I throw cards
		if (coMasz == "nothing" && table->getPlayerRate() > 50 && (rand() % 2) == 1)
			return AI_ThrowCards;	// player put on the table, so you have to level up

		if (table->getPlayerRate() > 10) {
			// player coœ wrzuci³ na stó³ wiêc trzeba wyrównaæ
			if (table->AI_toCall() == false) {
				// za ma³o geldów
	//			std::cout << "AI does not level the bet and throws the cards" << std::endl;
				return AI_ThrowCards;
			}
		}

		if (ileZagrac > 0)
		{

			std::cout << "AI raises the bet on " << ileZagrac << std::endl;
			table->AI_raiseBet(app, ileZagrac);
			for (int i = 0; i < 5; i++) {
				if (cardsToExchange[i] == -1) {
					tempk++;
					//					std::cout << "wymieniam cards" << std::endl;
					opponent.setCard(i, deckOfcards[15 + i]);
					//			cards[i + 5].settings(sCard[opponent.getCard(i)], 445 + i * 80, 220);
				}
			}
			if (tempk == 5) tempk = 4;
			message = "Computer replaces " + std::to_string(tempk) + " cards";
			return AI_increasesRate;
		}

		// wymieñ ju¿ teraz cards, na nastêpn¹ turê.
		for (int i = 0; i < 5; i++) {
			if (cardsToExchange[i] == -1) {
				tempk++;
				opponent.setCard(i, deckOfcards[15 + i]);
				//			cards[i+5].settings(sCard[opponent.getCard(i)], 445 + i * 80, 220);
			}
		}
		// ma³e oszustwo, komputer moze wymienic 5 kart, ale player siê o tym nie dowie :)
		if (tempk == 5) tempk = 4;
		message = "Computer replaces " + std::to_string(tempk) + " cards";

	}

	if (round == 2) {
		std::cout << "second check of cards" << std::endl;
		// nothing = throw the cards
		if (coMasz == "nothing" && (rand() % 2) == 1)
			return AI_ThrowCards;	// there is nothing, I throw the cards

		// wyrownaj stawke
		if (table->AI_toCall() == false) {
			// za ma³o geldów
			std::cout << "round 2 AI does not level the bet and throws the cards" << std::endl;
			return AI_ThrowCards;
		}

		// depending on the cards, raise the rate
		std::cout << "ile zagrac =" << ileZagrac << std::endl;
		if (ileZagrac > 0)
		{
			std::cout << "round2 AI raises the bet on " << ileZagrac << std::endl;
			table->AI_raiseBet(app, ileZagrac);
			return AI_increasesRate;
		}

	}

	if (round == 3) {
		std::string mojeKarty = "nothing";
		std::string aiKarty = "nothing";
		int mfig1, mfig2, aifig1, aifig2;
		int aiMoc, tyMoc;

		//		for (int i = 0; i < 5; i++) {
		std::cout << "final check card" << std::endl;
		std::cout << "disclose cards" << std::endl;
		//			cards[i + 5].settings(sCard[opponent.getCard(i)], 445 + i * 80, 220);
		//		}
		checkCards(player, opponent, "AI", coMasz, figura1, figura2, MOC_KART, cardsToExchange);
		aiKarty = coMasz;
		aiMoc = MOC_KART;
		aifig1 = figura1;
		aifig2 = figura2;
		checkCards(player, opponent, "YOU", coMasz, figura1, figura2, MOC_KART, cardsToExchange);
		mojeKarty = coMasz;
		tyMoc = MOC_KART;
		mfig1 = figura1;
		mfig2 = figura2;
		std::cout << "The results are:" << std::endl;
		std::cout << "AI:" << aiKarty << " moc=" << aiMoc << "  figury:" << aifig1 << "  " << aifig2 << std::endl;
		std::cout << "YOU:" << mojeKarty << " moc=" << tyMoc << "  figury:" << mfig1 << "  " << mfig2 << std::endl;

		if (aiMoc > tyMoc) {
			std::cout << "COMPUTER WIN\n";
			return Draw_Defeat;
		}
		else if (aiMoc == tyMoc) {
			if (aiMoc == 3) {
				// two couples, two figures to check
				aifig1 = aifig1 + aifig2;
				mfig1 = mfig1 + mfig2;
			}
			if (aifig1 > mfig1) {
				std::cout << "COMPUTER WIN\n";
				return Draw_Defeat;
			}
			else if (aifig1 == mfig1) {
				std::cout << "DRAW\n";
				return Draw_Score;
			}
			else if (aifig1 < mfig1) {
				std::cout << "YOU WIN, LUCKY GUY.\n";
				return Draw_Win;
			}

		}
		else if (aiMoc < tyMoc) {
			std::cout << "YOU WIN, LUCKY GUY.\n";
			return Draw_Win;
		}
	}

	return 0;		// 0 - brak reakcji
}
void checkCards(Player& player, Opponent& opponent, std::string who, std::string& coMasz, int& figura1, int& figura2, int& MOC_KART, int* cardsToExchange) {
	int delta = 13;		// przesuniêcie w kartach, figurach
	int whatCards[13];		// jakie cards w jakich ilosciach wystepuja
	coMasz = "nothing";
	figura1 = figura2 = -1;

	// wpisz do tablicy po ile razy wystepuja konkretne cards
	for (int i = 0; i < 13; i++) {
		whatCards[i] = 0;
		for (int j = 0; j < 5; j++) {
			if (who == "AI") {
				if (opponent.getCard(j) == i || opponent.getCard(j) == i + delta || opponent.getCard(j) == i + (delta * 2) || opponent.getCard(j) == i + (delta * 3)) {
					whatCards[i]++;
				}
			}
			else if (who == "YOU")
			{
				if (player.getCard(j) == i || player.getCard(j) == i + delta || player.getCard(j) == i + (delta * 2) || player.getCard(j) == i + (delta * 3)) {
					whatCards[i]++;
				}
			}
		}
	}

	int temp[5];		// zmniejsza talie do 12 figura, bez kolorów
	for (int i = 0; i < 5; i++) {
		if (who == "AI") {
			temp[i] = opponent.getCard(i);
		}
		else if (who == "YOU")
		{
			temp[i] = player.getCard(i);
		}
		if (temp[i] > 12 && temp[i] < 26)		temp[i] -= delta;
		if (temp[i] >= 26 && temp[i] < 39)	temp[i] -= delta * 2;
		if (temp[i] >= 39)					temp[i] -= delta * 3;
	}


	// i sprawdz czy powtarzaj¹ siê figury (nie dzia³a na strit, trzeba osobno rozpisac)
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			if (i != j && temp[i] == temp[j])
				cardsToExchange[i] = 0;
		}
	}

	//	for (int i = 0; i < 5; i++) {
	//		std::cout << "karta =" << temp[i] << std::endl;
	//		std::cout << i << "=" << cardsToExchange[i] << std::endl;
	//	}

		// check for couples
	for (int i = 0; i < 13; i++) {
		if (whatCards[i] == 2) {
			if (coMasz == "couple") {
				coMasz = "two couples";
				figura2 = i;
				MOC_KART = 3;
			}
			if (coMasz == "nothing") {
				coMasz = "couple";
				figura1 = i;
				MOC_KART = 1;
			}
			//			std::cout << "couple: " << i << std::endl;
		}
	}
	// are there triples
	for (int i = 0; i < 13; i++) {
		if (whatCards[i] == 3) {
			//			std::cout << "three: " << i << std::endl;
			if (coMasz == "nothing") {
				coMasz = "three";
				figura1 = i;
				MOC_KART = 5;
			}
			if (coMasz == "couple") {
				coMasz = "full";
				figura2 = i;
				MOC_KART = 8;
			}

		}
	}
	// maybe four
	for (int i = 0; i < 13; i++) {
		if (whatCards[i] == 4) {
			//			std::cout << "four: " << i << std::endl;
			coMasz = "four";
			figura1 = i;
			MOC_KART = 10;
		}
	}

	// maybe you have strit
	std::sort(temp, temp + 5, std::greater < int >());		// sort arrays in descending order

	// if cards end with an ace it increases the value of the smallest
	if (temp[0] == 12 && temp[1] == 11 && temp[2] == 10 && temp[3] == 9 && temp[4] == 0)
		temp[4] = 8;
	if (temp[0] == 12 && temp[1] == 11 && temp[2] == 10 && temp[3] == 1 && temp[4] == 0) {
		temp[3] = 9;
		temp[4] = 8;
	}
	if (temp[0] == 12 && temp[1] == 11 && temp[2] == 2 && temp[3] == 1 && temp[4] == 0) {
		temp[2] = 10;
		temp[3] = 9;
		temp[4] = 8;
	}
	if (temp[0] == 12 && temp[1] == 3 && temp[2] == 2 && temp[3] == 1 && temp[4] == 0) {
		temp[1] = 11;
		temp[2] = 10;
		temp[3] = 9;
		temp[4] = 8;
	}

	// check if you have straight hmstraight - how much in the straight
	int hmStraight = 0;
	if (coMasz == "nothing") {
		for (int i = 0; i < 4; i++) {
			int t1 = temp[i];
			int t2 = temp[i + 1];
			if (t1 - t2 == 1) hmStraight++;
		}
		int t1 = temp[4];		// check the last card with the first
		int t2 = temp[0];
		if (t1 - t2 == -12) hmStraight++;		// 2 -> As - that's the difference 12
		if (hmStraight == 4) {
			coMasz = "straight";
			MOC_KART = 6;
			for (int i = 0; i < 5; i++) {
				cardsToExchange[i] = 0;
			}
		}
		if (hmStraight == 3) {
			coMasz = "almost straight";
			// we ignore the straight and we exchange 4 cards randomly, but we don't throw them in the first round
		}

	}


}
void mixCards(int* deckOfcards, Player& player, Opponent& opponent) {
	int temp[52];	// table indicating whether the number has already been drawn (then = -1)
	for (int i = 0; i < 52; i++)	temp[i] = i;

	// mix the table
	int n;
	for (int i = 0; i < 52; i++) {
		bool Ok = false;
		do
		{	// draw a number until it repeats
			n = (rand() % 52);
			if (temp[n] != -1) {
				Ok = true;
				temp[n] = -1;
				deckOfcards[i] = n;
			}
		} while (Ok == false);
	}

	//	deckOfcards[5] = 10;
	//	deckOfcards[6] = 11;
	//	deckOfcards[7] = 12;
	//	deckOfcards[8] = 13;
	//	deckOfcards[9] = 14;

	playSound("images/cardFan1.wav");

	// deal the cards
	for (int i = 0; i < 5; i++) {
		player.setCard(i, deckOfcards[i]);
		opponent.setCard(i, deckOfcards[i + 5]);
	}
	player.resetSelectedCards ();
}
void playSound(std::string plik) {
	sf::SoundBuffer buffer;
	buffer.loadFromFile(plik);
	sf::Sound sound(buffer);
	sound.play();
	while (sound.getStatus() == sf::Sound::Playing)
	{
	}

}
void delay(int htmsec) {
	Clock clockTemp;
	clockTemp.restart();
	do {		// wait time
	} while (clockTemp.getElapsedTime().asMilliseconds() < htmsec);
}