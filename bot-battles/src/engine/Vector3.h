#ifndef __VECTOR3_H__
#define __VECTOR3_H__

#include "Memory.h"

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	class Vector3
	{
	public:
		Vector3();
		Vector3(F32 x, F32 y, F32 z);
		~Vector3();

		void operator+=(const Vector3& vector)
		{
			x += vector.x;
			y += vector.y;
		}

		void operator-=(const Vector3& vector)
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
		static const Vector3 zero;
		static const Vector3 one;

	public:

		union {
			struct 
			{
				F32 x;
				F32 y;
				F32 z;
			};

			F32 coord[3];
		};
	};
}

#endif