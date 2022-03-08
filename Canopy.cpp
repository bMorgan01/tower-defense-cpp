//
// Created by Benjamin on 1/30/2022.
//

#include "Canopy.h"

Canopy::Canopy(const sf::Texture &texture, float x, float y, float angle, float scale, int origin) : Sprite(texture, x, y, angle, scale, origin) {}

void Canopy::hover() {
    Sprite::hover();

    setColor(sf::Color(255, 255, 255, 128));
}

void Canopy::unHover() {
    Sprite::unHover();

    setColor(sf::Color(255, 255, 255, 255));
}