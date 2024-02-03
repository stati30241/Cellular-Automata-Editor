#include "Grid.h"


Grid::Grid(sf::RenderWindow* window, const sf::Vector2i& cellSize, const sf::Color& color)
	: m_window{ window }, m_cellSize{ cellSize }, m_color{ color } {

}


void Grid::pan(const sf::Vector2f& newMousePos) {
	// Return if we are not supposed to be panning right now
	if (!m_panning) return;

	// Calculates the offset and moves the view accordingly
	const sf::Vector2f offset = m_oldMousePos - newMousePos;
	sf::View view = m_window->getView();
	view.move(offset * m_totalZoom);
	m_window->setView(view);

	// Sets the old mouse position to the current mouse position
	m_oldMousePos = newMousePos;
}


void Grid::zoomAtPixel(float scrollDelta) {
	// Calculates if the user is zooming in or out
	float zoom = 1.0f;

	if (scrollDelta < 0.0f) {
		// User is zooming in
		if (m_totalZoom >= m_zoomLimitHigh) return;
		zoom = m_zoomFactor;
	} else if (scrollDelta > 0.0f) {
		// User is zooming out
		if (m_totalZoom <= m_zoomLimitLow) return;
		zoom = 1.0f / m_zoomFactor;
	}
	m_totalZoom *= zoom;

	// Calculates and zooms the view
	const sf::Vector2i pixel = sf::Mouse::getPosition(*m_window);
	const sf::Vector2f oldPixelPos = m_window->mapPixelToCoords(pixel);

	sf::View view = m_window->getView();
	view.zoom(zoom);
	m_window->setView(view);

	const sf::Vector2f newPixelPos = m_window->mapPixelToCoords(pixel);
	const sf::Vector2f offset = oldPixelPos - newPixelPos;

	view.move(offset);
	m_window->setView(view);
}


void Grid::handleInputs(const sf::Event& sfmlEvent) {
	switch (sfmlEvent.type) {
		// The mouse wheel is scrolled
	case sf::Event::MouseWheelScrolled:
		zoomAtPixel(sfmlEvent.mouseWheelScroll.delta);
		break;

		// A mouse button is pressed
	case sf::Event::MouseButtonPressed:
		if (sfmlEvent.mouseButton.button == sf::Mouse::Middle) {
			m_oldMousePos = sf::Vector2f{ sf::Mouse::getPosition(*m_window) };
			m_panning = true;
		}
		break;

		// A mouse button is released
	case sf::Event::MouseButtonReleased:
		if (sfmlEvent.mouseButton.button == sf::Mouse::Middle) {
			m_panning = false;
		}
		break;

		// The mouse has been moved
	case sf::Event::MouseMoved:
		pan(sf::Vector2f{ sf::Mouse::getPosition(*m_window) });
		break;
	}
}


void Grid::render() {
	// Calculates the top left and bottom right of the view
	sf::View view = m_window->getView();
	sf::Vector2f topLeft = view.getCenter() - view.getSize() / 2.0f;
	sf::Vector2f bottomRight = view.getCenter() + view.getSize() / 2.0f;

	// Rounds the values of top left up and bottom right down
	sf::Vector2i topLeftRounded, bottomRightRounded;
	topLeftRounded.x = std::truncf(topLeft.x) + 1;
	topLeftRounded.y = std::truncf(topLeft.y) + 1;
	bottomRightRounded.x = std::truncf(bottomRight.x);
	bottomRightRounded.y = std::truncf(bottomRight.y);

	// Modular arithmetic to determine closest multiple of cellSize
	sf::Vector2f topLeftOffset, bottomRightOffset;
	topLeftOffset.x = mod(topLeftRounded.x, m_cellSize.x);
	topLeftOffset.y = mod(topLeftRounded.y, m_cellSize.y);
	bottomRightOffset.x = mod(bottomRightRounded.x, m_cellSize.x);
	bottomRightOffset.y = mod(bottomRightRounded.x, m_cellSize.x);

	// Gets the start and end positions
	topLeft = sf::Vector2f{ topLeftRounded } - topLeftOffset;
	bottomRight = sf::Vector2f{ bottomRightRounded } - bottomRightOffset + sf::Vector2f{ m_cellSize };

	// Calculates the lines for the grid
	sf::VertexArray grid{ sf::Lines };
	for (float i = topLeft.x; i <= bottomRight.x; i += m_cellSize.x) {
		grid.append({ { i, topLeft.y }, m_color });
		grid.append({ { i, bottomRight.y }, m_color });
	}
	for (float j = topLeft.y; j <= bottomRight.y; j += m_cellSize.y) {
		grid.append({ { topLeft.x, j }, m_color });
		grid.append({ {bottomRight.x, j}, m_color });
	}

	// Renders the vertex array
	m_window->draw(grid);
}
