// STL
#include <iostream>
#include <vector>
#include <chrono>
#include <random>
// SFML
#include <SFML/Graphics.hpp>
// My includes
#include "FortuneAlgorithm.h"
#include "Vector2Vector.h"

constexpr float WINDOW_WIDTH = 600.0f;
constexpr float WINDOW_HEIGHT = 600.0f;
constexpr float POINT_RADIUS = 0.005f;
constexpr float OFFSET = 1.0f;

Vector2Vector generatePoints(int nbPoints)
{
    uint64_t seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::cout << "seed: " << seed << '\n';
    std::default_random_engine generator(seed);
    std::uniform_real_distribution<double> distribution (0.0, 1.0);

    Vector2Vector points;
	for (int i = 0; i < nbPoints; ++i) {
		double x = distribution(generator);
		double y = distribution(generator);
		std::cout << "Pushing:\t(" << x << ", " << y << ")" << std::endl;
		points.push_back(new Vector2{ distribution(generator), distribution(generator) });
	}
    return points;
}

void drawPoint(sf::RenderWindow& window, Vector2 point, sf::Color color)
{
    sf::CircleShape shape(POINT_RADIUS);
    shape.setPosition(sf::Vector2f(point.x - POINT_RADIUS, 1 - point.y - POINT_RADIUS));
    shape.setFillColor(color);
    window.draw(shape);
}

void drawEdge(sf::RenderWindow& window, Vector2 origin, Vector2 destination, sf::Color color)
{
    sf::Vertex line[] =
    {
        sf::Vertex(sf::Vector2f(origin.x, 1.0f - origin.y), color),
        sf::Vertex(sf::Vector2f(destination.x, 1.0f - destination.y), color)
    };
    window.draw(line, 2, sf::Lines);
}

void drawPoints(sf::RenderWindow& window, VoronoiDiagram& diagram)
{
    for (unsigned int i = 0; i < diagram.getNbSites(); ++i)
        drawPoint(window, diagram.getSite(i)->point, sf::Color(100, 250, 50));
}

void drawCentroids(sf::RenderWindow& window, Vector2Vector myCentroids)
{
	for (unsigned int i = 0; i < myCentroids.size(); ++i)
		drawPoint(window, myCentroids[i], sf::Color(255, 0, 250));
}

void drawDiagram(sf::RenderWindow& window, VoronoiDiagram& diagram)
{
    for (unsigned int i = 0; i < diagram.getNbSites(); ++i)
    {
        const VoronoiDiagram::Site* site = diagram.getSite(i);
        Vector2 center = site->point;
        VoronoiDiagram::Face* face = site->face;
        VoronoiDiagram::HalfEdge* halfEdge = face->outerComponent;
        if (halfEdge == nullptr)
            continue;
        while (halfEdge->prev != nullptr)
        {
            halfEdge = halfEdge->prev;
            if (halfEdge == face->outerComponent)
                break;
        }
        VoronoiDiagram::HalfEdge* start = halfEdge;
        while (halfEdge != nullptr)
        {
            if (halfEdge->origin != nullptr && halfEdge->destination != nullptr)
            {
                Vector2 origin = (halfEdge->origin->point - center) * OFFSET + center;
                Vector2 destination = (halfEdge->destination->point - center) * OFFSET + center;
                drawEdge(window, origin, destination, sf::Color(251,22,121));
            }
            halfEdge = halfEdge->next;
            if (halfEdge == start)
                break;
        }
    }
}

VoronoiDiagram generateRandomDiagram(unsigned int nbPoints)
{
    // Generate points and construct diagram
	FortuneAlgorithm algorithm(generatePoints(nbPoints));
    auto start = std::chrono::steady_clock::now();
    algorithm.construct();
    auto duration = std::chrono::steady_clock::now() - start;
    std::cout << "construction: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << '\n';

    // Bound the diagram
    start = std::chrono::steady_clock::now();
    algorithm.bound(Box{-0.05, -0.05, 1.05, 1.05}); // Take the bounding box slightly bigger than the intersection box
    duration = std::chrono::steady_clock::now() - start;
    std::cout << "bounding: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << '\n';
    VoronoiDiagram diagram = algorithm.getDiagram();

    // Intersect the diagram with a box
    start = std::chrono::steady_clock::now();
    bool valid = diagram.intersect(Box{0.0, 0.0, 1.0, 1.0});
    duration = std::chrono::steady_clock::now() - start;
    std::cout << "intersection: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << '\n';
    if (!valid)
        throw std::runtime_error("An error occured in the box intersection algorithm");

    return diagram;
}

int main()
{
    unsigned int nbPoints = 30;
    VoronoiDiagram diagram = generateRandomDiagram(nbPoints);
	Vector2Vector c = diagram.getCentroids();
	std::cout << "\n\nAll vertices:\n" << std::endl;
	VoronoiDiagram::VertexList myVertices = diagram.getVertices();
	VoronoiDiagram::Vertex* tmp = myVertices.head;
	while (tmp->listNext != nullptr) {
		std::cout << "(" << tmp->point.x << ", " << tmp->point.y << ")" << std::endl;
		tmp = tmp->listNext;
	}

    // Display the diagram
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Voronoi Diagram", sf::Style::Default, settings);
    window.setView(sf::View(sf::FloatRect(-0.1f, -0.1f, 1.2f, 1.2f)));

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Key::N)
                diagram = generateRandomDiagram(nbPoints);
        }

        window.clear(sf::Color::Black);

        drawDiagram(window, diagram);
        drawPoints(window, diagram);
		//drawCentroids(window, c);

        window.display();
    }

    return 0;
}
