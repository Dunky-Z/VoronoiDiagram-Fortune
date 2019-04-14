#pragma once

// My includes
#include "Box.h"
#include "Vector2Vector.h"


class FortuneAlgorithm;

class VoronoiDiagram
{
public:
    struct HalfEdge;
    struct Face;

    struct Site
    {
        unsigned int index;
        Vector2 point;
        Face* face;

		// Used in SiteVector
		Site* next;
    };

	struct SiteVector {
		Site* head;
		Site* tail;
		unsigned int mSize;

		// Constructor
		SiteVector();

		// Operations
		void push_back(Site* e);
		Site* back();
		const unsigned int size() const;

		// Operators
		Site* operator[](unsigned int index);

	};

    struct Vertex
    {
        Vector2 point;

		//Used in VertexList
		Vertex* listNext;

		//Used in VertexUnorderedSet
		Vertex* setNext;

    private:
        friend VoronoiDiagram;
    };

	struct VertexUnorderedSet {
		Vertex *head;
		int mSize;

		// Constructor
		VertexUnorderedSet();

		// Operations
		void emplace(Vertex* e);
		Vertex* pop();
	};

	struct VertexList {
		Vertex* head;
		Vertex* tail;
		int mSize;

		// Constructor
		VertexList();

		// Operations
		void emplace_back(Vertex* v);
		Vertex* back();
		void erase(Vertex* e);
		Vertex* get(unsigned int i);

	};

    struct HalfEdge
    {
        Vertex* origin = nullptr;
        Vertex* destination = nullptr;
        HalfEdge* twin = nullptr;
        Face* incidentFace;
        HalfEdge* prev = nullptr;
        HalfEdge* next = nullptr;

		// Used in HalfEdgeUnorderedSet
		HalfEdge* setNext;

		// Used in HalfEdgeList
		HalfEdge* listNext;

    private:
        friend VoronoiDiagram;
    };

	struct HalfEdgeUnorderedSet {
		HalfEdge* head;
		int mSize;

		// Constructor
		HalfEdgeUnorderedSet();

		// Operations
		void emplace(HalfEdge* e);
		bool find(HalfEdge* e);

	};

	struct HalfEdgeList {
		HalfEdge* head;
		HalfEdge* tail;
		int mSize;

		// Constructor
		HalfEdgeList();

		// Operations
		void emplace_back(HalfEdge* e);
		HalfEdge* back();
		void erase(HalfEdge *e);

	};

    struct Face
    {
        Site* site;
        HalfEdge* outerComponent;

		// Used in FaceVertex
		Face* next;
		int index;
    };

	struct FaceVector {
		Face* head;
		Face* tail;
		unsigned int mSize;

		// Constructor
		FaceVector();

		// Operations
		void push_back(Face* e);
		Face* back();
		Face* pop_front();
		unsigned int size();

		// Operators
		Face* operator[](unsigned int index);
	};

    VoronoiDiagram(Vector2Vector points);



    // Accessors
    Site* getSite(unsigned int i);
    unsigned int getNbSites() const;
    Face* getFace(unsigned int i);
    const VertexList getVertices() const;

    // Intersection with a box
    bool intersect(Box box);

	 Vector2Vector getFaceVertex(Face f);								// Gets all vertexes of a face
	 Vector2* getCentroid(Vector2Vector myVertices);						// Gets the centroid of a face
	 Vector2Vector getCentroids();										// Gets the centroids of all faces
		
private:
    SiteVector mSites;
    FaceVector mFaces;
    VertexList mVertices;
    HalfEdgeList mHalfEdges;
	Box::intersectionArray intersections;

    // Diagram construction
    friend FortuneAlgorithm;

    Vertex* createVertex(Vector2 point);
    Vertex* createCorner(Box box, Box::Side side);
    HalfEdge* createHalfEdge(Face* face);

    // Intersection with a box
    void link(Box box, HalfEdge* start, Box::Side startSide, HalfEdge* end, Box::Side endSide);
    void removeVertex(Vertex* vertex);
    void removeHalfEdge(HalfEdge* halfEdge);
};
