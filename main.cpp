#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <fstream>
#include <time.h>
#include "constants.h"
#include "gui_util.h"
#include "pieces_util.h"
using namespace std;

void resetGameState() {
    setSquares();
    resetPiece();
    score = 0, level = 1, TIME = 800, lost = false;
    textScore.setString("Score: 0");
    textLevel.setString("Level: 1");
    music.setVolume(30), music.play();
}

void updateMain(sf::RenderWindow& window, sf::RectangleShape& rect, bool p, bool& zoomColor) {
    if(p) {
        rect.setFillColor(GRAY);
        window.setMouseCursor(hand);
        zoomColor = true;
    }
    else if(zoomColor) {
        rect.setFillColor(sf::Color::Black);
        window.setMouseCursor(arrow);
        zoomColor = false;
    }
}

void chooseMode(sf::RenderWindow& window);

void playScreen(sf::RenderWindow& window) {
    int playX = 530, playY = 100, playSzX = 190, playSzY = 100;
    int highX = playX - 100, highY = playY + 180, highSzX = 390, highSzY = 400;
    int numX = highX + 150, numY = highY + 100;
    int sz = 50;
    vector<sf::Text> texts;
    vector<sf::RectangleShape> shapes;
    texts.push_back(setText("HIGHSCORES", highX + 60, highY, 50, 3));
    texts.push_back(setText("PLAY", playX + 20, playY, 75, 3));
    for(int i = 0; i < highScores.size(); i++) {
        int cur = (highScores[0].size() - highScores[i].size()) * sz / 2;
        texts.push_back(setText(highScores[i], numX + cur, numY + i * ((highSzY + highY - numY) / 5), sz, 2, LIGHT_GRAY, BLUE));
    }
    if(hasChanged) {
        texts.push_back(setText("TEMPERED WITH SCORES\n\n        SCORES DELETED", highX + 15, numY, 35, 3));
        hasChanged = false;
    }
    shapes.push_back(setRectangle({highSzX, highSzY}, {highX, highY}, 7));
    shapes.push_back(setRectangle({playSzX, playSzY}, {playX, playY}, 7));
    draw(window, texts, shapes);
    bool zoom = false;
    while(window.isOpen()) {
        sf::Event event;
        while(window.pollEvent(event)) {
            if(event.type == sf::Event::Closed) {
                window.close();
            }
            else if(event.type == sf::Event::MouseMoved) {
                int x = event.mouseMove.x, y = event.mouseMove.y;
                updateMain(window, shapes[1], check(x, y, playX, playX + playSzX, playY, playY + playSzY), zoom);
            }
            else if(event.type == sf::Event::MouseButtonPressed) {
                int x = event.mouseButton.x, y = event.mouseButton.y;
                if(check(x, y, playX, playX + playSzX, playY, playY + playSzY)) {
                    chooseMode(window);
                    return;
                }
            }
        }
        draw(window, texts, shapes);
    }
}

void modeLogic(sf::RenderWindow& window) {
    int pozX = 175, pozY = 400, chooseX = 425, chooseY = 100;
    int colorfulX = pozX - 40, colorfulY = pozY - 30, colorfulSzX = 410, colorfulSzY = 160;
    int oneColorX = pozX + 560, oneColorY = pozY - 30, oneColorSzX = 540, oneColorSzY = 160;
    int exitX = 550, exitY = 610, exitSzX = 210, exitSzY = 80;
    vector<sf::Text> texts;
    vector<sf::RectangleShape> shapes;
    shapes.push_back(setRectangle({colorfulSzX, colorfulSzY}, {colorfulX, colorfulY}, 7));
    shapes.push_back(setRectangle({oneColorSzX, oneColorSzY}, {oneColorX, oneColorY}, 7));
    shapes.push_back(setRectangle({exitSzX, exitSzY}, {exitX, exitY}, 7));
    texts.push_back(setText("COLORFUL", colorfulX + 40, colorfulY + 25, 75, 3));
    texts.push_back(setText("ONE COLORED", oneColorX + 40, oneColorY + 25, 75, 3));
    texts.push_back(setText("CHOOSE MODE", chooseX, chooseY, 75, 3));
    texts.push_back(setText("BACK", exitX + 20, exitY - 10, 75, 3));
    draw(window, texts, shapes);
    bool zoomColor = false, zoomOne = false, zoomExit = false;
    while(window.isOpen()) {
        sf::Event event;
        while(window.pollEvent(event)) {
            if(event.type == sf::Event::Closed) {
                window.close();
            }
            else if(event.type == sf::Event::MouseMoved) {
                int x = event.mouseMove.x, y = event.mouseMove.y;
                updateMain(window, shapes[0], check(x, y, colorfulX, colorfulX + colorfulSzX, colorfulY, colorfulY + colorfulSzY), zoomColor);
                updateMain(window, shapes[1], check(x, y, oneColorX, oneColorX + oneColorSzX, oneColorY, oneColorY + oneColorSzY), zoomOne);
                updateMain(window, shapes[2], check(x, y, exitX, exitX + exitSzX, exitY, exitY + exitSzY), zoomExit);
            }
            else if(event.type == sf::Event::MouseButtonPressed) {
                int x = event.mouseButton.x, y = event.mouseButton.y;
                if(event.mouseButton.button == sf::Mouse::Left) {
                    if(check(x, y, colorfulX, colorfulX + colorfulSzX, colorfulY, colorfulY + colorfulSzY)) {
                        one_color = false;
                        return;
                    }
                    if(check(x, y, oneColorX, oneColorX + oneColorSzX, oneColorY, oneColorY + oneColorSzY)) {
                        one_color = true;
                        return;
                    }
                    if(check(x, y, exitX, exitX + exitSzX, exitY, exitY + exitSzY)) {
                        playScreen(window);
                        return;
                    }
                }
            }
        }
        draw(window, texts, shapes);
    }
}

void chooseMode(sf::RenderWindow& window) {
    modeLogic(window);
    window.clear();
    window.setMouseCursor(arrow);
}

void gameScreen(sf::RenderWindow& window) {
    resetGameState();
    window.display();
    sf::Clock clock;
    while (window.isOpen()) {
        sf::Event event;
        erasePiece(piece);
        while(window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::KeyPressed) {
                if(event.key.code == sf::Keyboard::Escape) {
                    if(!paused) {
                        paused = true;
                        pausedMilli = clock.getElapsedTime().asMilliseconds();
                    }
                    else {
                        clock.restart();
                        paused = false;
                    }
                }
                else if(paused)
                    continue;
                else if((event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::W) && checkRotate(piece)) {
                    rotatePiece(piece);
                }
                else if((event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::A) && checkLeft(piece)) {
                    pocY--;
                }
                else if((event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::D) && checkRight(piece)) {
                    pocY++;
                }
                else if((event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::S) && checkDown(piece)) {
                    pocX++;
                    clock.restart();
                }
                else if(event.key.code == sf::Keyboard::Space) {
                    while(checkDown(piece)) {
                        pocX++;
                    }
                    spaceClicked = true;
                }
            }
        }
        if(!paused) {
            if(checkDown(piece)) {
                if(clock.getElapsedTime().asMilliseconds() + pausedMilli >= TIME) {
                    pocX++;
                    clock.restart();
                    pausedMilli = 0;
                }
            }
            else {
                if(clock.getElapsedTime().asMilliseconds() + pausedMilli >= 750 || spaceClicked) {
                    setPiece(piece);
                    updateMap(piece);
                    updatePiece();
                    clock.restart();
                    pausedMilli = 0;
                    spaceClicked = false;
                }
            }
        }
        setPiece(piece);
        window.clear();
        window.draw(nextSquare);
        for(auto& x : squares) {
            for(auto& y : x) {
                window.draw(y);
            }
        }
        for(auto& x : nextPieceGUI) {
            window.draw(x);
        }
        window.draw(textScore);
        window.draw(textLevel);
        window.draw(textNext);
        if(paused) {
            window.draw(textPaused);
        }
        if(lost) {
            music.setVolume(15);
            window.draw(textLose);
            window.display();
            sf::sleep(sf::milliseconds(4000));
            window.clear();
            return;
        }
        window.display();
    }
}

void initHighscore() {
    ifstream f("highscores.txt");
    if(!f.is_open()) {
        return;
    }
    string s;
    bool p = false;
    while(getline(f, s) && !hasChanged) {
        if(s.size() == 0 || (s.size() == 1 && s[0] == '\n')) continue;
        if(s[0] == 'K') {
            p = true;
            s.erase(0, 1);
            hasChanged = (stoi(s) != calc());
        }
        else {
            highScores.push_back(s);
        }
    }
    f.close();
    if((!p && highScores.size()) || hasChanged) {
        highScores.clear();
        hasChanged = true;
        ofstream f1("highscores.txt");
        if(f1.is_open()) {
            f1.close();
        }
    }
}

void saveHighscore() {
    int i = 0;
    for(; i < highScores.size(); i++) {
        if(score >= stoi(highScores[i])) {
            break;
        }
    }
    highScores.insert(highScores.begin() + i, to_string(score));
    if(highScores.size() > 5) {
        highScores.pop_back();
    }
    ofstream f("highscores.txt");
    if(!f.is_open()) {
        return;
    }
    for(auto& x : highScores) {
        f << x << endl;
    }
    f << "K" << calc() << endl;
    f.close();
}

void init() {
    srand(time(0));
    initHighscore();
    nextSquare = setRectangle({NEXT * sz, NEXT * sz}, {nextOfX, nextOfY}, 5);
    textScore = setText("Score: 0", scoreX, scoreY + 50);
    textLevel = setText("Level: 1", scoreX, scoreY);
    textLose = setText("You lost!", 0, 100, 400, 10);
    textNext = setText("NEXT", nextOfY + ((NEXT - 2) * sz) / 2, nextOfX - sz - 10);
    textPaused = setText("PAUSED", 0, 100, 440, 10);
    setMusic(music);
    setCursor(hand, sf::Cursor::Hand), setCursor(arrow, sf::Cursor::Arrow);
}

int main() {
    init();
    sf::RenderWindow window(sf::VideoMode(1400, 800), "TETRIS");
    while(window.isOpen()) {
        playScreen(window);
        music.stop();
        if(!window.isOpen())
            break;
        gameScreen(window);
        saveHighscore();
    }
    return 0;
}
