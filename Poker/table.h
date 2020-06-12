#pragma once
#include <SFML/Graphics.hpp>
#include "Graphics.h"
#include "Object.h"

class Table
{
	// zawiera menu, plansze koncowe i pauze
private:
	sf::Texture gameOver, pause, menu, textTlo, textPanel, textChips;
	sf::Sprite tlo, panel, chips[5];
	sf::Font font;
	sf::Text text;

	int PlayerRate;
	int OponentRate;
	int Bank;
	int rate;
	int PlayerCash;
	int OpponentCash;

public:
	Table();
	~Table();
	void pause(sf::RenderWindow&);
	void gameOver(sf::RenderWindow&);

	void youwon(sf::RenderWindow&);
	void youlose(sf::RenderWindow&);
	void draw(sf::RenderWindow&);

	void ostatnieDodanieDoStawki(int);
	int getRateAI() { return OponentRate; }
	int getPlayerRate() { return PlayerRate; }
	void wyrownajStawki();
	void dodajDoStawki(int);
	void AI_podbijaStawke(sf::RenderWindow&, int);
	bool AI_wyrownujeStawke();
	void AI_RzucilKarty(sf::RenderWindow&);
	void rzucKarty(sf::RenderWindow&);
	void noweRozdanie();
	void rysujPlansze(sf::RenderWindow&);
	// funkcje odpowiedzialne za czcionki i napisy
	bool setCzcionka(sf::String, int, sf::Color);
	void napisz(sf::RenderWindow&, sf::Vector2f, sf::String);
	void playSound(std::string);
	void czekaj(int htmsec) {
		sf::Clock clockTemp;
		clockTemp.restart();
		do {		// odczekaj czas
		} while (clockTemp.getElapsedTime().asMilliseconds() < htmsec);
	}
};