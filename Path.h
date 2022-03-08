//
// Created by Benjamin on 1/26/2022.
//

#ifndef SFML_TEMPLATE_PATH_H
#define SFML_TEMPLATE_PATH_H

#include "Terrain.h"

class Path : public Terrain {
private:
    Path *next_waypoint;
public:
    Path(sf::Texture &, float, float, float, int);
};


#endif //SFML_TEMPLATE_PATH_H
