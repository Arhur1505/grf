#include "SFML/Graphics.hpp"
#include "ShareTechMono-Regular.h"
#include <numeric>
#include <iostream>
#include <functional>
#include <cmath>

std::shared_ptr<sf::Font> font;
using colorGenerator = std::function<sf::Color(int, int, float)>;

sf::Color converteCmykToRgb(float C, float M, float Y) {

    return sf::Color(255 * (1 - C), 255 * (1 - M), 255 * (1 - Y));
}

class Slider : public sf::Drawable {
    sf::RectangleShape borders;
    sf::RectangleShape clickBorders;
    sf::RectangleShape slider;
    sf::VertexArray vertexes;
    std::function<void(float)> onChange;
    float latestK = 1.f;
public:
    Slider() : vertexes(sf::PrimitiveType::Quads, 4) {}
    void setBorders(const sf::Vector2f& topLeft, const sf::Vector2f& bottomRight);
    void handleMouseHold(int x, int y);
    inline void setOnChangeBehavior(std::function<void(float)> onChange) { this->onChange = onChange; }
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

void Slider::handleMouseHold(int x, int y) {
    if (!clickBorders.getGlobalBounds().contains(x, y)) return;

    float newY = y;
    if (y < borders.getPosition().y) {
        newY = borders.getPosition().y;
    }
    else if (y > borders.getPosition().y + borders.getSize().y) {
        newY = borders.getPosition().y + borders.getSize().y;
    }

    slider.setPosition(slider.getPosition().x, newY);

    float k = (newY - borders.getPosition().y) / static_cast<float>(borders.getSize().y);
    if (k != latestK) {
        latestK = k;
        onChange(k);
    }
}

void Slider::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(borders, states);
    target.draw(vertexes, states);
    target.draw(slider, states);
}

void Slider::setBorders(const sf::Vector2f& topLeft, const sf::Vector2f& bottomRight) {
    vertexes[0].position = topLeft;
    vertexes[1].position = sf::Vector2f(bottomRight.x, topLeft.y);
    vertexes[2].position = bottomRight;
    vertexes[3].position = sf::Vector2f(topLeft.x, bottomRight.y);

    vertexes[0].color = sf::Color::Black;
    vertexes[1].color = sf::Color::Black;
    vertexes[2].color = sf::Color::White;
    vertexes[3].color = sf::Color::White;

    borders.setSize(sf::Vector2f(bottomRight.x - topLeft.x, bottomRight.y - topLeft.y));
    borders.setPosition(topLeft);
    borders.setOutlineThickness(1);
    borders.setOutlineColor(sf::Color::Black);

    clickBorders.setSize(sf::Vector2f(bottomRight.x - topLeft.x, bottomRight.y - topLeft.y + 30));
    clickBorders.setPosition(topLeft.x, topLeft.y - 20);

    slider.setSize(sf::Vector2f(bottomRight.x - topLeft.x + 10, 12));
    slider.setOrigin(0, slider.getSize().y / 2);
    slider.setPosition(borders.getPosition().x - 5, borders.getPosition().y + borders.getSize().y);
    slider.setFillColor(sf::Color::Blue);

}

class Hexagon : public sf::Drawable
{

protected:
    sf::Texture textureTop;
    sf::Texture textureLeft;
    sf::Texture textureRight;
    sf::Sprite spriteTop;
    sf::Sprite spriteLeft;
    sf::Sprite spriteRight;
    float k = 1.f;

    sf::Image image;
    sf::Vector2f left_top, right_bottom, center;
    sf::Vector2f p[6]; // Kolejno   punkt w opisana w pliku PDF do laboratorium.
public:
    void Set_Borders(sf::Vector2f _left_top, sf::Vector2f _right_bottom);
    void Draw_Border(sf::RenderTarget& target, sf::RenderStates states, sf::String name) const;
    void drawSprites(sf::RenderTarget& target, sf::RenderStates states) const;
    inline void setK(float k) { this->k = k; }

    Hexagon() {
        image.create(256, 256);
    }
    void calcTextures(colorGenerator topGenerator, colorGenerator leftGenerator, colorGenerator rightGenerator);
    virtual void calcTextures() = 0;
    virtual std::string getName() const = 0;
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

void Hexagon::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    Draw_Border(target, states, getName());
    drawSprites(target, states);
}

void Hexagon::calcTextures(colorGenerator topGenerator, colorGenerator leftGenerator, colorGenerator rightGenerator)
{
    {
        for (int i = 0; i < 256; i++) {
            for (int j = 0; j < 256; j++) {
                image.setPixel(255 - i, 255 - j, topGenerator(i, j, k));
            }
        }
        textureTop.loadFromImage(image);
        for (int i = 0; i < 256; i++) {
            for (int j = 0; j < 256; j++) {
                image.setPixel(255 - i, 255 - j, leftGenerator(i, j, k));
            }
        }
        textureLeft.loadFromImage(image);
        for (int i = 0; i < 256; i++) {
            for (int j = 0; j < 256; j++) {
                image.setPixel(255 - i, 255 - j, rightGenerator(i, j, k));
            }
        }
        textureRight.loadFromImage(image);

        spriteTop.setTexture(textureTop);
        spriteLeft.setTexture(textureLeft);
        spriteRight.setTexture(textureRight);
    }
}

void Hexagon::drawSprites(sf::RenderTarget& target, sf::RenderStates states) const {
    sf::RectangleShape shape;
    int height = center.y - p[0].y;

    sf::Transform transRed;
    transRed.translate(center).rotate(180).scale(height * sqrt(3.0) / (256.0 * sqrt(2.0)), height / (256.0 * sqrt(2.0))).rotate(45);
    target.draw(spriteTop, transRed);

    sf::Transform transGreen;
    transGreen.translate(center).rotate(60).scale(height * sqrt(3.0) / (256.0 * sqrt(2.0)), height / (256.0 * sqrt(2.0))).rotate(45);
    target.draw(spriteLeft, transGreen);

    sf::Transform transBlue;
    transBlue.translate(center).rotate(-60).scale(height * sqrt(3.0) / (256.0 * sqrt(2.0)), height / (256.0 * sqrt(2.0))).rotate(45);
    target.draw(spriteRight, transBlue);
}

void Hexagon::Set_Borders(sf::Vector2f _left_top, sf::Vector2f _right_bottom)
{
    left_top = _left_top;
    right_bottom = _right_bottom;
    float a = ((right_bottom.y - left_top.y) / 2.0f + 0.5f) - 20.0f;
    center = left_top + sf::Vector2f((right_bottom.x - left_top.x) / 2.0f + 0.5f, (right_bottom.y - left_top.y) / 2.0f + 0.5f + 10);

    p[0] = center - sf::Vector2f(0.0f, a);
    p[1] = center - sf::Vector2f(0.5f * sqrt(3.0f) * a, 0.5f * a);
    p[2] = center - sf::Vector2f(0.5f * sqrt(3.0f) * a, -0.5f * a);
    p[3] = center + sf::Vector2f(0.0f, a);
    p[4] = center + sf::Vector2f(0.5f * sqrt(3.0f) * a, 0.5f * a);
    p[5] = center + sf::Vector2f(0.5f * sqrt(3.0f) * a, -0.5f * a);
}

void Hexagon::Draw_Border(sf::RenderTarget& target, sf::RenderStates states, sf::String name) const
{
    int textSize = 15;
    sf::Text textTop(name[0], *font, textSize);
    textTop.setPosition(p[0]);
    textTop.setFillColor(sf::Color::Black);
    textTop.setOrigin(textSize / 2, textSize * 1.1);

    sf::Text textLeft(name[1], *font, textSize);
    textLeft.setPosition(p[2]);
    textLeft.setFillColor(sf::Color::Black);
    textLeft.setOrigin(textSize / 2, 0);

    sf::Text textRight(name[2], *font, textSize);
    textRight.setPosition(p[4]);
    textRight.setFillColor(sf::Color::Black);

    sf::RectangleShape borders(sf::Vector2f(right_bottom.x - left_top.x, right_bottom.y - left_top.y));
    borders.setPosition(left_top);
    borders.setOutlineThickness(1);
    borders.setOutlineColor(sf::Color::Black);

    sf::ConvexShape hexagon(6);
    for (int i = 0; i < 6; i++) {
        hexagon.setPoint(i, p[i]);
    }
    hexagon.setOutlineThickness(1);
    hexagon.setOutlineColor(sf::Color::Black);
    target.draw(borders, states);
    target.draw(hexagon, states);
    target.draw(textTop, states);
    target.draw(textLeft, states);
    target.draw(textRight, states);
}

class Hexagon_RGB : public Hexagon
{
private:
    colorGenerator redGenerator = [](int i, int j, float k) {return sf::Color(k * 255, k * j, k * i); };
    colorGenerator greenGenerator = [](int i, int j, float k) {return sf::Color(k * i, k * 255, k * j); };
    colorGenerator blueGenerator = [](int i, int j, float k) {return sf::Color(k * j, k * i, k * 255); };

public:
    Hexagon_RGB() {
        calcTextures();
    }
    virtual void calcTextures() override {
        Hexagon::calcTextures(redGenerator, greenGenerator, blueGenerator);
    }
    virtual std::string getName() const override { return "RGB"; }
};

class Hexagon_CMY : public Hexagon
{
private:
    colorGenerator cyanGenerator = [](int i, int j, float k) {return converteCmykToRgb(k, k * j / 255.f, k * i / 255.f); };
    colorGenerator magentaGenerator = [](int i, int j, float k) {return converteCmykToRgb(k * i / 255.f, k, k * j / 255.f); };
    colorGenerator yellowGenerator = [](int i, int j, float k) {return converteCmykToRgb(k * j / 255.f, k * i / 255.f, k); };
public:
    Hexagon_CMY() {
        calcTextures();
    }
    virtual void calcTextures() {
        Hexagon::calcTextures(cyanGenerator, magentaGenerator, yellowGenerator);
    }
    virtual std::string getName() const override { return "CMY"; }
};

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "GFK Lab 02", sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize);

    Hexagon_RGB h_RGB;
    Hexagon_CMY h_CMY;
    Slider slider;

    sf::Clock frame_clock, around_half_secound_clock;
    sf::Int64 mean_frames_time = 0;
    std::vector<sf::Int64> frame_times;
    sf::Text text;

    font = std::make_shared<sf::Font>();
    font->loadFromMemory(&(ShareTechMono_Regular_ttf[0]), ShareTechMono_Regular_ttf.size());
    text.setFont(*font);
    text.setCharacterSize(21);
    text.setFillColor(sf::Color::Black);
    text.setPosition(10, 10);

    slider.setOnChangeBehavior([&](float k) -> void {
        h_RGB.setK(k);
        h_CMY.setK(k);

        h_RGB.calcTextures();
        h_CMY.calcTextures();
        });


    around_half_secound_clock.restart();

    auto handleResize = [&](int widthInt, int heightInt) -> void {
        float width = static_cast<float>(widthInt);
        float height = static_cast<float>(heightInt);
        float sliderWidth = width * 0.2;
        float minDimention = std::min(width - sliderWidth, height);
        window.setView(sf::View(sf::FloatRect(0, 0, width, height)));
        h_RGB.Set_Borders(sf::Vector2f(0, 0), sf::Vector2f(minDimention * 0.5, minDimention * 0.5));
        h_CMY.Set_Borders(sf::Vector2f(minDimention * 0.5, 0), sf::Vector2f(minDimention, minDimention * 0.5));
        text.setPosition(sf::Vector2f(width * 0.85, height * 0.85));
        slider.setBorders(sf::Vector2f(width * 0.85, height * 0.05), sf::Vector2f(width * 0.95, height * 0.8));
        };

    handleResize(window.getSize().x, window.getSize().y);
    bool mouseHold = false;
    while (window.isOpen())
    {
        sf::Event event;
        window.clear(sf::Color::White);

        frame_clock.restart(); // Start pomiaru czasu.
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) window.close();
            else if (event.type == sf::Event::Resized)
            {
                handleResize(event.size.width, event.size.height);
            }
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            auto mousePosition = sf::Mouse::getPosition(window);
            slider.handleMouseHold(mousePosition.x, mousePosition.y);
        }

        window.draw(h_RGB);
        window.draw(h_CMY);
        //window.draw(h_HSL);
        //window.draw(h_HSB);
        window.draw(slider);
        text.setString(std::to_string(mean_frames_time) + "us");
        window.draw(text);

        frame_times.push_back(frame_clock.getElapsedTime().asMicroseconds());
        if (around_half_secound_clock.getElapsedTime().asSeconds() >= 0.5f && frame_times.size() >= 1)
        {
            mean_frames_time = (sf::Int64)((float)std::reduce(frame_times.begin(), frame_times.end()) / (float)frame_times.size());
            frame_times.clear();
            around_half_secound_clock.restart();
        }

        frame_clock.restart();
        window.display();
    }

    font.reset();

    return 0;
}
