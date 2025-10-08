#include "ObstacleObject.h"
#include "main.h"
#include <queue>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
#include <algorithm>
#include "PlayerObject.h"

using namespace obstacle;
int clock_initialised = 0;

namespace obstacle {
	constexpr float x_coord = SCREEN_WIDTH;
	constexpr float width = 150;
	constexpr float destroy_x_coord = -width;
	constexpr int buffer = 30;

	constexpr float speed = 600.f;

	constexpr int min_radius = 50;
	constexpr float radius_variance = 1.6f;
	const int max_radius = round(min_radius * radius_variance);
	const int max_radius_spread = max_radius - min_radius;
	
	constexpr float min_period = 0.8f;
	constexpr float period_variance = 1.6f;
	constexpr float max_period = min_period * period_variance;
	constexpr float max_period_spread = max_period - min_period;
	float curr_period = 0;
	// Create a random number generator engine
	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_real_distribution<> rand_dist(0, 1.0);
	
	Obstacle* generateObstacle(int center, int radius) {
		float topLength = center - radius;
		float botLength = SCREEN_HEIGHT - center - radius;
		try {
			if (topLength < buffer || botLength < buffer) {
				throw "OBSTACLE: top or bottom length smaller than buffer! center: " +
					std::to_string(center) + ", radius: " + std::to_string(radius);
			}
			else if (radius < min_radius) {
				throw "radius too small! center: " +
					std::to_string(center) + ", radius: " + std::to_string(radius);
			}
		}
		catch (std::string msg) {
			std::cout << msg << '\n';
			throw;
		}
		sf::RectangleShape topShape({ width, topLength });
		topShape.setPosition({ x_coord, 0 });

		sf::RectangleShape botShape({ width, botLength });
		botShape.setPosition({ x_coord, (float)(center + radius) });

		return new Obstacle { topShape, botShape, topLength, botLength, false };
	}

	auto get_random() {
		return rand_dist(generator);
	}
	

	std::queue<Obstacle*> obstacles;
	void addObstacle(int center, int radius) {
		Obstacle* ptr = generateObstacle(center, radius);
		obstacles.push(ptr);
	}

	sf::Clock period_timer;
	void renewPeriod() {
		clock_initialised = 1;
		period_timer.restart();
		curr_period = min_period + get_random() * max_period_spread;
	}

	void initObstacles() {
		while (!obstacles.empty()) {
			auto ptr = obstacles.front();
			delete ptr;
			obstacles.pop();
		}
		renewPeriod();
	}

	// creates obstacle, and adds to queue
	void maybeInstantiateObstacle() {
		try {
			if (clock_initialised == 0) {
				throw "haven't initalised clock before running computeObstacle";
			}
		}
		catch (std::string msg) {
			std::cout << msg << '\n';
			throw "";
		}
		if (period_timer.getElapsedTime().asSeconds() >= curr_period) {
			// calculate the radius and spread
			int curr_radius = min_radius + round(get_random() * max_radius_spread);
			//int curr_radius = min_radius;

			// calculate the center  
			auto center_lower_bound = buffer + curr_radius;
			auto center_upper_bound = SCREEN_HEIGHT - buffer - curr_radius;
			auto center_spread = center_upper_bound - center_lower_bound;
			int curr_center = center_lower_bound + round(get_random() * center_spread);

			addObstacle(curr_center, curr_radius);
			renewPeriod();
		}
	}

	// player position already changed this frame
	std::tuple<bool, Obstacle*> iterateObstacleQueue(sf::RenderWindow& window, const sf::Time& deltaTime, const float radius, const sf::Vector2f& playerPosition) {
		int size_current_frame = obstacles.size();
		for (int i = 0; i < size_current_frame; i++) {
			Obstacle* ob = obstacles.front();
			obstacles.pop();
			////////////////////////////////////////
			window.draw(ob->top_rect);
			window.draw(ob->bot_rect);
 			if (collidedPlayer(*ob, radius, playerPosition)) {
				return {false, ob};
				// rest of objects still in queue can be cleared out afterwards
			}
			else { 
				ob->top_rect.move({ -speed * deltaTime.asSeconds(), 0 });
				ob->bot_rect.move({ -speed * deltaTime.asSeconds(), 0 });
				if (ob->top_rect.getPosition().x > destroy_x_coord)
					obstacles.push(ob);
				else
					delete ob;
			}
		}

		return { true, nullptr};
	}
	
	// NEW: calculates score as well
	bool collidedPlayer(Obstacle& ob, float radius, const sf::Vector2f& center) {
		float leftBound = ob.top_rect.getPosition().x;
		const float distanceFromCenter = leftBound - center.x;
		if (distanceFromCenter <= radius) {
			if (distanceFromCenter <= -radius - width) {
				if (!ob.scored) {
					incScore();
					ob.scored = true;
				}
				return false;
			}
			if (center.y <= ob.topHeight + radius || ob.bot_rect.getPosition().y - radius <= center.y)
  				return true;
			else {
				if (leftBound <= center.x <= leftBound + width) {
					if (ob.topHeight + radius < center.y < ob.bot_rect.getPosition().y - radius)
						return false;
					else {
						return true;
					}
				}
				else {
					sf::Vector2f top_L = ob.top_rect.getPosition() + ob.top_rect.getPoint(2);
					sf::Vector2f top_R = ob.top_rect.getPosition() + ob.top_rect.getPoint(3);
					sf::Vector2f bot_L = ob.bot_rect.getPosition();
					sf::Vector2f bot_R = ob.bot_rect.getPosition() + ob.bot_rect.getPoint(1);

					float dist_top_L = (center - top_L).length();
					float dist_top_R = (center - top_R).length();
					float dist_bot_L = (center - bot_L).length();
					float dist_bot_R = (center - bot_R).length();

					float dist_top = std::min({ dist_top_L, dist_top_R });
					float dist_bot = std::min({ dist_bot_L, dist_bot_R });

					if (std::min({ dist_top, dist_bot }) <= radius) {
						return true;
					}
					else return false;
				}
			}
		}
		else return false;
	}
}