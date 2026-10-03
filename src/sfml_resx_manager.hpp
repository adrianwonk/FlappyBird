#pragma once
#include <SFML/Graphics.hpp>
#include "main.h"

/***********************************************/
/*  Inits SFML window and text objects.        */
/***********************************************/
class sfml_resx_manager {
    public:
    void init_window(sf::RenderWindow& window){
        auto windowStyle = sf::Style::Close | sf::Style::Titlebar;
        window = sf::RenderWindow(sf::VideoMode({ SCREEN_WIDTH, SCREEN_HEIGHT }), "FlappyBird", windowStyle);
        window.setKeyRepeatEnabled(false);
        window.setFramerateLimit(FPS);
    }


    void set_default_text(sf::Text& text){
        text.setString("Hello world");
        text.setCharacterSize(50);
        text.setFillColor(sf::Color::White);
    }
};
