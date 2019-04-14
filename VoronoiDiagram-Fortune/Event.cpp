#include "Event.h"

Event::Event(VoronoiDiagram::Site* site) : type(Type::SITE), y(site->point.y), index(-1), site(site)
{

}

Event::Event():type(Type::CIRCLE) {
	
}

Event::Event(double y, Vector2 point, Arc* arc) : type(Type::CIRCLE), y(y), index(-1), point(point), arc(arc)
{


}
bool operator<(const Event& lhs, const Event& rhs)
{
    return lhs.y < rhs.y;
}

void Event::operator=(const Event& rhs) {
	arc = rhs.arc;
	next = rhs.next;
	point = rhs.point;
	site = rhs.site;
	index = rhs.index;
	y = rhs.y;
	type = rhs.type;
}


