//
// Created by Benjamin on 1/30/2022.
//

#ifndef SFML_TEMPLATE_TOWERGUI_H
#define SFML_TEMPLATE_TOWERGUI_H

#include "Button.h"

class TowerGUI : public Button {
public:
    TowerGUI(const sf::Texture &, float, float, float, int, const std::function<void()> &, Sprite*);

    void hover() override;
    void unHover() override;
    void setVisible(bool visible);
};


#endif //SFML_TEMPLATE_TOWERGUI_H
