#ifndef __VECTOR_2_INT_H__
#define __VECTOR_2_INT_H__

#include "Memory.h"

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	class Vector2Int
	{
	public:
		Vector2Int();
		Vector2Int(I32 x, I32 y);
		~Vector2Int();

		void operator+=(const Vector2Int& vector)
		{
			x += vector.x;
			y += vector.y;
		}

		void operator-=(const Vector2Int& vector)
		{
			x -= vector.x;
			y -= vector.y;
		}

		void operator*=(I32 value)
		{
			x *= value;
			y *= value;
		}

		void operator/=(I32 value)
		{
			if (value != 0)
			{
				x /= value;
				y /= value;
			}
		}

	public:
		static const Vector2Int zero;
		static const Vector2Int one;

	public:

		union {
			struct
			{
				I32 x;
				I32 y;
			};

			I32 coord[2];
		};
	};
}

#endif