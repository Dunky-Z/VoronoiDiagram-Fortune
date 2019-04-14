#include "VoronoiDiagram.h"

VoronoiDiagram::VoronoiDiagram(Vector2Vector points)
{
    for(unsigned int i = 0; i < points.size(); ++i)
    {
        mSites.push_back(new VoronoiDiagram::Site{i, points[i], nullptr});
        mFaces.push_back(new VoronoiDiagram::Face{mSites.back(), nullptr});
        mSites.back()->face = mFaces.back();
    }
}

VoronoiDiagram::Site* VoronoiDiagram::getSite(unsigned int i)
{
    return mSites[i];
}

unsigned int VoronoiDiagram::getNbSites() const
{
    return mSites.size();
}

VoronoiDiagram::Face* VoronoiDiagram::getFace(unsigned int i)
{
    return mFaces[i];
}

const VoronoiDiagram::VertexList VoronoiDiagram::getVertices() const
{
    return mVertices;
}


bool VoronoiDiagram::intersect(Box box)
{
    bool error = false;
    HalfEdgeUnorderedSet processedHalfEdges;
    VertexUnorderedSet verticesToRemove;
    for (int i = 0; i < mSites.size(); i++)
    {
		Site site = *mSites[i];
        HalfEdge* halfEdge = site.face->outerComponent;
        bool inside = box.contains(halfEdge->origin->point);
        bool outerComponentDirty = !inside;
        HalfEdge* incomingHalfEdge = nullptr; // First half edge coming in the box
        HalfEdge* outgoingHalfEdge = nullptr; // Last half edge going out the box
        Box::Side incomingSide, outgoingSide;
		
        do
        {
			int nbIntersections = box.getIntersections(halfEdge->origin->point, halfEdge->destination->point, intersections);			
            bool nextInside = box.contains(halfEdge->destination->point);
            HalfEdge* nextHalfEdge = halfEdge->next;
            // The two points are outside the box 
            if (!inside && !nextInside)
            {
                // The edge is outside the box
                if (nbIntersections == 0)
                {
                    verticesToRemove.emplace(halfEdge->origin);
                    removeHalfEdge(halfEdge);
                }
                // The edge crosses twice the frontiers of the box
                else if (nbIntersections == 2)
                {
                    verticesToRemove.emplace(halfEdge->origin);
                    if (processedHalfEdges.find(halfEdge->twin))
                    {
                        halfEdge->origin = halfEdge->twin->destination;
                        halfEdge->destination = halfEdge->twin->origin;
                    }
                    else
                    {
                        halfEdge->origin = createVertex(intersections[0]->point);
                        halfEdge->destination = createVertex(intersections[1]->point);
                    }
                    if (outgoingHalfEdge != nullptr)
                        link(box, outgoingHalfEdge, outgoingSide, halfEdge, intersections[0]->side);
                    if (incomingHalfEdge == nullptr)
                    {
                       incomingHalfEdge = halfEdge;
                       incomingSide = intersections[0]->side;
                    }
                    outgoingHalfEdge = halfEdge;
                    outgoingSide = intersections[1]->side;
                    processedHalfEdges.emplace(halfEdge);
                }
                else
                    error = true;
            }
            // The edge is going outside the box
            else if (inside && !nextInside)
            {
                if (nbIntersections == 1)
                {
                    if (processedHalfEdges.find(halfEdge->twin))
                        halfEdge->destination = halfEdge->twin->origin;
                    else
                        halfEdge->destination = createVertex(intersections[0]->point);
                    outgoingHalfEdge = halfEdge;
                    outgoingSide = intersections[0]->side;
                    processedHalfEdges.emplace(halfEdge);
                }
                else
                    error = true;
            }
            // The edge is coming inside the box
            else if (!inside && nextInside)
            {
                if (nbIntersections == 1)
                {
                    verticesToRemove.emplace(halfEdge->origin);
                    if (processedHalfEdges.find(halfEdge->twin))
                        halfEdge->origin = halfEdge->twin->destination;
                    else
                        halfEdge->origin = createVertex(intersections[0]->point);
                    if (outgoingHalfEdge != nullptr)
                        link(box, outgoingHalfEdge, outgoingSide, halfEdge, intersections[0]->side);
                    if (incomingHalfEdge == nullptr)
                    {
                       incomingHalfEdge = halfEdge;
                       incomingSide = intersections[0]->side;
                    }
                    processedHalfEdges.emplace(halfEdge);
                }
                else
                    error = true;
            }
            halfEdge = nextHalfEdge;
            // Update inside
            inside = nextInside;
        } while (halfEdge != site.face->outerComponent);
        // Link the last and the first half edges inside the box
        if (outerComponentDirty && incomingHalfEdge != nullptr)
            link(box, outgoingHalfEdge, outgoingSide, incomingHalfEdge, incomingSide);
        // Set outer component
        if (outerComponentDirty)
            site.face->outerComponent = incomingHalfEdge;
    }


	for (int i = 0; i < verticesToRemove.mSize-1; i++) {
		Vertex* v = verticesToRemove.pop();
		if (v == nullptr) continue;
		removeVertex(v);
	}
    // Return the status
    return !error;
}

VoronoiDiagram::Vertex* VoronoiDiagram::createVertex(Vector2 point)
{
	Vertex* v = new Vertex();
	v->point = point;
	mVertices.emplace_back(v);
	return mVertices.back();
}

VoronoiDiagram::Vertex* VoronoiDiagram::createCorner(Box box, Box::Side side)
{
    switch (side)
    {
        case Box::Side::LEFT:
            return createVertex(Vector2(box.left, box.top));
        case Box::Side::BOTTOM:
            return createVertex(Vector2(box.left, box.bottom));
        case Box::Side::RIGHT:
            return createVertex(Vector2(box.right, box.bottom));
        case Box::Side::TOP:
            return createVertex(Vector2(box.right, box.top));
        default:
            return nullptr;
    }
}

VoronoiDiagram::HalfEdge* VoronoiDiagram::createHalfEdge(Face* face)
{
	HalfEdge *h = new HalfEdge();
	h->incidentFace = face;
    mHalfEdges.emplace_back(h);
    if(face->outerComponent == nullptr)
        face->outerComponent = mHalfEdges.back();
    return mHalfEdges.back();
}

void VoronoiDiagram::link(Box box, HalfEdge* start, Box::Side startSide, HalfEdge* end, Box::Side endSide)
{
    HalfEdge* halfEdge = start;
    int side = static_cast<int>(startSide);
    while (side != static_cast<int>(endSide))
    {
        side = (side + 1) % 4;
        halfEdge->next = createHalfEdge(start->incidentFace);
        halfEdge->next->prev = halfEdge;
        halfEdge->next->origin = halfEdge->destination;
        halfEdge->next->destination = createCorner(box, static_cast<Box::Side>(side));
        halfEdge = halfEdge->next;
    }
    halfEdge->next = createHalfEdge(start->incidentFace);
    halfEdge->next->prev = halfEdge;
    end->prev = halfEdge->next;
    halfEdge->next->next = end;
    halfEdge->next->origin = halfEdge->destination;
    halfEdge->next->destination = end->origin;
}

void VoronoiDiagram::removeVertex(Vertex* vertex)
{
	mVertices.erase(vertex);
}

void VoronoiDiagram::removeHalfEdge(HalfEdge* halfEdge)
{
    mHalfEdges.erase(halfEdge);
}


// Algorithms for finding centroids

Vector2Vector VoronoiDiagram::getFaceVertex(Face f) {
	Vector2Vector myVertices;
	HalfEdge *startingEdge = f.outerComponent;
	myVertices.push_back(new Vector2{ f.outerComponent->origin->point.x, f.outerComponent->origin->point.y });		// Push outercomponent's starting vertex

	// Push all other edge's starting vertexs
	HalfEdge *tmp = startingEdge->next;
	while (tmp != startingEdge) {
		myVertices.push_back(new Vector2{ tmp->origin->point.x, tmp->origin->point.y });
		tmp = tmp->next;
	}

	return myVertices;
}

Vector2* VoronoiDiagram::getCentroid(Vector2Vector myVertices) {
	Vector2* centroid = new Vector2{ 0, 0 };
	double signedArea = 0.0;
	double x0 = 0.0; // Current vertex X
	double y0 = 0.0; // Current vertex Y
	double x1 = 0.0; // Next vertex X
	double y1 = 0.0; // Next vertex Y
	double a = 0.0;  // Partial signed area

	// For all vertices
	int i = 0;
	for (i = 0; i < myVertices.size(); ++i)
	{
		x0 = myVertices[i].x;
		y0 = myVertices[i].y;
		x1 = myVertices[(i + 1) % myVertices.size()].x;
		y1 = myVertices[(i + 1) % myVertices.size()].y;
		a = x0 * y1 - x1 * y0;
		signedArea += a;
		centroid->x += (x0 + x1)*a;
		centroid->y += (y0 + y1)*a;
	}

	signedArea *= 0.5;
	centroid->x /= (6.0*signedArea);
	centroid->y /= (6.0*signedArea);

	return centroid;
}

 Vector2Vector VoronoiDiagram::getCentroids() {
	 Vector2Vector centroids;
	 for (unsigned int i = 0; i < mFaces.mSize; ++i) {
		 centroids.push_back(getCentroid(getFaceVertex(*mFaces[i])));
	 }
	 return centroids;
 }

// Site Vector

VoronoiDiagram::SiteVector::SiteVector() {
	head = nullptr;
	tail = nullptr;
}

void VoronoiDiagram::SiteVector::push_back(Site* e) {
	Site *tmp = head;
	e->next = nullptr;

	if (head == nullptr)			// If list is empty make Site e the head
	{
		head = e;
		tail = e;
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

VoronoiDiagram::Site* VoronoiDiagram::SiteVector::back() {
	return tail;
}

const unsigned int VoronoiDiagram::SiteVector::size() const {
	return mSize;
}

VoronoiDiagram::Site* VoronoiDiagram::SiteVector::operator[](unsigned int index) {
	if (index > mSize) {
		return nullptr;					// If index is out of bounds return null
	}
	Site* tmp = head;
	while (tmp->index != index) {
		tmp = tmp->next;
	}
	return tmp;
}

// VertexUnorderedSet

VoronoiDiagram::VertexUnorderedSet::VertexUnorderedSet() {
	head = nullptr;
	mSize = 0;
}

// Traverse through the set, if no vertex in the unordered set matches e, then put e at the end of the set.
// Only inserts element if the value is unique within the set
void VoronoiDiagram::VertexUnorderedSet::emplace(Vertex* e) {
	// Base Case: If head is null then set head equal to e
	if (head == nullptr) {
		head = e;
		mSize++; 
		return;
	}

	Vertex *tmp = head;
	Vertex *tmpPrev = head;
	while (tmp != nullptr) {
		if (tmp->point == e->point) {
			return;
		}
		else {
			tmpPrev = tmp;
			tmp = tmp->setNext;
		}
	}
	tmpPrev->setNext = tmp;
	mSize++;						// Increment mSize
	return;
}

VoronoiDiagram::Vertex* VoronoiDiagram::VertexUnorderedSet::pop() {
	Vertex* tmp = head;				// Get head
	if (tmp == nullptr) return nullptr;
	head = head->setNext;			// Set head to 2nd in list
	tmp->setNext = nullptr;			// Set next to null
	mSize--;						// Decrement mSize
	return tmp;						// Return
}

// HalfEdgeUnorderedSet

VoronoiDiagram::HalfEdgeUnorderedSet::HalfEdgeUnorderedSet() {
	head = nullptr;
}

void VoronoiDiagram::HalfEdgeUnorderedSet::emplace(HalfEdge* e) {
	// Base Case: If head is null then set head equal to e
	if (head == nullptr) {
		head = e;
		mSize++;
		return;
	}

	HalfEdge *tmp = head;
	HalfEdge *tmpPrev = head;
	while (tmp != nullptr) {
		if (tmp->origin != e->origin) {
			return;
		}
		else {
			tmpPrev = tmp;
			tmp = tmp->setNext;
		}
	}
	tmpPrev->setNext = tmp;
	mSize++;						// Increment mSize
	return;
}

bool VoronoiDiagram::HalfEdgeUnorderedSet::find(HalfEdge* e) {
	HalfEdge* tmp = head;  // Initialize current 
	while (tmp != nullptr)
	{
		if (tmp->origin == e->origin)
			return true;
		tmp = tmp->setNext;
	}
	return false;
}

// FaceVector

VoronoiDiagram::FaceVector::FaceVector() {
	head = nullptr;
	tail = nullptr;
}

void VoronoiDiagram::FaceVector::push_back(Face* e) {
	Face *tmp = head;
	e->next = nullptr;
	e->index = size();

	if (head == nullptr)			// If list is empty make Face e the head
	{
		head = e;
		tail = e;
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

VoronoiDiagram::Face* VoronoiDiagram::FaceVector::back() {
	return tail;
}

unsigned int VoronoiDiagram::FaceVector::size() {
	return mSize;
}

VoronoiDiagram::Face* VoronoiDiagram::FaceVector::operator[](unsigned int index) {
	if (index > mSize) {
		return nullptr;					// If index is out of bounds return null
	}
	Face* tmp = head;
	while (tmp->index != index) {
		tmp = tmp->next;
	}
	return tmp;
}


VoronoiDiagram::Face* VoronoiDiagram::FaceVector::pop_front() {

	Face* prevHead = head;
	head = head->next;				// Set new head

	Face* tmp = head;						// Set tmp to new head
	while (tmp != nullptr) {	// Decrement all indexes
		tmp->index--;
		tmp = tmp->next;
	}

	mSize--;							// Decrement size
	prevHead->next = nullptr;			// Remove previous head's next link
	return prevHead;
}

// HalfEdgeList

VoronoiDiagram::HalfEdgeList::HalfEdgeList() {
	head = nullptr;
	tail = nullptr;
	mSize = 0;
}

void VoronoiDiagram::HalfEdgeList::emplace_back(HalfEdge* e) {
	HalfEdge *tmp = head;
	e->listNext = nullptr;

	if (head == nullptr)			// If list is empty make HalfEdge e the head
	{
		head = e;
		tail = e;
		mSize++;
		return;
	}

	while (tmp->listNext != nullptr)		// Else traverse list until the last node
		tmp = tmp->listNext;

	tmp->listNext = e;					// Set tail's next as e
	tail = e;						// Set e as new tail
	mSize++;
	return;
}

VoronoiDiagram::HalfEdge* VoronoiDiagram::HalfEdgeList::back() {
	return tail;
}


void VoronoiDiagram::HalfEdgeList::erase(HalfEdge* e) {
	// Store head node 
	HalfEdge *tmp = head;
	HalfEdge* tmpPrev = head;

	// If head node itself holds the key to be deleted 
	if (tmp == e)
	{
		head = head->listNext;   // Changed head 
		mSize--;
		return;
	}

	// Search for the key to be deleted, keep track of the 
	// previous node as we need to change 'tmpPrev->listNext' 
	while (tmp != nullptr && tmp != e)
	{
		tmpPrev = tmp;
		tmp = tmp->listNext;
	}

	// If halfedge was not present in linked list 
	if (tmp == nullptr) return;

	// Unlink the halfedge from linked list 
	tmpPrev->listNext = tmp->listNext;
	mSize--;

}

// VertexList

VoronoiDiagram::VertexList::VertexList() {
	head = nullptr;
	tail = nullptr;
	mSize = 0;
}

void VoronoiDiagram::VertexList::emplace_back(Vertex* e) {
	Vertex* tmp = head;
	e->listNext = nullptr;

	if (head == nullptr)			// If list is empty make HalfEdge e the head
	{
		head = e;
		tail = e;
		mSize++;
		return;
	}

	while (tmp->listNext != nullptr)		// Else traverse list until the last node
		tmp = tmp->listNext;

	tmp->listNext = e;					// Set tail's next as e
	tail = e;						// Set e as new tail
	mSize++;
	return;
}

VoronoiDiagram::Vertex* VoronoiDiagram::VertexList::back() {
	return tail;
}

void VoronoiDiagram::VertexList::erase(Vertex *e) {
	// Store head node 
	Vertex* tmp = head;
	Vertex* tmpPrev = head;

	// If head node itself holds the key to be deleted 
	if (tmp == e)
	{
		head = head->listNext;   // Changed head 
		mSize--;
		return;
	}

	// Search for the key to be deleted, keep track of the 
	// previous node as we need to change 'tmpPrev->listNext' 
	while (tmp != nullptr && tmp != e)
	{
		tmpPrev = tmp;
		tmp = tmp->listNext;
	}

	// If halfedge was not present in linked list 
	if (tmp == nullptr) return;

	// Unlink the halfedge from linked list 
	tmpPrev->listNext = tmp->listNext;
	mSize--;

}

VoronoiDiagram::Vertex* VoronoiDiagram::VertexList::get(unsigned int index) {
	// Base Case: if index is greater than node in list, return nullptr
	if (index > mSize) {
		return nullptr;
	}

	// Else traverse list until tmp is at position of index
	Vertex* tmp = head;
	int counter = 0;

	while (counter != index) {
		tmp = tmp->listNext;
		counter++;
	}
	return tmp;
}