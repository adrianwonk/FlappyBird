#include "ObstacleObject.h"
#include "main.h"
#include <queue>
using namespace obstacle;

namespace obstacle {
	// const float buffer;
	const float x_coord = 4;
	const float width = 20;
	const float speed = 10.f;
	const float destroy_x_coord = -20.f;
	std::queue<Obstacle> obstacles;
	
	Obstacle generateObstacle(int center, int radius){
		int topLength = center - radius;
		int botLength = SCREEN_HEIGHT - center - radius;
		
		if (topLength < buffer || botLength < buffer){
			throw "OBSTACLE: top or bottom length smaller than buffer! center: " + str(center) + ", radius: " + str(radius);
		}
		
		sf::RectangleShape topShape{width, topLength};
		topShape.setPosition({x_coord, 0});
		
		sf::RectangleShape botShape{width, botLength};
		botShape.setPosition({x_coord, center + radius});
		
		return {topShape, botShape, x_coord};
	}
	
	void addObstacle(const Obstacle& ob){
		obstacles.push(ob);
	}
	
	void renderObstacles(const sf::RenderWindow& window, sf::Time deltaTime){
		for (Obstacle ob: obstacles){
			window.draw(ob.top_rect);
			window.draw(ob.bot_rect);
			ob.top_rect -= speed * deltaTime;
			ob.bot_rect -= speed * deltaTime;
			ob.position -= speed * deltTime;
			if (ob.position <= destroy_x_coord) {
				obstacles.pop();
			}
		}
	}
	
	
}