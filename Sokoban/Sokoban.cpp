//Uwaga! Co najmniej C++17!!!
//Project-> ... Properties->Configuration Properties->General->C++ Language Standard = ISO C++ 17 Standard (/std:c++17)

#include "SFML/Graphics.hpp"
#include <fstream>
#include <iostream>

enum class Field { VOID, FLOOR, WALL, BOX, PARK, PLAYER };

class Sokoban : public sf::Drawable
{
public:
	void LoadMapFromFile(std::string fileName);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	void SetDrawParameters(sf::Vector2u draw_area_size);
	void Move_Player_Left();
	void Move_Player_Right();
	void Move_Player_Up();
	void Move_Player_Down();
	bool Is_Victory() const;
	std::vector<sf::Texture> textures;

private:
	std::vector<std::vector<Field>> map;
	sf::Vector2f shift, tile_size;
	sf::Vector2i player_position;
	std::vector<sf::Vector2i> park_positions;

	void move_player(int dx, int dy);
};

void Sokoban::LoadMapFromFile(std::string fileName)
{
	std::string str;
	std::vector<std::string> vos;

	std::ifstream in(fileName.c_str());
	while (std::getline(in, str)) { vos.push_back(str); }
	in.close();

	map.clear();
	map.resize(vos.size(), std::vector<Field>(vos[0].size()));
	for (auto [row, row_end, y] = std::tuple{ vos.cbegin(), vos.cend(), 0 }; row != row_end; ++row, ++y)
		for (auto [element, end, x] = std::tuple{ row->begin(), row->end(), 0 }; element != end; ++element, ++x)
			switch (*element)
			{
			case 'X': map[y][x] = Field::WALL; break;
			case '*': map[y][x] = Field::VOID; break;
			case ' ': map[y][x] = Field::FLOOR; break;
			case 'B': map[y][x] = Field::BOX; break;
			case 'P': map[y][x] = Field::PARK; park_positions.push_back(sf::Vector2i(x, y));  break;
			case 'S': map[y][x] = Field::PLAYER; player_position = sf::Vector2i(x, y);  break;
			}
}

void Sokoban::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	for (int y = 0; y < map.size(); ++y) {
		for (int x = 0; x < map[y].size(); ++x) {
			sf::RectangleShape rectangle(tile_size); // Ustawiamy rozmiar prostokąta na rozmiar kafelka
			rectangle.setPosition(x * tile_size.x + shift.x, y * tile_size.y + shift.y); // Pozycjonujemy prostokąt

			// Ustawienie tekstury dla prostokąta zamiast koloru
			rectangle.setTexture(&textures[static_cast<int>(map[y][x])]);

			target.draw(rectangle, states); // Rysujemy prostokąt na ekranie
		}
	}
}


void Sokoban::SetDrawParameters(sf::Vector2u draw_area_size)
{
	this->tile_size = sf::Vector2f(
		std::min(std::floor((float)draw_area_size.x / (float)map[0].size()), std::floor((float)draw_area_size.y / (float)map.size())),
		std::min(std::floor((float)draw_area_size.x / (float)map[0].size()), std::floor((float)draw_area_size.y / (float)map.size()))
	);
	this->shift = sf::Vector2f(
		((float)draw_area_size.x - this->tile_size.x * map[0].size()) / 2.0f,
		((float)draw_area_size.y - this->tile_size.y * map.size()) / 2.0f
	);
}

void Sokoban::Move_Player_Left()
{
	move_player(-1, 0);
}

void Sokoban::Move_Player_Right()
{
	move_player(1, 0);
}

void Sokoban::Move_Player_Up()
{
	move_player(0, -1);
}

void Sokoban::Move_Player_Down()
{
	move_player(0, 1);
}

void Sokoban::move_player(int dx, int dy)
{
	bool allow_move = false; // Pesymistyczne załóżmy, że gracz nie może się poruszyć.
	sf::Vector2i new_pp(player_position.x + dx, player_position.y + dy); //Potencjalna nowa pozycja gracza.
	Field fts = map[new_pp.y][new_pp.x]; //Element na miejscu na które gracz zamierza przejść.
	Field ftsa = map[new_pp.y + dy][new_pp.x + dx]; //Element na miejscu ZA miejscem na które gracz zamierza przejść. :-D

	//Gracz może się poruszyć jeśli pole na którym ma stanąć to podłoga lub miejsce na skrzynki.
	if (fts == Field::FLOOR || fts == Field::PARK) allow_move = true;
	//Jeśli pole na które chce się poruszyć gracz zawiera skrzynkę to może się on poruszyć jedynie jeśli kolejne pole jest puste lub zawiera miejsce na skrzynkę  - bo wtedy może przepchnąć skrzynkę.
	if (fts == Field::BOX && (ftsa == Field::FLOOR || ftsa == Field::PARK))
	{
		allow_move = true;
		//Przepychamy skrzynkę.
		map[new_pp.y + dy][new_pp.x + dx] = Field::BOX;
		//Oczywiście pole na którym stała skrzynka staje się teraz podłogą.
		map[new_pp.y][new_pp.x] = Field::FLOOR;
	}

	if (allow_move)
	{
		//Przesuwamy gracza.
		map[player_position.y][player_position.x] = Field::FLOOR;
		player_position = new_pp;
		map[player_position.y][player_position.x] = Field::PLAYER;
	}

	//Niestety w czasie ruchu mogły „ucierpieć” miejsca na skrzynkę. ;-(
	for (auto park_position : park_positions) if (map[park_position.y][park_position.x] == Field::FLOOR) map[park_position.y][park_position.x] = Field::PARK;
}

bool Sokoban::Is_Victory() const
{
	//Tym razem dla odmiany optymistycznie zakładamy, że gracz wygrał.
	//No ale jeśli na którymkolwiek miejscu na skrzynki nie ma skrzynki to chyba założenie było zbyt optymistyczne... : -/
	for (auto park_position : park_positions) if (map[park_position.y][park_position.x] != Field::BOX) return false;
	return true;
}

int main()
{
	sf::RenderWindow window(sf::VideoMode(800, 600), "Sokoban Game", sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize);
	Sokoban sokoban;

	// Załadowanie tekstur z plików
	sf::Texture texture_void, texture_floor, texture_wall, texture_box, texture_park, texture_player;
	if (!texture_void.loadFromFile("texture_void.png")) {
		std::cout << "Błąd tekstury: texture_void.png" << std::endl;
	}
	if (!texture_floor.loadFromFile("texture_floor.png")) {
		std::cout << "Błąd tekstury: texture_floor.png" << std::endl;
	}
	if (!texture_wall.loadFromFile("texture_wall.png")) {
		std::cout << "Błąd tekstury: texture_wall.png" << std::endl;
	}
	if (!texture_box.loadFromFile("texture_box.png")) {
		std::cout << "Błąd tekstury: texture_box.png" << std::endl;
	}
	if (!texture_park.loadFromFile("texture_park.png")) {
		std::cout << "Błąd tekstury: texture_park.png" << std::endl;
	}
	if (!texture_player.loadFromFile("texture_player.png")) {
		std::cout << "Błąd tekstury: texture_player.png" << std::endl;
	}

	// Dodanie wczytanych tekstur do wektora
	sokoban.textures.push_back(texture_void);
	sokoban.textures.push_back(texture_floor);
	sokoban.textures.push_back(texture_wall);
	sokoban.textures.push_back(texture_box);
	sokoban.textures.push_back(texture_park);
	sokoban.textures.push_back(texture_player);


	sokoban.LoadMapFromFile("plansza.txt");
	sokoban.SetDrawParameters(window.getSize());

	sf::Font font;
	if (!font.loadFromFile("Oxygen-Regular.ttf")) {
		std::cout << "Could not load font!" << std::endl;
	}

	sf::Text victoryText;
	victoryText.setFont(font);
	victoryText.setCharacterSize(24);
	victoryText.setFillColor(sf::Color::Yellow);
	victoryText.setString("Brawo! Ukonczyles gre!");
	sf::FloatRect textRect = victoryText.getLocalBounds();
	victoryText.setOrigin(textRect.left + textRect.width / 2.0f,
		textRect.top + textRect.height / 2.0f);
	victoryText.setPosition(sf::Vector2f(window.getSize().x / 2.0f, window.getSize().y / 2.0f));

	bool gameWon = false;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) window.close();

			if (!gameWon) {
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
					sokoban.Move_Player_Right();
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
					sokoban.Move_Player_Down();
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
					sokoban.Move_Player_Up();
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
					sokoban.Move_Player_Left();
			}
		}

		if (!gameWon && sokoban.Is_Victory()) {
			gameWon = true;
		}

		window.clear();
		window.draw(sokoban);
		if (gameWon) {
			window.draw(victoryText);
		}
		window.display();
	}

	return 0;
}
