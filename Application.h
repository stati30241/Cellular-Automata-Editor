#pragma once

#include "CellularAutomata.h"


// Main class that manages everything
class Application {
private:
	sf::RenderWindow* m_window;
	sf::Clock m_clock;
	bool m_windowInFocus = true;

	Grid* m_grid;
	CellularAutomata* m_cellularAutomata;
	std::string m_currentState;

	bool m_guiInFocus = false;
	bool m_startGenerations = false;

	const float m_tickSpeed = 0.1f;
	float m_timer = 0.0f;

private:
	void initialize();

	void handleInputs();
	void update();
	void render();

public:
	Application();
	~Application();

	void run();
};
