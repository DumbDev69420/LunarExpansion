#include "GlobalTypes.h"

float Vector2F::GetDistance(Vector2F& other)
{
	return std::sqrt(((other.x - this->x) * (other.x - this->x)) + ((other.y - this->y) * (other.y - this->y)));
}

float Vector3F::GetDistance(Vector3F& other)
{
	return std::sqrt(((other.x - this->x) * (other.x - this->x)) + ((other.y - this->y) * (other.y - this->y)) + ((other.z - this->z) * (other.z - this->z)));
}