#pragma once
#include <SFML/Graphics.hpp>
#include "Util.h"
#include "Object.h"

class Table
{

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
	void spause(sf::RenderWindow&);
	void sgameOver(sf::RenderWindow&);

	void youwon(sf::RenderWindow&);
	void youlose(sf::RenderWindow&);
	void draw(sf::RenderWindow&);

	void addedToRate(int);
	int getRateAI() { return OponentRate; }
	int getPlayerRate() { return PlayerRate; }
	void raiseRate();
	void addToRate(int);
	void AI_raiseRate(sf::RenderWindow&, int);
	bool AI_equalRate();
	void AI_RzucilKarty(sf::RenderWindow&);
	void rzucKarty(sf::RenderWindow&);
	void noweRozdanie();
	void rysujPlansze(sf::RenderWindow&);
	// funkcje odpowiedzialne za czcionki i napisy
	bool setFont(sf::String, int, sf::Color);
	void napisz(sf::RenderWindow&, sf::Vector2f, sf::String);
	void playSound(std::string);
	void delay(int htmsec) {
		sf::Clock clockTemp;
		clockTemp.restart();
		do {
		} while (clockTemp.getElapsedTime().asMilliseconds() < htmsec);
	}
};