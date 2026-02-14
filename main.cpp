#include <Windows.h>
#include <iostream>
#include <random>
#include <setjmp.h>
#include <vector>

#define mainLoop while (true)

int x = 58, y = 13;
jmp_buf env;
std::vector<std::pair<int, int>> bodyParts;
int bodyLength = 0;

struct Snake {
    char head = '@';
    char body = '#';
} snake;

void positioning(int ix, int iy) {
    COORD pos = { (SHORT)ix, (SHORT)iy };
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(hConsole, pos);
}

void initOutputSnake() {
    for (const auto& part : bodyParts) {
        positioning(part.first, part.second);
        std::cout << snake.body;
    }
    positioning(x, y);
    std::cout << snake.head;
}

std::pair<int, int> coordinates() {
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> dis1(5, 116);
    std::uniform_int_distribution<> dis2(2, 27);

    int r1 = dis1(gen);
    int r2 = dis2(gen);

    return { r1, r2 };
}

void GameOver() {
    positioning(52, 10);
    std::cout << "============" << std::endl;
    positioning(51, 11);
    std::cout << "||Game Over!||" << std::endl;
    positioning(52, 12);
    std::cout << "============" << std::endl;
    positioning(43, 13);
    std::cout << "Press [1] to restart the game" << std::endl;
    positioning(44, 14);
    std::cout << "Press [esc] to exit the game" << std::endl;
    while (true) {
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            exit(0);
        }
        else {
            if (GetAsyncKeyState('1') & 0x8000) {
                longjmp(env, 1);
            }
        }
    }
}

void snakeMovement() {
    static int lastDir = 0;
    static int moveCounter = 0;

    moveCounter++;
    if (moveCounter % 5 != 0) return;

    int oldX = x;
    int oldY = y;

    int newX = x;
    int newY = y;

    if (GetAsyncKeyState(VK_UP) & 0x8000) {
        if (y - 1 < 0) GameOver();
        else newY = y - 1;
        lastDir = VK_UP;
    }
    else if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
        if (y + 1 > 27) GameOver();
        else newY = y + 1;
        lastDir = VK_DOWN;
    }
    else if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
        if (x - 1 < 0) GameOver();
        else newX = x - 1;
        lastDir = VK_LEFT;
    }
    else if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
        if (x + 1 > 116) GameOver();
        else newX = x + 1;
        lastDir = VK_RIGHT;
    }
    else if (lastDir != 0) {
        if (lastDir == VK_UP) {
            if (y - 1 < 0) GameOver();
            else newY = y - 1;
        }
        else if (lastDir == VK_DOWN) {
            if (y + 1 > 27) GameOver();
            else newY = y + 1;
        }
        else if (lastDir == VK_LEFT) {
            if (x - 1 < 0) GameOver();
            else newX = x - 1;
        }
        else if (lastDir == VK_RIGHT) {
            if (x + 1 > 116) GameOver();
            else newX = x + 1;
        }
    }

    if (newX != oldX || newY != oldY) {
        for (const auto& part : bodyParts) {
            if (newX == part.first && newY == part.second) {
                GameOver();
                return;
            }
        }

        x = newX;
        y = newY;

        bodyParts.push_back({ oldX, oldY });
        if (bodyParts.size() > bodyLength) {
            bodyParts.erase(bodyParts.begin());
        }
    }
}

int main() {
    if (setjmp(env) == 0) {}
    else {
        x = 58;
        y = 13;
        bodyParts.clear();
        bodyLength = 0;
    }

    CONSOLE_CURSOR_INFO cursorInfo = { 1, FALSE };
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);

    positioning(44, 11);
    std::cout << "===========================" << std::endl;
    positioning(43, 12);
    std::cout << "||Welcome to the Snake game||" << std::endl;
    positioning(44, 13);
    std::cout << "===========================" << std::endl;
    positioning(44, 14);
    std::cout << "Press [1] to start the game" << std::endl;
    positioning(101, 27);
    std::cout << "made by LiJunyi" << std::endl;
    positioning(5, 27);
    std::cout << "version 1.0" << std::endl;

    while (true) {
        if (GetAsyncKeyState('1') & 0x8000) {
            goto GameStart;
        }
    }

GameStart:
    bodyParts.clear();
    bodyLength = 0;
    auto food = coordinates();
    int fx = food.first;
    int fy = food.second;

    bool justAte = false;

    mainLoop{
        system("cls");

        positioning(fx, fy);
        std::cout << "*";

        initOutputSnake();

        if (x == fx && y == fy && !justAte) {
            bodyLength++;
            auto newFood = coordinates();
            fx = newFood.first;
            fy = newFood.second;
            justAte = true;
        }
        else {
            justAte = false;
        }

        snakeMovement();
    }
}