#include "main.h"
#include "PlayerObject.h"
#include "ObstacleObject.h"
#include <iostream>
#include <SFML/Graphics.hpp>


void processEvents(sf::RenderWindow&, bool&, bool&, bool&);
void displayPlayer(sf::RenderWindow& window);
void computeGame(sf::Time, sf::RenderWindow&,bool&);
void resetGame();
void displayDeadScreen(sf::RenderWindow& window, obstacle::Obstacle*);

obstacle::Obstacle* killerOb = nullptr;
void updateKillerOb(obstacle::Obstacle* ob) {
	if (killerOb == ob)
		return;
	delete killerOb;
	killerOb = ob;
}

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
    player::sprite_radius = sprite_radius;
    player::sprite = sf::CircleShape(player::sprite_radius);

    while (window.isOpen()) {
		window.clear();
		processEvents(window, gameRunning, deathScreen, prestartScreen);
		sf::Time deltaTime = frameClock.restart();

		if (gameRunning) {
			// game running
			computeGame(deltaTime, window, gameRunning); // one frame is between frameClock getting restarted
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
			else {
				obstacle::initObstacles(); // delete excess obstacles
			}
			displayDeadScreen(window, killerOb);
		}
		else if (deathScreen) {
			// deathScreen
			text.setString("dead motherfucker");
			text.setFillColor(sf::Color::Red);
			window.draw(text);
			displayDeadScreen(window, killerOb);
		}
		else {
			// prestartScreen
			text.setString("Welcome to flappy bird!");
			text.setFillColor(sf::Color::White);
			window.draw(text);
			displayPlayer(window);
		}
		window.display();
	}
}

////////////////////////////////////////////////////
// processEvents - keypresses -> functions (clocks)
// computeGame - frame1 -> frame2
// resetGame - frame0
// displayPlayer
// displayDeadScreen
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

// calculates all game variables and renders sprites
void computeGame(sf::Time deltaTime, sf::RenderWindow& window, bool& gameRunning) {
	player::fall(deltaTime);
	player::maybeProcessJump(deltaTime);			// changes player position
	displayPlayer(window);

	obstacle::maybeInstantiateObstacle();
	std::tuple<bool, obstacle::Obstacle*> notCollided = obstacle::iterateObstacleQueue(window, deltaTime, player::sprite_radius, player::getCenter()); // changes obstacle position
	
	if (!std::get<0>(notCollided)) {
		gameRunning = false;
		updateKillerOb(std::get<1>(notCollided));
		return;
	}
}

// initialises game object prior to round start
void resetGame() {
	player::curr_y = INITIAL_Y_COORD;
	player::initClocks();
	std::cout << "resetGame()!\n";
	return;
}

void displayDeadScreen(sf::RenderWindow& window, obstacle::Obstacle* ob) {
	displayPlayer(window);
	ob->top_rect.setFillColor(sf::Color::Red);
	ob->bot_rect.setFillColor(sf::Color::Red);
	window.draw(ob->top_rect);
	window.draw(ob->bot_rect);
}

void displayPlayer(sf::RenderWindow& window) {
	player::updateSpriteCoords();
    window.draw(player::sprite);
}