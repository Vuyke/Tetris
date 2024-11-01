#pragma once
#include "constants.h"
#include "font_data.h"
using sf::Color;

sf::Font& getFont() {
    static sf::Font font;
    if (!font.loadFromMemory(ide_ttf, ide_ttf_len))
    {
        std::cerr << "Error loading font!" << std::endl;
    }
    return font;
}

const sf::Font font = getFont();

sf::Text setText(std::string s, int poz1, int poz2, int sz = 40, int p = 0, Color fill = Color(220, 220, 220), Color outline = sf::Color(60, 60, 60)) {
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(sz);
    text.setFillColor(sf::Color::White);
    text.setPosition(poz1, poz2);
    text.setString(s);
    if(p) {
        text.setOutlineColor(outline);
        text.setOutlineThickness(p);
        text.setFillColor(fill);
    }
    return text;
}

sf::RectangleShape setRectangle(std::pair<int, int> sz, std::pair<int, int> pos, int thick = 0, Color outline = Color::White, Color fill = Color::Black) {
    sf::RectangleShape cur(sf::Vector2f(sz.first, sz.second));
    cur.setFillColor(fill);
    cur.setOutlineColor(outline);
    cur.setOutlineThickness(thick);
    cur.setPosition(pos.first, pos.second);
    return cur;
}

void setCursor(sf::Cursor& cursor, sf::Cursor::Type c) {
    if(!cursor.loadFromSystem(c)) {
        std::cerr << "Error loading cursor!" << std::endl;
    }
}

void setMusic(sf::Music& music) {
    if(!music.openFromFile("tetris_theme.ogg")) {
        std::cerr << "Error loading music!" << std::endl;
    }
    music.setLoop(true);
}

void draw(sf::RenderWindow& window, std::vector<sf::Text>& texts, std::vector<sf::RectangleShape>& shapes) {
    window.clear();
    for(auto &t : shapes)
        window.draw(t);
    for(auto& t: texts)
        window.draw(t);
    window.display();
}

void setnextPieceGUI() {
    nextPieceGUI.clear();
    int n = nextPiece.v.size(), outWidth = 2;
    int curY = nextOfY + (NEXT - n) * sz / 2;
    int curX = nextOfX + sz;
    if(n == 4) curX -= sz - 2;
    sf::RectangleShape squareColor(sf::Vector2f(sz, sz));
    squareColor.setFillColor(nextPiece.color);
    squareColor.setOutlineColor(BACKGROUND_OUTLINE);
    squareColor.setOutlineThickness(outWidth);
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            if(nextPiece.v[i][j]) {
                squareColor.setPosition(curY + j * (sz + outWidth), curX + i * (sz + outWidth));
                nextPieceGUI.push_back(squareColor);
            }
        }
    }
}

sf::Color reduceOpacity(sf::Color& c) {
    return sf::Color(c.r, c.g, c.b, 128);
}

sf::Color addColors(sf::Color c1, sf::Color c2, double k) {
    if(k < 0) k = 0;
    if(k > 1) k = 1;
    double k2 = 1 - k;
    return sf::Color(c1.r * k + c2.r * k2, c1.g * k + c2.g * k2, c1.b * k + c2.b * k2, c1.a * k + c2.a * k2);
}

void setSquares() {
    sf::RectangleShape square(sf::Vector2f(sz, sz));
    square.setOutlineColor(BACKGROUND_OUTLINE);
    square.setFillColor(BACKGROUND);
    square.setOutlineThickness(2);
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < M; j++) {
            square.setPosition(ofY + j * sz, ofX + i * sz);
            squares[i][j] = square;
        }
    }
}

int calc() {
    long long res = 0;
    int akum = POW * POW * POW * POW * POW;
    for(int i = 0; i < highScores.size(); i++) {
        res = ((stoi(highScores[i]) - i) * akum + res + MOD / 8) % MOD;
        akum /= POW;
    }
    return res;
}
