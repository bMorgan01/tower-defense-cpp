//
// Created by Benjamin on 1/25/2022.
//

#ifndef SFML_TEMPLATE_BUTTON_H
#define SFML_TEMPLATE_BUTTON_H

#include <functional>
#include "Sprite.h"

class Button : public Sprite {
public:
    explicit Button(const sf::Texture &, const std::function<void()> &);
    Button(const sf::Texture &, float, float, float, int, const std::function<void()> &);

    void click() override;
    void hover() override;
    void unHover() override;
protected:
    std::function<void()> command;
};


#endif //SFML_TEMPLATE_BUTTON_H
