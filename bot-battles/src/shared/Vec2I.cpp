#include "Vec2I.h"

namespace sand
{

	const Vec2I Vec2I::zero = Vec2I();
	const Vec2I Vec2I::one = Vec2I(1, 1);

	//----------------------------------------------------------------------------------------------------
	Vec2I::Vec2I() : x(0), y(0)
	{
	}

	//----------------------------------------------------------------------------------------------------
	Vec2I::Vec2I(I32 x, I32 y) : x(x), y(y)
	{
	}

	//----------------------------------------------------------------------------------------------------
	Vec2I::~Vec2I()
	{
	}
}
