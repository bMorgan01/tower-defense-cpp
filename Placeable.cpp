//
// Created by Benjamin on 1/30/2022.
//

#include "Placeable.h"
#include "Collision.h"



Placeable::Placeable(const sf::Texture &texture, float x, float y, float scale, int origin, float tileWidth, int price, int* energy, Placeable **inHand, std::vector<int> &collideLayers, Group *collideGroup, Sprite *disk, const std::function<void()> &uncommand) : Sprite(texture, x, y, scale, origin) {
    riding = disk;
    this->inHand = inHand;
    this->collideLayers = collideLayers;
    this->collideGroup = collideGroup;
    this->uncommand = uncommand;
    this->price = price;
    this->energy = energy;

    setReach(tileWidth);
}

void Placeable::setReach(float reach) {
    this->reach = reach;

    sf::Vector2u size = riding->getTexture()->getSize();
    riding->setScale((reach*2)/((float)size.x), (reach*2)/((float)size.y));
}

void Placeable::setCommand(const std::function<void()> &command) {
    this->command = command;
}

void Placeable::update() {
    Sprite::update();

    if (placing) {
        float x = sf::Mouse::getPosition().x;
        float y = sf::Mouse::getPosition().y;
        setPosition(x, y);
        riding->setPosition(x, y);

        if (!isColliding()) {
            riding->setColor(sf::Color(255, 255, 255, 128));
        } else {
            riding->setColor(sf::Color(255, 0, 0, 128));
        }
    }
}

void Placeable::click() {
    if (placing && !isColliding() && (price == 0 || *energy >= price)) {
        placing = !placing;
        if (price != 0)
            *energy -= price;
        riding->setColor(sf::Color(0, 0, 0, 128));
    }

    if (!placing) {
        if (*inHand == this)
            *inHand = nullptr;

        if (!*inHand) {
            clicked = true;
            command();

            riding->setVisible(true);
        }
    }
}

void Placeable::noClick() {
    riding->setVisible(false);

    if (clicked)
        uncommand();

    clicked = false;
}

bool Placeable::isColliding() {
    bool colliding = false;
    for (int layer : collideLayers) {
        for (Sprite *s : *collideGroup->getLayerSprites(layer)) {
            if (s != this && s->getGlobalBounds().intersects(this->getGlobalBounds()) && Collision::PixelPerfectTest(*s, *this)) {
                colliding = true;
                break;
            }

            if (colliding) {
                break;
            }
        }
    }

    return colliding;
}

bool Placeable::isPlacing() const {
    return placing;
}