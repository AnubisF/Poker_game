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
	int talia[52];			// deck with card numbers, 0-4 player, 5-9 and, 10+ to choose 

	Util myUtil;		// mouse click zones
	myUtil.setZone(1, 265, 645, 395, 695);			// rzu� cards
	myUtil.setZone(2, 405, 645, 620, 695);			// sprawd� / pass
	myUtil.setZone(3, 630, 645, 780, 695);			// wymie� cards
	myUtil.setZone(4, 790, 645, 930, 695);			// nowe rozdanie

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
	sf::String wiadomosc = "";

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
		if (selectedZone == 2 && round == 1) {		// pierwsze sprawdzenie kart
			AI_reaction = ai(app, player, opponent, table, cards, sCard, talia, round, wiadomosc);
			if (AI_reaction == AI_ThrowCards) {
				// ods�o� cards opponenta
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
				table->AI_RzucilKarty(app);
			}
			else
			{
				round = 2;
			}
		}

		//		sprawdz opponenta, nastepna runda
		if (selectedZone == 2 && AI_reaction != AI_increasesRate && (round == 2 || round == 3)) {
			AI_reaction = ai(app, player, opponent, table, cards, sCard, talia, 2, wiadomosc);
			if (AI_reaction == AI_ThrowCards) {
				// ods�o� cards opponenta
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
				table->AI_RzucilKarty(app);
			}
			else
			{
				round = 4;
			}
		}

		if (wiadomosc.getSize() > 5 && round == 2 && AI_reaction != AI_increasesRate) {
			table->write(app, sf::Vector2f(370, 700), wiadomosc);
			app.display();
			delay(2200);
			wiadomosc = "";
		}

		if (selectedZone == 2 && AI_reaction != AI_increasesRate && round == 4) {		// ostatnie sprawdzenie kart

			// ods�o� cards opponenta
			for (int i = 0; i < 5; i++) {
				cards[i + 5].settings(sCard[opponent.getCard(i)], 445 + i * 80, 220);
				objects.push_back(&cards[i + 5]);
				drawAll(app, objects, table);
			}
			AI_reaction = ai(app, player, opponent, table, cards, sCard, talia, 3, wiadomosc);
			if (AI_reaction == Draw_Win) {
				playSound("images/you_win.ogg");
				opponent.settings(sOpponentDefeat, 480, 100);
				objects.push_back(&opponent);
				drawAll(app, objects, table);
				table->youwon(app);
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
					player.setCard(i, talia[10 + i]);
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
			mixCards(talia, player, opponent);
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
				table->raiseRate();
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
int ai(sf::RenderWindow& app, Player& player, Opponent& opponent, Table *table, Cards *cards, Animation *sCard, int* talia, int runda, sf::String& wiadomosc) {
	int cardsDoWymiany[5]; // kt�re cards wymieni� = 0
	std::string coMasz = "nic";			// tutaj: para, dwie pary, trojka, trojka i para
	std::string czyStrit = "nic";		// tutaj: strit, prawie strit (4 cards takie same)
	int figura1, figura2;		// numery figur ktore masz, im wyzszy numer tym wyzsza figura
	figura1 = figura2 = -1;

	int MOC_KART = 0;
	int MOC_FIGUR = 0;
	//	wiadomosc = "nic";

	for (int i = 0; i < 5; i++)	cardsDoWymiany[i] = -1;		// wyczy�� (ai) cards do wymiany

	// sprawdz jakie cards ma opponent
	checkCards(player, opponent, "AI", coMasz, figura1, figura2, MOC_KART, cardsDoWymiany);

	// teraz juz wiadomo co ma komputer, wi�c czas na reakcje ai
	std::cout << coMasz << std::endl;

	int ileZagrac = ((rand() % 9) * 5) * MOC_KART;
	if (MOC_KART == 0)
		ileZagrac = (rand() % 7) * 5;

	// RUNDA 1
	if (runda == 1) {
		int tempk = 0;

		std::cout << "RUNDA 1" << std::endl;
		if (coMasz == "nic" && (rand() % 3) == 1)
			return AI_ThrowCards;	// nic nie ma, wiec rzucam cards
		if (coMasz == "nic" && table->getPlayerRate() > 50 && (rand() % 2) == 1)
			return AI_ThrowCards;	// nic nie ma, a zawodnik sporo postawil

		if (table->getPlayerRate() > 10) {
			// player co� wrzuci� na st� wi�c trzeba wyr�wna�
			if (table->AI_equalRate() == false) {
				// za ma�o geld�w
	//			std::cout << "AI NIE wyrownuje stawki i rzuca kartami" << std::endl;
				return AI_ThrowCards;
			}
		}

		if (ileZagrac > 0)
		{

			std::cout << "AI podbija stawke o " << ileZagrac << std::endl;
			table->AI_raiseRate(app, ileZagrac);
			for (int i = 0; i < 5; i++) {
				if (cardsDoWymiany[i] == -1) {
					tempk++;
					//					std::cout << "wymieniam cards" << std::endl;
					opponent.setCard(i, talia[15 + i]);
					//			cards[i + 5].settings(sCard[opponent.getCard(i)], 445 + i * 80, 220);
				}
			}
			if (tempk == 5) tempk = 4;
			wiadomosc = "Komputer wymienia " + std::to_string(tempk) + " cards";
			return AI_increasesRate;
		}

		// wymie� ju� teraz cards, na nast�pn� tur�.
		for (int i = 0; i < 5; i++) {
			if (cardsDoWymiany[i] == -1) {
				tempk++;
				opponent.setCard(i, talia[15 + i]);
				//			cards[i+5].settings(sCard[opponent.getCard(i)], 445 + i * 80, 220);
			}
		}
		// ma�e oszustwo, komputer moze wymienic 5 kart, ale player si� o tym nie dowie :)
		if (tempk == 5) tempk = 4;
		wiadomosc = "Komputer wymienia " + std::to_string(tempk) + " cards";

	}

	if (runda == 2) {
		std::cout << "drugie sprawdzenie kart" << std::endl;
		// nic nie ma = rzuc cards
		if (coMasz == "nic" && (rand() % 2) == 1)
			return AI_ThrowCards;	// nic nie ma, wiec rzucam cards

		// wyrownaj stawke
		if (table->AI_equalRate() == false) {
			// za ma�o geld�w
			std::cout << "runda 2 AI NIE wyrownuje stawki i rzuca kartami" << std::endl;
			return AI_ThrowCards;
		}

		// w zaleznosc od kart podbij stawke
		std::cout << "ile zagrac =" << ileZagrac << std::endl;
		if (ileZagrac > 0)
		{
			std::cout << "runda2 AI podbija stawke o " << ileZagrac << std::endl;
			table->AI_raiseRate(app, ileZagrac);
			return AI_increasesRate;
		}

	}

	if (runda == 3) {
		std::string mojeKarty = "nic";
		std::string aiKarty = "nic";
		int mfig1, mfig2, aifig1, aifig2;
		int aiMoc, tyMoc;

		//		for (int i = 0; i < 5; i++) {
		std::cout << "ostateczne sprawdzenie kart" << std::endl;
		std::cout << "odslaniam cards" << std::endl;
		//			cards[i + 5].settings(sCard[opponent.getCard(i)], 445 + i * 80, 220);
		//		}
		checkCards(player, opponent, "AI", coMasz, figura1, figura2, MOC_KART, cardsDoWymiany);
		aiKarty = coMasz;
		aiMoc = MOC_KART;
		aifig1 = figura1;
		aifig2 = figura2;
		checkCards(player, opponent, "TY", coMasz, figura1, figura2, MOC_KART, cardsDoWymiany);
		mojeKarty = coMasz;
		tyMoc = MOC_KART;
		mfig1 = figura1;
		mfig2 = figura2;
		std::cout << "Wyniki to:" << std::endl;
		std::cout << "AI:" << aiKarty << " moc=" << aiMoc << "  figury:" << aifig1 << "  " << aifig2 << std::endl;
		std::cout << "Ty:" << mojeKarty << " moc=" << tyMoc << "  figury:" << mfig1 << "  " << mfig2 << std::endl;

		if (aiMoc > tyMoc) {
			std::cout << "KOMPUTER WYGRAL\n";
			return Draw_Defeat;
		}
		else if (aiMoc == tyMoc) {
			if (aiMoc == 3) {
				// dwie pary, wiec dwie figury do sprawdzenia
				aifig1 = aifig1 + aifig2;
				mfig1 = mfig1 + mfig2;
			}
			if (aifig1 > mfig1) {
				std::cout << "KOMPUTER WYGRAL\n";
				return Draw_Defeat;
			}
			else if (aifig1 == mfig1) {
				std::cout << "REMIS\n";
				return Draw_Score;
			}
			else if (aifig1 < mfig1) {
				std::cout << "WYGRALES FARCIARZU\n";
				return Draw_Win;
			}

		}
		else if (aiMoc < tyMoc) {
			std::cout << "WYGRALES FARCIARZU\n";
			return Draw_Win;
		}
	}

	return 0;		// 0 - brak reakcji
}
void checkCards(Player& player, Opponent& opponent, std::string kto, std::string& coMasz, int& figura1, int& figura2, int& MOC_KART, int* cardsDoWymiany) {
	int delta = 13;		// przesuni�cie w kartach, figurach
	int jakieKarty[13];		// jakie cards w jakich ilosciach wystepuja
	coMasz = "nic";
	figura1 = figura2 = -1;

	// wpisz do tablicy po ile razy wystepuja konkretne cards
	for (int i = 0; i < 13; i++) {
		jakieKarty[i] = 0;
		for (int j = 0; j < 5; j++) {
			if (kto == "AI") {
				if (opponent.getCard(j) == i || opponent.getCard(j) == i + delta || opponent.getCard(j) == i + (delta * 2) || opponent.getCard(j) == i + (delta * 3)) {
					jakieKarty[i]++;
				}
			}
			else if (kto == "TY")
			{
				if (player.getCard(j) == i || player.getCard(j) == i + delta || player.getCard(j) == i + (delta * 2) || player.getCard(j) == i + (delta * 3)) {
					jakieKarty[i]++;
				}
			}
		}
	}

	int temp[5];		// zmniejsza talie do 12 figura, bez kolor�w
	for (int i = 0; i < 5; i++) {
		if (kto == "AI") {
			temp[i] = opponent.getCard(i);
		}
		else if (kto == "TY")
		{
			temp[i] = player.getCard(i);
		}
		if (temp[i] > 12 && temp[i] < 26)		temp[i] -= delta;
		if (temp[i] >= 26 && temp[i] < 39)	temp[i] -= delta * 2;
		if (temp[i] >= 39)					temp[i] -= delta * 3;
	}


	// i sprawdz czy powtarzaj� si� figury (nie dzia�a na strit, trzeba osobno rozpisac)
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			if (i != j && temp[i] == temp[j])
				cardsDoWymiany[i] = 0;
		}
	}

	//	for (int i = 0; i < 5; i++) {
	//		std::cout << "karta =" << temp[i] << std::endl;
	//		std::cout << i << "=" << cardsDoWymiany[i] << std::endl;
	//	}

		// sprawdz czy s� pary
	for (int i = 0; i < 13; i++) {
		if (jakieKarty[i] == 2) {
			if (coMasz == "para") {
				coMasz = "dwie pary";
				figura2 = i;
				MOC_KART = 3;
			}
			if (coMasz == "nic") {
				coMasz = "para";
				figura1 = i;
				MOC_KART = 1;
			}
			//			std::cout << "para: " << i << std::endl;
		}
	}
	// czy s� tr�jki
	for (int i = 0; i < 13; i++) {
		if (jakieKarty[i] == 3) {
			//			std::cout << "trojka: " << i << std::endl;
			if (coMasz == "nic") {
				coMasz = "trojka";
				figura1 = i;
				MOC_KART = 5;
			}
			if (coMasz == "para") {
				coMasz = "full";
				figura2 = i;
				MOC_KART = 8;
			}

		}
	}
	// a mo�e czw�reczka
	for (int i = 0; i < 13; i++) {
		if (jakieKarty[i] == 4) {
			//			std::cout << "czworka: " << i << std::endl;
			coMasz = "czworka";
			figura1 = i;
			MOC_KART = 10;
		}
	}

	// a mo�e masz strit, lub prawie strit
	std::sort(temp, temp + 5, std::greater < int >());		// posotuj tablice malejaco

															// jezeli cards koncza si� na asie to zwieksz wartosc najmniejszych
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

	// sprawdz czy masz strita
	int ileWStrit = 0;
	if (coMasz == "nic") {
		for (int i = 0; i < 4; i++) {
			int t1 = temp[i];
			int t2 = temp[i + 1];
			if (t1 - t2 == 1) ileWStrit++;
		}
		int t1 = temp[4];		// sprawdz ostatnia kart� z pierwsz�
		int t2 = temp[0];
		if (t1 - t2 == -12) ileWStrit++;		// 2 -> As - dlatego r�nica 12
		if (ileWStrit == 4) {
			coMasz = "strit";
			MOC_KART = 6;
			for (int i = 0; i < 5; i++) {
				cardsDoWymiany[i] = 0;
			}
		}
		if (ileWStrit == 3) {
			coMasz = "prawie strit";
			// igonrujemy prawie strita, i wymieniamy losowo 4 cards, ale nie rzucamy ich w pierwszej rundzie
		}

	}


}
void mixCards(int* talia, Player& player, Opponent& opponent) {
	int temp[52];	// tablica wskazuj�ca czy liczba by�a ju� wylosowana (wtedy =-1)
	for (int i = 0; i < 52; i++)	temp[i] = i;

	// mieszaj tablic�
	int n;
	for (int i = 0; i < 52; i++) {
		bool jestOk = false;
		do
		{	// losuj liczb� dop�ki si� nie powtarza
			n = (rand() % 52);
			if (temp[n] != -1) {
				jestOk = true;
				temp[n] = -1;
				talia[i] = n;
			}
		} while (jestOk == false);
	}

	//	talia[5] = 10;
	//	talia[6] = 11;
	//	talia[7] = 12;
	//	talia[8] = 13;
	//	talia[9] = 14;

	playSound("images/cardFan1.wav");

	// rozdaj cards
	for (int i = 0; i < 5; i++) {
		player.setCard(i, talia[i]);
		opponent.setCard(i, talia[i + 5]);
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