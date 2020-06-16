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
	int Rate;
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
	void toCall();
	void addToRate(int);
	void AI_raiseBet(sf::RenderWindow&, int);
	bool AI_toCall();
	void AI_dropCards(sf::RenderWindow&);
	void dropCards(sf::RenderWindow&);
	void newDealCards();
	void drawTable(sf::RenderWindow&);
	// functions responsible for fonts and subtitles
	bool setFont(sf::String, int, sf::Color);
	void write(sf::RenderWindow&, sf::Vector2f, sf::String);
	void playSound(std::string);
	void delay(int htmsec) {
		sf::Clock clockTemp;
		clockTemp.restart();
		do {
		} while (clockTemp.getElapsedTime().asMilliseconds() < htmsec);
	}
};