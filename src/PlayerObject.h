#ifndef PLAYER_OBJECT_H
#define PLAYER_OBJECT_H
#include <SFML/Graphics.hpp>

namespace player {
	constexpr float def_x = 150.f;
	extern float curr_y;
	extern sf::Clock jumpClock;
	extern float sprite_radius;
	extern sf::CircleShape sprite;

	void initClocks();
	sf::Vector2f updateSpriteCoords();
	void fall(sf::Time deltaTime);
	void jump();
	void maybeProcessJump(sf::Time deltaTime);
	sf::Vector2f getCenter();

} // namespace player

#endif // PLAYER_OBJECT_H
