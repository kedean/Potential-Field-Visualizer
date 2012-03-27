#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <vector>

struct cell{
	int x, y;
	int attraction;
	bool visited;
	sf::Sprite cell;
};

typedef struct cell Cell;

#define alpha 50.f
#define beta 10.f

/*void oldShiftDownAndNeighbors(Cell** grid, int x, int y, int stepSize, int maxDepth){
//	if(grid[x][y].visited == true){
//		return;
//	}
	
	if(x >= 100 || y >= 100 || x < 0 || y < 0 || grid[x][y].visited)
		return;
	
	bool endAfter = (grid[x][y].attraction == 0);
	printf("attr = %d\n", grid[x][y].attraction);
	
	grid[x][y].attraction += stepSize;
	if(grid[x][y].attraction < -127)
		grid[x][y].attraction = -127;
	else if(grid[x][y].attraction > 127)
		grid[x][y].attraction = 127;
	
	grid[x][y].visited = true;
	
	if(endAfter == false && maxDepth > 0){ //once the amount is zero, done radiating
		maxDepth -= 1;
		
		shiftDownAndNeighbors(grid, x+1, y, stepSize, maxDepth);
		shiftDownAndNeighbors(grid, x, y+1, stepSize, maxDepth);
		shiftDownAndNeighbors(grid, x-1, y, stepSize, maxDepth);
		shiftDownAndNeighbors(grid, x, y-1, stepSize, maxDepth);
	}
}*/

void shiftDownAndNeighbors(Cell** grid, int x, int y, int stepSize, int maxDepth){
	std::vector<Cell*> workingset;
	std::vector<Cell*> siblings;
	
	workingset.push_back(&grid[x][y]); //initial position
	
	do{
		for(std::vector<Cell*>::iterator it = workingset.begin(); it < workingset.end(); it++){
			if((*it)->visited == false){
				(*it)->attraction += stepSize;
				if((*it)->attraction > 10)
					(*it)->attraction = 10;
				else if((*it)->attraction < -10)
					(*it)->attraction = -10;
				(*it)->visited = true;
				if((*it)->x < 9)
					siblings.push_back(&grid[(*it)->x + 1][(*it)->y]);
				if((*it)->y < 9)
					siblings.push_back(&grid[(*it)->x][(*it)->y + 1]);
				if((*it)->x > 0)
					siblings.push_back(&grid[(*it)->x - 1][(*it)->y]);
				if((*it)->y > 0)
					siblings.push_back(&grid[(*it)->x][(*it)->y - 1]);
			}
		}
		
		workingset.swap(siblings);
		maxDepth -= 1;
	} while(maxDepth >= 0);
}

void clearVisitation(Cell** grid, int dim){
	for(int x = 0; x < dim; x++){
		for(int y = 0; y < dim; y++){
			grid[x][y].visited = false;
		}
	}
}

void getAttractors(Cell** source, int dim, std::vector<Cell*>& destination){
	for(int x = 0; x < dim; x++){
		for(int y = 0; y < dim; y++){
			if(source[x][y].attraction > 0)
				destination.push_back(&source[x][y]);
		}
	}
}
void getRepulsors(Cell** source, int dim, std::vector<Cell*>& destination){
	for(int x = 0; x < dim; x++){
		for(int y = 0; y < dim; y++){
			if(source[x][y].attraction < 0)
				destination.push_back(&source[x][y]);
		}
	}
}

void drawGrid(sf::RenderWindow& App, Cell** pfGrid, int pfDim){
	
	std::vector<Cell*> attractors;
	std::vector<Cell*> repulsors;
	std::vector<Cell*>::iterator attrIt;
	
	getAttractors(pfGrid, pfDim, attractors);
	getRepulsors(pfGrid, pfDim, repulsors);
	
	for(int x = 0; x < pfDim; x++){
		for(int y = 0; y < pfDim; y++){
			if(pfGrid[x][y].attraction != 0){
				pfGrid[x][y].cell.SetScale(0.001, 1.f);
				pfGrid[x][y].cell.SetRotation(0.f);
			}
			else{
				float angle = 0.f;
				float magnitude = 0.f;
				float maxMag = 0;
				
				sf::Vector2f direction(0,0);
				
				//loop through attractors and set the attraction vectors
				for(attrIt = attractors.begin(); attrIt < attractors.end(); attrIt++){
					float distance = sqrt(
										  ((*attrIt)->cell.GetPosition().x - pfGrid[x][y].cell.GetPosition().x) * ((*attrIt)->cell.GetPosition().x - pfGrid[x][y].cell.GetPosition().x) + 
										  ((*attrIt)->cell.GetPosition().y - pfGrid[x][y].cell.GetPosition().y) * ((*attrIt)->cell.GetPosition().y - pfGrid[x][y].cell.GetPosition().y)
										  );
					
					if(distance == 0)
						continue;
					
					maxMag += 500.f;
					
					float radians = atan2(((*attrIt)->cell.GetPosition().y - pfGrid[x][y].cell.GetPosition().y), ((*attrIt)->cell.GetPosition().x - pfGrid[x][y].cell.GetPosition().x));
					radians = (3.14 / 2.f) + radians;
					
					float factor = 0.01;
					float distFact = factor * distance;
					if(distance > (*attrIt)->attraction * beta)
						distFact = (*attrIt)->attraction * beta * factor;
					
					sf::Vector2f v = sf::Vector2f(distFact * cos(radians), distFact * sin(radians));
					
					direction = sf::Vector2f(direction.x + v.x, direction.y + v.y);
				}
				
				//loop through repulsors and set the repulsion vectors
				/*for(attrIt = repulsors.begin(); attrIt < repulsors.end(); attrIt++){
					float distance = sqrt(
										  ((*attrIt)->cell.GetPosition().x - pfGrid[x][y].cell.GetPosition().x) * ((*attrIt)->cell.GetPosition().x - pfGrid[x][y].cell.GetPosition().x) + 
										  ((*attrIt)->cell.GetPosition().y - pfGrid[x][y].cell.GetPosition().y) * ((*attrIt)->cell.GetPosition().y - pfGrid[x][y].cell.GetPosition().y)
										  );
					
					float attractionFactor = alpha * abs((*attrIt)->attraction);
					float scaleX = (attractionFactor - distance) / attractionFactor;
					if(scaleX <= 0 || distance == 0.f)
						magnitude += 0;
					else{
						magnitude += scaleX;
						
						float radians = atan2(((*attrIt)->cell.GetPosition().y - pfGrid[x][y].cell.GetPosition().y), ((*attrIt)->cell.GetPosition().x - pfGrid[x][y].cell.GetPosition().x));
						float degrees = radians * (180.f / 3.14f);
						
						
						degrees = 90 - degrees;
						
						//if(pfGrid[x][y].cell.GetPosition().x > (*attrIt)->cell.GetPosition().x)
						//	degrees -= 180;
						
						angle += degrees;
					}
					
				}
				*/
				
				angle = atan2(direction.x, direction.y) * 180.f/3.14f;
				magnitude = sqrt(direction.x*direction.x + direction.y*direction.y);
				if(magnitude != 0.f){
					if(magnitude < 0)
						magnitude = 0;
				}
				else{
					magnitude = 0.f;
				}
				
				pfGrid[x][y].cell.SetRotation(angle);
				
				pfGrid[x][y].cell.SetScale(magnitude + 0.0001, 1.f);
			}
			App.Draw(pfGrid[x][y].cell);
		}
	}
}

void step(Cell** grid, int dim, sf::Sprite& guy){
	sf::Vector2f pos = guy.GetPosition();
	
	std::vector<Cell*> attractors;
	std::vector<Cell*> repulsors;
	std::vector<Cell*>::iterator attrIt;
	
	getAttractors(grid, dim, attractors);
	getRepulsors(grid, dim, repulsors);
	
	sf::Vector2f direction(0,0);
	
	float angle = 0.f;
	float magnitude = 0.f;
	float maxMag = 0;
	
	//loop through attractors and set the attraction vectors
	for(attrIt = attractors.begin(); attrIt < attractors.end(); attrIt++){
		float distance = sqrt(
							  ((*attrIt)->cell.GetPosition().x - pos.x) * ((*attrIt)->cell.GetPosition().x - pos.x) + 
							  ((*attrIt)->cell.GetPosition().y - pos.y) * ((*attrIt)->cell.GetPosition().y - pos.y)
							  );
		
		if(distance == 0)
			continue;
		
		maxMag += 500.f;
		
		float radians = atan2(((*attrIt)->cell.GetPosition().y - pos.y), ((*attrIt)->cell.GetPosition().x - pos.x));
		radians = (3.14 / 2.f) + radians;
		
		float factor = 0.01;
		float distFact = factor * distance;
		if(distance > (*attrIt)->attraction * beta)
			distFact = (*attrIt)->attraction * beta * factor;
		
		sf::Vector2f v = sf::Vector2f(distFact * cos(radians), distFact * sin(radians));
		
		direction = sf::Vector2f(direction.x + v.x, direction.y + v.y);
	}
	
	
	angle = atan2(direction.x, direction.y);
	magnitude = sqrt(direction.x*direction.x + direction.y*direction.y);
	if(magnitude != 0.f){
		if(magnitude < 0)
			magnitude = 0;
	}
	else{
		magnitude = 0.f;
	}
	
	float deltaX = alpha * magnitude * cos(angle);
	float deltaY = -alpha * magnitude * sin(angle);
	
	printf("x = %f, y = %f\n", deltaX, deltaY);
	
	guy.SetPosition(pos.x + deltaX, pos.y + deltaY);
}

int main(int argc, char* argv[])
{
	sf::Image arrowImg;
	arrowImg.LoadFromFile("arrow.png");
	arrowImg.SetSmooth(false);
	
	const int pfDim = 20;
	
	float cellSize = 32;
	long shiftAmount = 1;
	
	Cell** pfGrid = new Cell*[pfDim];
	
	for(int x = 0; x < pfDim; x++){
		pfGrid[x] = new Cell[pfDim];
		for(int y = 0; y < pfDim; y++){
			float xPos = x*cellSize + 16;
			float yPos = y*cellSize + 16;
			
			pfGrid[x][y].x = x;
			pfGrid[x][y].y = y;
			pfGrid[x][y].attraction = 0;
			pfGrid[x][y].visited = false;
			pfGrid[x][y].cell = sf::Sprite(arrowImg, sf::Vector2f(xPos, yPos), sf::Vector2f(0, 1), 0.f);
			pfGrid[x][y].cell.SetCenter(16,16);
		}
	}
	
	sf::RenderWindow App(sf::VideoMode(cellSize*pfDim, cellSize*pfDim, 32), "Potential Field Visualizer");
	
	App.SetFramerateLimit(15);
	
	drawGrid(App, pfGrid, pfDim);
	
	sf::Image guyImg;
	guyImg.LoadFromFile("guy.png");
	
	sf::Sprite guy(guyImg, sf::Vector2f(App.GetWidth() / 2.f, App.GetHeight() - 50), sf::Vector2f(1,1), 0.f);
	
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
					step(pfGrid, pfDim, guy);
				}
			}
        }
		
		
		App.SetActive();
		App.Clear(sf::Color(255,255,255,255));
		
		drawGrid(App, pfGrid, pfDim);
		
		App.Draw(guy);
		
		App.Display();
    }
	
	delete[] pfGrid;
	
    return EXIT_SUCCESS;
}
