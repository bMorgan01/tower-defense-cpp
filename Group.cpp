//
// Created by Benjamin on 1/25/2022.
//

#include <iostream>
#include "Group.h"

Group::Group() {
    window = nullptr;
}

Group::Group(sf::RenderWindow *window) {
    this->window = window;
}

std::vector<Sprite *> * Group::getLayerSprites(unsigned int l) {
    if (layers.size() > l)
        return &sprites[l];
    else
        return nullptr;
}

std::vector<Label *> * Group::getLayerLabels(unsigned int l) {
    if (layers.size() > l)
        return &labels[l];
    else
        return nullptr;
}

void Group::add(Sprite *s, int layer) {
    while (layers.size() < layer + 1) {
        layers.emplace_back();
    }
    while(sprites.size() < layer + 1) {
        sprites.emplace_back();
    }

    layers.at(layer).push_back(0);
    sprites.at(layer).push_back(s);
}

void Group::add(Label *l, int layer) {
    while (layers.size() < layer + 1) {
        layers.emplace_back();
    }
    while(labels.size() < layer + 1) {
        labels.emplace_back();
    }

    layers.at(layer).push_back(1);
    labels.at(layer).push_back(l);
}

void Group::kill(Sprite *target) {
    int sprite;
    int label;

    for (int i = 0; i < layers.size(); i++) {
        sprite = 0;
        label = 0;
        for (int j = 0; j < layers[i].size(); j++) {
            if (layers[i][j] == 0) {
                if (sprites[i][sprite] == target) {
                    sprites[i][sprite]->kill();
                    layers[i].erase(layers[i].begin() + j);
                    sprites[i].erase(sprites[i].begin() + sprite);
                }
                sprite++;
            } else {
                label++;
            }
        }
    }
}

void Group::empty() {
    for (int i = 0; i < layers.size(); i++) {
        empty_layer(i);
    }

    layers.clear();
}

void Group::empty_layer(unsigned int l) {
    if (sprites.size() > l) {
        for (auto s : sprites[l]) {
            s->kill();
            delete s;
        }
        sprites[l].clear();
    }

    if (labels.size() > l) {
        for (auto s : labels[l]) {
            delete s;
        }
        labels[l].clear();
    }

    layers[l].clear();
}

void Group::update() {
    int sprite;
    int label;

    for (int i = 0; i < layers.size(); i++) {
        sprite = 0;
        label = 0;
        for (int type : layers[i]) {
            if (type == 0) {
                if (sprites[i][sprite]->isVisible())
                    sprites[i][sprite]->update();
                sprite++;
            } else {
                labels[i][label]->update();
                label++;
            }
        }
    }
}

void Group::draw() {
    int sprite;
    int label;

    for (int i = 0; i < layers.size(); i++) {
        sprite = 0;
        label = 0;
        for (int type : layers[i]) {
            if (type == 0) {
                sprites[i][sprite]->draw(window);
                sprite++;
            } else {
                labels[i][label]->draw(window);
                label++;
            }
        }
    }
}

void Group::do_clicked(sf::Vector2i &pos) {
    sf::Vector2f mousePosF(static_cast<float>( pos.x ), static_cast<float>( pos.y ));

    for (auto &layer : sprites) {
        for (Sprite *s : layer) {
            if (s->isVisible()) {
                if (s->getGlobalBounds().contains(mousePosF)) {
                    s->click();
                }
                else {
                    s->noClick();
                }
            }
        }
    }
}

void Group::hide_layer(unsigned int l) {
    if (sprites.size() > l) {
        for (auto *s : sprites[l]) {
            s->setVisible(false);
        }
    }

    if (labels.size() > l) {
        for (auto *s : labels[l]) {
            s->setVisible(false);
        }
    }
}

void Group::show_layer(unsigned int l) {
    if (sprites.size() > l) {
        for (auto *s : sprites[l]) {
            s->setVisible(true);
        }
    }

    if (labels.size() > l) {
        for (auto *s : labels[l]) {
            s->setVisible(true);
        }
    }
}
