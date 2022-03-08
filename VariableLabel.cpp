//
// Created by Benjamin on 2/28/2022.
//

#include "VariableLabel.h"

VariableLabel::VariableLabel(int *track, const sf::Font& font, unsigned int size, float x, float y, int origin) : Label(std::to_string(*track), font, size, x, y, origin) {
    this->track = track;
}

void VariableLabel::update() {
    setString(std::to_string(*track));
}
