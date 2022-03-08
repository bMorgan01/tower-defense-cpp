//
// Created by Benjamin on 1/30/2022.
//

#ifndef SFML_TEMPLATE_PLACEABLE_H
#define SFML_TEMPLATE_PLACEABLE_H


#include <functional>
#include "Group.h"

class Placeable : public Sprite {
protected:
    bool placing = true;
    std::vector<int> collideLayers;
    Group *collideGroup;

    std::function<void()> uncommand;
    Placeable **inHand;

    float reach;
    int price;
    int* energy;

    bool isColliding();
public:
    Placeable(const sf::Texture &, float, float, float, int, float, int, int*, Placeable **, std::vector<int>&, Group *, Sprite *, const std::function<void()> &);

    void setReach(float reach);
    void setCommand(const std::function<void()> &);
    bool isPlacing() const;
    std::function<void()> command;
    void update() override;
    void click() override;
    void noClick() override;
};


#endif //SFML_TEMPLATE_PLACEABLE_H
