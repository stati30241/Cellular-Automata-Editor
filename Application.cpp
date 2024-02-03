#include "Application.h"


Application::Application() {
	initialize();
}


Application::~Application() {
	ImGui::SFML::Shutdown();

	delete m_window;
}


void Application::initialize() {
	sf::VideoMode windowSize = { 1600u, 900u };
	sf::String windowTitle = "Cellular Automata Editor";
	size_t windowStyle = sf::Style::Close;

	m_window = new sf::RenderWindow{ windowSize, windowTitle, windowStyle };
	ImGui::SFML::Init(*m_window);

	m_grid = new Grid{ m_window, { 20, 20 }, sf::Color::Black };
	m_cellularAutomata = new CellularAutomata{ { 20, 20 } };
	m_cellularAutomata->loadFromFile("brians_brain.txt");
	m_currentState = m_cellularAutomata->getStates().at(0);
}


void Application::handleInputs() {
	sf::Event sfmlEvent{ };
	while (m_window->pollEvent(sfmlEvent)) {
		ImGui::SFML::ProcessEvent(sfmlEvent);
		if (!m_guiInFocus) m_grid->handleInputs(sfmlEvent);

		switch (sfmlEvent.type) {
		case sf::Event::Closed:
			m_window->close();
			break;

		case sf::Event::LostFocus:
			m_windowInFocus = false;
			break;

		case sf::Event::GainedFocus:
			m_windowInFocus = true;
			break;
		}
	}

	if (m_guiInFocus) return;

	// Drawing and erasing cells
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		if (!m_windowInFocus) return;
		m_cellularAutomata->setCell(m_cellularAutomata->mapToCellPos(m_window, sf::Mouse::getPosition(*m_window)), m_currentState);
	}
}


void Application::update() {
	sf::Time clockRestart = m_clock.restart();
	ImGui::SFML::Update(*m_window, clockRestart);

	float deltaTime = clockRestart.asSeconds();
	m_timer += deltaTime;
	if (m_timer >= m_tickSpeed) {
		if (m_startGenerations) m_cellularAutomata->doGeneration();
		m_timer = 0.0f;
	}

	std::vector<std::string> states = m_cellularAutomata->getStates();
	ImGui::Begin("stuff");
	m_guiInFocus = ImGui::IsWindowHovered();
	if (ImGui::BeginCombo("##combo", m_currentState.data())) // The second parameter is the label previewed before opening the combo.
	{
		for (int n = 0; n < states.size(); n++) {
			bool is_selected = (m_currentState == states.at(n)); // You can store your selection however you want, outside or inside your objects
			if (ImGui::Selectable(states.at(n).data(), is_selected)) {
				m_currentState = states[n];
				if (is_selected)
					ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
				}
			}
		ImGui::EndCombo();
	}

	if (ImGui::Button("Start/Stop", { 100, 50 })) m_startGenerations = !m_startGenerations;

	ImGui::End();
}


void Application::render() {
	m_window->clear({ 50, 40, 80 });

	m_cellularAutomata->render(m_window);
	m_grid->render();

	ImGui::SFML::Render(*m_window);
	m_window->display();
}


void Application::run() {
	while (m_window->isOpen()) {
		handleInputs();
		update();
		render();
	}
}
