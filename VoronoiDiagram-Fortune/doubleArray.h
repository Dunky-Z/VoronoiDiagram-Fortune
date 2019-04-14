#pragma once

class doubleArray {
	double first;
	double second;

public:

	double& operator[](unsigned int i) {
		if (i == 0) return first;
		else return second;
	}
};