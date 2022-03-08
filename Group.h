//
// Created by Benjamin on 1/25/2022.
//

#ifndef SFML_TEMPLATE_GROUP_H
#define SFML_TEMPLATE_GROUP_H

#include <vector>
#include "Sprite.h"
#include "Label.h"

class Group {
public:
    Group();
    explicit Group(sf::RenderWindow *);

    std::vector<Sprite *>* getLayerSprites(unsigned int);
    std::vector<Label *>* getLayerLabels(unsigned int);
    void add(Sprite *, int = 0);
    void add(Label *, int = 0);
    void kill(Sprite *);
    void empty();
    void empty_layer(unsigned int);

    void update();
    void draw();
    void do_clicked(sf::Vector2i &);
    void hide_layer(unsigned int);
    void show_layer(unsigned int);
private:
    sf::RenderWindow *window;

    std::vector<std::vector<int>> layers;
    std::vector<std::vector<Sprite *>> sprites;
    std::vector<std::vector<Label *>> labels;
};


#endif //SFML_TEMPLATE_GROUP_H
