//
// Created by Benjamin on 2/27/2022.
//

#include <sstream>
#include "TextBox.h"

std::string wordWrap(const std::string &text, float length, const sf::Font &font, unsigned int charSize) {
    std::istringstream iss(text);
    std::vector<std::string> results((std::istream_iterator<std::string>(iss)), std::istream_iterator<std::string>());

    sf::Text temp;
    temp.setFont(font);
    temp.setCharacterSize(charSize);

    std::string tempStr;
    std::string returnStr;
    for (const std::string &s : results) {
        tempStr += s + " ";
        temp.setString(tempStr);
        if (temp.getGlobalBounds().width < length) returnStr += s + " ";
        else {
            returnStr += "\n" + s + " ";
            tempStr = s;
        }
    }

    return returnStr;
}

TextBox::TextBox(const std::string &text, const sf::Font &font, unsigned int size, float x, float y, float w, int origin) : Label(wordWrap(text, w, font, size), font, size, x, y, origin) {}
