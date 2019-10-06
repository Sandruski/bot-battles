#ifndef __VECTOR_2_H__
#define __VECTOR_2_H__

#include "Memory.h"

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	class Vector2
	{
	public:
		Vector2();
		Vector2(F32 x, F32 y);
		~Vector2();

		void operator+=(const Vector2& vector)
		{
			x += vector.x;
			y += vector.y;
		}

		void operator-=(const Vector2& vector)
		{
			x -= vector.x;
			y -= vector.y;
		}

		void operator*=(F32 value)
		{
			x *= value;
			y *= value;
		}

		void operator/=(F32 value)
		{
			if (value != 0.0f)
			{
				x /= value;
				y /= value;
			}
		}

	public:
		static const Vector2 zero;
		static const Vector2 one;

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