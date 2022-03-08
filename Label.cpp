//
// Created by Benjamin on 1/26/2022.
//

#include <iostream>
#include "Label.h"

Label::Label(const std::string& text, const sf::Font& font, unsigned int size, float x, float y, int origin) : sf::Text(text, font, size) {
    setOrigin(origin);
    setPosition(x, y);
}

void Label::setOrigin(int origin) {
    if (origin == TOP_LEFT) {
        sf::Text::setOrigin(getLocalBounds().left, getLocalBounds().top);
    } else if (origin == CENTER) {
        sf::Text::setOrigin(getLocalBounds().left + getLocalBounds().width/2.0,getLocalBounds().top + getLocalBounds().height/2.0);
    } else if (origin == LEFT_CENTER) {
        sf::Text::setOrigin(getLocalBounds().left, getLocalBounds().top + getLocalBounds().height/2.0);
    }
}

void Label::setVisible(bool visible) {
    this->visible = visible;
}

void Label::draw(sf::RenderWindow *window) {
    if (visible) {
        window->draw(*this);
    }
}
