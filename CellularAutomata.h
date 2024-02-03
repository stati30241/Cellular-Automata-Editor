#pragma once

#include "Grid.h"


template <>
struct std::hash<sf::Vector2i> {
	std::size_t operator() (const sf::Vector2i& vec) const {
		return std::hash<int>()(vec.x) ^ std::hash<int>()(vec.y);
	}
};


class Rule {
public:
	char comparison;
	int number;
	std::string value;

	bool compare(int n) const;
};


class CellularAutomata {
private:
	enum class NeighborhoodType {
		MOORE,
		VON_NEUMANN,
		MAX_NEIGHBORHOODS
	};

private:
	NeighborhoodType m_neighborhood;
	int m_neighborhoodRadius;
	std::unordered_map<std::string, sf::Color> m_colors;
	std::unordered_map<std::string, std::vector<Rule>> m_rules;

	sf::Vector2i m_cellSize;
	std::unordered_map<sf::Vector2i, std::string> m_cells;

public:
	CellularAutomata(const sf::Vector2i& cellSize);

	std::vector<std::string> getStates() const;

	bool loadFromFile(std::string_view filename);

	void setCell(const sf::Vector2i& pos, std::string_view state);
	sf::Vector2i mapToCellPos(const sf::RenderTarget* target, const sf::Vector2i& pixel);
	void render(sf::RenderTarget* target);
	void doGeneration();
};
