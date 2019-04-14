#include "intersectionArray.h"

intersectionArray::intersectionArray() {

}

intersectionArray::~intersectionArray() {
	

}

Box::Intersection* intersectionArray::operator[](unsigned int i) {
	if (i == 0) return first;
	else return second;
}

void intersectionArray::swap() {
	Box::Intersection *tmp = first;
	first = second;
	second = tmp;
}
