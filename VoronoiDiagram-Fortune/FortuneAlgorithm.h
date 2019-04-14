#pragma once

// My includes
#include "PriorityQueue.h"
#include "VoronoiDiagram.h"
#include "Beachline.h"
#include "Vector2Vector.h"


struct Arc;
class Event;

class FortuneAlgorithm
{
public:
    
    FortuneAlgorithm(Vector2Vector points);
    ~FortuneAlgorithm();

    void construct();
    bool bound(Box box);

    VoronoiDiagram getDiagram();

private:
    VoronoiDiagram mDiagram;
    Beachline mBeachline;
    PriorityQueue mEvents;
    double mBeachlineY;

    // Algorithm
    void handleSiteEvent(Event* event);
    void handleCircleEvent(Event* event);

    // Arcs
    Arc* breakArc(Arc* arc, VoronoiDiagram::Site* site);
    void removeArc(Arc* arc, VoronoiDiagram::Vertex* vertex);

    // Breakpoint
    bool isMovingRight(const Arc* left, const Arc* right) const;
    double getInitialX(const Arc* left, const Arc* right, bool movingRight) const;

    // Edges
    void addEdge(Arc* left, Arc* right);
    void setOrigin(Arc* left, Arc* right, VoronoiDiagram::Vertex* vertex);
    void setDestination(Arc* left, Arc* right, VoronoiDiagram::Vertex* vertex);
    void setPrevHalfEdge(VoronoiDiagram::HalfEdge* prev, VoronoiDiagram::HalfEdge* next);

    // Events
    void addEvent(Arc* left, Arc* middle, Arc* right);
    void deleteEvent(Arc* arc);
    Vector2 computeConvergencePoint(const Vector2& point1, const Vector2& point2, const Vector2& point3, double& y) const;

    // Bounding

    struct LinkedVertex
    {
        VoronoiDiagram::HalfEdge* prevHalfEdge;
        VoronoiDiagram::Vertex* vertex;
        VoronoiDiagram::HalfEdge* nextHalfEdge;

		// Operation
		void setData(LinkedVertex* ptr);

		//Used in LinkedVertexList
		LinkedVertex* listNext;

		// Used in LinkedVertexArray
		LinkedVertex* arrayNext;
		unsigned int index;
    };


public:
	double min(double x, double y);
	double max(double x, double y);

	struct LinkedVertexList {
		LinkedVertex* head;
		LinkedVertex* tail;

		// Constructor
		LinkedVertexList();

		// Operations
		void emplace_back(LinkedVertex*);
		LinkedVertex* back();
	};

	struct LinkedVertexArray {

		// Used in UnorderedMapLVA
		unsigned int index;
		unsigned int order;
		LinkedVertexArray* next;

		int mSize = 0;
		LinkedVertex* head = nullptr;

		// Constructor
		LinkedVertexArray(unsigned int i, unsigned int o);

		// Operators
		LinkedVertex* operator[](unsigned int i);

		// Operations
		LinkedVertex*& get(int i);
		bool find(int i);
		bool isEmpty();
		LinkedVertex* initialize(unsigned int i);
	};

	// Unordered Linked Vertex Array Map
	struct UnorderedMapLVA {

		LinkedVertexArray *head;
		int mSize;

		// Constructor
		UnorderedMapLVA();

		// Operations
		bool find(unsigned int i);
		void initialize(unsigned int index);
		LinkedVertexArray* get(unsigned int i);

		// Operators
		LinkedVertexArray* operator[](unsigned int i);

	};

	
};
