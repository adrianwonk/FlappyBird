#ifndef PLAYER_OBJECT_H
#define PLAYER_OBJECT_H

#ifndef SFML
#define SFML
#include <SFML/Graphics.hpp>
#endif

namespace player {
	extern const float x_coord;
	extern float y_coord;
	extern sf::Clock jumpClock;
	extern float sprite_radius;
	extern sf::CircleShape sprite;
    
	void initClocks();
	sf::Vector2f updateCoords();
    void fall(sf::Time deltaTime);
    void jump();
    void maybeProcessJump(sf::Time deltaTime);

} // namespace player

#endif // PLAYER_OBJECT_H

