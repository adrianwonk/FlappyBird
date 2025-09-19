#include "main.h"
#include "PlayerObject.h"
#include "ObstacleObject.h"
#include <iostream>
#include <SFML/Graphics.hpp>


void processEvents(sf::RenderWindow&, bool&, bool&, bool&);
void displayPlayer(sf::RenderWindow& window);
void computeGame(sf::Time deltaTime, bool&);
void resetGame();
void displayObstacles(sf::RenderWindow& window, sf::Time dt);

// TODO!!
	// add obstacles
		// spawn in constant x_coordinate
		// variable negative space r >=r_min
			// r_min<=r<=min(center - buffer, SCREEN_HEIGHT - center - buffer)
			
		// variable center (center_min = 0 + r_min + buffer)
							// (center_max = SCEEN_HEIGHT - buffer - r_min)
	// add to linked_list of obstacle structs
	
	// linked list of obstacle structs
	// progress all their x coordinate per frame (linear iteration)
	// 


// for tweaking
const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 480;
const float DEATH_SCRN_DELAY = 1;
const float INITIAL_Y_COORD = 150.f;
const int FPS = 10;
//////////////////

void main()
{
	// window shit
    auto windowStyle = sf::Style::Close | sf::Style::Titlebar;
    auto window = sf::RenderWindow(sf::VideoMode({ SCREEN_WIDTH, SCREEN_HEIGHT }), "FlappyBird", windowStyle);
    window.setKeyRepeatEnabled(false);
    window.setFramerateLimit(FPS);
	
    // game shit
    sf::Clock frameClock;
	bool prestartScreen = true;
    bool gameRunning = false;
	bool deathScreen = false;
	sf::Font font("src/resx/main_font.ttf");
	sf::Text text(font);


	text.setString("Hello world");
	text.setCharacterSize(50);
	text.setFillColor(sf::Color::White);
	
	sf::Clock deathScreenClock;
	deathScreenClock.reset();
	resetGame();
    
    // game shit: player
    player::sprite_radius = 11.f;
    player::sprite = sf::CircleShape(player::sprite_radius);

   
    while (window.isOpen()) {
		window.clear();
		processEvents(window, gameRunning, deathScreen, prestartScreen);
		sf::Time deltaTime = frameClock.restart();

		if (gameRunning) {
			// game running
			computeGame(deltaTime, gameRunning); // one frame is between frameClock getting restarted
			displayObstacles(window, deltaTime);
		}
		else if (!deathScreen && !prestartScreen){
			// death delay
			if (!deathScreenClock.isRunning()){
				deathScreenClock.restart();
			} else if (deathScreenClock.getElapsedTime().asSeconds() > DEATH_SCRN_DELAY) {
				// play death screen
				prestartScreen = false;
				gameRunning = false;
				deathScreen = true;	
				deathScreenClock.reset();
			}
		}
		else if (deathScreen) {
			// deathScreen
			text.setString("dead motherfucker");
			text.setFillColor(sf::Color::Red);
			window.draw(text);
		}
		else {
			// prestartScreen
			text.setString("Welcome to flappy bird!");
			text.setFillColor(sf::Color::White);
			window.draw(text);
		}
		displayPlayer(window);
		window.display();
	}
}

////////////////////////////////////////////////////
void processEvents(sf::RenderWindow& window, bool& gameRunning, bool& deathScreen, bool& prestartScreen) {
    while (const std::optional event = window.pollEvent()) {
        
		if (event->is<sf::Event::Closed>())
            window.close();
        
		if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
			std::cout << "pressed!\n";
			if (prestartScreen) {
				player::jump();
				obstacle::initObstacles();
				prestartScreen = false;
				gameRunning = true;
				deathScreen = false;
			}
            else if (gameRunning){
				std::cout << "Jump()!\n";
				player::jump();
			}
			else if (deathScreen) {
				resetGame();
				prestartScreen = true;
				gameRunning = false;
				deathScreen = false;
			}
		}
    }
}

// calculates all game variables prior to rendering
void computeGame(sf::Time deltaTime, bool& gameRunning) {
	player::fall(deltaTime);
	player::maybeProcessJump(deltaTime);
	obstacle::computeObstacle();

	if (player::curr_y > SCREEN_HEIGHT + 100.f){
		gameRunning = false;
	}
}

void displayObstacles(sf::RenderWindow& window, sf::Time dt) {
	// test code
	obstacle::renderObstacles(window, dt);
	/////
}

void displayPlayer(sf::RenderWindow& window) {
    player::updateSpriteCoords();
    window.draw(player::sprite);
}

void resetGame() {
    player::curr_y = INITIAL_Y_COORD;
	player::initClocks();
	std::cout << "resetGame()!\n";
	return;
}