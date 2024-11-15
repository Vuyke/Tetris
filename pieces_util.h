#pragma once
#include "constants.h"

void createRandomPermutation() {
    vector<int> v;
    for(int i = 0; i < pieceNumber; i++) {
        v.push_back(i);
        std::swap(v[i], v[rand() % (i + 1)]);
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

bool isEmpty(int i, int j) {
    return squares[i][j].getFillColor() == BACKGROUND;
}

bool checkSpotUtil(Piece& p, int i, int j) {
    return check(i + pocX, j + pocY) && isEmpty(i + pocX, j + pocY);
}

bool checkRotateUtil(Piece& p, int i, int j) {
    if(!checkUtil(i + pocX, N)) return false;
    return !checkUtil(j + pocY, M) ||  isEmpty(i + pocX, j + pocY);
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

void editPiece(Piece& piece, sf::Color color, int curX = pocX, int curY = pocY) {
    int n = piece.v.size();
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            if(piece.v[i][j]) {
                squares[i + curX][j + curY].setFillColor(color);
            }
        }
    }
}

void initDownPiece(Piece& piece) {
    int cur = pocX;
    while(checkDown(piece)) {
        pocX++;
    }
    downX = pocX;
    downY = pocY;
    pocX = cur;
}

void setPiece(Piece& piece) {
    initDownPiece(piece);
    editPiece(piece, addColors(piece.color, BACKGROUND, 0.6), downX, downY);
    editPiece(piece, piece.color);
}

void erasePiece(Piece& piece) {
    editPiece(piece, BACKGROUND, downX, downY);
    editPiece(piece, BACKGROUND);
}

void startPiece(Piece& piece) {
    int n = piece.v.size();
    int startJ = (M - 1 >> 1) - (n - 1 >> 1);
    pocX = 0, pocY = startJ;
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            if(piece.v[i][j] && !isEmpty(i + pocX, j + pocY)) {
                lost = true;
            }
        }
    }
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
                if(isEmpty(i, j))
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
            squares[i][j].setFillColor(BACKGROUND);
        }
    }
    updateScore(cnt);
}
