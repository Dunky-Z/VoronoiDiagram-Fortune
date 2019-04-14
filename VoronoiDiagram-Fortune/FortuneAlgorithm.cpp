#include "FortuneAlgorithm.h"
// My includes
#include "Arc.h"
#include "Event.h"


FortuneAlgorithm::FortuneAlgorithm(Vector2Vector points) : mDiagram(points)
{

}

FortuneAlgorithm::~FortuneAlgorithm() = default;

void FortuneAlgorithm::construct()
{
    // Initialize event queue
    for (unsigned int i = 0; i < mDiagram.getNbSites(); ++i)
        mEvents.push(new Event(mDiagram.getSite(i)));

    // Process events
    while (!mEvents.isEmpty())
    {
        Event *event = mEvents.pop();
        mBeachlineY = event->y;
        if(event->type == Event::Type::SITE)
            handleSiteEvent(event);
        else
            handleCircleEvent(event);
    }
}

VoronoiDiagram FortuneAlgorithm::getDiagram()
{
    return mDiagram;
}

void FortuneAlgorithm::handleSiteEvent(Event* event)
{
    VoronoiDiagram::Site* site = event->site;
    // 1. Check if the bachline is empty
    if (mBeachline.isEmpty())
    {
        mBeachline.setRoot(mBeachline.createArc(site));
        return;
    }
    // 2. Look for the arc above the site
    Arc* arcToBreak = mBeachline.locateArcAbove(site->point, mBeachlineY);
    deleteEvent(arcToBreak);
    // 3. Replace this arc by the new arcs
    Arc* middleArc = breakArc(arcToBreak, site);
    Arc* leftArc = middleArc->prev; 
    Arc* rightArc = middleArc->next;
    // 4. Add an edge in the diagram
    addEdge(leftArc, middleArc);
    middleArc->rightHalfEdge = middleArc->leftHalfEdge;
    rightArc->leftHalfEdge = leftArc->rightHalfEdge;
    // 5. Check circle events
    // Left triplet
    if (!mBeachline.isNil(leftArc->prev))
        addEvent(leftArc->prev, leftArc, middleArc);
    // Right triplet
    if (!mBeachline.isNil(rightArc->next))
        addEvent(middleArc, rightArc, rightArc->next);
}

void FortuneAlgorithm::handleCircleEvent(Event* event)
{
    Vector2 point = event->point;
    Arc* arc = event->arc;
    // 1. Add vertex
    VoronoiDiagram::Vertex* vertex = mDiagram.createVertex(point);
    // 2. Delete all the events with this arc
    Arc* leftArc = arc->prev;
    Arc* rightArc = arc->next;
    deleteEvent(leftArc);
    deleteEvent(rightArc);
    // 3. Update the beachline and the diagram
    removeArc(arc, vertex);
    // 4. Add new circle events
    // Left triplet
    if (!mBeachline.isNil(leftArc->prev))
        addEvent(leftArc->prev, leftArc, rightArc);
    // Right triplet
    if (!mBeachline.isNil(rightArc->next))
        addEvent(leftArc, rightArc, rightArc->next);
}

Arc* FortuneAlgorithm::breakArc(Arc* arc, VoronoiDiagram::Site* site)
{
    // Create the new subtree
    Arc* middleArc = mBeachline.createArc(site);
    Arc* leftArc = mBeachline.createArc(arc->site);
    leftArc->leftHalfEdge = arc->leftHalfEdge;
    Arc* rightArc = mBeachline.createArc(arc->site);
    rightArc->rightHalfEdge = arc->rightHalfEdge;
    // Insert the subtree in the beachline
    mBeachline.replace(arc, middleArc);
    mBeachline.insertBefore(middleArc, leftArc);
    mBeachline.insertAfter(middleArc, rightArc);
    // Delete old arc
    delete arc;
    // Return the middle arc
    return middleArc;
}

void FortuneAlgorithm::removeArc(Arc* arc, VoronoiDiagram::Vertex* vertex)
{
    // End edges
    setDestination(arc->prev, arc, vertex);
    setDestination(arc, arc->next, vertex);
    // Join the edges of the middle arc
    arc->leftHalfEdge->next = arc->rightHalfEdge;
    arc->rightHalfEdge->prev = arc->leftHalfEdge;
    // Update beachline
    mBeachline.remove(arc);
    // Create a new edge
    VoronoiDiagram::HalfEdge* prevHalfEdge = arc->prev->rightHalfEdge;
    VoronoiDiagram::HalfEdge* nextHalfEdge = arc->next->leftHalfEdge;
    addEdge(arc->prev, arc->next);
    setOrigin(arc->prev, arc->next, vertex);
    setPrevHalfEdge(arc->prev->rightHalfEdge, prevHalfEdge);
    setPrevHalfEdge(nextHalfEdge, arc->next->leftHalfEdge);
    // Delete node
    delete arc;
}

bool FortuneAlgorithm::isMovingRight(const Arc* left, const Arc* right) const
{
    return left->site->point.y < right->site->point.y;
}

double FortuneAlgorithm::getInitialX(const Arc* left, const Arc* right, bool movingRight) const
{
    return movingRight ? left->site->point.x : right->site->point.x;
}

void FortuneAlgorithm::addEdge(Arc* left, Arc* right)
{
    // Create two new half edges
    left->rightHalfEdge = mDiagram.createHalfEdge(left->site->face);
    right->leftHalfEdge = mDiagram.createHalfEdge(right->site->face);
    // Set the two half edges twins
    left->rightHalfEdge->twin = right->leftHalfEdge;
    right->leftHalfEdge->twin = left->rightHalfEdge;
}

void FortuneAlgorithm::setOrigin(Arc* left, Arc* right, VoronoiDiagram::Vertex* vertex)
{
    left->rightHalfEdge->destination = vertex;
    right->leftHalfEdge->origin = vertex;
}

void FortuneAlgorithm::setDestination(Arc* left, Arc* right, VoronoiDiagram::Vertex* vertex)
{
    left->rightHalfEdge->origin = vertex;
    right->leftHalfEdge->destination = vertex;
}

void FortuneAlgorithm::setPrevHalfEdge(VoronoiDiagram::HalfEdge* prev, VoronoiDiagram::HalfEdge* next)
{
    prev->next = next;
    next->prev = prev;
}

void FortuneAlgorithm::addEvent(Arc* left, Arc* middle, Arc* right)
{
    double y;
    Vector2 convergencePoint = computeConvergencePoint(left->site->point, middle->site->point, right->site->point, y);
    bool isBelow = y <= mBeachlineY;
    bool leftBreakpointMovingRight = isMovingRight(left, middle);
    bool rightBreakpointMovingRight = isMovingRight(middle, right);
    double leftInitialX = getInitialX(left, middle, leftBreakpointMovingRight);
    double rightInitialX = getInitialX(middle, right, rightBreakpointMovingRight);
    bool isValid =
        ((leftBreakpointMovingRight && leftInitialX < convergencePoint.x) ||
        (!leftBreakpointMovingRight && leftInitialX > convergencePoint.x)) &&
        ((rightBreakpointMovingRight && rightInitialX < convergencePoint.x) ||
        (!rightBreakpointMovingRight && rightInitialX > convergencePoint.x));
    if (isValid && isBelow)
    {
        Event *event = new Event(y, convergencePoint, middle);
		middle->event = event;
        mEvents.push(event);
    }
}

void FortuneAlgorithm::deleteEvent(Arc* arc)
{
    if (arc->event != nullptr)
    {
        mEvents.remove(arc->event->index);
        arc->event = nullptr;
    }
}

Vector2 FortuneAlgorithm::computeConvergencePoint(const Vector2& point1, const Vector2& point2, const Vector2& point3, double& y) const
{
    Vector2 v1 = (point1 - point2).getOrthogonal();
    Vector2 v2 = (point2 - point3).getOrthogonal();
    Vector2 delta = 0.5 * (point3 - point1);
    double t = delta.getDet(v2) / v1.getDet(v2);
    Vector2 center = 0.5 * (point1 + point2) + t * v1;
    double r = center.getDistance(point1);
    y = center.y - r;
    return center;
}

// Bound
bool FortuneAlgorithm::bound(Box box)
{
    // Make sure the bounding box contains all the vertices
	VoronoiDiagram::VertexList myVertices = mDiagram.getVertices();
	for(unsigned int i = 0; i < myVertices.mSize; i++)
    {
		VoronoiDiagram::Vertex* vertex = myVertices.get(i);
        box.left = min(vertex->point.x, box.left);
        box.bottom = min(vertex->point.y, box.bottom);
        box.right = max(vertex->point.x, box.right);
        box.top = max(vertex->point.y, box.top);
    }
    // Retrieve all non bounded half edges from the beach line
    LinkedVertexList linkedVertices;
    UnorderedMapLVA vertices;
    if (!mBeachline.isEmpty())
    {
        Arc* leftArc = mBeachline.getLeftmostArc();
        Arc* rightArc = leftArc->next;
        while (!mBeachline.isNil(rightArc))
        {
            // Bound the edge
            Vector2 direction = (leftArc->site->point - rightArc->site->point).getOrthogonal();
            Vector2 origin = (leftArc->site->point + rightArc->site->point) * 0.5f;
            // Line-box intersection
            Box::Intersection intersection = box.getFirstIntersection(origin, direction);
            // Create a new vertex and ends the half edges
            VoronoiDiagram::Vertex* vertex = mDiagram.createVertex(intersection.point);
            setDestination(leftArc, rightArc, vertex);
            // Initialize pointers
            if (vertices.find(leftArc->site->index) == false) 
                vertices.initialize(leftArc->site->index);
            if (vertices.find(rightArc->site->index) == false) 
                vertices.initialize(rightArc->site->index); 
            // Store the vertex on the boundaries
            linkedVertices.emplace_back(new LinkedVertex{nullptr, vertex, leftArc->rightHalfEdge});
			vertices[leftArc->site->index]->get(2 * static_cast<int>(intersection.side) + 1) = linkedVertices.back();
			vertices[leftArc->site->index]->get(2 * static_cast<int>(intersection.side) + 1)->setData(&*linkedVertices.back());
            linkedVertices.emplace_back(new LinkedVertex{rightArc->leftHalfEdge, vertex, nullptr});
            vertices[rightArc->site->index]->get(2 * static_cast<int>(intersection.side)) = linkedVertices.back();
			vertices[rightArc->site->index]->get(2 * static_cast<int>(intersection.side))->setData(&*linkedVertices.back());
            // Next edge
            leftArc = rightArc;
            rightArc = rightArc->next;
        }
    }
    // Add corners
    for (int j = 0; j < vertices.mSize; j++)
    {
		LinkedVertexArray* cellVertices = vertices.get(j);
        // We check twice the first side to be sure that all necessary corners are added
        for (unsigned int i = 0; i < 5; ++i)
        {
            unsigned int side = i % 4;
            unsigned int nextSide = (side + 1) % 4;
            // Add first corner
            if (cellVertices->find(2 * side) == false && cellVertices->find(2 * side + 1) == true)
            {
                unsigned int prevSide = (side + 3) % 4;
                VoronoiDiagram::Vertex* corner = mDiagram.createCorner(box, static_cast<Box::Side>(side));
                linkedVertices.emplace_back(new LinkedVertex{nullptr, corner, nullptr});
                cellVertices->get(2 * prevSide + 1) = linkedVertices.back();
				cellVertices->get(2 * prevSide + 1)->setData(&*linkedVertices.back());
                cellVertices->get(2 * side) = linkedVertices.back();
				cellVertices->get(2 * side)->setData(&*linkedVertices.back());
            }
            // Add second corner
            else if (cellVertices->find(2 * side) == true && cellVertices->find(2 * side + 1) == false)
            {
                VoronoiDiagram::Vertex* corner = mDiagram.createCorner(box, static_cast<Box::Side>(nextSide));
                linkedVertices.emplace_back(new LinkedVertex{nullptr, corner, nullptr});
                cellVertices->get(2 * side + 1) = linkedVertices.back();
				cellVertices->get(2 * side + 1)->setData(&*linkedVertices.back());
                cellVertices->get(2 * nextSide) = linkedVertices.back();
				cellVertices->get(2 * nextSide)->setData(&*linkedVertices.back());
            }
        }
    }

    // Join the half edges
    for (int i = 0; i < vertices.mSize; i++)
    {
		LinkedVertexArray* cellVertices = &*vertices.get(i);
		unsigned int index = cellVertices->index;
		for (unsigned int side = 0; side < 4; ++side)
        {
			
            if (cellVertices->find(2 * side) == true)
            {
				// Link vertices 
				VoronoiDiagram::HalfEdge* halfEdge = mDiagram.createHalfEdge(mDiagram.getFace(index));
				halfEdge->origin = cellVertices->get(2 * side)->vertex;
				halfEdge->destination = cellVertices->get(2 * side + 1)->vertex;
				cellVertices->get(2 * side)->nextHalfEdge = halfEdge;
				int x = ((2 * side) - 1);
				if (x == -1) { x = 7;}
				if (cellVertices->find(x))
					cellVertices->get(x)->nextHalfEdge = halfEdge;
				halfEdge->prev = cellVertices->get(2 * side)->prevHalfEdge;
				if (cellVertices->get(2 * side)->prevHalfEdge != nullptr)
					cellVertices->get(2 * side)->prevHalfEdge->next = halfEdge;
				cellVertices->get(2 * side + 1)->prevHalfEdge = halfEdge;
				if (cellVertices->find((2 * side + 2)) % 8)
					cellVertices->get(2 * side + 2)->prevHalfEdge = halfEdge;
				halfEdge->next = cellVertices->get(2 * side + 1)->nextHalfEdge;
				if (cellVertices->get(2 * side + 1)->nextHalfEdge != nullptr)
					cellVertices->get(2 * side + 1)->nextHalfEdge->prev = halfEdge;
            }
        }
    }
    return true; 
}

// Linked Vertex

void FortuneAlgorithm::LinkedVertex::setData(LinkedVertex* ptr) {
	this->nextHalfEdge = ptr->nextHalfEdge;
	this->prevHalfEdge = ptr->prevHalfEdge;
	this->vertex = ptr->vertex;
}

// LinkedVertexList

FortuneAlgorithm::LinkedVertexList::LinkedVertexList() {
	head = nullptr;
	tail = nullptr;
}

void FortuneAlgorithm::LinkedVertexList::emplace_back(LinkedVertex* e) {
	LinkedVertex *tmp = head;
	e->listNext = nullptr;

	if (head == nullptr)			// If list is empty make Event e the head
	{
		head = e;
		tail = e;
		return;
	}

	while (tmp->listNext != nullptr)		// Else traverse list until the last node
		tmp = tmp->listNext;

	tmp->listNext = e;					// Set tail's next as e
	tail = e;						// Set e as new tail
	return;
}

FortuneAlgorithm::LinkedVertex* FortuneAlgorithm::LinkedVertexList::back() {
	return tail;
}


// LinkedVertexArray

FortuneAlgorithm::LinkedVertexArray::LinkedVertexArray(unsigned int i, unsigned int o) {
	index = i;
	order = o;
}

FortuneAlgorithm::LinkedVertex* FortuneAlgorithm::LinkedVertexArray::initialize(unsigned int i) {
	// Base Case: If head is null then make the new LinkedVertexArray the head
	if (head == nullptr) {
		head = new LinkedVertex();
		head->index = i;
		head->arrayNext = nullptr;
		mSize++;
		return head;
	}
	else {				// Else initialize new linked vertex and add it to the front
		LinkedVertex* tmp = new LinkedVertex();
		tmp->index = i;
		tmp->arrayNext = head;
		head = tmp;
		mSize++;
		return head;
	}
}

FortuneAlgorithm::LinkedVertex*& FortuneAlgorithm::LinkedVertexArray::get(int i) {
	LinkedVertex* tmp = head;
	while (tmp != nullptr) {
		if (tmp->index == i)
			return tmp;
		tmp = tmp->arrayNext;
	}
	tmp = initialize(i);
	return tmp;
}

bool FortuneAlgorithm::LinkedVertexArray::isEmpty() {
	if (head == nullptr) {
		return true;
	}
	else {
		return false;
	}
}

bool FortuneAlgorithm::LinkedVertexArray::find(int i) {
	LinkedVertex* tmp = head;
	while (tmp != nullptr) {
		if (tmp->index == i)
			return true;
		tmp = tmp->arrayNext;
	}
	return false;
}

FortuneAlgorithm::LinkedVertex* FortuneAlgorithm::LinkedVertexArray::operator[](unsigned int i) {
	LinkedVertex* tmp = head;
	while (tmp != nullptr) {
		if (tmp->index == i)
			return tmp;
		tmp = tmp->arrayNext;
	}
	return initialize(i);
}

// Unordered Linked Vertex Array Map

FortuneAlgorithm::UnorderedMapLVA::UnorderedMapLVA() {
	head = nullptr;
	mSize = 0;
}

bool FortuneAlgorithm::UnorderedMapLVA::find(unsigned int i) {
	LinkedVertexArray* tmp = head;  // Initialize current 
	while (tmp != nullptr)
	{
		if (tmp->index == i)
			return true;
		tmp = tmp->next;
	}
	return false;
}

void FortuneAlgorithm::UnorderedMapLVA::initialize(unsigned int index) {
	// Base Case: If head is null then make the new LinkedVertexArray the head
	if (head == nullptr) {
		head = new LinkedVertexArray(index, mSize);
		head->next = nullptr;
		mSize++;
		return;
	}
	else {				// Else initialize new array and add it to the front
		LinkedVertexArray* tmp = new LinkedVertexArray(index, mSize);
		tmp->next = head;
		head = tmp;
		mSize++;
		return;
	}
}

FortuneAlgorithm::LinkedVertexArray* FortuneAlgorithm::UnorderedMapLVA::get(unsigned int i) {
	LinkedVertexArray* tmp = head;
	while (tmp != nullptr) {
		if (tmp->order == i)
			return tmp;
		tmp = tmp->next;
	}
}

FortuneAlgorithm::LinkedVertexArray* FortuneAlgorithm::UnorderedMapLVA::operator[](unsigned int i) {
	LinkedVertexArray* tmp = head;
	while (tmp != nullptr) {
		if (tmp->index == i) 
			return tmp;
		tmp = tmp->next;
	}
}

double FortuneAlgorithm::min(double x, double y) {
	if (x < y)
		return x;
	else
		return y;
}

double FortuneAlgorithm::max(double x, double y) {
	if (x > y)
		return x;
	else
		return y;
}

