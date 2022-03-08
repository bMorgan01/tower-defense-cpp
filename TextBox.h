//
// Created by Benjamin on 2/27/2022.
//

#ifndef SFML_TEMPLATE_TEXTBOX_H
#define SFML_TEMPLATE_TEXTBOX_H

#include "Label.h"

class TextBox : public Label {
public:
    TextBox(const std::string&, const sf::Font&, unsigned int, float, float, float, int);
};


#endif //SFML_TEMPLATE_TEXTBOX_H
