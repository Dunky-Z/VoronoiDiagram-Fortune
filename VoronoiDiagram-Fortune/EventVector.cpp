#include "EventVector.h"

// Constructor
eventVector::eventVector() {
	head = nullptr;
}

//Destructor
eventVector::~eventVector() {
	delete head;
}

bool eventVector::empty() const {
	if (mSize == 0) {
		return true;
	}
	else {
		return false;
	}
}

unsigned int eventVector::size() {
	return mSize;
}

void eventVector::push_back(Event *e) {

	Event *tmp = head;  
	e->next = nullptr;

	if (head == nullptr)			// If list is empty make Event e the head
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

void eventVector::emplace_back(Event *e) {
	push_back(e);
}

Event* eventVector::pop_back() {
	if (mSize == 0) {
		return nullptr;			// If vector is empty return null
	}
	Event *tmp;
	if (mSize == 1) {						// If head is the only member of vector, return head
		tmp = head;
		head = nullptr;
		mSize = 0;
		tmp->next = nullptr;
		return tmp;
	}
		
	Event *i = head;
	while(i->next->next != nullptr){
		i = i->next;
	}
	tail = i;
	tmp = tail->next;
	tail->next = nullptr;
	mSize--;
	return tmp;
}


void eventVector::swap(unsigned int i, unsigned int j) {
	if (i == j) return;							// If i == j, do nothing

	
	Event *prevI = nullptr, *currI = head;			// Search for Event I and its preceeding event
	while (currI && currI->index != i)
	{
		prevI = currI;
		currI = currI->next;
	}

	
	Event *prevJ = nullptr, *currJ = head;			// Search for Event J and its preceeding Event
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
	Event *tmp = currJ->next;
	currJ->next = currI->next;
	currI->next = tmp;

	// Set new indices
	currI->index = j;
	currJ->index = i;
}





Event* eventVector::operator[](unsigned int index) {
	if (index > mSize) {
		return nullptr;					// If index is out of bounds return null
	}
	Event *tmp = head;
	while(tmp->index != index) {
		tmp = tmp->next;
	}
	return tmp;
}