#ifndef OBSTACLEOBJECT_H
#define OSTACLEOBJECT_H
#include <SFML/Graphics.hpp>

namespace obstacle {
	struct Obstacle {
		sf::RectangleShape top_rect {};
	 	sf::RectangleShape bot_rect {};
	};
	void initClock();
	void initObstacles();
	void computeObstacle();
	void renderObstacles(sf::RenderWindow& window, sf::Time deltaTime);
	//void testObstacle();
}
#endif