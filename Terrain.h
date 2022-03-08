//
// Created by Benjamin on 1/26/2022.
//

#ifndef SFML_TEMPLATE_TERRAIN_H
#define SFML_TEMPLATE_TERRAIN_H


#include "Sprite.h"

class Terrain : public Sprite {
public:
    Terrain(sf::Texture &, float, float, float, int);
};


#endif //SFML_TEMPLATE_TERRAIN_H
