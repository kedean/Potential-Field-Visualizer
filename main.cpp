#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <vector>

struct cell{
	int x, y;
	int attraction;
	bool visited;
	sf::Shape cell;
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
				if((*it)->attraction > 127)
					(*it)->attraction = 127;
				else if((*it)->attraction < -127)
					(*it)->attraction = -127;
				(*it)->visited = true;
				if((*it)->x < 99)
					siblings.push_back(&grid[(*it)->x + 1][(*it)->y]);
				if((*it)->y < 99)
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

void drawGrid(sf::RenderWindow& App, Cell** pfGrid, int pfDim){
	
	App.SetActive();
	
	glClear(GL_COLOR_BUFFER_BIT);
	
	
	for(int x = 0; x < pfDim; x++){
		for(int y = 0; y < pfDim; y++){
			int val = pfGrid[x][y].attraction + 127;
			sf::Color cellColor =  sf::Color(val, val, val, 255);
			pfGrid[x][y].cell.SetPointColor(0, cellColor);
			pfGrid[x][y].cell.SetPointColor(1, cellColor);
			pfGrid[x][y].cell.SetPointColor(2, cellColor);
			pfGrid[x][y].cell.SetPointColor(3, cellColor);
			App.Draw(pfGrid[x][y].cell);
		}
	}
	
	
	App.Display();
	
}

int main(int argc, char* argv[])
{
	const int pfDim = 100;
	
	float cellSize = 10;
	long shiftAmount = 5;
	
	Cell** pfGrid = new Cell*[pfDim];
	
	for(int x = 0; x < pfDim; x++){
		pfGrid[x] = new Cell[pfDim];
		for(int y = 0; y < pfDim; y++){
			float xPos = x*cellSize;
			float yPos = y*cellSize;
			
			pfGrid[x][y].x = x;
			pfGrid[x][y].y = y;
			pfGrid[x][y].attraction = 0;
			pfGrid[x][y].visited = false;
			pfGrid[x][y].cell = sf::Shape::Rectangle(xPos, yPos, xPos+cellSize, yPos+cellSize, sf::Color(0, 0, 0, 255));
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
	
	App.SetFramerateLimit(30);
	
	drawGrid(App, pfGrid, pfDim);
	
	float elapsedSinceLastDraw = 0.f;
	sf::Clock clock;
	
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
				int cellX = Event.MouseButton.X / cellSize;
				int cellY = Event.MouseButton.Y / cellSize;
				
				if(Event.MouseButton.Button == 0){ //left click, increase at this spot
					/*pfGrid[cellX][cellY] -= shiftAmount;
					if(pfGrid[cellX][cellY] < -127){
						pfGrid[cellX][cellY] = -127;
						
					}*/
					
					shiftDownAndNeighbors(pfGrid, cellX, cellY, shiftAmount, pfGrid[cellX][cellY].attraction/shiftAmount);
					clearVisitation(pfGrid, pfDim);
				}
				
				else if(Event.MouseButton.Button == 1){ //right click, decrease at this spot
					/*pfGrid[cellX][cellY] += shiftAmount;
					if(pfGrid[cellX][cellY] > 127)
						pfGrid[cellX][cellY] = 127;*/
					
					shiftDownAndNeighbors(pfGrid, cellX, cellY, -shiftAmount, abs(pfGrid[cellX][cellY].attraction/shiftAmount));
					clearVisitation(pfGrid, pfDim);
				}
				
				
				
				/*printf("Val = %ld\n", pfGrid[cellX][cellY]);*/
			}
			
            // Resize event : adjust viewport
            if (Event.Type == sf::Event::Resized)
                glViewport(0, 0, Event.Size.Width, Event.Size.Height);
        }
		
		elapsedSinceLastDraw += clock.GetElapsedTime();
		
		if(elapsedSinceLastDraw >= 1.f/10.f){
			drawGrid(App, pfGrid, pfDim);
			elapsedSinceLastDraw = 0.f;
		}
    }
	
	delete[] pfGrid;
	
    return EXIT_SUCCESS;
}
