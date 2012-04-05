#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <vector>

struct cell{
	int x, y;
	float attraction;
	bool visited;
	sf::Sprite cell;
	sf::Vector2f direction;
};

typedef struct cell Cell;

#define alpha 25.f
#define beta 100.f
#define cellSize 16
#define pfDim 40


void getAttractors(Cell** source, std::vector<Cell*>& destination){
	for(int x = 0; x < pfDim; x++){
		for(int y = 0; y < pfDim; y++){
			if(source[x][y].attraction > 0)
				destination.push_back(&source[x][y]);
		}
	}
}
void getRepulsors(Cell** source, std::vector<Cell*>& destination){
	for(int x = 0; x < pfDim; x++){
		for(int y = 0; y < pfDim; y++){
			if(source[x][y].attraction < 0)
				destination.push_back(&source[x][y]);
		}
	}
}

float initAngles[20][20];

sf::Vector2f getDetectors(Cell** pfGrid, sf::Vector2f position, std::vector<Cell*> attractorsAndRepulsors){
	std::vector<Cell*>::iterator attrIt;
	
	int xPos = position.x / cellSize;
	int yPos = position.y / cellSize;
	
	sf::Vector2f direction(1.f * cos(initAngles[xPos][yPos]), 1.f * sin(initAngles[xPos][yPos]));
	//loop through attractors and repulsors and set the attraction vectors
	for(attrIt = attractorsAndRepulsors.begin(); attrIt < attractorsAndRepulsors.end(); attrIt++){
		float distance = sqrt(
							  ((*attrIt)->cell.GetPosition().x - position.x) * ((*attrIt)->cell.GetPosition().x - position.x) + 
							  ((*attrIt)->cell.GetPosition().y - position.y) * ((*attrIt)->cell.GetPosition().y - position.y)
							  );
		
		if(distance == 0)
			continue;
		
		float radians = atan2(((*attrIt)->cell.GetPosition().y - position.y), ((*attrIt)->cell.GetPosition().x - position.x));
		radians = (3.14 / 2.f) + radians;
		
		float maxMag = 1.0f;
		
		float factor = 0.01;
		float distFact = factor * distance;
		
		if((*attrIt)->attraction > 0){
			
			if(distance > (*attrIt)->attraction * beta){
				distFact = 1.f;
			}
			else{
				distFact = distFact / ((*attrIt)->attraction * beta * factor);
			}
		}
		else{
			if(distance > abs((*attrIt)->attraction * beta)){
				distFact = 0.f;
			}
			else{
				distFact = (abs((*attrIt)->attraction * beta * factor) - distFact) / abs((*attrIt)->attraction * beta * factor);
			}
			radians += 3.14;
		}
		
		sf::Vector2f v = sf::Vector2f(distFact * cos(radians), distFact * sin(radians));
		
		direction = sf::Vector2f(direction.x + v.x, direction.y + v.y);
	}
	
	return direction;
}

void drawGrid(sf::RenderWindow& App, Cell** pfGrid){
	
	std::vector<Cell*> attractorsAndRepulsors;
	getAttractors(pfGrid, attractorsAndRepulsors);
	getRepulsors(pfGrid, attractorsAndRepulsors);
	
	for(int x = 0; x < pfDim; x++){
		for(int y = 0; y < pfDim; y++){
			if(pfGrid[x][y].attraction != 0){
				pfGrid[x][y].cell.SetScale(0.1, 1.f);
				pfGrid[x][y].cell.SetRotation(0.f);
			}
			else{
				pfGrid[x][y].direction = getDetectors(pfGrid, pfGrid[x][y].cell.GetPosition(), attractorsAndRepulsors);
				
				float angle = 0.f;
				float magnitude = 0.f;
				float maxMag = 0;
				
				angle = atan2(pfGrid[x][y].direction.x, pfGrid[x][y].direction.y) * 180.f/3.14f;
				magnitude = sqrt(pfGrid[x][y].direction.x*pfGrid[x][y].direction.x + pfGrid[x][y].direction.y*pfGrid[x][y].direction.y);
				pfGrid[x][y].cell.SetRotation(angle);
				
				pfGrid[x][y].cell.SetScale(magnitude + 0.0001, 1.f);
			}
			App.Draw(pfGrid[x][y].cell);
		}
	}
}

void step(Cell** grid, sf::Sprite& guy){
	sf::Vector2f pos = guy.GetPosition();
	int xPos = pos.x / cellSize;
	int yPos = pos.y / cellSize;
	
	grid[xPos][yPos].attraction = 0.f;
	
	std::vector<Cell*> attractorsAndRepulsors;
	std::vector<Cell*>::iterator attrIt;
	
	getAttractors(grid, attractorsAndRepulsors);
	getRepulsors(grid, attractorsAndRepulsors);
	
	sf::Vector2f direction = getDetectors(grid, pos, attractorsAndRepulsors);
	
	float angle = atan2(direction.x, direction.y);
	float magnitude = sqrt(direction.x*direction.x + direction.y*direction.y);
	if(magnitude < 0){
		magnitude = 0;
	}
	
	float deltaX = alpha * cos(angle);
	float deltaY = -alpha * sin(angle);
	
//	printf("x = %f, y = %f\n", deltaX, deltaY);
	
//	angle = angle * 180.f / 3.15f;
//	angle = (int)angle % 360;
	
	guy.SetPosition(pos.x + deltaX, pos.y + deltaY);
	
//	int gX = pos.x / 32;
//	int gY = pos.x / 32;
	
//	if(angle < 90)
//		guy.SetPosition(grid[
}

void resetGrid(sf::RenderWindow& App, Cell** pfGrid, sf::Image& withImg, sf::Sprite& sprite){
	sprite.SetPosition(sf::Vector2f(App.GetWidth() / 2.f, App.GetHeight() - 100));
	for(int x = 0; x < pfDim; x++){
		pfGrid[x] = new Cell[pfDim];
		for(int y = 0; y < pfDim; y++){
			initAngles[x][y] = (x > 4 ? -45 : 45);
			
			
			float xPos = x*cellSize + cellSize/2;
			float yPos = y*cellSize + cellSize/2;
			
			pfGrid[x][y].x = x;
			pfGrid[x][y].y = y;
			pfGrid[x][y].attraction = 0;
			pfGrid[x][y].visited = false;
			pfGrid[x][y].cell = sf::Sprite(withImg, sf::Vector2f(xPos, yPos), sf::Vector2f(0, 1), 0.f);
			pfGrid[x][y].cell.SetCenter(cellSize/2, cellSize/2);
		}
	}
}

int main(int argc, char* argv[])
{
	
	long shiftAmount = 1;
	
	sf::RenderWindow App(sf::VideoMode(cellSize*pfDim, cellSize*pfDim, 32), "Potential Field Visualizer");
	
	App.SetFramerateLimit(15);
	
	sf::Image arrowImg;
	arrowImg.LoadFromFile("arrow.png");
	arrowImg.SetSmooth(false);
	
	Cell** pfGrid = new Cell*[pfDim];
	
	sf::Image guyImg;
	guyImg.LoadFromFile("guy.png");
	
	sf::Sprite guy(guyImg, sf::Vector2f(App.GetWidth() / 2.f, App.GetHeight() - 200), sf::Vector2f(1,1), 0.f);
	
	resetGrid(App, pfGrid, arrowImg, guy);
	
	
	// Start game loop
    while (App.IsOpened())
    {
        // Process events
        sf::Event Event;
        while (App.GetEvent(Event))
        {
            // Close window : exit
            if (Event.Type == sf::Event::Closed)
                App.Close();
			
            // Escape key : exit
            if ((Event.Type == sf::Event::KeyPressed) && (Event.Key.Code == sf::Key::Escape))
                App.Close();
			
			if(Event.Type == sf::Event::MouseButtonPressed){
				int cellX = (Event.MouseButton.X) / cellSize;
				int cellY = (Event.MouseButton.Y) / cellSize;
				
				pfGrid[cellX][cellY].attraction += (Event.MouseButton.Button == 0) ? 1 : -1;
			}
			
            // Resize event : adjust viewport
            if (Event.Type == sf::Event::Resized)
                glViewport(0, 0, Event.Size.Width, Event.Size.Height);
			
			if(Event.Type == sf::Event::KeyPressed){
				if(Event.Key.Code == sf::Key::Space){
					step(pfGrid, guy);
				}
				else if(Event.Key.Code == 'r'){
					resetGrid(App, pfGrid, arrowImg, guy);
				}
			}
        }
		
		
		App.SetActive();
		App.Clear(sf::Color(255,255,255,255));
		
		drawGrid(App, pfGrid);
		
		App.Draw(guy);
		
		App.Display();
    }
	
	delete[] pfGrid;
	
    return EXIT_SUCCESS;
}
