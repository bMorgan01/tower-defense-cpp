//
// Created by Benjamin on 1/25/2022.
//

#ifndef SFML_TEMPLATE_GAME_H
#define SFML_TEMPLATE_GAME_H

#include "Group.h"
#include "Terrain.h"
#include "Placeable.h"
#include "Tower.h"

#include <iostream>
#include <map>

class Game {
public:
    const sf::Vector2f BASE_RESOLUTION = sf::Vector2f(1920, 1080);

    Game();
private:
    static const int MAP_HEIGHT = 30;
    static const int MAP_WIDTH = 40;

    static const int EASY = 1;
    static const int MEDIUM = 2;
    static const int HARD = 3;
    static const int EXPERT = 4;
    const int DIFFICULTY_WAYPOINTS[4][2] = {{5, 6}, {3, 4}, {1, 2}, {0, 1}};
    const int DIFFICULTY_OBSTACLE_PERCS[4] = {5, 10, 15, 20};

    static const int PLAINS = 1;

    static const int TERRAIN_LAYER = 0;
    static const int PATH_LAYER = 1;
    static const int OBSTACLE_LAYER = 2;
    static const int TOWER_LAYER = 3;
    static const int CANOPY_LAYER = 4;
    static const int GUI_LAYER = 5;
    static const int TOWER_BUTTONS_LAYER = 6;
    static const int TOWER_DESC_LAYER = 7;

    static const int TILE_UNIT = 36;

    const std::string foci_strings[10] = {"First", "Last", "Strong", "Weak", "Fast", "Slow", "In-file", "Clustered", "Healthy", "Unhealthy"};

    int pathDifficulty = EASY;
    int environment = PLAINS;

    std::vector<sf::Vector2i> waypoints;
    float mapCorner;
    float mapBottom;

    int energy = 100, gears = 100, health = 100;

    Placeable* inHand = nullptr;
    Sprite* sidebar;
    bool mouseLeftMenu = false;

    Label *focus1Label, *focus2Label;

    sf::Vector2f screenRatio;

    sf::RenderWindow *window;
    Group *displayGroup;

    Group menu;
    Group play;

    sf::Texture startButtonTexture;
    sf::Texture quitButtonTexture;
    sf::Texture selectArrowTextureLeft;
    sf::Texture selectArrowTextureRight;

    sf::Texture sidebarTexture;
    sf::Texture energyTexture;
    sf::Texture gearsTexture;
    sf::Texture damageTexture;
    sf::Texture rangeTexture;
    sf::Texture speedTexture;
    sf::Texture pierceTexture;
    sf::Texture splashRangeTexture;
    sf::Texture splashDamageTexture;
    sf::Texture moneyBoxTexture;
    sf::Texture livesBoxTexture;
    sf::Texture livesBarTexture;
    sf::Texture towerBoxTexture;
    std::vector<sf::Texture> terrainTextures;
    std::vector<sf::Texture> pathTextures;
    std::vector<sf::Texture> obstacleBases;
    std::vector<bool> obstacleHasTops;
    std::vector<sf::Texture> obstacleTops;
    std::vector<sf::Texture> towerTextures;
    sf::Texture towerMenuHoverTexture;
    sf::Texture placeableDiskTexture;

    std::vector<std::map<std::string, std::string>> towerData;

    sf::Font pixelFont;

    void create_menu();
    void create_play();
    void load_textures();
    void load_tower_data();

    void generate_obstacles();
    void show_dash();
    void fill_tower_box(const sf::Rect<float>&);

    int generate_path();
    void draw_path(int *, int *);
    void draw_line(int *, const int *, int dir);
    bool distances_ok();

    void pick_tower(int, const sf::Rect<float>&);
    void select_tower(int, Tower*, const sf::Rect<float>&);
    void show_stats(int, const sf::Rect<float>&);
    void show_stats(int, Tower*, const sf::Rect<float>&);
    void prev_focus(int, Tower*);
    void next_focus(int, Tower*);
    void unpick_tower();

    void end_game();
    void clean_up();
};


#endif //SFML_TEMPLATE_GAME_H
