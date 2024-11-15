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

void chooseMode(sf::RenderWindow& window);

void playScreen(sf::RenderWindow& window) {
    int playX = 530, playY = 100, playSzX = 190, playSzY = 100;
    int highX = playX - 100, highY = playY + 180, highSzX = 390, highSzY = 400;
    int numX = highX + 150, numY = highY + 100;
    int sz = 50;
    vector<sf::Text> texts;
    vector<sf::RectangleShape> shapes;
    vector<Button> buttons;
    buttons.push_back(Button(setRectangle({playSzX, playSzY}, {playX, playY}, 7), "PLAY", 75));
    texts.push_back(setText("HIGHSCORES", highX + 60, highY, 50, 3));
    shapes.push_back(setRectangle({highSzX, highSzY}, {highX, highY}, 7));
    for(int i = 0; i < highScores.size(); i++) {
        int cur = (highScores[0].size() - highScores[i].size()) * sz / 2;
        texts.push_back(setText(highScores[i], numX + cur, numY + i * ((highSzY + highY - numY) / 5), sz, 2, BLUE, LIGHT_GRAY));
    }
    if(hasChanged) {
        texts.push_back(setText("TEMPERED WITH SCORES\n\n        SCORES DELETED", highX + 15, numY, 35, 3));
        hasChanged = false;
    }
    draw(window, shapes, texts, buttons);
    while(window.isOpen()) {
        sf::Event event;
        while(window.pollEvent(event)) {
            sf::Vector2f pos = mousePosition(window);
            if(event.type == sf::Event::Closed) {
                window.close();
            }
            else if(event.type == sf::Event::MouseMoved) {
                buttons[0].updateState(pos.x, pos.y);
            }
            else if(event.type == sf::Event::MouseButtonPressed) {
                if(buttons[0].checkInside(pos.x, pos.y)) {
                    chooseMode(window);
                    return;
                }
            }
        }
        draw(window, shapes, texts, buttons);
    }
}

void modeLogic(sf::RenderWindow& window) {
    pi choose = {425, 100}, colorful = {135, 370}, colorfulSz = {410, 160};
    pi oneColor = {735, 370}, oneColorSz = {540, 160};
    pi exit = {550, 610}, exitSz = {210, 80};
    vector<Button> buttons;
    vector<sf::Text> texts;
    texts.push_back(setText("CHOOSE MODE", choose.first, choose.second, 75, 3));
    buttons.push_back(Button(setRectangle({colorfulSz.first, colorfulSz.second}, {colorful.first, colorful.second}, 7), "COLORFUL", 75));
    buttons.push_back(Button(setRectangle({oneColorSz.first, oneColorSz.second}, {oneColor.first, oneColor.second}, 7), "MONOCHROME", 75));
    buttons.push_back(Button(setRectangle({exitSz.first, exitSz.second}, {exit.first, exit.second}, 7), "BACK", 60));
    draw(window, buttons, texts);
    while(window.isOpen()) {
        sf::Event event;
        while(window.pollEvent(event)) {
            sf::Vector2f pos = mousePosition(window);
            if(event.type == sf::Event::Closed) {
                window.close();
            }
            else if(event.type == sf::Event::MouseMoved) {
                for(auto& button : buttons) {
                    button.updateState(pos.x, pos.y);
                }
            }
            else if(event.type == sf::Event::MouseButtonPressed) {
                if(event.mouseButton.button == sf::Mouse::Left) {
                    if(buttons[0].checkInside(pos.x, pos.y)) {
                        one_color = false;
                        return;
                    }
                    if(buttons[1].checkInside(pos.x, pos.y)) {
                        one_color = true;
                        return;
                    }
                    if(buttons[2].checkInside(pos.x, pos.y)) {
                        playScreen(window);
                        return;
                    }
                }
            }
        }
        draw(window, buttons, texts);
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
                        pausedMilli += clock.getElapsedTime().asMilliseconds();
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
    sf::RenderWindow window(windowSize(), "TETRIS");
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
