//
// Created by Benjamin on 1/26/2022.
//

#ifndef SFML_TEMPLATE_LABEL_H
#define SFML_TEMPLATE_LABEL_H

#include <SFML/Graphics.hpp>

class Label : public sf::Text {
protected:
    int origin = TOP_LEFT;

    bool visible = true;
public:
    static const int TOP_LEFT = 0;
    static const int CENTER = 1;
    static const int LEFT_CENTER = 3;

    Label(const std::string&, const sf::Font&, unsigned int, float, float, int);

    void setOrigin(int);
    void setVisible(bool);

    virtual void update() {};
    void draw(sf::RenderWindow *window);
};


#endif //SFML_TEMPLATE_LABEL_H
