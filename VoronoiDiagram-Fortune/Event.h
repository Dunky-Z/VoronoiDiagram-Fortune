#pragma once

// My includes
#include "Vector2.h"
#include "VoronoiDiagram.h"

struct Arc;

class Event
{
public:
    enum class Type{SITE, CIRCLE};

	// Used in eventVector
	Event *next;

	Event();
    // Site event
    Event(VoronoiDiagram::Site* site);
    // Circle event
    Event(double y, Vector2 point, Arc* arc);

    Type type;
    double y;
    int index;
    // Site event
    VoronoiDiagram::Site* site;
    // Circle event
    Vector2 point;
    Arc* arc;

	void operator=(const Event& rhs);

};

bool operator<(const Event& lhs, const Event& rhs);

