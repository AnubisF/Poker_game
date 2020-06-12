#pragma once

#include "Animation.h"
using namespace sf;


class Object
{
protected:
	float x, y;
	std::string id;
	bool onTheTable;

public:
	Animation anim;
	sf::Sprite getSprite() {
		return anim.getSprite();
	}

	bool getLife() { return onTheTable; }
	Animation getAnim() { return anim; }
	std::string getName() { return id; }

	void setX(float newValue) { x = newValue; }
	void setY(float newValue) { y = newValue; }
	void setAnim(Animation newValue) { anim = newValue; }
	void setLife(bool newValue) { onTheTable = newValue; }

	Object()
	{
		onTheTable = true;
	}

	void settings(Animation & a, int X, int Y)
	{
		anim = a;
		x = X; y = Y;
	}

	virtual void update() {};

	void draw(RenderWindow& app)
	{
		anim.sprite.setPosition(x, y);
		app.draw(anim.sprite);
	}

	virtual ~Object() {};
};
