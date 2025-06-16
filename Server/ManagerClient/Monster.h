#pragma once
#include "GameObject.h"

using MonsterRef = std::shared_ptr<class Monster>;

class Monster : public GameObject
{
public:
	Monster()
	{
		_symbol = MONSTER_SYMBOL;
	}
public:
	void Update() override
	{

	}

	void PreRender() override
	{
		// ���� ��ġ �����
		GotoXY(_prevX, _prevY);
		std::cout << ' ';
	}

	void Render() override
	{
		GotoXY(_posInfo.posx(), _posInfo.posy());
		SetColor(FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		std::cout << _symbol;
		SetColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	}
};
