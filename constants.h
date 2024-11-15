#pragma once
#include <list>
using namespace std;
using pi = pair<int, int>;

const int N = 15, M = 10, scoreGap = 5000, scorePoints = 800, scoreBonus = 200, pieceNumber = 7, NEXT = 5;
const float sz = 45.f, ofX = 50.f, ofY = 320.f, scoreX = ofY + (N - 2) * sz, scoreY = (N - 2) * sz, timeCoef = 0.9;
const float nextOfX = 100, nextOfY = ofY + (N - 2) * sz;
float TIME = 750;
int score, pocX, pocY, downX, downY, level = 1, pausedMilli, MOD = 1e9 + 7, POW = 3;
vector<vector<sf::RectangleShape>> squares(N, vector<sf::RectangleShape>(M));
vector<sf::RectangleShape> nextPieceGUI;
vector<string> highScores;
sf::RectangleShape nextSquare;
bool lost = false, paused = false, one_color = false, hasChanged = false, spaceClicked = false;
const sf::Color ROSE = sf::Color(220, 26, 255), BLUE = sf::Color(66, 142, 255), YELLOW = sf::Color(235, 200, 0);
const sf::Color ORANGE = sf::Color(255, 117, 26), LIGHT_BLUE = sf::Color(16, 235, 235), GRAY = sf::Color(60, 60, 60);
const sf::Color LIGHT_GRAY = sf::Color(220, 220, 220);
const sf::Color BACKGROUND = sf::Color::Black, BACKGROUND_OUTLINE = LIGHT_GRAY;
sf::Music music;
sf::Cursor hand, arrow;
sf::Text textScore, textLevel, textLose, textNext, textPaused;

enum Piece_Type {
    Z = 0,
    INVERSE_Z = 1,
    SQUARE = 2,
    LINE = 3,
    L = 4,
    INVERSE_L = 5,
    T = 6
};

struct Piece {
    vector<vector<bool>> v;
    sf::Color color;
    Piece() {}
    Piece(int x) {
        switch(x) {
            case SQUARE:
                v.resize(2, vector<bool>(2, true));
                color = YELLOW;
                break;
            case LINE:
                v.resize(4, vector<bool>(4, false));
                color = LIGHT_BLUE;
                for(int i = 0; i < 4; i++) v[1][i] = true;
                break;
            case Z:
                v.resize(3, vector<bool>(3, false));
                color = sf::Color::Red;
                v[0][0] = true, v[0][1] = true, v[1][1] = true, v[1][2] = true;
                break;
            case INVERSE_Z:
                v.resize(3, vector<bool>(3, false));
                color = sf::Color::Green;
                v[0][1] = true, v[0][2] = true, v[1][1] = true, v[1][0] = true;
                break;
            case T:
                v.resize(3, vector<bool>(3, false));
                color = ROSE;
                v[0][1] = true;
                for(int i = 0; i < 3; i++) v[1][i] = true;
                break;
            case INVERSE_L:
                v.resize(3, vector<bool>(3, false));
                color = BLUE;
                v[0][0] = true;
                for(int i = 0; i < 3; i++) v[1][i] = true;
                break;
            case L:
                v.resize(3, vector<bool>(3, false));
                color = ORANGE;
                v[0][2] = true;
                for(int i = 0; i < 3; i++)  v[1][i] = true;
                break;
        }
    }
};

std::list<Piece> pieces;
Piece piece, nextPiece, downPiece;
