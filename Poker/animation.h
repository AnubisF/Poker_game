#pragma once
#include <SFML/Graphics.hpp>


using namespace sf;

class Animation
{
public:
	float Frame, speed;
	sf::Sprite sprite;
	std::vector<sf::IntRect> frames;

	Animation() {}

	Animation(sf::Texture& t, int x, int y, int w, int h, int count, int levelW, float Speed)
	{
		Frame = 0;
		speed = Speed;
		int WX = 0;
		int WY = 0;

		for (int i = 0; i < count; i++) {
			frames.push_back(sf::IntRect(x + WX * w, WY + y, w, h));
			WX++;
			if (WX == levelW && levelW != 0)
			{
				WX = 0;
				WY += h;
			}
		}

		sprite.setTexture(t);
		sprite.setOrigin(w / 2, h / 2);
		sprite.setTextureRect(frames[0]);
	}

	sf::Sprite getSprite() { return sprite; }
	void reduceSprite() {
		sprite.scale(0.5, 0.5);
	}
	void update()
	{
		Frame += speed;
		int n = frames.size();
		if (Frame >= n) Frame -= n;
		if (n > 0) sprite.setTextureRect(frames[int(Frame)]);
	}

	bool isEnd()
	{
		return Frame + speed >= frames.size();
	}

};