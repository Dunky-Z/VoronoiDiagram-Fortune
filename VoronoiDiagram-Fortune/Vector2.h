#pragma once

// Declarations

class Vector2;
Vector2 operator-(Vector2 lhs, const Vector2& rhs);

// Implementations

class Vector2
{
public:
    double x;
    double y;

	// Used in Vector2Vector
	int index;
	Vector2* next = nullptr;

    Vector2(double x = 0.0, double y = 0.0);

    // Unary operators

    Vector2 operator-() const;
    Vector2& operator+=(const Vector2& other);
    Vector2& operator-=(const Vector2& other);
    Vector2& operator*=(double t);
	bool operator==(const Vector2& other);
    
    // Other operations
    
    Vector2 getOrthogonal() const;
    double dot(const Vector2& other) const;
    double getNorm() const;
    double getDistance(const Vector2& other) const;
    double getDet(const Vector2& other) const;
	double squareRoot(double n) const;
};

// Binary operators

Vector2 operator+(Vector2 lhs, const Vector2& rhs);
Vector2 operator-(Vector2 lhs, const Vector2& rhs);
Vector2 operator*(double t, Vector2 vec);
Vector2 operator*(Vector2 vec, double t);

