#ifndef __VEC_2_H__
#define __VEC_2_H__

#include "Memory.h"

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	class Vec2
	{
	public:
		Vec2();
		Vec2(F32 x, F32 y);
		~Vec2();

		void operator+=(const Vec2& vec)
		{
			x += vec.x;
			y += vec.y;
		}

		void operator-=(const Vec2& vec)
		{
			x -= vec.x;
			y -= vec.y;
		}

		void operator*=(F32 val)
		{
			x *= val;
			y *= val;
		}

		void operator/=(F32 val)
		{
			if (val != 0.0f)
			{
				x /= val;
				y /= val;
			}
		}

	public:
		static const Vec2 zero;
		static const Vec2 one;

	public:

		union {
			struct
			{
				F32 x;
				F32 y;
			};

			F32 coord[2];
		};
	};
}

#endif
