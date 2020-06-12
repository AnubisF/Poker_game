#pragma once
#include "Object.h"

class Cards : public Object
{
private:
	int x, y;

public:
	Cards() { id = "Cards"; }
	~Cards() {};
	void update() {};

};
