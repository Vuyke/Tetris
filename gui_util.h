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

sf::VideoMode windowSize() {
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    return sf::VideoMode(desktop.width * 0.75f, desktop.height * 0.7f);
}


const sf::Font font = getFont();

sf::Text setText(std::string s, int poz1, int poz2, int sz = 40, int thick = 0, Color outline = GRAY, Color fill = LIGHT_GRAY) {
    sf::Text text;
    text.setFont(font);
    text.setString(s);
    text.setPosition(poz1, poz2);
    text.setCharacterSize(sz);
    text.setFillColor(fill);
    text.setOutlineColor(outline);
    text.setOutlineThickness(thick);
    return text;
}

sf::RectangleShape setRectangle(std::pair<int, int> sz, std::pair<int, int> pos, int thick = 0, Color outline = Color::White, Color fill = Color::Black) {
    sf::RectangleShape cur(sf::Vector2f(sz.first, sz.second));
    cur.setPosition(pos.first, pos.second);
    cur.setFillColor(fill);
    cur.setOutlineColor(outline);
    cur.setOutlineThickness(thick);
    return cur;
}

sf::Vector2f mousePosition(sf::RenderWindow& window) {
    return window.mapPixelToCoords(sf::Mouse::getPosition(window));
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

void setnextPieceGUI() {
    nextPieceGUI.clear();
    int n = nextPiece.v.size(), outWidth = 2;
    int curX = nextOfY + sz;
    int curY = nextOfX + (NEXT - n) * sz / 2;
    if(n == 4) curX -= sz / 2;
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

class Button : public sf::Drawable {
    public:
        Button(sf::RectangleShape r, string t, int sz) {
            rect = r; text = setText(t, 0, 0, sz, 3);
            color = rect.getFillColor();
            clicked = color;
            clicked.r += 60; clicked.g += 60; clicked.b += 60;
            text.setOrigin(text.getGlobalBounds().getSize() / 2.f + text.getLocalBounds().getPosition());
            text.setPosition(rect.getPosition() + (rect.getSize() / 2.f));
        }

        void select() {
            rect.setFillColor(clicked);
        }

        void unselect() {
            rect.setFillColor(color);
        }

        bool checkInside(int x, int y) {
            return rect.getGlobalBounds().contains((float)x, (float)y);
        }

        void updateState(int x, int y) {
            if(checkInside(x, y))
                select();
            else
                unselect();
        }
    private:
        sf::RectangleShape rect;
        sf::Text text;
        sf::Color color;
        sf::Color clicked;

        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
            target.draw(rect, states);
            target.draw(text, states);
        }
};

template <typename... Drawable>
void draw(sf::RenderWindow& window, Drawable&&... vectors) {
    window.clear();
    auto drawVector = [&window](auto&& vec) {
        for(const auto& obj : vec) {
            window.draw(obj);
        }
    };
    (drawVector(vectors), ...);
    window.display();
}
