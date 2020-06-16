#include "stdafx.h"
#include "Table.h"
#include <iostream>
#include "SFML/Audio.hpp"

using namespace std;

Table::Table()
{
	Bank = Rate = PlayerRate = OponentRate = 0;
	PlayerCash = OpponentCash = 1000;

	gameOver.loadFromFile("images/GameOver.png");
	pause.loadFromFile("images/Pause.png");
	menu.loadFromFile("images/menu.png");
	textTlo.loadFromFile("images/background.png");
	tlo.setTexture(textTlo);
	textPanel.loadFromFile("images/panel.png");
	panel.setTexture(textPanel);
	panel.setPosition(260, 640);

	textChips.loadFromFile("images/chips.png");
	chips[0].setTexture(textChips);
	chips[0].setTextureRect(sf::IntRect(0, 0, 68, 68));
	chips[0].setPosition(450, 500);
	chips[1].setTexture(textChips);
	chips[1].setTextureRect(sf::IntRect(0, 262, 68, 68));
	chips[1].setPosition(530, 500);
	chips[2].setTexture(textChips);
	chips[2].setTextureRect(sf::IntRect(0, 372, 68, 68));
	chips[2].setPosition(610, 500);
	chips[3].setTexture(textChips);
	chips[3].setTextureRect(sf::IntRect(0, 440, 68, 68));
	chips[3].setPosition(690, 500);

}

Table::~Table()
{
}

void Table::AI_raiseRate(sf::RenderWindow& app, int howMuchToAdd) {
	if (howMuchToAdd <= OpponentCash && howMuchToAdd > 0) {
		OponentRate += howMuchToAdd;
		OpponentCash -= howMuchToAdd;
		Bank = PlayerRate + OponentRate;
		sf::String title = "The opponent raises the Rate " + std::to_string(howMuchToAdd);
		write(app, sf::Vector2f(370, 700), title);
		app.display();
		delay(1500);
	}
}

void Table::raiseRate() {
	// Need to calculate the difference and subtract from the bank
	int difference = PlayerRate - OponentRate;
	//	std::cout << "difference = " << difference << "\n";
	PlayerRate = OponentRate;
	//	std::cout << "difference = " << PlayerRate<< "\n";
	//	std::cout << "difference = " << OponentRate << "\n";
	Bank = PlayerRate + OponentRate;
	PlayerCash += difference;
}
void Table::addedToRate(int howMuchToAdd) {
	PlayerRate += howMuchToAdd;
	PlayerCash -= howMuchToAdd;
	int difference = PlayerRate - OponentRate;
	if (PlayerRate > OponentRate) {
		PlayerRate = OponentRate;
		PlayerCash += difference;
	}
	playSound("images/dieShuffle1.wav");
}
bool Table::AI_equalRate() {
	int howMuchToAdd = PlayerRate - OponentRate;
	std::cout << "How much to add =" << howMuchToAdd << std::endl;
	if (howMuchToAdd <= OpponentCash && howMuchToAdd >= 0) {
		//		OponentRate += howMuchToAdd;
		OponentRate = PlayerRate;
		OpponentCash -= howMuchToAdd;
		Bank = PlayerRate + OponentRate;
		return true;
	}

	return false;
}
void Table::addToRate(int howMuchToAdd) {
	if (howMuchToAdd <= PlayerCash) {
		playSound("images/dieShuffle1.wav");
		PlayerRate += howMuchToAdd;
		PlayerCash -= howMuchToAdd;
	}
	Bank = PlayerRate + OponentRate;
}
void Table::newDealCards() {
	PlayerRate = OponentRate = 10;
	PlayerCash -= 10;
	OpponentCash -= 10;
	Bank = PlayerRate + OponentRate;
}
void Table::dropCards(sf::RenderWindow& app) {
	Bank = PlayerRate + OponentRate;
	OpponentCash += Bank;
	sf::String title = "You threw the cards. Opponent wins " + to_string(Bank);
	write(app, sf::Vector2f(370, 700), title);
	app.display();
	//	delay(2500);
}
void Table::AI_RzucilKarty(sf::RenderWindow& app) {
	Bank = PlayerRate + OponentRate;
	PlayerCash += Bank;
	sf::String title = "The opponent threw the cards. You won " + to_string(Bank);
	write(app, sf::Vector2f(370, 700), title);
	app.display();
	delay(500);
}
void Table::drawTable(sf::RenderWindow& app) {
	app.draw(tlo);
	app.draw(panel);
	app.draw(chips[0]);
	app.draw(chips[1]);
	app.draw(chips[2]);
	app.draw(chips[3]);
	write(app, sf::Vector2f(475, 520), "5");
	write(app, sf::Vector2f(555, 520), "10");
	write(app, sf::Vector2f(630, 520), "20");
	write(app, sf::Vector2f(710, 520), "50");

	sf::String title;
	title = "PULA " + to_string(Bank) + "\n     Ty " + to_string(PlayerRate) + "\n      AI " + to_string(OponentRate);
	write(app, sf::Vector2f(150, 320), title);
	title = "Ty " + to_string(PlayerCash) + "\nAi " + to_string(OpponentCash);
	write(app, sf::Vector2f(900, 320), title);

	Vector2i mx = Mouse::getPosition(app);
	title = "x=" + std::to_string(mx.x) + "  y=" + std::to_string(mx.y) + "  ";
	write(app, sf::Vector2f(50, 50), title);

}
void Table::spause(sf::RenderWindow& app) {
	Util util;
	sf::Sprite temp(pause);
	temp.setPosition(250, 280);
	app.draw(temp);
	app.display();
	util.delay(200);
	do {} while (!sf::Keyboard::isKeyPressed(sf::Keyboard::P));
	util.delay(200);
}
void Table::sgameOver(sf::RenderWindow& app) {
	std::cout << "game over" << std::endl;
	Util util;
	sf::Sprite temp(gameOver);
	temp.setPosition(325, 250);
	app.draw(temp);
	app.display();
	util.delay(1500);
}
bool Table::setFont(sf::String fontname, int size, sf::Color color) {
	if (!font.loadFromFile(fontname)) {
		cout << "Failed to load font." << endl;
		return false;
	}

	// selecting a font
	text.setFont(font);
	text.setCharacterSize(size); //in pixels!!!
	text.setColor(color);
	return true;
}
void Table::write(sf::RenderWindow& window, sf::Vector2f stringPosition, sf::String string) {
	text.setPosition(stringPosition);
	text.setString(string);
	window.draw(text);
}
void Table::youwon(sf::RenderWindow& app) {
	Bank = PlayerRate + OponentRate;
	PlayerCash += Bank;
	sf::String title = "You win " + to_string(Bank);
	write(app, sf::Vector2f(370, 700), title);
	app.display();
	delay(2500);
}
void Table::youlose(sf::RenderWindow& app) {
	Bank = PlayerRate + OponentRate;
	OpponentCash += Bank;
	sf::String title = "You lose " + to_string(Bank);
	write(app, sf::Vector2f(370, 700), title);
	app.display();
	delay(2500);
}
void Table::draw(sf::RenderWindow& app) {
	Bank = PlayerRate + OponentRate;
	OpponentCash += OponentRate;
	PlayerCash += PlayerRate;
	sf::String title = "The bank returns to you " + std::to_string(PlayerRate) + "  AI " + std::to_string(OponentRate);
	write(app, sf::Vector2f(370, 700), title);
	app.display();
	delay(2500);
}

void Table::playSound(std::string plik) {
	sf::SoundBuffer buffer;
	buffer.loadFromFile(plik);
	sf::Sound sound(buffer);
	sound.play();
	while (sound.getStatus() == sf::Sound::Playing)
	{
	}
}