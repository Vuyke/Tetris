#pragma once
#include "constants.h"

void createRandomPermutation() {
    vector<int> v;
    for(int i = 0; i < pieceNumber; i++) {
        v.push_back(i);
        int randInd = rand() % (i + 1);
        std::swap(v[i], v[randInd]);
    }
    for(int i = 0; i < v.size(); i++) {
        Piece p(v[i]);
        if(one_color)
            p.color = sf::Color::Red;
        pieces.push_back(p);
    }
}

pi leftRotate(pi x, int n) {
    return {n - 1 - x.second, x.first};
}

bool checkUtil(int x, int A, int B = 0) {
    return x >= B && x < A;
}

bool check(int x, int y, int A = N, int B = M) {
    return checkUtil(x, A) && checkUtil(y, B);
}

bool check(int x, int y, int A, int B, int C, int D) {
    return checkUtil(x, B, A) && checkUtil(y, D, C);
}

bool checkSpotUtil(Piece& p, int i, int j) {
    return check(i + pocX, j + pocY) && ((check(i, j, p.v.size(), p.v.size()) && p.v[i][j])
                                         || squares[i + pocX][j + pocY].getFillColor() == sf::Color::Black);
}

bool checkRotateUtil(Piece& p, int i, int j) {
    if(!checkUtil(i + pocX, N)) return false;
    return !checkUtil(j + pocY, M) || ((check(i, j, p.v.size(), p.v.size()) && p.v[i][j])
                                         || squares[i + pocX][j + pocY].getFillColor() == sf::Color::Black);
}

bool checkRotate(Piece& p) {
    int n = p.v.size();
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            pi from = leftRotate({i, j}, n);
            if(p.v[from.first][from.second] && !checkRotateUtil(p, i, j))
                return false;
        }
    }
    return true;
}

void rotatePiece(Piece& p) {
    int n = p.v.size();
    for(int i = 0; i < n / 2; i++) {
        for(int j = 0; j < n + 1 >> 1; j++) {
            bool val = p.v[i][j];
            pi cordStart = {i, j}, cordCur = {i, j}, cordGet = leftRotate(cordCur, n);
            while(cordGet != cordStart) {
                p.v[cordCur.first][cordCur.second] = p.v[cordGet.first][cordGet.second];
                cordCur = cordGet;
                cordGet = leftRotate(cordGet, n);
            }
            p.v[cordCur.first][cordCur.second] = val;
        }
    }
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            if(p.v[i][j]) {
                if(pocY + j < 0) {
                    pocY = -j;
                }
                if(pocY + j >= M) {
                    pocY = M - j - 1;
                }
            }
        }
    }
}

void editPiece(Piece& piece, sf::Color color) {
    int n = piece.v.size();
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            if(piece.v[i][j]) {
                squares[i + pocX][j + pocY].setFillColor(color);
            }
        }
    }
}

void setPiece(Piece& piece) {
    editPiece(piece, piece.color);
}

void erasePiece(Piece& piece) {
    editPiece(piece, sf::Color::Black);
}

void startPiece(Piece& piece) {
    int n = piece.v.size();
    int startJ = (M - 1 >> 1) - (n - 1 >> 1);
    pocX = 0, pocY = startJ;
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            if(piece.v[i][j] && squares[i + pocX][j + pocY].getFillColor() != sf::Color::Black) {
                lost = true;
            }
        }
    }
    setPiece(piece);
}

bool checkMoveUtil(Piece& piece, pi x) {
    int n = piece.v.size();
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            if(piece.v[i][j] && !checkSpotUtil(piece, i + x.first, j + x.second)) {
                return false;
            }
        }
    }
    return true;
}

bool checkDown(Piece& piece) {
    return checkMoveUtil(piece, {1, 0});
}

bool checkRight(Piece& piece) {
    return checkMoveUtil(piece, {0, 1});
}

bool checkLeft(Piece& piece) {
    return checkMoveUtil(piece, {0, -1});
}

void updatePiece() {
    if(!pieces.size())
        createRandomPermutation();
    piece = nextPiece;
    nextPiece = pieces.front();
    pieces.pop_front();
    setnextPieceGUI();
    startPiece(piece);
}

void resetPiece() {
    pieces.clear();
    createRandomPermutation();
    piece = pieces.front();
    pieces.pop_front();
    nextPiece = pieces.front();
    pieces.pop_front();
    setnextPieceGUI();
    startPiece(piece);
}

void updateScore(int cnt) {
    if(!cnt) return;
    int x = scorePoints * cnt + scoreBonus * (cnt - 1) * (cnt - 1);
    if((score + x) / scoreGap > score / scoreGap) {
        level++;
        TIME *= timeCoef;
        textLevel.setString("Level: " + std::to_string(level));
    }
    score += x;
    textScore.setString("Score: " + std::to_string(score));
}

void updateMap(Piece& piece) {
    int n = piece.v.size(), cnt = 0;
    for(int i = min(pocX + n - 1, N - 1); i > -1; i--) {
        bool cur = (i >= pocX);
        if(cur) {
            for(int j = 0; j < M; j++) {
                if(squares[i][j].getFillColor() == sf::Color::Black)
                    cur = false;
            }
        }
        if(!cur && cnt) {
            for(int j = 0; j < M; j++) {
                squares[i + cnt][j].setFillColor(squares[i][j].getFillColor());
            }
        }
        cnt += cur;
    }
    for(int i = 0; i < cnt; i++) {
        for(int j = 0; j < M; j++) {
            squares[i][j].setFillColor(sf::Color::Black);
        }
    }
    updateScore(cnt);
}

void setSquares() {
    sf::RectangleShape square(sf::Vector2f(sz, sz));
    square.setOutlineColor(sf::Color::White);
    square.setFillColor(sf::Color::Black);
    square.setOutlineThickness(2);
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < M; j++) {
            square.setPosition(ofY + j * sz, ofX + i * sz);
            squares[i][j] = square;
        }
    }
}