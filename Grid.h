#pragma once

#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/Graphics.hpp>

#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>


// Modulus function because the % operator doesn't work quiet right for negative values
inline int mod(int a, int b) {
	return ((a % b) + b) % b;
}


// Class to manage rendering the grid
class Grid {
private:
	sf::RenderWindow* m_window;
	sf::Vector2i m_cellSize;

	bool m_panning = false;
	sf::Vector2f m_oldMousePos;

	float m_zoomFactor = 1.1f;
	float m_zoomLimitLow = 0.4f;
	float m_zoomLimitHigh = 4.0f;
	float m_totalZoom = 1.0f;

	sf::Color m_color;

private:
	// Pans the view according to the movement of the mouse
	void pan(const sf::Vector2f& newMousePos);
	// Zooms the view at the given pixel
	void zoomAtPixel(float scrollDelta);

public:
	// Constructors
	Grid(sf::RenderWindow* window, const sf::Vector2i& cellSize, const sf::Color& color);

	// Getters and setters
	float getZoomFactor() const { return m_zoomFactor; }
	void setZoomFactor(float zoomFactor) { m_zoomFactor = zoomFactor; }
	void setZoomLimitLow(float zoomLimitLow) { m_zoomLimitLow = zoomLimitLow; }
	void setZoomLimitHigh(float zoomLimitHigh) { m_zoomLimitHigh = zoomLimitHigh; }
	void setColor(const sf::Color& color) { m_color = color; }

	// Handles panning and zooming based on the user input
	void handleInputs(const sf::Event& sfmlEvent);

	// Renders the grid onto the window
	void render();
};
