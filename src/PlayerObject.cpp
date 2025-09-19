#include "PlayerObject.h"
#include <iostream>
#include <string>

//using namespace player;
	// this class provides the following functionalities:
		// Storing a sprite object to represent the player, the position of which is updated per frame via updateCoords();
		// displaces the sprite in 4 stages:
			// no displacement 						(at the beginning of game)
			// 1. upward displacement to neutural 	(started by jump()) (interrupt)
			// 2. neutral to downward 				(started after upward 1.)
			// 3. downward infinitum 				(started after 2.) 
				// increases PAST gravity
			
		// the main variables are:
			// gravity 					-- determines the average downward force of the world.
			// fallTransition 			-- determines length of 2.
			// constantFallMultiplier 	-- determines the coefficient of t during constant fall (t: time since fall began)


namespace player {
	const double pi = 3.14159265358979323846f;
	const double piSquared = pi * pi;
	const float gravity = 480.f;
	const float x_coord = 150.f;
	float y_coord;

	sf::Clock jumpClock;
	sf::Clock fallClock;
	const float fallTransition = 0.28f;
	const float jumpDuration = .2f;
	const float constantFallMultiplier = 1.5f;
	float sprite_radius;
	sf::CircleShape sprite;

	//Bhāskara I's sine approximation formula
	static double sin_fast(float x) {
		double numerator = 16 * x * (pi - x);
		double denominator = 5 * piSquared - 4 * x * (pi - x);
		return numerator / denominator;
	}

	static double cos_fast(float x) {
		double xSquared = x * x;
		double numerator = piSquared - 4 * xSquared;
		double denominator = piSquared + xSquared;
		return numerator / denominator;
	}

	void initClocks() {
		jumpClock.reset();
		fallClock.reset();
	}

	sf::Vector2<float> updateCoords() { // updates sprite x,y at the END of each frame, prior to window refresh. Ran in displayGame()
		sprite.setPosition({ x_coord, y_coord });
		return { x_coord, y_coord };
	}

	////////////////////////////////////////////

	// interrupts fall
	void jump() {
		jumpClock.restart();
		fallClock.stop();
	}
	// Ran in computeGame()
	void fall(sf::Time deltaTime) {
		if (fallClock.isRunning()) {
			// transition into a constant fall
			if (fallClock.getElapsedTime().asSeconds() < fallTransition) {
				// sine wave with the period being fallTransition * 4 (uses the first quarter of a wave)
				y_coord += sin_fast(pi / (2 * fallTransition) * fallClock.getElapsedTime().asSeconds()) * gravity * deltaTime.asSeconds();
			}

			// constant fall
			else {
				y_coord += gravity * (1.4 + fallClock.getElapsedTime().asSeconds() * constantFallMultiplier) * deltaTime.asSeconds();
			}
		}
	}

	void maybeProcessJump(sf::Time deltaTime) { // ran in computeGame()
		if (jumpClock.isRunning()) {
			if (jumpClock.getElapsedTime().asSeconds() < jumpDuration) {
				// cosine wave with the period being fallTransition * 4 (uses the first quarter of a wave)
				y_coord -= cos_fast(pi / (2 * jumpDuration) * jumpClock.getElapsedTime().asSeconds()) * gravity * deltaTime.asSeconds();
			}
			else {
				// jumpClock running but exceeds upper time bound
				jumpClock.reset();
				fallClock.restart();
			}
		}
	}
}