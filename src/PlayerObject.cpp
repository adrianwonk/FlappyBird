#include "PlayerObject.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>

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

//tweakables
	const float gravity =					480.f;
	const float fallTransition =			0.28f;
	const float jumpDuration =				.2f;
	const float constantFallMultiplier =	1.5f;

//Bhāskara I's sine approximation formula
	const double pi = 3.14159265358979323846f;
	const double piSquared = pi * pi;
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

namespace player {

	const float def_x = 150.f;
	float curr_y {};

	sf::Clock jumpClock;
		sf::Clock fallClock;
	float sprite_radius {};
	sf::CircleShape sprite;

	void initClocks() {
		jumpClock.reset();
		fallClock.reset();
	}

	sf::Vector2<float> updateSpriteCoords() { // updates sprite x,y at the END of each frame, prior to window refresh. Ran in displayGame()
		sprite.setPosition({ def_x, curr_y });
		return { def_x, curr_y };
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
				curr_y += sin_fast(pi / (2 * fallTransition) * fallClock.getElapsedTime().asSeconds()) * gravity * deltaTime.asSeconds();
			}

			// constant fall
			else {
				curr_y += gravity * (1.4 + fallClock.getElapsedTime().asSeconds() * constantFallMultiplier) * deltaTime.asSeconds();
			}
		}
	}

	void maybeProcessJump(sf::Time deltaTime) { // ran in computeGame()
		if (jumpClock.isRunning()) {
			if (jumpClock.getElapsedTime().asSeconds() < jumpDuration) {
				// cosine wave with the period being fallTransition * 4 (uses the first quarter of a wave)
				curr_y -= cos_fast(pi / (2 * jumpDuration) * jumpClock.getElapsedTime().asSeconds()) * gravity * deltaTime.asSeconds();
			}
			else {
				// jumpClock running but exceeds upper time bound
				jumpClock.reset();
				fallClock.restart();
			}
		}
	}
}