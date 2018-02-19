#pragma once

#include <thread>

class Input
{
public:
	typedef enum KEYS
	{
		A = 65,
		B = 66,
		C = 67,
		D = 68,
		E = 69,
		F = 70,
		G = 71,
		H = 72,
		I = 73,
		J = 74,
		K = 75,
		L = 76,
		M = 77,
		N = 78,
		O = 79,
		P = 80,
		Q = 81,
		R = 82,
		S = 83,
		T = 84,
		U = 85,
		V = 86,
		W = 87,
		X = 88,
		Y = 89,
		Z = 90,

		ZERO = 48,
		ONE = 49,
		TWO = 50,
		THREE = 51,
		FUOR = 52,
		FIVE = 53,
		SIX = 54,
		SEVEN = 55,
		EIGHT = 56,
		NINE = 57,

		LEFT_ARROW = 37,
		UP_ARROW = 38,
		RIGHT_ARROW = 39,
		DOWN_ARROW = 40,

		SPACE = 32,
		CTRL = 17
	} KEYS;
public:
	Input();
	~Input();

	bool Initialize();

	void KeyDown(unsigned int const iKeyIndex);
	void KeyUp(unsigned int const iKeyIndex);

	const bool IsKeyDown(unsigned int const iKeyIndex);


private:
	
	bool m_bKeys[256];
};
