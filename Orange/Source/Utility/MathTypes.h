#ifndef _MATHTYPES_H
#define _MATHTYPES_H

#include <cstdint>

#include "Utility.h"

namespace Orange
{
	struct Vec2
	{
		Vec2() = default;
		Vec2(const float _x) { x = _x; y = _x; }
		Vec2(const float _x, const float _y) { x = _x; y = _y; }
		Vec2(const int32_t _x, const int32_t _y) { x = static_cast<float>(_x), y = static_cast<float>(_y); }
		Vec2(const uint32_t _x, const uint32_t _y) { x = static_cast<float>(_x), y = static_cast<float>(_y); }
		Vec2(const Vec2& other) { x = other.x; y = other.y; }
		~Vec2() = default;

		void operator=(const Vec2& other)
		{
			x = other.x;
			y = other.y;
		}

		Vec2 operator+(const Vec2& other)
		{
			return Vec2(x + other.x, y + other.y);
		}

		Vec2 operator-(const Vec2& other)
		{
			return Vec2(x - other.x, y - other.y);
		}
		Vec2 operator-(Vec2 other) const
		{
			return Vec2(x - other.x, y - other.y);
		}

		void operator+=(const Vec2& other)
		{
			x += other.x;
			y += other.y;
		}

		void operator-=(const Vec2& other)
		{
			x -= other.x;
			y -= other.y;
		}

		bool operator==(const Vec2& other)
		{
			if
			(
				x == other.x &&
				y == other.y
			) return true;

			return false;
		}
		const bool operator==(const Vec2& other) const
		{
			if
			(
				x == other.x &&
				y == other.y
			) return true;

			return false;
		}

		// Scalar/vector operation
		void operator*=(const float& scalar)
		{
			x *= scalar;
			y *= scalar;
		}

		Vec2 operator*(const float& scalar) const
		{
			return Vec2(x * scalar, y * scalar);
		}

		Vec2 operator/(const float& scalar) const
		{
			return Vec2(x / scalar, y / scalar);
		}

		const float operator[](const uint32_t index)
		{
			OG_ASSERT_MSG(index >= 0 && index < 2, "Attempting to retrieve vector axis with invalid index");

			// Not sure if this might be problematic down the road, and reinterpret_cast might be slow?
			return *(reinterpret_cast<float*>(this) + index);
		}

		float x, y;
	};

	struct Vec3
	{
		Vec3() = default;
		Vec3(const float _x) { x = _x; y = _x; z = _x; }
		Vec3(const float _x, const float _y, const float _z) { x = _x; y = _y; z = _z; }
		Vec3(const int32_t _x, const int32_t _y, const int32_t _z) { x = static_cast<float>(_x), y = static_cast<float>(_y); z = static_cast<float>(_z); }
		Vec3(const Vec3& other) { x = other.x; y = other.y; z = other.z;  }
		~Vec3() = default;

		void operator=(const Vec3& other)
		{
			x = other.x;
			y = other.y;
			z = other.z;
		}

		Vec3 operator+(const Vec3& other)
		{
			return Vec3(x + other.x, y + other.y, z + other.z);
		}

		Vec3 operator-(const Vec3& other)
		{
			return Vec3(x - other.x, y - other.y, z - other.z);
		}

		void operator+=(const Vec3& other)
		{
			x += other.x;
			y += other.y;
			z += other.z;
		}

		void operator-=(const Vec3& other)
		{
			x -= other.x;
			y -= other.y;
			z -= other.z;
		}

		bool operator==(const Vec3& other)
		{
			if
			(
				x == other.x &&
				y == other.y &&
				z == other.z
			) return true;

			return false;
		}

		// Scalar/vector operation
		void operator*=(const float& scalar)
		{
			x *= scalar;
			y *= scalar;
			z *= scalar;
		}

		float operator[](const uint32_t index)
		{
			OG_ASSERT_MSG(index >= 0 && index < 3, "Attempting to retrieve vector axis with invalid index");

			// Not sure if this might be problematic down the road, and reinterpret_cast might be slow?
			return *(reinterpret_cast<float*>(this) + index);
		}

		float x, y, z;
	};

	struct Vec4
	{
		Vec4() = default;
		Vec4(const float _x) { x = _x; y = _x; z = _x; w = _x; }
		Vec4(const float _x, const float _y, const float _z, const float _w) { x = _x; y = _y; z = _z; w = _w; }
		Vec4(const int32_t _x, const int32_t _y, const int32_t _z, const int32_t _w) {
			x = static_cast<float>(_x), y = static_cast<float>(_y); z = static_cast<float>(_z); w = static_cast<float>(_w); }
		Vec4(const Vec4& other) { x = other.x; y = other.y; z = other.z; w = other.w; }
		~Vec4() = default;

		void operator=(const Vec4& other)
		{
			x = other.x;
			y = other.y;
			z = other.z;
			w = other.w;
		}

		Vec4 operator+(const Vec4& other)
		{
			return Vec4(x + other.x, y + other.y, z + other.z, w + other.w);
		}

		Vec4 operator-(const Vec4& other)
		{
			return Vec4(x - other.x, y - other.y, z - other.z, w - other.w);
		}

		Vec4 operator*(const float& scalar) const
		{
			return Vec4(x * scalar, y * scalar, z * scalar, w * scalar);
		}

		Vec4 operator/(const float& scalar) const
		{
			return Vec4(x / scalar, y / scalar, z / scalar, w / scalar);
		}

		void operator+=(const Vec4& other)
		{
			x += other.x;
			y += other.y;
			z += other.z;
			w += other.w;
		}

		void operator-=(const Vec4& other)
		{
			x -= other.x;
			y -= other.y;
			z -= other.z;
			w -= other.w;
		}

		bool operator==(const Vec4& other)
		{
			if
			(
				x == other.x &&
				y == other.y &&
				z == other.z &&
				w == other.w
			) return true;

			return false;
		}

		// Scalar/vector operation
		void operator*=(const float& scalar)
		{
			x *= scalar;
			y *= scalar;
			z *= scalar;
			//w *= scalar; // Maybe we don't want to include the 'w' component in this operation??
		}

		const float operator[](const uint32_t index)
		{
			OG_ASSERT_MSG(index >= 0 && index < 4, "Attempting to retrieve vector axis with invalid index");

			// Not sure if this might be problematic down the road, and reinterpret_cast might be slow?
			return *(reinterpret_cast<float*>(this) + index);
		}

		float x, y, z, w;
	};
}


#endif