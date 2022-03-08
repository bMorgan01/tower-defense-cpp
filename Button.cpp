//
// Created by Benjamin on 1/25/2022.
//

#include "Button.h"

Button::Button(const sf::Texture &texture, const std::function<void()> &command) : Sprite(texture) {
    this->command = command;
}

Button::Button(const sf::Texture &texture, float x, float y, float scale, int origin, const std::function<void()> &command) : Sprite(texture, x, y, scale, origin) {
    this->command = command;
}

void Button::click() {
    command();
}

void Button::hover() {
    Sprite::hover();

    sf::Vector2f pos = getPosition();
    setScale(getScale().x + 0.1, getScale().x + 0.1);
    setPosition(pos.x, pos.y);
}

void Button::unHover() {
    Sprite::unHover();

    sf::Vector2f pos = getPosition();
    setScale(getScale().x - 0.1, getScale().x - 0.1);
    setPosition(pos.x, pos.y);
}
