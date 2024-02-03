#include "CellularAutomata.h"


bool Rule::compare(int n) const {
	switch (comparison) {
	case '=':
		return n == number;
	case '>':
		return n > number;
	case '<':
		return n < number;
	case ']':
		return n >= number;
	case '[':
		return n <= number;
	}
}


CellularAutomata::CellularAutomata(const sf::Vector2i& cellSize)
	: m_cellSize{ cellSize }, m_neighborhood { },
	m_neighborhoodRadius{ }, m_colors{}, m_rules{} {

}


bool CellularAutomata::loadFromFile(std::string_view filename) {
	std::ifstream ifs{ filename.data() };
	if (!ifs.is_open()) return false;

	int neighborhood;
	std::string token = "";
	ifs >> token >> neighborhood >> m_neighborhoodRadius;
	m_neighborhood = static_cast<NeighborhoodType>(neighborhood);

	std::string currentName = "";
	for (std::string line = ""; std::getline(ifs, line);) {
		if (line.empty()) continue;

		std::stringstream lineStream{ line };
		std::string name;
		lineStream >> name;
		if (name != "-") {
			uint32_t colorNum;
			sf::Color color;
			lineStream >> std::hex >> colorNum;
			color.r = (colorNum & 0xff000000UL) >> 24;
			color.g = (colorNum & 0x00ff0000UL) >> 16;
			color.b = (colorNum & 0x0000ff00UL) >> 8;
			color.a = (colorNum & 0x000000ffUL);

			m_colors.insert(std::make_pair(name, color));
			m_rules.insert(std::make_pair(name, std::vector<Rule>{}));
			currentName = name;
			continue;
		}

		Rule rule;
		lineStream >> token >> rule.comparison >> rule.number >> rule.value;
		m_rules.at(currentName).push_back(rule);
	}

	return true;
}


std::vector<std::string> CellularAutomata::getStates() const {
	std::vector<std::string> states;
	for (const auto& pair : m_colors) {
		states.push_back(pair.first);
	}

	return states;
}


void CellularAutomata::setCell(const sf::Vector2i& pos, std::string_view state) {
	if (m_colors.at(state.data()) == sf::Color{ 0, 0, 0, 0 }) m_cells.erase(pos);
	else m_cells.insert(std::make_pair(pos, state.data()));
}


sf::Vector2i CellularAutomata::mapToCellPos(const sf::RenderTarget* target, const sf::Vector2i& pixel) {
	const sf::Vector2i wordCoords = sf::Vector2i{ target->mapPixelToCoords(pixel) };
	sf::Vector2i cellPos;
	cellPos.x = wordCoords.x - mod(wordCoords.x, m_cellSize.x);
	cellPos.y = wordCoords.y - mod(wordCoords.y, m_cellSize.y);
	return cellPos;
}


void CellularAutomata::render(sf::RenderTarget* target) {
	// If the target pointer points to null
	if (!target) return;

	// Calculates the viewport
	sf::View view = target->getView();
	sf::FloatRect viewPort = { view.getCenter() - view.getSize() / 2.0f, view.getSize() };

	// Renders all the live cells one by one
	for (const auto& cell : m_cells) {
		// Don't render the cell if its out of the viewport
		if (!viewPort.contains(sf::Vector2f{ cell.first })) continue;

		// Calculates the rectangle for the cell and renders it
		sf::RectangleShape cellRect{ sf::Vector2f{ m_cellSize } };
		cellRect.setPosition(sf::Vector2f{ cell.first });
		cellRect.setFillColor(m_colors.at(cell.second));
		target->draw(cellRect);
	}
}


void CellularAutomata::doGeneration() {
	std::vector<sf::Vector2i> neighboringCells;
	switch (m_neighborhood) {
	case NeighborhoodType::MOORE:
		neighboringCells = {
			{ -1, -1 }, { 0, -1 }, { +1, -1 },
			{ -1, 0 }, { +1, 0 },
			{ -1, +1 }, { 0, +1 }, { +1, +1 }
		};
		break;
	case NeighborhoodType::VON_NEUMANN:
		neighboringCells = {
			{ 0, -1 }, { -1, 0 }, { +1, 0 }, { 0, +1 }
		};
		break;
	}

	std::unordered_map<sf::Vector2i, std::string> cellsCopy = m_cells;
	std::unordered_set<sf::Vector2i> otherCells;
	for (auto& cell : m_cells) {
		int numNeighbors = 0;
		for (const auto& neighbor : neighboringCells) {
			if (!m_cells.contains(m_cellSize.x * neighbor + cell.first)) {
				otherCells.insert(m_cellSize.x * neighbor + cell.first);
				continue;
			}
			if (m_cells.at(m_cellSize.x * neighbor + cell.first) == "Alive") numNeighbors++;
		}

		for (const Rule& rule : m_rules.at(cell.second)) {
			if (rule.compare(numNeighbors)) cellsCopy.at(cell.first) = rule.value;
		}
	}

	for (auto& cell : otherCells) {
		int numNeighbors = 0;
		for (const auto& neighbor : neighboringCells) {
			if (m_cells.contains(m_cellSize.x * neighbor + cell) && m_cells.at(m_cellSize.x * neighbor + cell) == "Alive") numNeighbors++;
		}

		for (const Rule& rule : m_rules.at("Dead")) {
			if (rule.compare(numNeighbors)) cellsCopy.insert(std::make_pair(cell, rule.value));
		}
	}

	m_cells = cellsCopy;
}
