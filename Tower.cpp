//
// Created by Benjamin on 1/30/2022.
//

#include "Tower.h"

Tower::Tower(const sf::Texture &texture, float x, float y, float scale, int origin, float reach, float tileWidth, int price, int* energy, Placeable **inHand, std::vector<int> &collideLayers, Group *collideGroup, Sprite *disk, const std::function<void()> &uncommand) : Placeable(texture, x, y, scale, origin, tileWidth, price, energy, inHand, collideLayers, collideGroup, disk, uncommand) {
    setReach(reach * tileWidth);
}

int Tower::getFocus1() const {
    return focus1;
}

void Tower::setFocus1(int focus1) {
    Tower::focus1 = focus1;
}

int Tower::getFocus2() const {
    return focus2;
}

void Tower::setFocus2(int focus2) {
    Tower::focus2 = focus2;
}
