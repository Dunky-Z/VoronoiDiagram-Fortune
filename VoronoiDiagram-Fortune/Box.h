#pragma once

// STL
// My includes
#include "Vector2.h"

class Box
{
public:
    // Be careful, y-axis is oriented to the top like in math
    enum class Side : int {LEFT, BOTTOM, RIGHT, TOP};

    struct Intersection
    {
        Side side;
        Vector2 point;
    };

	struct intersectionArray {
		Intersection *first;
		Intersection *second;

		// Constructor
		intersectionArray() {
			first = new Intersection();
			second = new Intersection();
		}

		// Operators
		Intersection* operator[](unsigned int i) {
			if (i == 0) return first;
			else return second;
		}

		// Operations
		void swap() {
			Intersection *tmp = first;
			first = second;
			second = tmp;
		}
	};

    double left;
    double bottom;
    double right;
    double top;

    bool contains(const Vector2& point) const;
    Intersection getFirstIntersection(const Vector2& origin, const Vector2& direction) const; // Useful for Fortune's algorithm
    int getIntersections(const Vector2& origin, const Vector2& destination, intersectionArray intersections) const; // Useful for diagram intersection

private:
    static constexpr double EPSILON = 0.00000000000001;		// Replace with Teensy machine epsilon
};

