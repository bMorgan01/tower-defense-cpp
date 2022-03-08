//
// Created by Benjamin on 1/25/2022.
//

#include "Sprite.h"

Sprite::Sprite(const sf::Texture &texture) : sf::Sprite(texture) {

}

Sprite::Sprite(const sf::Texture &texture, int origin) : sf::Sprite(texture) {
    setOrigin(origin);
}

Sprite::Sprite(const sf::Texture &texture, float x, float y, float scale, int origin) : Sprite(texture, origin) {
    setScale(scale, scale);
    setPosition(x, y);
}

Sprite::Sprite(const sf::Texture &texture, float x, float y, float angle, float scale, int origin) : Sprite(texture, x, y, scale, origin) {
    setRotation(angle);
}

Sprite::~Sprite() noexcept {
    removeMounts();
}

void Sprite::setOrigin(int origin) {
    if (origin == TOP_LEFT) {
        sf::Sprite::setOrigin(0, 0);
    } else if (origin == CENTER) {
        sf::Sprite::setOrigin(getLocalBounds().width/2.0,getLocalBounds().height/2.0);
    } else if (origin == BOTTOM_RIGHT) {
        sf::Sprite::setOrigin(getLocalBounds().width, getLocalBounds().height);
    }
}

bool Sprite::isVisible() const {
    return visible;
}

void Sprite::setVisible(bool visible) {
    this->visible = visible;

    if (hasMount()) {
        riding->setVisible(visible);
    }
}

void Sprite::setMount(Sprite *s) {
    removeMounts();
    riding = s;
}

void Sprite::removeMounts() {
    if (hasMount()) {
        riding->removeMounts();
        delete riding;
        riding = nullptr;
    }
}

void Sprite::kill() {
    removeMounts();
}

void Sprite::update() {
    sf::Vector2f mousePosF(static_cast<float>( sf::Mouse::getPosition().x ), static_cast<float>( sf::Mouse::getPosition().y ));

    if (getGlobalBounds().contains(mousePosF)) {
        if (!isHovered()) {
            hover();
        }
    } else if (isHovered()) {
        unHover();
    }
}

void Sprite::draw(sf::RenderWindow *window) {
    if (hasMount()) {
        riding->draw(window);
    }

    if (visible) {
        window->draw(*this);
    }
}
