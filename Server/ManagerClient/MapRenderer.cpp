#include "pch.h"
#include "MapRenderer.h"


int playerX = 10;
int playerY = 5;
int prevX = 10;
int prevY = 5;

/*
const char PLAYER_CHAR = '@';
const char MONSTER_CHAR = 'M';
const int MAP_WIDTH = 40;
const int MAP_HEIGHT = 20;


// �ܼ� Ŀ�� �̵�


// �ܼ� ��ü �ʱ�ȭ
void clearScreen() {
	system("cls");
}

// �ܼ� ������ ����: Ŀ�� �����
void hideCursor() {
	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(hConsole, &cursorInfo);
	cursorInfo.bVisible = FALSE;
	SetConsoleCursorInfo(hConsole, &cursorInfo);
}

// �� Ʋ ���
void drawMapBorder() {
	for (int y = 0; y < MAP_HEIGHT; ++y) {
		for (int x = 0; x < MAP_WIDTH; ++x) {
			MapRenderer::gotoxy(x, y);
			if (y == 0 || y == MAP_HEIGHT - 1 || x == 0 || x == MAP_WIDTH - 1)
				std::cout << '#';  // ���
			else
				std::cout << ' ';
		}
	}
}

void MapRenderer::gotoxy(int x, int y) 
{
	COORD pos = { (SHORT)x, (SHORT)y };
	SetConsoleCursorPosition(hConsole, pos);
}

void MapRenderer::Init()
{
	hideCursor();
	clearScreen();
	drawMapBorder();
}

void MapRenderer::DrawPlayer(int32 x, int32 y)
{
	gotoxy(playerX, playerY);
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
	std::cout << PLAYER_CHAR;
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void MapRenderer::DrawMonster(int32 x, int32 y)
{
	gotoxy(x, y);
	SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	std::cout << MONSTER_CHAR;
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void MapRenderer::PreRender()
{
	// ���� ��ġ �����
	gotoxy(prevX, prevY);
	std::cout << ' ';
}

void MapRenderer::Render()
{
	DrawPlayer(0, 0);
}
*/