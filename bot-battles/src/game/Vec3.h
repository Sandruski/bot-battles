#ifndef __VEC_3_H__
#define __VEC_3_H__

#include "Memory.h"

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	class Vec3
	{
	public:
		Vec3();
		Vec3(F32 x, F32 y, F32 z);
		~Vec3();

		void operator+=(const Vec3& vec)
		{
			x += vec.x;
			y += vec.y;
			z += vec.z;
		}

		void operator-=(const Vec3& vec)
		{
			x -= vec.x;
			y -= vec.y;
			z -= vec.z;
		}

		void operator*=(F32 val)
		{
			x *= val;
			y *= val;
			z *= val;
		}

		void operator/=(F32 val)
		{
			if (val != 0.0f)
			{
				x /= val;
				y /= val;
				z /= val;
			}
		}

	public:
		static const Vec3 zero;
		static const Vec3 one;

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