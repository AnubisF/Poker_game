#pragma once
#include "Object.h"

class Opponent : public Object
{
private:
	int cards[5];

public:
	Opponent() {
		id = "opponent";
	}
	~Opponent() {};
	void update() {};
	int getCard(int whichCard) { return cards[whichCard]; }
	void setCard(int whichCard, int whatCard) { cards[whichCard] = whatCard; }
};
