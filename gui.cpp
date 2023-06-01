#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <math.h>
#include <utility>
#include <filesystem>
#include "gui.hpp"
#include "image.hpp"
#include "hopfield.hpp"
#include "errors.cpp"
#define Matrix_D std::vector<std::vector<double>>

sf::Font font;
HopfieldNetwork network;

Matrix_D conv_array(const std::vector<double> &v, int w, int h)
{
    // array (1 dim) -> matrix (2 dims)

    if (h * w != v.size())
    {
        throw MatrixSizeException("Can't convert vector to matrix with given size");
    }

    Matrix_D m(h, std::vector<double>(w));
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            m[i][j] = v[i * w + j];
        }
    }
    return m;
}

void clearDesk(std::vector<std::vector<uint8_t>> &matrix)
{
    for (auto &row : matrix)
    {
        for (auto &pixel : row)
        {
            pixel = 255; // Set all pixels to white (255)
        }
    }
}

void setButton(sf::RectangleShape &button, const sf::Vector2f &position, const sf::Vector2f &size, const sf::Color &fillColor, const sf::Color &outlineColor, const float outlineThickness, sf::Text &buttonText, const std::string &text, const sf::Font &font, const unsigned int characterSize, const sf::Color &textColor)
{
    button.setPosition(position);
    button.setSize(size);
    button.setFillColor(fillColor);
    button.setOutlineColor(outlineColor);
    button.setOutlineThickness(outlineThickness);

    buttonText.setString(text);
    buttonText.setFont(font);
    buttonText.setCharacterSize(characterSize);
    buttonText.setFillColor(textColor);
    buttonText.setPosition(position.x + size.x / 2 - buttonText.getLocalBounds().width / 2, position.y + size.y / 2 - buttonText.getLocalBounds().height / 2);
}

std::pair<bool, std::string> inputScreen(std::string wName, std::string inputBoxText){
    sf::RenderWindow inputWindow(sf::VideoMode(InputWindowWidth, InputWindowHeight), wName);
    inputWindow.setFramerateLimit(60);

    sf::RectangleShape inputBox(sf::Vector2f(400, 50));
    inputBox.setPosition((InputWindowWidth - inputBox.getSize().x) / 2, (InputWindowHeight - inputBox.getSize().y) / 2);
    inputBox.setFillColor(sf::Color::White);
    inputBox.setOutlineThickness(2);
    inputBox.setOutlineColor(sf::Color::Black);

    sf::Text boxText;
    boxText.setFont(font);
    boxText.setString(inputBoxText);
    boxText.setCharacterSize(24);
    boxText.setFillColor(sf::Color::Black);
    boxText.setStyle(sf::Text::Bold);
    sf::FloatRect boxTextBounds = boxText.getLocalBounds();
    boxText.setPosition(inputBox.getPosition().x + (inputBox.getSize().x - boxTextBounds.width) / 2,
                        inputBox.getPosition().y - boxTextBounds.height - 10);

    sf::Text inputText;
    inputText.setFont(font);
    inputText.setCharacterSize(24);
    inputText.setFillColor(sf::Color::Black);
    inputText.setStyle(sf::Text::Bold);
    inputText.setPosition(inputBox.getPosition().x, inputBox.getPosition().y + 5);

    std::string fileName;
    bool textEntered = false;

    while (inputWindow.isOpen())
    {
        sf::Event event;
        while (inputWindow.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) // Check for window close event
                inputWindow.close();
            else if (event.type == sf::Event::TextEntered) // Check for text input event
            {
                if (event.text.unicode >= 32 && event.text.unicode <= 126) // Accept only printable ASCII characters
                {
                    fileName += static_cast<char>(event.text.unicode);
                    inputText.setString(fileName);
                }
                else if (event.text.unicode == 13 && !fileName.empty()) // Handle enter key press
                {
                    textEntered = true;
                    inputWindow.close();
                }
            }
            else if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Backspace && !fileName.empty())
                {
                    fileName.pop_back();
                    inputText.setString(fileName);
                }
            }
        }

        inputWindow.clear(BackgroundColor);

        inputWindow.draw(inputBox);
        inputWindow.draw(inputText);
        inputWindow.draw(boxText);

        inputWindow.display(); // Update the window
    }
    return std::make_pair(textEntered, fileName);
}

void reset()
{
    network.clear_weights();
}

bool loadNetwork()
{
    std::pair<bool, std::string> p = inputScreen("Load Network", "Enter file path:");
    bool textEntered = p.first;
    std::string path = p.second;
    if (textEntered)
    {
        bool loaded = false;
        std::filesystem::path filePath(path);

        if (std::filesystem::exists(filePath))
        {
            network.load_network(path);
            loaded = true;
        }
        else
        {
            std::cerr << "File does not exist." << std::endl;
        }
        return network.neuron_count > 2 && network.neuron_count <= 55 && loaded ? true : false;
    }
    return false;
}

void saveNetwork(){
    std::pair<bool, std::string> p = inputScreen("Save Network", "Enter file name:");
    bool textEntered = p.first;
    std::string fileName = p.second;
    if (textEntered)
    {
        std::filesystem::path filePath("./");
        std::filesystem::path samplesPath = filePath.parent_path() / "networks";

        if (std::filesystem::exists(samplesPath) && std::filesystem::is_directory(samplesPath)){}
        else {std::filesystem::create_directory(samplesPath);}
        network.save_network("./networks/" + fileName + ".hop");
    }
}

void saveMatrixAsIamge(std::vector<std::vector<uint8_t>> &matrix)
{
    std::pair<bool, std::string> p = inputScreen("Save Image", "Enter file name:");
    bool textEntered = p.first;
    std::string fileName = p.second;
    if (textEntered)
    {
        std::filesystem::path filePath("./");
        std::filesystem::path samplesPath = filePath.parent_path() / "samples";

        if (std::filesystem::exists(samplesPath) && std::filesystem::is_directory(samplesPath)){}
        else {std::filesystem::create_directory(samplesPath);}
        WriteImg(matrix, "./samples/" + fileName + ".bmp");
    }
}

void proceed(std::vector<std::vector<uint8_t>> &matrix)
{
    std::vector<double> res = network.compare(matrix);
    Matrix_D m = conv_array(res, matrix.size(), matrix[0].size());
    for (int i = 0; i < matrix.size(); i++)
    {
        for (int j = 0; j < matrix[0].size(); j++)
        {
            matrix[i][j] = static_cast<uint8_t>(m[i][j]);
        }
    }
}

void learnSample(std::vector<std::vector<uint8_t>> &matrix, bool check)
{
    network.load_sample(matrix, check);
    clearDesk(matrix);
}

bool drawWindowWorking = false;
void runPixelArtEditor(int fieldSize)
{
    sf::RenderWindow drawingWindow(sf::VideoMode(DrawingWindowWidth, DrawingWindowHeight), "Pixel Art Editor"); // Create a window for drawing
    drawingWindow.setFramerateLimit(60);                                                                        // Limit the frame rate to 60 FPS

    int pixelSize = (DrawingWindowHeight - 200) / fieldSize;
    int fieldX = (DrawingWindowWidth - fieldSize * (pixelSize + BorderWidth)) / 2;
    int fieldY = (DrawingWindowHeight - fieldSize * (pixelSize + BorderWidth)) / 2;

    std::vector<std::vector<uint8_t>> matrix(fieldSize, std::vector<uint8_t>(fieldSize, 255));

    sf::RectangleShape saveButton;
    sf::Text saveButtonText;

    setButton(saveButton, sf::Vector2f((DrawingWindowWidth - ButtonWidth) / 2 - ButtonWidth / 2 - 10, DrawingWindowHeight - ButtonHeight - 10),
              sf::Vector2f(ButtonWidth, ButtonHeight), Gray, sf::Color::Black, 2, saveButtonText, "Save\nimage", font, 12, sf::Color::Black);

    sf::RectangleShape clearButton;
    sf::Text clearButtonText;

    setButton(clearButton, sf::Vector2f(saveButton.getPosition().x + ButtonWidth + 20, saveButton.getPosition().y),
            sf::Vector2f(ButtonWidth, ButtonHeight), Gray, sf::Color::Black, 2, clearButtonText, "Clear\nimage", font, 12, sf::Color::Black);

    sf::RectangleShape learnButton;
    sf::Text learnButtonText;

    setButton(learnButton, sf::Vector2f((DrawingWindowWidth - ButtonWidth) / 2 - ButtonWidth / 2 - 10, ButtonHeight - 20),
            sf::Vector2f(ButtonWidth, ButtonHeight), Gray, sf::Color::Black, 2, learnButtonText, "Learn", font, 24, sf::Color::Black);

    sf::RectangleShape proceedButton(sf::Vector2f(ButtonWidth, ButtonHeight));
    sf::Text proceedButtonText;

    setButton(proceedButton, sf::Vector2f(learnButton.getPosition().x + ButtonWidth + 20, learnButton.getPosition().y),
            sf::Vector2f(ButtonWidth, ButtonHeight), Gray, sf::Color::Black, 2, proceedButtonText, "Run", font, 24, sf::Color::Black);

    sf::RectangleShape checkBox(sf::Vector2f(CheckBoxSize, CheckBoxSize));
    checkBox.setFillColor(sf::Color::White);
    checkBox.setOutlineThickness(2);
    checkBox.setOutlineColor(sf::Color::Black);
    checkBox.setPosition(50, 100);

    sf::RectangleShape innerBox(sf::Vector2f(CheckBoxSize - 4, CheckBoxSize - 4));
    innerBox.setFillColor(sf::Color::Transparent);
    innerBox.setPosition(checkBox.getPosition().x + 2,
                         checkBox.getPosition().y + 2);

    sf::Text checkBoxText;
    checkBoxText.setFont(font);
    checkBoxText.setString("Check network\nmemory");
    checkBoxText.setCharacterSize(18);
    checkBoxText.setFillColor(sf::Color::Black);
    checkBoxText.setStyle(sf::Text::Bold);
    sf::FloatRect checkBoxButtonTextBounds = checkBoxText.getLocalBounds();
    checkBoxText.setPosition(10,
                             checkBox.getPosition().y - checkBoxButtonTextBounds.height - 5);

    sf::Text sampleCountText;
    sampleCountText.setFont(font);
    sampleCountText.setString("Loaded\nsamples:\n" + std::to_string(network.sample_count) + "/" + std::to_string(static_cast<int>(network.N_COEFF * network.neuron_count)));
    sampleCountText.setCharacterSize(18);
    sampleCountText.setFillColor(sf::Color::Black);
    sampleCountText.setStyle(sf::Text::Bold);
    sampleCountText.setPosition(10,
                                checkBox.getPosition().y + 30 + CheckBoxSize);

    sf::RectangleShape resetButton;
    sf::Text resetButtonText;

    setButton(resetButton, sf::Vector2f(DrawingWindowWidth - 95, 50),
            sf::Vector2f(ButtonWidth, ButtonHeight), Gray, sf::Color::Black, 2, resetButtonText, "Reset\nnetwork", font, 12, sf::Color::Black);

    sf::RectangleShape saveNetworkButton;
    sf::Text saveNetworkButtonText;

    setButton(saveNetworkButton, sf::Vector2f(DrawingWindowWidth - 95, resetButton.getPosition().y + ButtonHeight + 10),
            sf::Vector2f(ButtonWidth, ButtonHeight), Gray, sf::Color::Black, 2, saveNetworkButtonText, "Save\nnetwork", font, 12, sf::Color::Black);

    sf::RectangleShape backButton;
    sf::Text backButtonText;

    setButton(backButton, sf::Vector2f(20, DrawingWindowHeight - ButtonHeight - 10),
            sf::Vector2f(ButtonWidth, ButtonHeight), Gray, sf::Color::Black, 2, backButtonText, "Back", font, 24, sf::Color::Black);

    bool isChecked = false;

    while (drawingWindow.isOpen())
    {
        sf::Event event;
        while (drawingWindow.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                drawingWindow.close();
                drawWindowWorking = false;
            }
            else if (event.type == sf::Event::MouseButtonPressed) // Check for mouse button press event
            {
                if (event.mouseButton.button == sf::Mouse::Left) // Handle left mouse button press
                {
                    int mouseX = event.mouseButton.x;
                    int mouseY = event.mouseButton.y;
                    if (mouseX > 100 && mouseY > 100 && mouseX < 510 && mouseY < 510)
                    {
                        int pixelX = (mouseX - fieldX) / (pixelSize + BorderWidth);
                        int pixelY = (mouseY - fieldY) / (pixelSize + BorderWidth);

                        if (pixelX >= 0 && pixelX < fieldSize && pixelY >= 0 && pixelY < fieldSize)
                        {
                            matrix[pixelY][pixelX] = (matrix[pixelY][pixelX] == 0) ? 255 : 0; // Toggle the pixel color (black or white)
                        }
                    }
                }
            }
            else if (event.type == sf::Event::KeyPressed) // Check for key press event
            {
                if (event.key.code == sf::Keyboard::Escape) // Handle escape key press
                {
                    drawingWindow.close();
                    drawWindowWorking = false;
                }
            }
            else if (event.type == sf::Event::MouseButtonReleased) // Check for mouse button release event
            {
                if (event.mouseButton.button == sf::Mouse::Left) // Handle left mouse button release
                {
                    int mouseX = event.mouseButton.x;
                    int mouseY = event.mouseButton.y;

                    if (mouseX >= clearButton.getPosition().x && mouseX <= (clearButton.getPosition().x + ButtonWidth) &&
                        mouseY >= clearButton.getPosition().y && mouseY <= (clearButton.getPosition().y + ButtonHeight))
                    {
                        clearDesk(matrix); // Call function to clear the matrix
                    }
                    else if (mouseX >= saveButton.getPosition().x && mouseX <= (saveButton.getPosition().x + ButtonWidth) &&
                             mouseY >= saveButton.getPosition().y && mouseY <= (saveButton.getPosition().y + ButtonHeight))
                    {
                        saveMatrixAsIamge(matrix); // Call function to save the matrix
                    }
                    else if (mouseX >= learnButton.getPosition().x && mouseX <= (learnButton.getPosition().x + ButtonWidth) &&
                             mouseY >= learnButton.getPosition().y && mouseY <= (learnButton.getPosition().y + ButtonHeight))
                    {
                        learnSample(matrix, isChecked); // Call function to learn sample
                        sampleCountText.setString("Loaded\nsamples:\n" + std::to_string(network.sample_count) + "/" + std::to_string(static_cast<int>(network.N_COEFF * network.neuron_count)));
                    }
                    else if (mouseX >= proceedButton.getPosition().x && mouseX <= (proceedButton.getPosition().x + ButtonWidth) &&
                             mouseY >= proceedButton.getPosition().y && mouseY <= (proceedButton.getPosition().y + ButtonHeight))
                    {
                        proceed(matrix); // Call function to run the network
                    }
                    else if (mouseX >= resetButton.getPosition().x && mouseX <= (resetButton.getPosition().x + ButtonWidth) &&
                             mouseY >= resetButton.getPosition().y && mouseY <= (resetButton.getPosition().y + ButtonHeight))
                    {
                        reset(); // Call function to reset neural the network
                        sampleCountText.setString("Loaded\nsamples:\n" + std::to_string(network.sample_count) + "/" + std::to_string(static_cast<int>(network.N_COEFF * network.neuron_count)));
                    }
                    else if (mouseX >= saveNetworkButton.getPosition().x && mouseX <= (saveNetworkButton.getPosition().x + ButtonWidth) &&
                             mouseY >= saveNetworkButton.getPosition().y && mouseY <= (saveNetworkButton.getPosition().y + ButtonHeight))
                    {
                        saveNetwork(); // Call function to reset neural the network
                    }
                    else if (mouseX >= backButton.getPosition().x && mouseX <= (backButton.getPosition().x + ButtonWidth) &&
                             mouseY >= backButton.getPosition().y && mouseY <= (backButton.getPosition().y + ButtonHeight))
                    {
                        drawingWindow.close(); //back to first menu
                    }
                    else if (mouseX >= checkBox.getPosition().x && mouseX <= (checkBox.getPosition().x + CheckBoxSize) &&
                             mouseY >= checkBox.getPosition().y && mouseY <= (checkBox.getPosition().y + CheckBoxSize))
                    {
                        isChecked = !isChecked; //checkbox
                    }
                }
            }
        }

        drawingWindow.clear(BackgroundColor);

        // Draw the pixels and borders
        for (int y = 0; y < fieldSize; ++y)
        {
            for (int x = 0; x < fieldSize; ++x)
            {
                sf::RectangleShape pixel(sf::Vector2f(pixelSize, pixelSize));
                pixel.setPosition(fieldX + x * (pixelSize + BorderWidth), fieldY + y * (pixelSize + BorderWidth));

                if (matrix[y][x] == 0)
                {
                    pixel.setFillColor(sf::Color::Black);
                }
                else
                {
                    pixel.setFillColor(sf::Color::White);
                }

                sf::RectangleShape border(sf::Vector2f(pixelSize, pixelSize));
                border.setPosition(fieldX + x * (pixelSize + BorderWidth), fieldY + y * (pixelSize + BorderWidth));
                border.setFillColor(sf::Color::Transparent);
                border.setOutlineThickness(BorderWidth);
                border.setOutlineColor(BorderColor);

                drawingWindow.draw(pixel);
                drawingWindow.draw(border);
            }
        }

        innerBox.setFillColor(isChecked ? sf::Color::Black : sf::Color::Transparent);

        drawingWindow.draw(saveButton);
        drawingWindow.draw(clearButton);
        drawingWindow.draw(learnButton);
        drawingWindow.draw(proceedButton);
        drawingWindow.draw(resetButton);
        drawingWindow.draw(backButton);
        drawingWindow.draw(saveNetworkButton);
        drawingWindow.draw(proceedButtonText);
        drawingWindow.draw(clearButtonText);
        drawingWindow.draw(learnButtonText);
        drawingWindow.draw(saveButtonText);
        drawingWindow.draw(checkBox);
        drawingWindow.draw(innerBox);
        drawingWindow.draw(checkBoxText);
        drawingWindow.draw(sampleCountText);
        drawingWindow.draw(resetButtonText);
        drawingWindow.draw(backButtonText);
        drawingWindow.draw(saveNetworkButtonText);

        drawingWindow.display(); // Update the window
    }
    return;
}

void make_gui()
{
    if (!font.loadFromFile("arial.ttf"))
    {
        std::cerr << "Failed to load font." << std::endl;
        return;
    }

    sf::RectangleShape inputBox(sf::Vector2f(200, 50));
    inputBox.setPosition((InputWindowWidth - inputBox.getSize().x) / 2, (InputWindowHeight - inputBox.getSize().y) / 2);
    inputBox.setFillColor(sf::Color::White);
    inputBox.setOutlineThickness(2);
    inputBox.setOutlineColor(sf::Color::Black);

    sf::Text inputText;
    inputText.setFont(font);
    inputText.setString("Enter field size (2 to 50):");
    inputText.setCharacterSize(24);
    inputText.setFillColor(sf::Color::Black);
    inputText.setStyle(sf::Text::Bold);
    sf::FloatRect inputTextBounds = inputText.getLocalBounds();
    inputText.setPosition(inputBox.getPosition().x + (inputBox.getSize().x - inputTextBounds.width) / 2,
                          inputBox.getPosition().y - inputTextBounds.height - 10);

    sf::Text sizeText;
    sizeText.setFont(font);
    sizeText.setCharacterSize(24);
    sizeText.setFillColor(sf::Color::Black);
    sizeText.setStyle(sf::Text::Bold);
    sizeText.setPosition(inputBox.getPosition().x,
                         inputBox.getPosition().y + 5);

    sf::RectangleShape loadButton;
    sf::Text loadButtonText;

    setButton(loadButton, sf::Vector2f((InputWindowWidth - ButtonWidth - 40) / 2, inputBox.getPosition().y + ButtonHeight + 20),
            sf::Vector2f(sf::Vector2f(ButtonWidth + 40, ButtonHeight)), Gray, sf::Color::Black, 2, loadButtonText, "load Network", font, 16, sf::Color::Black);

    int fieldSize = 0;

    bool isWorking = true;

    do
    {
        bool loaded = false;
        network.reset_network();

        sf::RenderWindow inputWindow(sf::VideoMode(InputWindowWidth, InputWindowHeight), "Pixel Art Editor - Size Input");
        inputWindow.setFramerateLimit(60);

        while (inputWindow.isOpen())
        {
            sf::Event event;
            while (inputWindow.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                {
                    inputWindow.close();
                    isWorking = false;
                }
                else if (event.type == sf::Event::TextEntered) // Check for text input event
                {
                    if (event.text.unicode >= '0' && event.text.unicode <= '9') // Accept only numeric input
                    {
                        sizeText.setString(sizeText.getString() + static_cast<char>(event.text.unicode));
                    }
                }
                else if (event.type == sf::Event::KeyPressed) // Check for key press event
                {
                    if (event.key.code == sf::Keyboard::Escape) // Handle escape key press
                    {
                        inputWindow.close();
                        isWorking = false;
                    }
                    if (event.key.code == sf::Keyboard::Enter) // Handle enter key press
                    {
                        fieldSize = std::stoi(static_cast<std::string>(sizeText.getString())); // Convert input text to integer
                        inputWindow.close();
                    }
                    else if (event.key.code == sf::Keyboard::BackSpace && !sizeText.getString().isEmpty())
                    {
                        std::string tmp = sizeText.getString();
                        tmp.pop_back();
                        sizeText.setString(tmp);
                    }
                }
                else if (event.type == sf::Event::MouseButtonReleased) // Check for mouse button release event
                {
                    if (event.mouseButton.button == sf::Mouse::Left) // Handle left mouse button release
                    {
                        int mouseX = event.mouseButton.x;
                        int mouseY = event.mouseButton.y;

                        if (mouseX >= loadButton.getPosition().x && mouseX <= (loadButton.getPosition().x + ButtonWidth) &&
                            mouseY >= loadButton.getPosition().y && mouseY <= (loadButton.getPosition().y + ButtonHeight))
                        {
                            loaded = loadNetwork(); // Call function to load network
                            if (loaded){
                                inputWindow.close();
                                fieldSize = floor(sqrt<int>(network.neuron_count));
                            }
                        }
                    }
                }
            }

            inputWindow.clear(BackgroundColor);

            inputWindow.draw(inputBox);
            inputWindow.draw(inputText);
            inputWindow.draw(sizeText);
            inputWindow.draw(loadButton);
            inputWindow.draw(loadButtonText);

            inputWindow.display(); // Update the window
        }

        if (isWorking){
            if (fieldSize <= 1 || fieldSize > 50)
            {
                std::cerr << "Invalid field size." << std::endl;
                return;
            }

            if (!loaded)
                network.resize_network(fieldSize * fieldSize);
            

            drawWindowWorking = true;
            runPixelArtEditor(fieldSize); // Run the pixel art editor
            if (!drawWindowWorking){
                isWorking = false;
            }
        }
    } 
    while (isWorking);
}
