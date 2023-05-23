#ifndef PIXELART_HPP
#define PIXELART_HPP

#include <SFML/Graphics.hpp>
#include <vector>

// Function to draw pixel art
void drawPixelArt(const std::vector<std::vector<uint8_t>>& matrix = {}, int pixelSize = 1)
{
    int height = matrix.size();
    int width = (height > 0) ? matrix[0].size() : 0;

    const int windowWidth = width * pixelSize;   // Window width in pixels
    const int windowHeight = height * pixelSize; // Window height in pixels

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Pixel Art"); // Create a window
    window.setFramerateLimit(60); // Limit the frame rate to 60 FPS

    sf::Image image;
    int imageHeight = matrix.size();
    int imageWidth = (imageHeight > 0) ? matrix[0].size() : 0;
    image.create(imageWidth, imageHeight);

    for (int y = 0; y < imageHeight; ++y)
    {
        for (int x = 0; x < imageWidth; ++x)
        {
            uint8_t color = (matrix.empty() || matrix[y][x] == 1) ? 255 : 0; // Convert 1 to white (255) and 0 to black (0)
            image.setPixel(x, y, sf::Color(color, color, color));
        }
    }

    sf::Texture texture;
    texture.loadFromImage(image); // Load the image into a texture

    sf::Sprite sprite(texture); // Create a sprite with the texture

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) // Check for window close event
                window.close();
        }

        window.clear(sf::Color::White); // Clear the window with white color
        window.draw(sprite); // Draw the sprite
        window.display(); // Update the window
    }
}

#endif  // PIXELART_HPP