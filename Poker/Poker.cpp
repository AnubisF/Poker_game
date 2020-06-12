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
#include "Graphics.h"

void mixCards(int*, Player&, Opponent&);
void playSound(std::string);
int ai(sf::RenderWindow&, Player&, Opponent&, Table*, Cards* cards, Animation*, int*, int, sf::String&);
void rysujWszystko(sf::RenderWindow&, std::list<Object*>, Table*);
void checkCards(Player&, Opponent&, std::string, std::string&, int&, int&, int&, int*);
void delay(int);

using namespace sf;

const int SzerokoscEkranu = 1207;
const int WysokoscEkranu = 831;
const int AI_RzucKarty = -10;
const int AI_WymienKarty = -20;
const int AI_PodbilStawke = -30;
const int WYNIK_WYGRANA = -10;
const int WYNIK_PRZEGRANA = -20;
const int WYNIK_REMIS = -30;


int main()
{
	srand(time(0));
	std::list<Object*> objects;


	RenderWindow app(VideoMode(SzerokoscEkranu, WysokoscEkranu), "SFML JNXEngine v1.0");
	app.setFramerateLimit(60);

	Table* table = new Table();
	table->setFont("BornAddict.ttf", 24, sf::Color::White);

	Texture t2, tKarty, tKartaTyl;
	t2.loadFromFile("images/opponent.png");
	tKarty.loadFromFile("images/playingCards.png");
	tKartaTyl.loadFromFile("images/playingCardBacks.png");

	Animation sOpponent(t2, 170, 0, 79, 110, 3, 0, 0.05);
	Animation sOpponentWin(t2, 0, 120, 140, 132, 8, 4, 0.05);
	Animation sOpponentPrzegrana(t2, 0, 390, 176, 110, 3, 0, 0.02);

	int pozycjeKart[120] = {		// pozycje sprajtów w pliku
		700 ,380,280 ,950,280 ,760,280 ,570,280 ,380,280 ,190,280 ,0,140 ,1710,140 ,1520,140 ,1140,140 ,760,140 ,950,140 ,1330,420 ,1710,420 ,1520,420 ,1330,420 ,1140,420 ,950,420 ,760,420 ,570,420 ,380,420 ,190,280 ,1710,280 ,1330,280 ,1520,420 ,0,140 ,380,140 ,190,140 ,0,0 ,1710,0 ,1520,0 ,1330,0 ,1140,0 ,950,0 ,760,0 ,380,0 ,0,0 ,190,0 ,570,280 ,1140,700 ,190,700 ,0,560 ,1710,560 ,1520,560 ,1330,560 ,1140,560 ,950,560 ,760,560 ,380,560 ,0,560 ,190,560 ,570
	};
	Animation sKarta[60];		//sprity kart 0-51 - talia, 52 ty³ karty
	for (int nr = 0; nr < 52; nr++) {
		Animation tempAnim(tKarty, pozycjeKart[nr * 2], pozycjeKart[nr * 2 + 1], 140, 190, 1, 0, 0);
		tempAnim.reduceSprite();
		sKarta[nr] = tempAnim;
	}
	Animation tempAnim(tKartaTyl, 0, 0, 140, 190, 1, 0, 0);		// ty³ karty
	tempAnim.reduceSprite();
	sKarta[52] = tempAnim;


	Opponent opponent;
	opponent.settings(sOpponent, 480, 100);
	objects.push_back(&opponent);

	Player player;

	Karta karty[20];		// karty do wyœwietlenia, 0-4 moje, 5-9 ai,
	int talia[52];			// talia z numerami kart, 0-4 moje, 5-9 ai, 10+ do dobrania 

	Graphics myGraphics;		// strefy do klikania myszk¹
	myGraphics.setZone(1, 265, 645, 395, 695);			// rzuæ karty
	myGraphics.setZone(2, 405, 645, 620, 695);			// sprawdŸ / pass
	myGraphics.setZone(3, 630, 645, 780, 695);			// wymieñ karty
	myGraphics.setZone(4, 790, 645, 930, 695);			// nowe rozdanie

	myGraphics.setZone(8, 410, 320, 475, 410);		// karty
	myGraphics.setZone(9, 490, 320, 560, 410);
	myGraphics.setZone(10, 570, 320, 640, 410);
	myGraphics.setZone(11, 650, 320, 720, 410);
	myGraphics.setZone(12, 730, 320, 800, 410);

	myGraphics.setZone(13, 450, 500, 515, 565);		// ¿etony
	myGraphics.setZone(14, 530, 500, 595, 565);
	myGraphics.setZone(15, 610, 500, 675, 565);
	myGraphics.setZone(16, 690, 500, 755, 565);

	int selectedZone;	// which zone was selected (clicked)
	int ktoraRunda = 0;
	// =0 - start gry, rozdanie kart
	// =1 - mozna rzucic karty, podniesc stawke, check (drop, rise, pass)
	// =11 (ai) - ai sprawdza czy: drop/rise/pass 
	// =2 - mozna rzucic karty, wymienic karty,  check, trzeba wyrownac stawke (drop, rise, change, pass)
	// =22 - ai: to co player wyzej poza wyrownaniem stawki (player wyrownuje i nie moze juz wiecej teraz postawic)
	// =2a teraz nastêpuje wymiana kart
	// =3 - mozna rzucic karty, podniesc stawke, sprawdzic (drop, rise, check)
	// =33 - ai: drop/wyrownaj/check

	int AI_reakcja = 0;
	sf::String wiadomosc = "";

	while (app.isOpen())
	{
		Event event;
		while (app.pollEvent(event))
		{
			if (event.type == Event::Closed)		app.close();
		}

		if (Keyboard::isKeyPressed(Keyboard::Escape))		app.close();
		if (Keyboard::isKeyPressed(Keyboard::P))			table->pause(app);
		if (Keyboard::isKeyPressed(Keyboard::G))			table->gameOver(app);

		selectedZone = myGraphics.checkZones(app);
		if (selectedZone == 1 && whatRound != 5) {	// rzucasz karty
			// ods³oñ karty opponenta
			for (int i = 0; i < 5; i++) {
				karty[i + 5].settings(sKarta[opponent.getCard(i)], 445 + i * 80, 220);
				objects.push_back(&karty[i + 5]);
				rysujWszystko(app, objects, table);
			}
			playSound("images/you_lose.ogg");
			table->rzucKarty(app);
			opponent.settings(sOpponentWin, 480, 100);
			objects.push_back(&opponent);
			rysujWszystko(app, objects, table);
			myGraphics.delay(200);
			whatRound = 5;
		}
		if (selectedZone == 2 && whatRound == 1) {		// pierwsze sprawdzenie kart
			AI_reakcja = ai(app, player, opponent, table, karty, sKarta, talia, whatRound, wiadomosc);
			if (AI_reakcja == AI_RzucKarty) {
				// ods³oñ karty opponenta
				for (int i = 0; i < 5; i++) {
					karty[i + 5].settings(sKarta[opponent.getCard(i)], 445 + i * 80, 220);
					objects.push_back(&karty[i + 5]);
					rysujWszystko(app, objects, table);
				}
				opponent.settings(sOpponentPrzegrana, 480, 100);
				objects.push_back(&opponent);
				whatRound = 5;
				playSound("images/you_win.ogg");
				for (int i = 0; i < 25; i++)
					rysujWszystko(app, objects, table);
				table->AI_RzucilKarty(app);
			}
			else
			{
				whatRound = 2;
			}
		}

		//		sprawdz opponenta, nastepna runda
		if (selectedZone == 2 && AI_reakcja != AI_PodbilStawke && (whatRound == 2 || whatRound == 3)) {
			AI_reakcja = ai(app, player, opponent, table, karty, sKarta, talia, 2, wiadomosc);
			if (AI_reakcja == AI_RzucKarty) {
				// ods³oñ karty opponenta
				for (int i = 0; i < 5; i++) {
					karty[i + 5].settings(sKarta[opponent.getCard(i)], 445 + i * 80, 220);
					objects.push_back(&karty[i + 5]);
					rysujWszystko(app, objects, table);
				}
				opponent.settings(sOpponentPrzegrana, 480, 100);
				objects.push_back(&opponent);
				whatRound = 5;
				playSound("images/you_win.ogg");
				for (int i = 0; i < 25; i++)
					rysujWszystko(app, objects, table);
				table->AI_RzucilKarty(app);
			}
			else
			{
				whatRound = 4;
			}
		}

		if (wiadomosc.getSize() > 5 && whatRound == 2 && AI_reakcja != AI_PodbilStawke) {
			table->napisz(app, sf::Vector2f(370, 700), wiadomosc);
			app.display();
			delay(2200);
			wiadomosc = "";
		}

		if (selectedZone == 2 && AI_reakcja != AI_PodbilStawke && whatRound == 4) {		// ostatnie sprawdzenie kart

			// ods³oñ karty opponenta
			for (int i = 0; i < 5; i++) {
				karty[i + 5].settings(sKarta[opponent.getCard(i)], 445 + i * 80, 220);
				objects.push_back(&karty[i + 5]);
				rysujWszystko(app, objects, table);
			}
			AI_reakcja = ai(app, player, opponent, table, karty, sKarta, talia, 3, wiadomosc);
			if (AI_reakcja == WYNIK_WYGRANA) {
				playSound("images/you_win.ogg");
				opponent.settings(sOpponentPrzegrana, 480, 100);
				objects.push_back(&opponent);
				rysujWszystko(app, objects, table);
				table->wygrales(app);
			}
			else if (AI_reakcja == WYNIK_REMIS) {
				table->remis(app);
			}
			else if (AI_reakcja == WYNIK_PRZEGRANA) {
				playSound("images/you_lose.ogg");
				opponent.settings(sOpponentWin, 480, 100);
				objects.push_back(&opponent);
				rysujWszystko(app, objects, table);
				table->przegrales(app);
			}

			whatRound = 5;
		}

		if (selectedZone == 3 && whatRound == 2 && AI_reakcja != AI_PodbilStawke) {		// wymien karty
			for (int i = 0; i < 5; i++) {
				if (player.CheckIsRaised(i) == true) {
					player.setKarta(i, talia[10 + i]);
					karty[i].settings(sKarta[player.getCard(i)], 445 + i * 80, 364);
					player.resetKarta(i);
				}
			}
			whatRound = 3;
		}
		if ((selectedZone == 4 && whatRound == 5) || whatRound == 0) {		// nowe rozdanie
			system("cls");
			opponent.settings(sOpponent, 480, 100);
			objects.push_back(&opponent);
			mixCards(talia, player, opponent);
			for (int i = 0; i < 5; i++) {
				karty[i].settings(sKarta[player.getCard(i)], 445 + i * 80, 364);
				//				karty[i+5].settings(sKarta[opponent.getCard(i)], 445 + i * 80, 220);
				karty[i + 5].settings(sKarta[52], 445 + i * 80, 220);
				objects.push_back(&karty[i]);
				objects.push_back(&karty[i + 5]);
			}
			table->noweRozdanie();
			whatRound = 1;
		}
		if (selectedZone >= 8 && selectedZone <= 12 && whatRound == 2) {
			player.changeRaised(selectedZone - 8);
			if (player.czyCzteryKartyWybrane() == true) {
				player.changeRaised(selectedZone - 8);
			}
			else
			{
				playSound("images/cardSlide1.wav");
				if (player.CheckIsRaised(selectedZone - 8) == true) {
					karty[selectedZone - 8].setY(350);
				}
				else if (player.CheckIsRaised(selectedZone - 8) == false)
				{
					karty[selectedZone - 8].setY(364);
				}
			}
		}
		if (selectedZone == 13 && (ktoraRunda == 2 || ktoraRunda == 1 || ktoraRunda == 3))	table->addToRate(5);
		if (selectedZone == 14 && (ktoraRunda == 2 || ktoraRunda == 1 || ktoraRunda == 3))	table->addToRate(10);
		if (selectedZone == 15 && (ktoraRunda == 2 || ktoraRunda == 1 || ktoraRunda == 3))	table->addToRate(20);
		if (selectedZone == 16 && (ktoraRunda == 2 || ktoraRunda == 1 || ktoraRunda == 3))	table->addToRate(50);
		if (selectedZone == 13 && ktoraRunda == 4)	table->addedToRate(5);
		if (selectedZone == 14 && ktoraRunda == 4)	table->addedToRate(10);
		if (selectedZone == 15 && ktoraRunda == 4)	table->addedToRate(20);
		if (selectedZone == 16 && ktoraRunda == 4)	table->addedToRate(50);
		if (AI_reakcja == AI_PodbilStawke) {
			if (table->getRateAI() == table->getPlayerRate()) {
				AI_reakcja = 0;
			}
			if (table->getRateAI() < table->getPlayerRate()) {
				AI_reakcja = 0;
				table->raiseRate();
			}
		}
		rysujWszystko(app, objects, table);
		myGraphics.delay(40);
	}
	return 0;
}
void rysujWszystko(sf::RenderWindow& app, std::list<Object*> objects, Table* table) {
	for (auto i = objects.begin(); i != objects.end();)
	{
		Object* e = *i;
		e->update();
		e->anim.update();
		if (e->getLife() == false) 		i = objects.erase(i);
		else	i++;

	}

	// rysuj
	app.clear();
	table->rysujPlansze(app);
	for (auto i : objects)
		i->draw(app);
	app.display();
}
int ai(sf::RenderWindow& app, Player& player, Opponent& opponent, Table* table, Karta* karty, Animation* sKarta, int* talia, int runda, sf::String& wiadomosc) {
	int kartyDoWymiany[5]; // które karty wymieniæ = 0
	std::string coMasz = "nic";			// tutaj: para, dwie pary, trojka, trojka i para
	std::string czyStrit = "nic";		// tutaj: strit, prawie strit (4 karty takie same)
	int figura1, figura2;		// numery figur ktore masz, im wyzszy numer tym wyzsza figura
	figura1 = figura2 = -1;

	int MOC_KART = 0;
	int MOC_FIGUR = 0;
	//	wiadomosc = "nic";

	for (int i = 0; i < 5; i++)	kartyDoWymiany[i] = -1;		// wyczyœæ (ai) karty do wymiany

	// sprawdz jakie karty ma opponent
	checkCards(player, opponent, "AI", coMasz, figura1, figura2, MOC_KART, kartyDoWymiany);

	// teraz juz wiadomo co ma komputer, wiêc czas na reakcje ai
	std::cout << coMasz << std::endl;

	int ileZagrac = ((rand() % 9) * 5) * MOC_KART;
	if (MOC_KART == 0)
		ileZagrac = (rand() % 7) * 5;

	// RUNDA 1
	if (runda == 1) {
		int tempk = 0;

		std::cout << "RUNDA 1" << std::endl;
		if (coMasz == "nic" && (rand() % 3) == 1)
			return AI_RzucKarty;	// nic nie ma, wiec rzucam karty
		if (coMasz == "nic" && table->getPlayerRate() > 50 && (rand() % 2) == 1)
			return AI_RzucKarty;	// nic nie ma, a zawodnik sporo postawil

		if (table->getPlayerRate() > 10) {
			// player coœ wrzuci³ na stó³ wiêc trzeba wyrównaæ
			if (table->AI_equalRate() == false) {
				// za ma³o geldów
	//			std::cout << "AI NIE wyrownuje stawki i rzuca kartami" << std::endl;
				return AI_RzucKarty;
			}
		}

		if (ileZagrac > 0)
		{

			std::cout << "AI podbija stawke o " << ileZagrac << std::endl;
			table->AI_raiseRate(app, ileZagrac);
			for (int i = 0; i < 5; i++) {
				if (kartyDoWymiany[i] == -1) {
					tempk++;
					//					std::cout << "wymieniam karty" << std::endl;
					opponent.setKarta(i, talia[15 + i]);
					//			karty[i + 5].settings(sKarta[opponent.getCard(i)], 445 + i * 80, 220);
				}
			}
			if (tempk == 5) tempk = 4;
			wiadomosc = "Komputer wymienia " + std::to_string(tempk) + " karty";
			return AI_PodbilStawke;
		}

		// wymieñ ju¿ teraz karty, na nastêpn¹ turê.
		for (int i = 0; i < 5; i++) {
			if (kartyDoWymiany[i] == -1) {
				tempk++;
				opponent.setKarta(i, talia[15 + i]);
				//			karty[i+5].settings(sKarta[opponent.getCard(i)], 445 + i * 80, 220);
			}
		}
		// ma³e oszustwo, komputer moze wymienic 5 kart, ale player siê o tym nie dowie :)
		if (tempk == 5) tempk = 4;
		wiadomosc = "Komputer wymienia " + std::to_string(tempk) + " karty";

	}

	if (runda == 2) {
		std::cout << "drugie sprawdzenie kart" << std::endl;
		// nic nie ma = rzuc karty
		if (coMasz == "nic" && (rand() % 2) == 1)
			return AI_RzucKarty;	// nic nie ma, wiec rzucam karty

		// wyrownaj stawke
		if (table->AI_equalRate() == false) {
			// za ma³o geldów
			std::cout << "runda 2 AI NIE wyrownuje stawki i rzuca kartami" << std::endl;
			return AI_RzucKarty;
		}

		// w zaleznosc od kart podbij stawke
		std::cout << "ile zagrac =" << ileZagrac << std::endl;
		if (ileZagrac > 0)
		{
			std::cout << "runda2 AI podbija stawke o " << ileZagrac << std::endl;
			table->AI_raiseRate(app, ileZagrac);
			return AI_PodbilStawke;
		}

	}

	if (runda == 3) {
		std::string mojeKarty = "nic";
		std::string aiKarty = "nic";
		int mfig1, mfig2, aifig1, aifig2;
		int aiMoc, tyMoc;

		//		for (int i = 0; i < 5; i++) {
		std::cout << "ostateczne sprawdzenie kart" << std::endl;
		std::cout << "odslaniam karty" << std::endl;
		//			karty[i + 5].settings(sKarta[opponent.getCard(i)], 445 + i * 80, 220);
		//		}
		checkCards(player, opponent, "AI", coMasz, figura1, figura2, MOC_KART, kartyDoWymiany);
		aiKarty = coMasz;
		aiMoc = MOC_KART;
		aifig1 = figura1;
		aifig2 = figura2;
		checkCards(player, opponent, "TY", coMasz, figura1, figura2, MOC_KART, kartyDoWymiany);
		mojeKarty = coMasz;
		tyMoc = MOC_KART;
		mfig1 = figura1;
		mfig2 = figura2;
		std::cout << "Wyniki to:" << std::endl;
		std::cout << "AI:" << aiKarty << " moc=" << aiMoc << "  figury:" << aifig1 << "  " << aifig2 << std::endl;
		std::cout << "Ty:" << mojeKarty << " moc=" << tyMoc << "  figury:" << mfig1 << "  " << mfig2 << std::endl;

		if (aiMoc > tyMoc) {
			std::cout << "KOMPUTER WYGRAL\n";
			return WYNIK_PRZEGRANA;
		}
		else if (aiMoc == tyMoc) {
			if (aiMoc == 3) {
				// dwie pary, wiec dwie figury do sprawdzenia
				aifig1 = aifig1 + aifig2;
				mfig1 = mfig1 + mfig2;
			}
			if (aifig1 > mfig1) {
				std::cout << "KOMPUTER WYGRAL\n";
				return WYNIK_PRZEGRANA;
			}
			else if (aifig1 == mfig1) {
				std::cout << "REMIS\n";
				return WYNIK_REMIS;
			}
			else if (aifig1 < mfig1) {
				std::cout << "WYGRALES FARCIARZU\n";
				return WYNIK_WYGRANA;
			}

		}
		else if (aiMoc < tyMoc) {
			std::cout << "WYGRALES FARCIARZU\n";
			return WYNIK_WYGRANA;
		}
	}

	return 0;		// 0 - brak reakcji
}
void checkCards(Player& player, Opponent& opponent, std::string kto, std::string& coMasz, int& figura1, int& figura2, int& MOC_KART, int* kartyDoWymiany) {
	int delta = 13;		// przesuniêcie w kartach, figurach
	int jakieKarty[13];		// jakie karty w jakich ilosciach wystepuja
	coMasz = "nic";
	figura1 = figura2 = -1;

	// wpisz do tablicy po ile razy wystepuja konkretne karty
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

	int temp[5];		// zmniejsza talie do 12 figura, bez kolorów
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


	// i sprawdz czy powtarzaj¹ siê figury (nie dzia³a na strit, trzeba osobno rozpisac)
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			if (i != j && temp[i] == temp[j])
				kartyDoWymiany[i] = 0;
		}
	}

	//	for (int i = 0; i < 5; i++) {
	//		std::cout << "karta =" << temp[i] << std::endl;
	//		std::cout << i << "=" << kartyDoWymiany[i] << std::endl;
	//	}

		// sprawdz czy s¹ pary
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
	// czy s¹ trójki
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
	// a mo¿e czwóreczka
	for (int i = 0; i < 13; i++) {
		if (jakieKarty[i] == 4) {
			//			std::cout << "czworka: " << i << std::endl;
			coMasz = "czworka";
			figura1 = i;
			MOC_KART = 10;
		}
	}

	// a mo¿e masz strit, lub prawie strit
	std::sort(temp, temp + 5, std::greater < int >());		// posotuj tablice malejaco

															// jezeli karty koncza siê na asie to zwieksz wartosc najmniejszych
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
		int t1 = temp[4];		// sprawdz ostatnia kartê z pierwsz¹
		int t2 = temp[0];
		if (t1 - t2 == -12) ileWStrit++;		// 2 -> As - dlatego ró¿nica 12
		if (ileWStrit == 4) {
			coMasz = "strit";
			MOC_KART = 6;
			for (int i = 0; i < 5; i++) {
				kartyDoWymiany[i] = 0;
			}
		}
		if (ileWStrit == 3) {
			coMasz = "prawie strit";
			// igonrujemy prawie strita, i wymieniamy losowo 4 karty, ale nie rzucamy ich w pierwszej rundzie
		}

	}


}
void mixCards(int* talia, Player& player, Opponent& opponent) {
	int temp[52];	// tablica wskazuj¹ca czy liczba by³a ju¿ wylosowana (wtedy =-1)
	for (int i = 0; i < 52; i++)	temp[i] = i;

	// mieszaj tablicê
	int n;
	for (int i = 0; i < 52; i++) {
		bool jestOk = false;
		do
		{	// losuj liczbê dopóki siê nie powtarza
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

	// rozdaj karty
	for (int i = 0; i < 5; i++) {
		player.setKarta(i, talia[i]);
		opponent.setKarta(i, talia[i + 5]);
	}
	player.resetIleKartWybranych();
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