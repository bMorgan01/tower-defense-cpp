//
// Created by Benjamin on 1/30/2022.
//

#include "TowerGUI.h"

TowerGUI::TowerGUI(const sf::Texture &texture, float x, float y, float scale, int origin, const std::function<void()> &command, Sprite* hoverSprite) : Button(texture, x, y, scale, origin, command) {
    riding = hoverSprite;
    riding->setVisible(false);
}

void TowerGUI::hover() {
    Sprite::hover();

    riding->setVisible(true);
}

void TowerGUI::unHover() {
    Sprite::unHover();

    riding->setVisible(false);
}

void TowerGUI::setVisible(bool visible) {
    Sprite::setVisible(visible);

    if (visible) {
        unHover();
    }
}
