#ifndef OBSTACLEOBJECT_H
#define OSTACLEOBJECT_H

#ifndef SFML
#define SFML
#include <SFML/Graphics.hpp>
#endif
namespace obstacle {
	struct Obstacle {
		sf::RectangleShape top_rect {};
	 	sf::RectangleShape bot_rect {};
		float position {};
	};
	Obstacle generateObstacle(int center, int radius);
	void renderObstacles(sf::RenderWindow& window, sf::Time deltaTime);
	void addObstacle(const Obstacle& ob);
}
#endif