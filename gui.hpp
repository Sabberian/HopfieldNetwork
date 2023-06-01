#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include "image.hpp"

#ifndef GUI_HPP
#define GUI_HPP

const int InputWindowWidth = 600;
const int InputWindowHeight = 600;
const int DrawingWindowWidth = 600;
const int DrawingWindowHeight = 600;
const int BorderWidth = 1;
const sf::Color BackgroundColor = sf::Color(245, 245, 220);
const sf::Color BorderColor = sf::Color(192, 192, 192);
const sf::Color Gray = sf::Color(50, 50, 50);
const int ButtonWidth = 80;
const int ButtonHeight = 40;
const int CheckBoxSize = 20;

void clearDesk(std::vector<std::vector<uint8_t>>& matrix);

void saveMatrixAsIamge(std::vector<std::vector<uint8_t>>& matrix);

void compareImage(std::vector<std::vector<uint8_t>>& matrix);

void learnImage(std::vector<std::vector<uint8_t>>& matrix);

void runPixelArtEditor(int fieldSize);

void make_gui();

#endif