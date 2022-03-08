//
// Created by Benjamin on 1/25/2022.
//

#ifndef SFML_TEMPLATE_SPRITE_H
#define SFML_TEMPLATE_SPRITE_H

#include <SFML/Graphics.hpp>

class Sprite : public sf::Sprite {
protected:
    bool hovered = false;
    bool clicked = false;
    bool visible = true;

    Sprite *riding = nullptr;
public:
    static const int TOP_LEFT = 0;
    static const int CENTER = 1;
    static const int BOTTOM_RIGHT = 2;

    explicit Sprite(const sf::Texture &);
    Sprite(const sf::Texture &, int);
    Sprite(const sf::Texture &, float, float, float, int);
    Sprite(const sf::Texture &, float, float, float, float, int);

    ~Sprite() noexcept;

    void setOrigin(int);
    bool isHovered() const { return hovered; };
    bool isVisible() const;
    virtual void setVisible(bool visible);

    void setMount(Sprite *);
    bool hasMount() { return riding; };
    void removeMounts();
    void kill();

    virtual void update();
    virtual void click() {};
    virtual void noClick() {};
    virtual void hover() { hovered = true; };
    virtual void unHover() { hovered = false; };

    void draw(sf::RenderWindow *window);
};

#endif //SFML_TEMPLATE_SPRITE_H
