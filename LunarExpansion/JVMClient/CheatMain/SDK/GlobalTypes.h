#pragma once

#include <math.h>
#include <iostream>

struct Vector2F {
	float x, y;

	Vector2F()
	{
		x = 0.0f, y = 0.0f;
	}

	float GetDistance(Vector2F& other) {
		return std::sqrt(((other.x - this->x) * (other.x - this->x)) + ((other.y - this->y) * (other.y - this->y)));
	}
};

struct Vector3F {
	float x, y, z;

	Vector3F()
	{
		x = 0.0f, y = 0.0f, z = 0.0f;
	}

	float GetDistance(Vector3F& other) {
		return std::sqrt(((other.x - this->x) * (other.x - this->x)) + ((other.y - this->y) * (other.y - this->y)) + ((other.z - this->z) * (other.z - this->z)));
	}
};