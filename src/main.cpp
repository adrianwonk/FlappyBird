#include "main.h"
#include "PlayerObject.h"
#include "ObstacleObject.h"
#include <iostream>
#include <SFML/Graphics.hpp>
#include "IMutator.hpp"
#include "view_controller.hpp"
#include "sfml_resx_manager.hpp"

namespace {
    /***********************************************/
    /*  All possible states of the program.        */
    /*                                             */
    /***********************************************/
    enum class state {
        start,
        running,
        dying,
        dead
    };

    sfml_resx_manager sfml_resx;

    /***********************************************/
    /*  UI resx for presentation of the game.      */
    /*                                             */
    /***********************************************/
    sf::RenderWindow window;
    sf::Font font {"src/resx/main_font.ttf"} ;
    sf::Text text {font} ;
	sf::Clock deathScreenClock;

    /***********************************************/
    /*  Frame clock used to measure time between   */
    /*  each subsequent frame.                     */
    /*                                             */
    /***********************************************/
    sf::Clock frameClock;
};


obstacle::Obstacle* killerOb = nullptr;

void processEvents( sf::RenderWindow& window, state& game_state );
void displayPlayer(sf::RenderWindow& window);
void computeGame(sf::Time deltaTime, sf::RenderWindow& window, state& game_state, sf::Text& text);
void resetGame();
void displayDeadScreen(sf::RenderWindow& window, obstacle::Obstacle*);

/***********************************************/
/*  Maintains a reference to the obstacle      */
/*  that the player died touching most         */
/*  recently.                                  */
/*                                             */
/***********************************************/
void updateKillerOb(obstacle::Obstacle* ob) {
	if (killerOb == ob)
		return;
	delete killerOb;
	killerOb = ob;
}

/***********************************************/
/*  Global score variable to be incremented    */
/*  when player passes an obstacle, and reset  */
/*  on death.                                  */
/*                                             */
/***********************************************/
int score = 0;
void incScore() {
	score++;
}
void resetScore() {
	score = 0;
}

/***********************************************/
/*  Overwrites the UI text displaying the      */
/*  score with the current global score value. */
/*                                             */
/***********************************************/
void showScore(sf::RenderWindow& window, sf::Text& text) {
	text.setString(std::to_string(score));
	window.draw(text);
}


/***********************************************/
/*  Organises input polling, game-mechanic     */
/*  computations, UI drawing, and program      */
/*  state handling, all synchronised with      */
/*  frame time.                                */
/*                                             */
/***********************************************/
int main()
{
    state game_state { state::start };

    sfml_resx.init_window(window);
    sfml_resx.set_default_text(text);
	
    deathScreenClock.reset();
	resetGame();
    

    /***********************************************/
    /*  Initialise player radius using constants   */
    /*  in order to calculate collision and draw   */
    /*  UI sprite.                                 */
    /*                                             */
    /***********************************************/
    player::sprite_radius = sprite_radius;
    player::sprite = sf::CircleShape(player::sprite_radius);

    while (window.isOpen()) {
		window.clear();
		processEvents(window, game_state);
		sf::Time deltaTime = frameClock.restart();

		if (game_state == state::running) {
			// game running
			computeGame(deltaTime, window, game_state, text); // one frame is between frameClock getting restarted
		}
		else if (game_state == state::dying){
			// death delay
			if (!deathScreenClock.isRunning()){
				deathScreenClock.restart();
			} else if (deathScreenClock.getElapsedTime().asSeconds() > DEATH_SCRN_DELAY) {
				// play death screen
                game_state = state::dead;
				deathScreenClock.reset();
			} else {
				obstacle::initObstacles(); // delete excess obstacles
			}
			showScore(window, text);
			displayDeadScreen(window, killerOb);
		}
		else if (game_state == state::dead) {
			// deathScreen
			text.setString("dead motherfucker");
			text.setFillColor(sf::Color({ 77, 14, 10, 255 }));
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
    return 0;
}

/***********************************************/
/*  Poll keyboard input, and call different    */
/*  functions depending on current program     */
/*  state.                                     */
/*                                             */
/***********************************************/
void processEvents( sf::RenderWindow& window, state& game_state ) {
    while (const std::optional event = window.pollEvent()) {
        
		if (event->is<sf::Event::Closed>())
            window.close();
        
		if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
			std::cout << "Button pressed!\n";
			if (game_state == state::start) {
				std::cout << "\tinterpret action: jump()! state: start->running\n";
				player::jump();
				obstacle::initObstacles();
                game_state = state::running;
			}
            else if (game_state == state::running){
				std::cout << "\tinterpret action: jump()! no state change\n";
				player::jump();
			}
			else if (game_state == state::dead) {
				std::cout << "\tinterpret action: resetGame()! state: dead->start\n";
				resetGame();
                game_state = state::start;
			}
		}
    }
}

/***********************************************/
/*  Computes the next frame of the game by     */
/*  polling for user input, applying gravity,  */
/*  and randomly spawning new obstacles.       */
/*  Updates killer object on player death.     */
/*                                             */
/***********************************************/
void computeGame(sf::Time deltaTime, sf::RenderWindow& window, state& game_state, sf::Text& text) {
	player::fall(deltaTime);
	player::maybeProcessJump(deltaTime);			// changes player position
	displayPlayer(window);

	obstacle::maybeInstantiateObstacle();
	std::tuple<bool, obstacle::Obstacle*> notCollided = obstacle::iterateObstacleQueue(window, deltaTime, player::sprite_radius, player::getCenter()); // changes obstacle position
	
	if (!std::get<0>(notCollided)) {
		game_state = state::dying;
		updateKillerOb(std::get<1>(notCollided));
		return;
	}
	else {
		showScore(window, text);
	}
}

/***********************************************/
/*  Prepares a new game by resetting clocks,   */
/*  player position, and score.                */
/*                                             */
/***********************************************/
void resetGame() {
	player::curr_y = INITIAL_Y_COORD;
	player::initClocks();
	std::cout << "resetGame()!\n";
	resetScore();
	return;
}

/***********************************************/
/*  Stylises obstacle the killed the player    */
/*  and draws it onto the screen afterwards.   */
/*                                             */
/***********************************************/
void displayDeadScreen(sf::RenderWindow& window, obstacle::Obstacle* ob) {
	displayPlayer(window);
	ob->top_rect.setFillColor(sf::Color::Red);
	ob->bot_rect.setFillColor(sf::Color::Red);
	window.draw(ob->top_rect);
	window.draw(ob->bot_rect);
}

/***********************************************/
/*  Updates player movement and draws the      */
/*  sprite.                                    */
/*                                             */
/***********************************************/
void displayPlayer(sf::RenderWindow& window) {
	player::updateSpriteCoords();
    window.draw(player::sprite);
}
