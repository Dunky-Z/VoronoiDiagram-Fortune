#pragma once
#include "Box.h"

struct intersectionArray {
	Box::Intersection *first;
	Box::Intersection *second;

	// Constructor and Destructor
	intersectionArray();
	~intersectionArray();

	// Operators
	Box::Intersection* operator[](unsigned int i);

	// Operations
	void swap();
};
