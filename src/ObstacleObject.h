#ifndef OBSTACLEOBJECT_H
#define OBSTACLEOBJECT_H
#include <tuple>
#include <SFML/Graphics.hpp>

namespace obstacle {
	struct Obstacle {
		sf::RectangleShape top_rect {};
	 	sf::RectangleShape bot_rect {};
		const float topHeight;
		const float botHeight;
		bool scored;
	};
	void initObstacles();
	void maybeInstantiateObstacle();
	std::tuple<bool, Obstacle*> iterateObstacleQueue(sf::RenderWindow& window, const sf::Time& deltaTime, const float radius, const sf::Vector2f& playerPosition);
	bool collidedPlayer(Obstacle& ob, float radius, const sf::Vector2f& center);
}
#endif
