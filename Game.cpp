//
// Created by Benjamin on 1/25/2022.
//

#include <cmath>
#include <dirent.h>
#include <random>
#include <fstream>

#include "Button.h"
#include "Game.h"
#include "Path.h"
#include "Obstacle.h"
#include "Canopy.h"
#include "TowerGUI.h"
#include "Collision.h"
#include "TextBox.h"
#include "VariableLabel.h"

std::vector<std::string> open(const std::string &path) {
    std::vector<std::string> files;

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(path.c_str())) != nullptr) {
        /* print all the files and directories within directory */
        while ((ent = readdir(dir)) != nullptr) {
            files.emplace_back(ent->d_name);
        }
        closedir(dir);
    } else {
        /* could not open directory */
        perror("");
    }

    return files;
}

int wrap(int n, int const nLowerBound, int const nUpperBound)
{
    int range_size = nUpperBound - nLowerBound + 1;

    if (n < nLowerBound)
        n += range_size * ((nLowerBound - n) / range_size + 1);

    return nLowerBound + (n - nLowerBound) % range_size;
}

Game::Game() {
    // displayGroup "Hello, World!" -- this still appears in our Run terminal as before
    std::cout << "Hello, World!" << std::endl;

    // create a RenderWindow object
    // specify the size to be 640x640
    // set the title to be "SFML Example Window"
    auto mode = sf::VideoMode::getDesktopMode();
    mode.height += 1;

    window = new sf::RenderWindow(mode, "Tower Defense", sf::Style::None);
    screenRatio = sf::Vector2f(window->getSize().x/BASE_RESOLUTION.x, window->getSize().y/BASE_RESOLUTION.y);

    float fps;
    sf::Clock clock;
    sf::Time previousTime = clock.getElapsedTime();
    sf::Time currentTime;
    pixelFont.loadFromFile("./assets/fonts/FreePixel.ttf");
    Label fps_label = Label("0", pixelFont, 14, 10, 10, Label::TOP_LEFT);

    menu = Group(window);
    play = Group(window);

    bool show_fps = false;

    load_tower_data();
    create_menu();

    // while our window is open, keep it open
    // this is our draw loop
    while( window->isOpen() ) {
        window->clear( sf::Color::Black );           // clear the contents of the old frame
        // by setting the window to black

        //****************************************
        //  ADD ALL OF OUR DRAWING BELOW HERE
        //****************************************

        if (displayGroup == &menu) {
            menu.update();
            menu.draw();
        } else if (displayGroup == &play) {
            play.update();

            sf::Vector2f mousePosF(static_cast<float>( sf::Mouse::getPosition().x ), static_cast<float>( sf::Mouse::getPosition().y ));
            if (!mouseLeftMenu) {
                if (!sidebar->getGlobalBounds().contains(mousePosF)) {
                    mouseLeftMenu = true;
                }
            } else {
                sf::FloatRect windowRect(window->getPosition().x, window->getPosition().y, window->getSize().x, window->getSize().y);
                if (inHand && (sidebar->getGlobalBounds().contains(mousePosF) || !windowRect.contains(mousePosF))) {
                    play.kill(inHand);
                    inHand = nullptr;
                    unpick_tower();
                }
            }

            play.draw();
        }

        if (show_fps) {
            currentTime = clock.getElapsedTime();
            fps = 1.0f / (currentTime.asSeconds() - previousTime.asSeconds()); // the asSeconds returns a float
            previousTime = currentTime;

            fps_label.setString(std::to_string((int)floor(fps)));
            window->draw(fps_label);
        }

        //****************************************
        //  ADD ALL OF OUR DRAWING ABOVE HERE
        //****************************************

        window->display();                       // displayGroup the window

        //****************************************
        // HANDLE EVENTS BELOW HERE
        //****************************************
        sf::Event event{};
        while( window->pollEvent(event) ) {      // ask the window if any events occurred
            if( event.type == sf::Event::Closed ) { // if event type is a closed event
                // i.e. they clicked the X on the window
                window->close();
            } else if ( event.type == sf::Event::MouseButtonReleased ) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(*window);

                displayGroup->do_clicked(mousePos);
            } else if ( event.type == sf::Event::KeyPressed ) {
                if (event.key.code == sf::Keyboard::F1 ) {
                    show_fps = !show_fps;
                } else if ( event.key.code == sf::Keyboard::Escape ) {
                    if (inHand) {
                        play.kill(inHand);
                        inHand = nullptr;
                        unpick_tower();
                    }
                }
            }
        }
    }

    clean_up();
}

void Game::create_menu() {
    startButtonTexture.loadFromFile("./assets/textures/menu/start_button.png");
    quitButtonTexture.loadFromFile("./assets/textures/menu/quit_button.png");

    menu.empty();
    auto *playButton = new Button(startButtonTexture, window->getSize().x/2.0, window->getSize().y/2.0, ((window->getSize().x / 10.0)/startButtonTexture.getSize().x) * screenRatio.x, Sprite::CENTER, [this] { create_play(); });
    menu.add(playButton);
    menu.add(new Button(quitButtonTexture, playButton->getPosition().x, playButton->getPosition().y + 1.5 * playButton->getGlobalBounds().height, playButton->getScale().x, Sprite::CENTER, [this] { end_game(); }));

    displayGroup = &menu;
}

void Game::create_play() {
    load_textures();

    play.empty();
    while(generate_path() != 0);
    generate_obstacles();
    show_dash();

    displayGroup = &play;
}

void Game::load_textures() {
    placeableDiskTexture.loadFromFile("./assets/textures/gui/placeable_disk.png");

    selectArrowTextureLeft.loadFromFile("./assets/textures/gui/select_arrow_left.png");
    selectArrowTextureRight.loadFromFile("./assets/textures/gui/select_arrow_right.png");

    damageTexture.loadFromFile("./assets/textures/gui/damage_icon.png");
    rangeTexture.loadFromFile("./assets/textures/gui/range_icon.png");
    speedTexture.loadFromFile("./assets/textures/gui/attack_speed_icon.png");
    pierceTexture.loadFromFile("./assets/textures/gui/pierce_icon.png");
    splashDamageTexture.loadFromFile("./assets/textures/gui/splash_damage_icon.png");
    splashRangeTexture.loadFromFile("./assets/textures/gui/splash_radius.png");

    terrainTextures.clear();
    pathTextures.clear();
    towerTextures.clear();

    //Load terrain textures
    for (const std::string &s : open((std::string) "./assets/textures/environments/terrain")) {
        if (s.find(std::to_string(environment) + "-") == std::string::npos) continue;
        terrainTextures.emplace_back();
        terrainTextures[terrainTextures.size() - 1].loadFromFile("./assets/textures/environments/terrain/" + s);
    }

    //Load path textures
    for (const std::string &s : open((std::string) "./assets/textures/environments/paths")) {
        if (s.find(std::to_string(environment) + "-") == std::string::npos) continue;
        pathTextures.emplace_back();
        pathTextures[pathTextures.size() - 1].loadFromFile("./assets/textures/environments/paths/" + s);
    }

    //Load obstacle textures
    for (std::string &s : open((std::string) "./assets/textures/environments/obstacles")) {
        if (s.find(std::to_string(environment) + "-") == std::string::npos || s.find("_top") != std::string::npos) continue;
        obstacleBases.emplace_back();
        obstacleBases[obstacleBases.size() - 1].loadFromFile("./assets/textures/environments/obstacles/" + s);
        if (s.find("_bottom") != std::string::npos) {
            obstacleHasTops.push_back(true);
            obstacleTops.emplace_back();
            int index;
            while((index = s.find("_bottom")) != std::string::npos) {
                s.replace(index, 7, "_top"); //remove and replace from that position
            }
            obstacleTops[obstacleTops.size() - 1].loadFromFile("./assets/textures/environments/obstacles/" + s);
        } else {
            obstacleHasTops.push_back(false);
        }
    }

    //Load tower textures
    for (const std::string &s : open((std::string) "./assets/textures/towers")) {
        if (s == "." || s == "..") continue;
        towerTextures.emplace_back();
        towerTextures[towerTextures.size() - 1].loadFromFile("./assets/textures/towers/" + s);
    }
}

void Game::load_tower_data() {
    std::ifstream dataFile("./assets/data/towers.dat");
    towerData.emplace_back();

    std::string line;
    while (std::getline(dataFile, line)) {
        if (line.empty()) {
            towerData.emplace_back();
        } else {
            unsigned int ind = line.find_first_of(' ');
            towerData[towerData.size() - 1].insert({line.substr(0, ind), line.substr(ind + 1, line.size() - ind)});
        }
    }

    dataFile.close();
}

void Game::generate_obstacles() {
    static std::mt19937 rng( time(nullptr) );
    float obstaclePerc = DIFFICULTY_OBSTACLE_PERCS[pathDifficulty]/100.0;
    float baseScale = 1.5;
    float obstacleScale = screenRatio.x * baseScale;

    float area = 0;
    while (area/(BASE_RESOLUTION.y * 4*(BASE_RESOLUTION.y/3)) < obstaclePerc) {
        int ind = std::uniform_int_distribution<int>(0, obstacleBases.size() - 1)(rng);
        sf::Texture t = obstacleBases[ind];
        area += (t.getSize().x * t.getSize().y * baseScale * baseScale);

        auto* obstacle = new Obstacle(obstacleBases[ind]);
        obstacle->setOrigin(Sprite::CENTER);
        obstacle->setScale(obstacleScale, obstacleScale);

        bool colliding = true;
        int angle, x, y;
        while (colliding) {
            angle = std::uniform_int_distribution<int>(0, 359)(rng);
            obstacle->setRotation(angle);

            x = std::uniform_int_distribution<int>(mapCorner + obstacle->getGlobalBounds().width/2.0, window->getSize().x - obstacle->getGlobalBounds().width/2.0)(rng);
            y = std::uniform_int_distribution<int>(obstacle->getGlobalBounds().height/2.0, mapBottom - obstacle->getGlobalBounds().height/2.0)(rng);
            obstacle->setPosition(x, y);

            colliding = false;
            for (auto *s : *play.getLayerSprites(PATH_LAYER)) {
                if (obstacle->getGlobalBounds().intersects(s->getGlobalBounds()) && Collision::PixelPerfectTest(*obstacle, *s)) {
                    colliding = true;
                    break;
                }
            }

            if (colliding) {
                continue;
            }

            //TODO fix this double usage
            if (play.getLayerSprites(OBSTACLE_LAYER) != nullptr) {
                for (auto *s : *play.getLayerSprites(OBSTACLE_LAYER)) {
                    if (obstacle->getGlobalBounds().intersects(s->getGlobalBounds()) && Collision::PixelPerfectTest(*obstacle, *s)) {
                        colliding = true;
                        break;
                    }
                }
            }
        }

        play.add(obstacle, OBSTACLE_LAYER);

        if (obstacleHasTops[ind]) {
            int canopyInd = 0;
            for (int i = 0; i < ind; i++) {
                if (obstacleHasTops[i]) {
                    canopyInd++;
                }
            }

            play.add(new Canopy(obstacleTops[canopyInd], x, y, std::uniform_int_distribution<int>(0, 359)(rng), obstacleScale, Sprite::CENTER), CANOPY_LAYER);
        }
    }
}

int Game::generate_path() {
    static std::mt19937 rng( time(nullptr) );

    int startPoint[2];
    int innerStartPoint[2];
    int endPoint[2];
    int innerEndPoint[2];

    int numWaypoints = DIFFICULTY_WAYPOINTS[pathDifficulty][std::uniform_int_distribution<int>(0, 1)(rng)];
    int startSide = std::uniform_int_distribution<int>(0, 1)(rng);
    if (startSide == 0) {
        startPoint[0] = 0;
        startPoint[1] = std::uniform_int_distribution<int>(1, 28)(rng);

        innerStartPoint[0] = 1;
        innerStartPoint[1] = startPoint[1];
    } else {
        startPoint[0] = std::uniform_int_distribution<int>(1, 38)(rng);
        startPoint[1] = 0;

        innerStartPoint[0] = startPoint[0];
        innerStartPoint[1] = 1;
    }

    int endSide = std::uniform_int_distribution<int>(0, 1)(rng);
    if (endSide == 0) {
        endPoint[0] = 39;
        endPoint[1] = std::uniform_int_distribution<int>(1, 28)(rng);

        innerEndPoint[0] = 38;
        innerEndPoint[1] = endPoint[1];
    } else {
        endPoint[0] = std::uniform_int_distribution<int>(1, 38)(rng);
        endPoint[1] = 29;

        innerEndPoint[0] = endPoint[0];
        innerEndPoint[1] = 28;
    }

    waypoints = {sf::Vector2i(startPoint[0], startPoint[1]), sf::Vector2i(innerStartPoint[0], innerStartPoint[1])};
    int prevWaypoint[2] = { innerStartPoint[0], innerStartPoint[1] };

    for (int i = 0; i < numWaypoints; i++) {
        int newWaypoint[2] = { waypoints[waypoints.size() - 1].x, waypoints[waypoints.size() - 1].y };
        while (newWaypoint[0] == prevWaypoint[0] || newWaypoint[1] == prevWaypoint[1] || (i == numWaypoints - 1 && (newWaypoint[0] == innerEndPoint[0] || newWaypoint[1] == innerEndPoint[1]))) {
            newWaypoint[0] = std::uniform_int_distribution<int>(1, 38)(rng);
            newWaypoint[1] = std::uniform_int_distribution<int>(1, 28)(rng);
        }
        int endOfLastPath[2] = { waypoints[waypoints.size() - 1].x, waypoints[waypoints.size() - 1].y };
        draw_path(endOfLastPath, newWaypoint);
        prevWaypoint[0] = newWaypoint[0];
        prevWaypoint[1] = newWaypoint[1];
    }

    int endOfLastPath[2] = { waypoints[waypoints.size() - 1].x, waypoints[waypoints.size() - 1].y };
    draw_path(endOfLastPath, innerEndPoint);
    waypoints.emplace_back(endPoint[0], endPoint[1]);

    if (!distances_ok()) {
        return 1;
    }

    float tileWidth = window->getSize().y/30.0;
    mapCorner = window->getSize().x - tileWidth * 40;
    mapBottom = tileWidth * 30;

    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            float x = mapCorner + j * tileWidth;
            float y = i * tileWidth;

            bool found = false;
            for (auto &waypoint : waypoints) {
                if (waypoint.x == j && waypoint.y == i) {
                    found = true;
                    break;
                }
            }

            if (found) {
                play.add(new Path(pathTextures[std::uniform_int_distribution<int>(0, pathTextures.size() - 1)(rng)], x, y, tileWidth / ((float) pathTextures[0].getSize().y), Sprite::TOP_LEFT), PATH_LAYER);
            } else {
                play.add(new Terrain(terrainTextures[std::uniform_int_distribution<int>(0, terrainTextures.size() - 1)(rng)], x, y, tileWidth / ((float) terrainTextures[0].getSize().y), Sprite::TOP_LEFT), TERRAIN_LAYER);
            }
        }
    }

    return 0;
}

void Game::draw_path(int *a, int *b) {
    static std::mt19937 rng(time(nullptr) );

    int order = std::uniform_int_distribution<int>(0, 1)(rng);

    if (order == 0) {
        for (auto & waypoint : waypoints) {
            if (waypoint.x == a[0] && waypoint.y == a[1] - abs(a[1] - b[1])/(a[1] - b[1])) {
                order = 1;
                break;
            }
        }
    } else {
        for (auto & waypoint : waypoints) {
            if (waypoint.x == a[0] - abs(a[0] - b[0])/(a[0] - b[0]) && waypoint.y == a[1]) {
                order = 0;
                break;
            }
        }
    }

    if (order == 0) {
        draw_line(a, b, 1); //vertical
        int endOfLastPath[2] = { waypoints[waypoints.size() - 1].x, waypoints[waypoints.size() - 1].y };
        draw_line(endOfLastPath, b, 0); //horz
    } else {
        draw_line(a, b, 0); //horz
        int endOfLastPath[2] = { waypoints[waypoints.size() - 1].x, waypoints[waypoints.size() - 1].y };
        draw_line(endOfLastPath, b, 1); //vertical
    }
}

void Game::draw_line(int *a, const int *b, int dir) {
    int diff = a[dir] - b[dir];

    int paired = 1;
    if (dir == 1) {
        paired = 0;
    }

    if (diff != 0) {
        int l = 1;
        int h = abs(diff) + 1;

        for (int i = l; i < h; i++) {
            int tile = i * abs(diff)/diff;

            int newWaypoint[2] = { a[paired] , a[dir] - tile};
            if (dir == 0) {
                newWaypoint[0] = a[dir] - tile;
                newWaypoint[1] = a[paired];
            }

            waypoints.emplace_back(newWaypoint[0], newWaypoint[1]);
        }
    }
}

bool Game::distances_ok() {
    int too_short = 0;
    for (int i = 0; i < waypoints.size(); i++) {
        for (int j = 0; j < waypoints.size(); j++) {
            if (abs(i - j) > 1 && sqrt(pow(waypoints[i].x - waypoints[j].x, 2) + pow(waypoints[i].y - waypoints[j].y, 2)) == 1) {
                too_short++;
            }
        }
    }

    return too_short <= 10;
}

void Game::show_dash() {
    sidebarTexture.loadFromFile("./assets/textures/gui/backdrop.png");
    energyTexture.loadFromFile("./assets/textures/gui/energy.png");
    gearsTexture.loadFromFile("./assets/textures/gui/gears.png");
    moneyBoxTexture.loadFromFile("./assets/textures/gui/money_box.png");
    livesBoxTexture.loadFromFile("./assets/textures/gui/lives_box.png");
    livesBarTexture.loadFromFile("./assets/textures/gui/health_bar.png");
    towerBoxTexture.loadFromFile("./assets/textures/gui/tower_box.png");

    sidebar = new Sprite(sidebarTexture, mapCorner, mapBottom, window->getSize().y/((float)sidebarTexture.getSize().y), Sprite::BOTTOM_RIGHT);
    play.add(sidebar, GUI_LAYER);

    float tileWidthRatio = (TILE_UNIT / 30.0) * screenRatio.y;
    float tileWidth = TILE_UNIT * screenRatio.y;
    float moneyBoxHeight = 70 * tileWidthRatio;
    float livesBoxHeight = 40 * tileWidthRatio;
    float towerBoxHeight = 540 * tileWidthRatio;
    float spacing = window->getSize().y / 72;
    float total_height = moneyBoxHeight + livesBoxHeight + towerBoxHeight + (2 * spacing);
    float gui_start_y = (window->getSize().y - total_height) / 2.0;
    float marginRight = 18 * window->getSize().y/((float)sidebarTexture.getSize().y);

    auto moneyBox = new Sprite(moneyBoxTexture, mapCorner - (marginRight + mapCorner + moneyBoxTexture.getSize().x * tileWidthRatio)/2.0, gui_start_y, moneyBoxHeight/((float)moneyBoxTexture.getSize().y), Sprite::TOP_LEFT);
    play.add(moneyBox, GUI_LAYER);
    auto energyIcon = new Sprite(energyTexture, moneyBox->getPosition().x + tileWidth / 4.0, moneyBox->getPosition().y + tileWidth / 4.0, tileWidth/((float)energyTexture.getSize().y), Sprite::TOP_LEFT);
    play.add(energyIcon, GUI_LAYER);
    auto gearsIcon = new Sprite(gearsTexture, moneyBox->getPosition().x + tileWidth / 4.0, moneyBox->getPosition().y + tileWidth * 1.25, tileWidth/((float)gearsTexture.getSize().y), Sprite::TOP_LEFT);
    play.add(gearsIcon, GUI_LAYER);
    play.add(new VariableLabel(&energy, pixelFont, TILE_UNIT*screenRatio.y, energyIcon->getPosition().x + tileWidth * 1.25, energyIcon->getPosition().y + tileWidth/2.0, Label::LEFT_CENTER), GUI_LAYER);
    play.add(new VariableLabel(&gears, pixelFont, TILE_UNIT*screenRatio.y, gearsIcon->getPosition().x + tileWidth * 1.25, gearsIcon->getPosition().y + tileWidth/2.0, Label::LEFT_CENTER), GUI_LAYER);
    auto livesBox = new Sprite(livesBoxTexture, mapCorner - (marginRight + mapCorner + livesBoxTexture.getSize().x * tileWidthRatio)/2.0, gui_start_y + moneyBoxHeight + spacing, livesBoxHeight/((float)livesBoxTexture.getSize().y), Sprite::TOP_LEFT);
    play.add(livesBox, GUI_LAYER);
    play.add(new Sprite(livesBarTexture, livesBox->getPosition().x + livesBox->getGlobalBounds().width/2.0, livesBox->getPosition().y + livesBox->getGlobalBounds().height/2.0, tileWidth/32.0, Sprite::CENTER), GUI_LAYER);
    play.add(new Label(std::to_string(health), pixelFont, TILE_UNIT*screenRatio.y, livesBox->getPosition().x + livesBox->getGlobalBounds().width/2.0, livesBox->getPosition().y + livesBox->getGlobalBounds().height/2.0, Label::CENTER), GUI_LAYER);

    auto towerBox = new Sprite(towerBoxTexture, mapCorner - (marginRight + mapCorner + towerBoxTexture.getSize().x * tileWidthRatio)/2.0, gui_start_y + moneyBoxHeight + livesBoxHeight + 2*spacing, towerBoxHeight/((float)towerBoxTexture.getSize().y), Sprite::TOP_LEFT);
    play.add(towerBox, GUI_LAYER);

    fill_tower_box(towerBox->getGlobalBounds());
}

void Game::fill_tower_box(const sf::Rect<float>& bounds) {
    sf::Vector2f pos(bounds.left, bounds.top);

    float towerWidth = bounds.width / 4;
    float startOffset = towerWidth / 4;

    towerMenuHoverTexture.loadFromFile("./assets/textures/gui/hover_tower.png");

    for (int i = 0; i < towerTextures.size(); i++) {
        int r = i/3;
        int c = i%3;
        auto *s = new Sprite(towerMenuHoverTexture, pos.x + startOffset + towerWidth * 1.25 * c, pos.y + startOffset + towerWidth * 1.5 * r, screenRatio.x, Sprite::TOP_LEFT);
        play.add(new TowerGUI(towerTextures[i], s->getPosition().x + s->getGlobalBounds().width/2, s->getPosition().y + s->getGlobalBounds().height/2, towerWidth/((float)towerTextures[i].getSize().x), Sprite::CENTER, [this, i, bounds] { pick_tower(i, bounds); }, s), TOWER_BUTTONS_LAYER);
    }
}

void Game::pick_tower(int i, const sf::Rect<float>& towerBoxBounds) {
    mouseLeftMenu = false;

    std::vector<int> layers = {PATH_LAYER, OBSTACLE_LAYER, TOWER_LAYER, GUI_LAYER};

    inHand = new Tower(towerTextures[i], sf::Mouse::getPosition().x, sf::Mouse::getPosition().y, screenRatio.x, Sprite::CENTER, std::stof(towerData[i].at("range")), TILE_UNIT, std::stoi(towerData[i].at("price")), &energy, &inHand, layers, &play, new Sprite(placeableDiskTexture, Sprite::CENTER), [this] { unpick_tower(); });
    Placeable *p = inHand;
    inHand->setCommand([this, i, p, towerBoxBounds] { select_tower(i, (Tower *)p, towerBoxBounds); });
    play.add(inHand, TOWER_LAYER);
    play.hide_layer(TOWER_BUTTONS_LAYER);

    show_stats(i, towerBoxBounds);
}

void Game::select_tower(int i, Tower* tower, const sf::Rect<float>& towerBoxBounds) {
    play.empty_layer(TOWER_DESC_LAYER);
    play.hide_layer(TOWER_BUTTONS_LAYER);

    show_stats(i, tower, towerBoxBounds);
}

void Game::show_stats(int i, const sf::Rect<float>& towerBoxBounds) {
    float iconSpacing = (towerBoxBounds.width - (TILE_UNIT * screenRatio.y) / 2.0)/3.0;
    float bottomOffsetMult = 3;
    if (towerData[i].find("pierce") != towerData[i].end() || towerData[i].find("splash_radius") != towerData[i].end()) {
        bottomOffsetMult = 4;
    }

    auto icon = new Sprite(towerTextures[i], towerBoxBounds.left + (TILE_UNIT * screenRatio.y) / 4.0, towerBoxBounds.top + (TILE_UNIT * screenRatio.y) / 2.0, (towerBoxBounds.width/6.0)/((float)towerTextures[i].getSize().x), Sprite::TOP_LEFT);
    play.add(icon, TOWER_DESC_LAYER);
    play.add(new Label(towerData[i]["name"], pixelFont, TILE_UNIT*screenRatio.y, icon->getPosition().x + icon->getGlobalBounds().width + (12 * screenRatio.y), icon->getPosition().y + icon->getGlobalBounds().height/2.0, Label::LEFT_CENTER), TOWER_DESC_LAYER);

    play.add(new TextBox(towerData[i]["description"], pixelFont, 16 * screenRatio.y, icon->getPosition().x, icon->getPosition().y + icon->getGlobalBounds().height, towerBoxBounds.width - icon->getPosition().x, Label::TOP_LEFT), TOWER_DESC_LAYER);

    float priceIconY = towerBoxBounds.top + towerBoxBounds.height - (TILE_UNIT * screenRatio.y) * bottomOffsetMult;
    float priceIconHeight = TILE_UNIT * screenRatio.y;

    auto energyIcon = new Sprite(energyTexture, towerBoxBounds.left + (TILE_UNIT * screenRatio.y) / 4.0, priceIconY, (TILE_UNIT * screenRatio.y) / ((float) energyTexture.getSize().y), Sprite::TOP_LEFT);
    play.add(energyIcon, TOWER_DESC_LAYER);
    play.add(new Label(towerData[i]["price"], pixelFont, TILE_UNIT * screenRatio.y, energyIcon->getPosition().x + energyIcon->getGlobalBounds().width + (12 * screenRatio.y), energyIcon->getPosition().y + energyIcon->getGlobalBounds().height / 2.0, Label::LEFT_CENTER), TOWER_DESC_LAYER);

    auto damageIcon = new Sprite(damageTexture, towerBoxBounds.left + (TILE_UNIT * screenRatio.y) / 4.0, priceIconY + priceIconHeight + (12 * screenRatio.y), (TILE_UNIT * screenRatio.y)/((float)energyTexture.getSize().y), Sprite::TOP_LEFT);
    play.add(damageIcon, TOWER_DESC_LAYER);
    play.add(new Label(towerData[i]["damage"], pixelFont, TILE_UNIT*screenRatio.y, damageIcon->getPosition().x + damageIcon->getGlobalBounds().width + (12 * screenRatio.y), damageIcon->getPosition().y + damageIcon->getGlobalBounds().height/2.0, Label::LEFT_CENTER), TOWER_DESC_LAYER);
    auto rangeIcon = new Sprite(rangeTexture, damageIcon->getPosition().x + iconSpacing, damageIcon->getPosition().y, (TILE_UNIT * screenRatio.y)/((float)energyTexture.getSize().y), Sprite::TOP_LEFT);
    play.add(rangeIcon, TOWER_DESC_LAYER);
    play.add(new Label(towerData[i]["range"], pixelFont, TILE_UNIT*screenRatio.y, rangeIcon->getPosition().x + rangeIcon->getGlobalBounds().width + (12 * screenRatio.y), rangeIcon->getPosition().y + rangeIcon->getGlobalBounds().height/2.0, Label::LEFT_CENTER), TOWER_DESC_LAYER);
    auto speedIcon = new Sprite(speedTexture, rangeIcon->getPosition().x + iconSpacing, damageIcon->getPosition().y, (TILE_UNIT * screenRatio.y)/((float)energyTexture.getSize().y), Sprite::TOP_LEFT);
    play.add(speedIcon, TOWER_DESC_LAYER);
    play.add(new Label(towerData[i]["speed"], pixelFont, TILE_UNIT*screenRatio.y, speedIcon->getPosition().x + speedIcon->getGlobalBounds().width + (12 * screenRatio.y), speedIcon->getPosition().y + speedIcon->getGlobalBounds().height/2.0, Label::LEFT_CENTER), TOWER_DESC_LAYER);

    float offset = 0;
    if (towerData[i].find("pierce") != towerData[i].end()) {
        auto pierceIcon = new Sprite(pierceTexture, damageIcon->getPosition().x, damageIcon->getPosition().y + damageIcon->getGlobalBounds().height + (12 * screenRatio.y), (TILE_UNIT * screenRatio.y)/((float)energyTexture.getSize().y), Sprite::TOP_LEFT);
        play.add(pierceIcon, TOWER_DESC_LAYER);
        play.add(new Label(towerData[i]["pierce"], pixelFont, TILE_UNIT*screenRatio.y, pierceIcon->getPosition().x + pierceIcon->getGlobalBounds().width + (12 * screenRatio.y), pierceIcon->getPosition().y + pierceIcon->getGlobalBounds().height/2.0, Label::LEFT_CENTER), TOWER_DESC_LAYER);
        offset++;
    }

    if (towerData[i].find("splash_radius") != towerData[i].end()) {
        auto splashRadiusIcon = new Sprite(splashRangeTexture, damageIcon->getPosition().x + iconSpacing*offset, damageIcon->getPosition().y + damageIcon->getGlobalBounds().height + (12 * screenRatio.y), (TILE_UNIT * screenRatio.y)/((float)energyTexture.getSize().y), Sprite::TOP_LEFT);
        play.add(splashRadiusIcon, TOWER_DESC_LAYER);
        play.add(new Label(towerData[i]["splash_radius"], pixelFont, TILE_UNIT*screenRatio.y, splashRadiusIcon->getPosition().x + splashRadiusIcon->getGlobalBounds().width + (12 * screenRatio.y), splashRadiusIcon->getPosition().y + splashRadiusIcon->getGlobalBounds().height/2.0, Label::LEFT_CENTER), TOWER_DESC_LAYER);
        offset++;

        auto splashDmgIcon = new Sprite(splashDamageTexture, damageIcon->getPosition().x + iconSpacing*offset, damageIcon->getPosition().y + damageIcon->getGlobalBounds().height + (12 * screenRatio.y), (TILE_UNIT * screenRatio.y)/((float)energyTexture.getSize().y), Sprite::TOP_LEFT);
        play.add(splashDmgIcon, TOWER_DESC_LAYER);
        play.add(new Label(towerData[i]["splash_damage"], pixelFont, TILE_UNIT*screenRatio.y, splashDmgIcon->getPosition().x + splashDmgIcon->getGlobalBounds().width + (12 * screenRatio.y), splashDmgIcon->getPosition().y + splashDmgIcon->getGlobalBounds().height/2.0, Label::LEFT_CENTER), TOWER_DESC_LAYER);
    }
}

void Game::show_stats(int i, Tower* tower, const sf::Rect<float>& towerBoxBounds) {
    float iconSpacing = (towerBoxBounds.width - (TILE_UNIT * screenRatio.y) / 2.0)/3.0;
    float bottomOffsetMult = 3;
    if (towerData[i].find("pierce") != towerData[i].end() || towerData[i].find("splash_radius") != towerData[i].end()) {
        bottomOffsetMult = 4;
    }

    auto icon = new Sprite(towerTextures[i], towerBoxBounds.left + (TILE_UNIT * screenRatio.y) / 4.0, towerBoxBounds.top + (TILE_UNIT * screenRatio.y) / 2.0, (towerBoxBounds.width/6.0)/((float)towerTextures[i].getSize().x), Sprite::TOP_LEFT);
    play.add(icon, TOWER_DESC_LAYER);
    play.add(new Label(towerData[i]["name"], pixelFont, TILE_UNIT*screenRatio.y, icon->getPosition().x + icon->getGlobalBounds().width + (12 * screenRatio.y), icon->getPosition().y + icon->getGlobalBounds().height/2.0, Label::LEFT_CENTER), TOWER_DESC_LAYER);

    float descY = icon->getPosition().y + icon->getGlobalBounds().height;

    focus1Label = new Label(foci_strings[tower->getFocus1()], pixelFont, 20 * screenRatio.y, towerBoxBounds.left + towerBoxBounds.width/2.0, descY, Label::CENTER);
    play.add(focus1Label, TOWER_DESC_LAYER);
    play.add(new Button(selectArrowTextureLeft, focus1Label->getPosition().x - 1.5*TILE_UNIT*screenRatio.y, focus1Label->getPosition().y, (20*screenRatio.y)/((float)selectArrowTextureLeft.getSize().y), Button::CENTER, [this, tower] { prev_focus(1, tower); }), TOWER_DESC_LAYER);
    play.add(new Button(selectArrowTextureRight, focus1Label->getPosition().x + 1.5*TILE_UNIT*screenRatio.y, focus1Label->getPosition().y, (20*screenRatio.y)/((float)selectArrowTextureLeft.getSize().y), Button::CENTER, [this, tower] { next_focus(1, tower); }), TOWER_DESC_LAYER);
    focus2Label = new Label(foci_strings[tower->getFocus2()], pixelFont, 20 * screenRatio.y, towerBoxBounds.left + towerBoxBounds.width/2.0, focus1Label->getPosition().y + focus1Label->getCharacterSize() + 6*screenRatio.y, Label::CENTER);
    play.add(focus2Label, TOWER_DESC_LAYER);
    play.add(new Button(selectArrowTextureLeft, focus2Label->getPosition().x - 1.5*TILE_UNIT*screenRatio.y, focus2Label->getPosition().y, (20*screenRatio.y)/((float)selectArrowTextureLeft.getSize().y), Button::CENTER, [this, tower] { prev_focus(2, tower); }), TOWER_DESC_LAYER);
    play.add(new Button(selectArrowTextureRight, focus2Label->getPosition().x + 1.5*TILE_UNIT*screenRatio.y, focus2Label->getPosition().y, (20*screenRatio.y)/((float)selectArrowTextureLeft.getSize().y), Button::CENTER, [this, tower] { next_focus(2, tower); }), TOWER_DESC_LAYER);
    play.add(new Label(foci_strings[0], pixelFont, 20 * screenRatio.y, towerBoxBounds.left + towerBoxBounds.width/2.0, focus2Label->getPosition().y + focus2Label->getCharacterSize() + 6*screenRatio.y, Label::CENTER), TOWER_DESC_LAYER);
    descY = descY + 2*TILE_UNIT * screenRatio.y;

    play.add(new TextBox(towerData[i]["description"], pixelFont, 16 * screenRatio.y, icon->getPosition().x, descY, towerBoxBounds.width - icon->getPosition().x, Label::TOP_LEFT), TOWER_DESC_LAYER);

    float priceIconY = towerBoxBounds.top + towerBoxBounds.height - (TILE_UNIT * screenRatio.y) * bottomOffsetMult;
    float priceIconHeight = TILE_UNIT * screenRatio.y;

    auto damageIcon = new Sprite(damageTexture, towerBoxBounds.left + (TILE_UNIT * screenRatio.y) / 4.0, priceIconY + priceIconHeight + (12 * screenRatio.y), (TILE_UNIT * screenRatio.y)/((float)energyTexture.getSize().y), Sprite::TOP_LEFT);
    play.add(damageIcon, TOWER_DESC_LAYER);
    play.add(new Label(towerData[i]["damage"], pixelFont, TILE_UNIT*screenRatio.y, damageIcon->getPosition().x + damageIcon->getGlobalBounds().width + (12 * screenRatio.y), damageIcon->getPosition().y + damageIcon->getGlobalBounds().height/2.0, Label::LEFT_CENTER), TOWER_DESC_LAYER);
    auto rangeIcon = new Sprite(rangeTexture, damageIcon->getPosition().x + iconSpacing, damageIcon->getPosition().y, (TILE_UNIT * screenRatio.y)/((float)energyTexture.getSize().y), Sprite::TOP_LEFT);
    play.add(rangeIcon, TOWER_DESC_LAYER);
    play.add(new Label(towerData[i]["range"], pixelFont, TILE_UNIT*screenRatio.y, rangeIcon->getPosition().x + rangeIcon->getGlobalBounds().width + (12 * screenRatio.y), rangeIcon->getPosition().y + rangeIcon->getGlobalBounds().height/2.0, Label::LEFT_CENTER), TOWER_DESC_LAYER);
    auto speedIcon = new Sprite(speedTexture, rangeIcon->getPosition().x + iconSpacing, damageIcon->getPosition().y, (TILE_UNIT * screenRatio.y)/((float)energyTexture.getSize().y), Sprite::TOP_LEFT);
    play.add(speedIcon, TOWER_DESC_LAYER);
    play.add(new Label(towerData[i]["speed"], pixelFont, TILE_UNIT*screenRatio.y, speedIcon->getPosition().x + speedIcon->getGlobalBounds().width + (12 * screenRatio.y), speedIcon->getPosition().y + speedIcon->getGlobalBounds().height/2.0, Label::LEFT_CENTER), TOWER_DESC_LAYER);

    float offset = 0;
    if (towerData[i].find("pierce") != towerData[i].end()) {
        auto pierceIcon = new Sprite(pierceTexture, damageIcon->getPosition().x, damageIcon->getPosition().y + damageIcon->getGlobalBounds().height + (12 * screenRatio.y), (TILE_UNIT * screenRatio.y)/((float)energyTexture.getSize().y), Sprite::TOP_LEFT);
        play.add(pierceIcon, TOWER_DESC_LAYER);
        play.add(new Label(towerData[i]["pierce"], pixelFont, TILE_UNIT*screenRatio.y, pierceIcon->getPosition().x + pierceIcon->getGlobalBounds().width + (12 * screenRatio.y), pierceIcon->getPosition().y + pierceIcon->getGlobalBounds().height/2.0, Label::LEFT_CENTER), TOWER_DESC_LAYER);
        offset++;
    }

    if (towerData[i].find("splash_radius") != towerData[i].end()) {
        auto splashRadiusIcon = new Sprite(splashRangeTexture, damageIcon->getPosition().x + iconSpacing*offset, damageIcon->getPosition().y + damageIcon->getGlobalBounds().height + (12 * screenRatio.y), (TILE_UNIT * screenRatio.y)/((float)energyTexture.getSize().y), Sprite::TOP_LEFT);
        play.add(splashRadiusIcon, TOWER_DESC_LAYER);
        play.add(new Label(towerData[i]["splash_radius"], pixelFont, TILE_UNIT*screenRatio.y, splashRadiusIcon->getPosition().x + splashRadiusIcon->getGlobalBounds().width + (12 * screenRatio.y), splashRadiusIcon->getPosition().y + splashRadiusIcon->getGlobalBounds().height/2.0, Label::LEFT_CENTER), TOWER_DESC_LAYER);
        offset++;

        auto splashDmgIcon = new Sprite(splashDamageTexture, damageIcon->getPosition().x + iconSpacing*offset, damageIcon->getPosition().y + damageIcon->getGlobalBounds().height + (12 * screenRatio.y), (TILE_UNIT * screenRatio.y)/((float)energyTexture.getSize().y), Sprite::TOP_LEFT);
        play.add(splashDmgIcon, TOWER_DESC_LAYER);
        play.add(new Label(towerData[i]["splash_damage"], pixelFont, TILE_UNIT*screenRatio.y, splashDmgIcon->getPosition().x + splashDmgIcon->getGlobalBounds().width + (12 * screenRatio.y), splashDmgIcon->getPosition().y + splashDmgIcon->getGlobalBounds().height/2.0, Label::LEFT_CENTER), TOWER_DESC_LAYER);
    }
}

void Game::prev_focus(int focus, Tower* tower) {
    if (focus == 1) {
        tower->setFocus1(wrap(tower->getFocus1() - 1, 0, 10));
        focus1Label->setString(foci_strings[tower->getFocus1()]);
    } else {
        tower->setFocus2(wrap(tower->getFocus2() - 1, 0, 10));
        focus2Label->setString(foci_strings[tower->getFocus2()]);
    }
}

void Game::next_focus(int focus, Tower* tower) {
    if (focus == 1) {
        tower->setFocus1(wrap(tower->getFocus1() + 1, 0, 10));
        focus1Label->setString(foci_strings[tower->getFocus1()]);
    } else {
        tower->setFocus2(wrap(tower->getFocus2() + 1, 0, 10));
        focus2Label->setString(foci_strings[tower->getFocus2()]);
    }
}

void Game::unpick_tower() {
    play.empty_layer(TOWER_DESC_LAYER);
    play.show_layer(TOWER_BUTTONS_LAYER);
}

void Game::end_game() {
    window->close();
}

void Game::clean_up() {
    menu.empty();
    play.empty();
    delete window;
}
