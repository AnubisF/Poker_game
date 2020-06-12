#pragma once
#include "Object.h"
using namespace sf;

class Player : public Object
{
private:
	int card[5];
	bool RaisedCard[5];
	int  FourCardsRaised;

public:


	Player() {
		id = "player";
		for (int i = 0; i < 5; i++)	RaisedCard[i] = false;
		 FourCardsRaised = 0;
	}
	~Player() {};

	bool CheckIsRaised(int whichCard) { return RaisedCard[whichCard]; }

	void changeRaised(int whichCard) {
		if (RaisedCard[whichCard] == false) {
			RaisedCard[whichCard] = true;
			 FourCardsRaised++;
			std::cout <<  FourCardsRaised << std::endl;
		}
		else
		{
			RaisedCard[whichCard] = false;
			 FourCardsRaised--;
			std::cout <<  FourCardsRaised << std::endl;
		}

	}

	bool FourSelectedCards() {
		if ( FourCardsRaised == 5)	return true;
		else return false;
	}

	void resetSelectedCards() {
		 FourCardsRaised = 0;
		for (int i = 0; i < 5; i++)	RaisedCard[i] = false;
	}


	void resetCard(int whichCard) {
		 FourCardsRaised--;
		RaisedCard[whichCard] = false;
	}

	int getCard(int whichCard) { return card[whichCard]; }
	void setKarta(int whichCard, int whatCard) { card[whichCard] = whatCard; }
	void update()
	{
	}

};