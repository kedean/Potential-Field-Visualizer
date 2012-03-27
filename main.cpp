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
	
	App.SetActive();
	
	//glClear(GL_COLOR_BUFFER_BIT);
	App.Clear(sf::Color(255,255,255,255));
	
	for(int x = 0; x < pfDim; x++){
		for(int y = 0; y < pfDim; y++){
			
			if(pfGrid[x][y].attraction != 0){
				pfGrid[x][y].cell.SetScale(0.001, 1.f);
				pfGrid[x][y].cell.SetRotation(0.f);
			}
			else{
//				float val = pfGrid[x][y].attraction / 10.f;
//				pfGrid[x][y].cell.SetScale(val, 1.f);
				pfGrid[x][y].cell.SetScale(0.5, 1.f);
				
				float angle = 90.f;
				float magnitude = 0.f;
				
				float alpha = 50.f; //scale everything, this is the cutoff factor
				float maxAttractionFactor = 10.f;
				
				for(attrIt = attractors.begin(); attrIt < attractors.end(); attrIt++){
					float radians = atan(((*attrIt)->cell.GetPosition().y - pfGrid[x][y].cell.GetPosition().y) / ((*attrIt)->cell.GetPosition().x - pfGrid[x][y].cell.GetPosition().x));
					float degrees = radians * (180.f / 3.14f);
					
					degrees = 90 - degrees;
					
					if(pfGrid[x][y].cell.GetPosition().x < (*attrIt)->cell.GetPosition().x)
						degrees -= 180;
					else if(pfGrid[x][y].cell.GetPosition().x == (*attrIt)->cell.GetPosition().x)
						degrees -= 180;
					
					angle += degrees;
					
					
					float distance = sqrt(
										  ((*attrIt)->cell.GetPosition().x - pfGrid[x][y].cell.GetPosition().x) * ((*attrIt)->cell.GetPosition().x - pfGrid[x][y].cell.GetPosition().x) + 
										  ((*attrIt)->cell.GetPosition().y - pfGrid[x][y].cell.GetPosition().y) * ((*attrIt)->cell.GetPosition().y - pfGrid[x][y].cell.GetPosition().y)
										  );
				//	magnitude += distance;
				//	if(magnitude == 0)
				//		magnitude = distance; //if this is the first attractor to be tested, then don't average anything
				//	else
				//		magnitude = (magnitude + distance) / 2.f; //average the magnitudes
					
					float scaleX = ((alpha * (*attrIt)->attraction) - distance) / (alpha * (*attrIt)->attraction);
					if(scaleX <= 0 || distance == 0.f)
						magnitude += 0;
					else
						magnitude += scaleX;
				}
				
				for(attrIt = repulsors.begin(); attrIt < repulsors.end(); attrIt++){
					float radians = atan(((*attrIt)->cell.GetPosition().y - pfGrid[x][y].cell.GetPosition().y) / ((*attrIt)->cell.GetPosition().x - pfGrid[x][y].cell.GetPosition().x));
					float degrees = radians * (180.f / 3.14f);
					
					
					degrees = 90 - degrees;
					
					if(pfGrid[x][y].cell.GetPosition().x > (*attrIt)->cell.GetPosition().x)
						degrees -= 180;
					
					angle += degrees;
					
					float distance = sqrt(
										  ((*attrIt)->cell.GetPosition().x - pfGrid[x][y].cell.GetPosition().x) * ((*attrIt)->cell.GetPosition().x - pfGrid[x][y].cell.GetPosition().x) + 
										  ((*attrIt)->cell.GetPosition().y - pfGrid[x][y].cell.GetPosition().y) * ((*attrIt)->cell.GetPosition().y - pfGrid[x][y].cell.GetPosition().y)
										  );
					
					float attractionFactor = alpha * abs((*attrIt)->attraction);
					float scaleX = (attractionFactor - distance) / attractionFactor;
					if(scaleX <= 0 || distance == 0.f)
						magnitude += 0;
					else
						magnitude += scaleX;
					
				}
				
				pfGrid[x][y].cell.SetRotation(angle);
				
				pfGrid[x][y].cell.SetScale(magnitude + 0.0001, 1.f);
			}
//			sf::Color cellColor =  sf::Color(val, val, val, 255);
//			pfGrid[x][y].cell.SetPointColor(0, cellColor);
//			pfGrid[x][y].cell.SetPointColor(1, cellColor);
//			pfGrid[x][y].cell.SetPointColor(2, cellColor);
//			pfGrid[x][y].cell.SetPointColor(3, cellColor);
			App.Draw(pfGrid[x][y].cell);
		}
	}
	
	
	App.Display();
	
}

int main(int argc, char* argv[])
{
	sf::Image arrowImg;
	arrowImg.LoadFromFile("arrow.jpg");
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
//			pfGrid[x][y].cell.SetRotation(45);
		}
	}
	
	/*
	int j = 30;
	for(int i = 20; i < 50; i++){
		shiftDownAndNeighbors(pfGrid, j, i, 5, 5);
		clearVisitation(pfGrid, pfDim);
		j+=2;
	}*/
	
	
	sf::RenderWindow App(sf::VideoMode(cellSize*pfDim, cellSize*pfDim, 32), "Potential Field Visualizer");
	
	App.SetFramerateLimit(15);
	
	drawGrid(App, pfGrid, pfDim);
	
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
        }
		
		drawGrid(App, pfGrid, pfDim);
    }
	
	delete[] pfGrid;
	
    return EXIT_SUCCESS;
}
