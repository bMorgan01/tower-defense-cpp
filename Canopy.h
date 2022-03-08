//
// Created by Benjamin on 1/30/2022.
//

#ifndef SFML_TEMPLATE_CANOPY_H
#define SFML_TEMPLATE_CANOPY_H


#include "Sprite.h"

class Canopy : public Sprite {
public:
    Canopy(const sf::Texture &, float, float, float, float, int);

    void hover() override;
    void unHover() override;
};


#endif //SFML_TEMPLATE_CANOPY_H
