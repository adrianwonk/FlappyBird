#include "ObstacleObject.h"
#include "main.h"
#include <queue>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>

using namespace obstacle;
int clock_initialised = 0;
namespace obstacle {
	const float x_coord = SCREEN_WIDTH;
	const float width = 50;
	const float destroy_x_coord = -width;
	const int buffer = 30;

	const float speed = 350.f;

	const int min_radius = 45;
	const float radius_variance = 1.7f;
	const int max_radius = round(min_radius * radius_variance);
	const int max_radius_spread = max_radius - min_radius;
	
	const float min_period = 1.f;
	const float period_variance = 1.7f;
	const float max_period = min_period * period_variance;
	const float max_period_spread = max_period - min_period;
	float curr_period = 0;
	// Create a random number generator engine
	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_real_distribution<> rand_dist(0, 1.0);
	
	auto get_random() {
		return rand_dist(generator);
	}
	
	sf::Clock period_timer;

	std::queue<Obstacle> obstacles;
	
	void addObstacle(const Obstacle& ob) {
		obstacles.push(ob);
	}
	
	void renewPeriod() {
		clock_initialised = 1;
		period_timer.restart();
		curr_period = min_period + get_random() * max_period_spread;
	}

	void initObstacles() {
		std::swap(obstacles, std::queue<Obstacle>());
		renewPeriod();
	}

	Obstacle generateObstacle(int center, int radius){
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
		topShape.setPosition({x_coord, 0});
		 
		sf::RectangleShape botShape({ width, botLength });
		botShape.setPosition({ x_coord, (float)(center + radius) });	
		
		return {topShape, botShape};
	}

	void instantiateObstacle(int center, int radius) {
		addObstacle(generateObstacle(center, radius));
	}

	// MUST be ran AFTER initClock()
	void computeObstacle() {
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
			
			// calculate the center  
			auto center_lower_bound = buffer + curr_radius;
			auto center_upper_bound = SCREEN_HEIGHT - buffer - curr_radius;
			auto center_spread = center_upper_bound - center_lower_bound;
			int curr_center = center_lower_bound + round(get_random() * center_spread);
		
			std::cout << "rad: " << curr_radius << '\n';
			std::cout << "cen: " << curr_center << '\n';
			instantiateObstacle(curr_center, curr_radius);
			renewPeriod();
		}
	}

	
	
	void renderObstacles(sf::RenderWindow& window, sf::Time deltaTime){
		//std::cout << obstacles.size()<<'\n';
		auto size_current_frame = obstacles.size();
		for (int i = 0; i < size_current_frame; i++) {
			Obstacle ob = obstacles.front();
			obstacles.pop();
			window.draw(ob.top_rect);
			window.draw(ob.bot_rect);
			ob.top_rect.move({ -speed * deltaTime.asSeconds(), 0 });
			ob.bot_rect.move({ -speed * deltaTime.asSeconds(), 0 });
			float temp = ob.top_rect.getPosition().x;
			if (temp > destroy_x_coord) {
				obstacles.push(ob);
			}
		}
	}
	
	void testObstacle() {
		//instantiateObstacle(SCREEN_HEIGHT / 2, 10);
		//instantiateObstacle(SCREEN_HEIGHT / 2, 15);
		//instantiateObstacle(SCREEN_HEIGHT / 2, 20);
		instantiateObstacle(SCREEN_HEIGHT / 2, 45);
	}
	
}