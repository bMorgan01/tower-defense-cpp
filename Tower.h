//
// Created by Benjamin on 1/30/2022.
//

#ifndef SFML_TEMPLATE_TOWER_H
#define SFML_TEMPLATE_TOWER_H

#include "Placeable.h"

class Tower : public Placeable {
private:
    int focus1 = FIRST, focus2 = FIRST;
public:
    static const int FIRST = 0;
    static const int LAST = 1;
    static const int STRONG = 2;
    static const int WEAK = 3;
    static const int HEALTHY = 4;
    static const int UNHEALTHY = 5;
    static const int FAST = 6;
    static const int SLOW = 7;
    static const int INFILE = 8;
    static const int CLUSTERED = 9;

    Tower(const sf::Texture &, float, float, float, int, float, float, int, int*, Placeable **, std::vector<int>&, Group *, Sprite *, const std::function<void()> &);

    int getFocus2() const;
    void setFocus2(int focus2);
    int getFocus1() const;
    void setFocus1(int focus1);
};


#endif //SFML_TEMPLATE_TOWER_H
