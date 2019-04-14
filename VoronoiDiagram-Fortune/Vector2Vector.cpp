#include "Vector2Vector.h"

// Constructor
Vector2Vector::Vector2Vector() {
	head = nullptr;
}



bool Vector2Vector::empty() const {
	if (mSize == 0) {
		return true;
	}
	else {
		return false;
	}
}

unsigned int Vector2Vector::size() {
	return mSize;
}

void Vector2Vector::push_back(Vector2 *e) {

	Vector2 *tmp = head;
	e->next = nullptr;
	e->index = mSize;

	if (head == nullptr)			// If list is empty make Vector2 e the head
	{
		head = e;
		mSize++;
		return;
	}

	while (tmp->next != nullptr)		// Else traverse list until the last node
		tmp = tmp->next;

	tmp->next = e;					// Set tail's next as e
	tail = e;						// Set e as new tail
	mSize++;
	return;
}

void Vector2Vector::emplace_back(Vector2 *e) {
	push_back(e);
}

Vector2* Vector2Vector::pop_back() {
	if (mSize == 0) {
		return nullptr;			// If vector is empty return null
	}
	Vector2 *tmp;
	if (mSize == 1) {						// If head is the only member of vector, return head
		tmp = head;
		head = nullptr;
		mSize = 0;
		tmp->next = nullptr;
		return tmp;
	}

	Vector2 *i = head;
	while (i->next->next != nullptr) {
		i = i->next;
	}
	tail = i;
	tmp = tail->next;
	tail->next = nullptr;
	mSize--;
	return tmp;
}


void Vector2Vector::swap(unsigned int i, unsigned int j) {
	if (i == j) return;							// If i == j, do nothing


	Vector2 *prevI = nullptr, *currI = head;			// Search for Vector2 I and its preceeding Vector2
	while (currI && currI->index != i)
	{
		prevI = currI;
		currI = currI->next;
	}


	Vector2 *prevJ = nullptr, *currJ = head;			// Search for Vector2 J and its preceeding Vector2
	while (currJ && currJ->index != j)
	{
		prevJ = currJ;
		currJ = currJ->next;
	}


	if (currI == nullptr || currJ == nullptr)			// If either x or y is not present, nothing to do
		return;


	if (prevI != nullptr)							// If i is not head of linked list 
		prevI->next = currJ;
	else										// Else make j as new head 
		head = currJ;

	// If j is not head of linked list 
	if (prevJ != nullptr)
		prevJ->next = currI;
	else  // Else make i as new head 
		head = currI;

	// Swap next pointers 
	Vector2 *tmp = currJ->next;
	currJ->next = currI->next;
	currI->next = tmp;

	// Set new indices
	currI->index = j;
	currJ->index = i;
}





Vector2 Vector2Vector::operator[](unsigned int index) {
	Vector2 *tmp = head;
	while (tmp->index != index) {
		tmp = tmp->next;
	}
	return *tmp;
}