#include "Vector2.h"

Vector2::Vector2(double x, double y) : x(x), y(y)
{

}

// Unary operators

Vector2 Vector2::operator-() const
{
    return Vector2(-x, -y);
}

Vector2& Vector2::operator+=(const Vector2& other)
{
    x += other.x;
    y += other.y;
    return *this;
}

Vector2& Vector2::operator-=(const Vector2& other)
{
    x -= other.x;
    y -= other.y;
    return *this;
}

Vector2& Vector2::operator*=(double t)  
{
    x *= t;
    y *= t;
    return *this; 
}

bool Vector2::operator==(const Vector2& other) {
	if (x != other.x || y != other.y) {						
		return true;
	}
	else {
		return false;
	}
}

// Other operations

Vector2 Vector2::getOrthogonal() const
{
    return Vector2(-y, x);
}

double Vector2::dot(const Vector2& other) const
{
    return x * other.x + y * other.y;
}

double Vector2::getNorm() const
{
	const double s = x * x + y * y;
	return squareRoot(s);
}

double Vector2::squareRoot(const double n) const 				
{
	if (n < 0) {
		return 0;
	}
	if (n > 1) {
		double error = 0.00001; //define the precision of your result
		double s = n;

		while ((s - n / s) > error) //loop until precision satisfied 
		{
			s = (s + n / s) / 2;
		}
		return s;
	}
	else {
		double temp = 0;
		double sqrt = n / 2;
		while (sqrt != temp)
		{
			temp = sqrt;
			sqrt = (n / sqrt + sqrt) / 2;
		}
		return sqrt;
	}
	
}

double Vector2::getDistance(const Vector2& other) const
{
    return (*this - other).getNorm();
}

double Vector2::getDet(const Vector2& other) const
{
    return x * other.y - y * other.x;
}

Vector2 operator+(Vector2 lhs, const Vector2& rhs)
{
    lhs += rhs;
    return lhs;
}

Vector2 operator-(Vector2 lhs, const Vector2& rhs) 
{
    lhs -= rhs;
    return lhs;
}

Vector2 operator*(double t, Vector2 vec)
{
    vec *= t;
    return vec;
}

Vector2 operator*(Vector2 vec, double t)
{
    return t * vec;
}



