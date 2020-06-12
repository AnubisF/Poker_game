#pragma once
#include <SFML/Graphics.hpp>

class Util
{
private:
	sf::Font font;
	sf::Text text;
	sf::Clock clock;
	float TotalSecondsTime;
	struct DataZone {
		int which;
		bool active;
		int x0, y0, x1, y1;
	} zone[50];

public:
	Util();
	~Util();

	// functions responsible for time
	void restartTime() { TotalSecondsTime += clock.getElapsedTime().asSeconds(); clock.restart(); }
	sf::Int32 getMillisecondTime() { return clock.getElapsedTime().asMilliseconds(); }
	float getSecondTime() { return clock.getElapsedTime().asSeconds(); }
	float getTotalTime() { return TotalSecondsTime; }
	void delay(int);

	// functions responsible for operating the zone
	void setZone(int, int, int, int, int);
	int checkZones(sf::RenderWindow& app);
};
