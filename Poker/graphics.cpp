#include "stdafx.h"
#include "Graphics.h"
#include <iostream>

using namespace std;

Table::Table()
{
}


Table::~Table()
{
}

void Table::wait(int htmsec) {
	sf::Clock clockTemp;
	clockTemp.restart();
	do {
	} while (clockTemp.getElapsedTime().asMilliseconds() < htmsec);
}

void Table::setZone(int whichZone, int x0, int y0, int x1, int y1) {
	zone[whichZone].x0 = x0;
	zone[whichZone].y0 = y0;
	zone[whichZone].x1 = x1;
	zone[whichZone].y1 = y1;
	zone[whichZone].active = true;
}

int Table::checkZones(sf::RenderWindow& app) {
	int whichZone = -1;
	sf::Vector2f MousePosition;
	MousePosition = sf::Vector2f(sf::Mouse::getPosition(app).x, sf::Mouse::getPosition(app).y);

	for (int i = 0; i < 50; i++) {
		if (MousePosition.x > zone[i].x0 && MousePosition.x < zone[i].x1 && MousePosition.y > zone[i].y0 && MousePosition.y < zone[i].y1 && (sf::Mouse::isButtonPressed(sf::Mouse::Left) == true)) {
			return i;
		}
		if (MousePosition.x > zone[i].x0 && MousePosition.x < zone[i].x1 && MousePosition.y > zone[i].y0 && MousePosition.y < zone[i].y1 && (sf::Mouse::isButtonPressed(sf::Mouse::Right) == true)) {
			return i + 100;
		}

	}
	return -1;
}