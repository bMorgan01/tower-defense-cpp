//
// Created by Benjamin on 2/28/2022.
//

#ifndef SFML_TEMPLATE_VARIABLELABEL_H
#define SFML_TEMPLATE_VARIABLELABEL_H

#include "Label.h"

class VariableLabel : public Label {
private:
    int* track;
public:
    VariableLabel(int* track, const sf::Font&, unsigned int, float, float, int);

    void update() override;
};


#endif //SFML_TEMPLATE_VARIABLELABEL_H
