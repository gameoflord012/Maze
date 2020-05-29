#include <vector>

using namespace std;

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

// Override base class with your custom functionality
class Maze : public olc::PixelGameEngine
{
public:
	Maze()
	{		
		sAppName = "Maze";

		nMazeWidth_ = 40;
		nMazeHeight_ = 24;

		maze_ = new int[nMazeWidth_ * nMazeHeight_];
		std::fill(maze_, maze_ + nMazeWidth_ * nMazeHeight_, 0);

		stack_.push_back({ 0, 0 });
		maze_[0] = CELL_VISITED;			

		//Graphics
		nCellWidth_ = 6;
		nCellHeight_ = 8;
		nPathWidth_ = 4;
		nPathHeight_ = 2;

		nScreenWidth = (nCellWidth_ + nPathWidth_) * nMazeWidth_;
		nScreenHeight = (nCellHeight_ + nPathHeight_) * nMazeHeight_;		
	}

public:
	bool OnUserCreate() override
	{		
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		this_thread::sleep_for(20ms);
		//Clear Screen
		Clear(olc::BLACK);

		//Draw Maze

		//{{Next Cell}, Direction}
		vector<pair<pair<int, int>, int>> adj;
		while (true) {
			if (stack_.empty()) break;

			for (int k = 0; k < 4; k++) {
				pair<int, int> top = stack_.back();
				top.first += (k & 1 ? 2 - k : 0);
				top.second += (k & 1 ? 0 : k - 1);

				if (top.first < 0 or top.first > nMazeWidth_ - 1 or top.second < 0 or top.second > nMazeHeight_ - 1 or
					(maze_[top.first + top.second * nMazeWidth_] & CELL_VISITED)) continue;		

				adj.push_back({ top, k });
			}

			if (adj.empty()) stack_.pop_back();
			else break;
		}
		

		if (not stack_.empty()) {
			int nextIdx = rand() % adj.size();

			int direction = adj[nextIdx].second;
			maze_[stack_.back().first + stack_.back().second * nMazeWidth_] |= INDEX_DIRECTION[direction];

			pair<int, int> nextCell = adj[nextIdx].first;
			maze_[nextCell.first + nextCell.second * nMazeWidth_] |= 0 | CELL_VISITED;
			INDEX_DIRECTION[direction ^ 0x02];

			stack_.push_back(nextCell);
			//drawCell(stack_.back(), HEAD_PIXEL);
		}		


//Draw=============		
		for(int x = 0; x < nMazeWidth_; x++)
			for (int y = 0; y < nMazeHeight_; y++) {				
				if (maze_[x + y * nMazeWidth_] & CELL_VISITED) {
					//DrawCell
					drawCell({ x, y });					
					drawCellLink({ x, y }, maze_[x + y * nMazeWidth_], CELL_PIXEL);					
				}				
			}		
		if (not stack_.empty()) 
			drawCell(stack_.back(), HEAD_PIXEL);
		return true;
	}

public:
	int nScreenWidth, nScreenHeight;
	static const olc::Pixel SPACE_PIXEL;
	static const olc::Pixel PATH_PIXEL;
	static const olc::Pixel CELL_PIXEL;
	static const olc::Pixel HEAD_PIXEL;
	static const int INDEX_DIRECTION[];	

private:
	int nMazeWidth_;
	int nMazeHeight_;

	int* maze_;

	enum {
		CELL_PATH_UP = 0x01,
		CELL_PATH_RIGHT = 0x02,
		CELL_PATH_DOWN = 0x04,
		CELL_PATH_LEFT = 0x08,
		CELL_VISITED = 0x10
	};	

	vector<pair<int, int>> stack_;

private:
	//Graphics	

	int nCellWidth_, nCellHeight_;
	int nPathWidth_, nPathHeight_;			

	pair<int, int> getDrawCoord(pair<int, int> coord) {
		int w = nCellWidth_ + nPathWidth_;
		int h = nCellHeight_ + nPathHeight_;
		return { (coord.first * w), (coord.second * h) };
	}

	void drawCell(pair<int, int> p, olc::Pixel color = CELL_PIXEL) {
		pair<int, int> coord = getDrawCoord(p);
		FillRect({ coord.first, coord.second }, { nCellWidth_, nCellHeight_ }, color);
	}

	void drawCellLink(pair<int, int> p, int bit, olc::Pixel color = CELL_PIXEL) {
		if (bit & CELL_PATH_UP) {
			drawCellLink({ p.first, p.second - 1 }, CELL_PATH_DOWN, color);
		}
		if (bit & CELL_PATH_DOWN) {
			pair<int, int> coord = getDrawCoord(p);
			coord.second += nCellHeight_;
			FillRect({ coord.first, coord.second }, { nCellWidth_, nPathHeight_ }, color);
		}
		if (bit & CELL_PATH_LEFT) {

			drawCellLink({ p.first - 1, p.second }, CELL_PATH_RIGHT, color);
		}
		if (bit & CELL_PATH_RIGHT) {	
			pair<int, int> coord = getDrawCoord(p);
			coord.first += nCellWidth_;
			FillRect({ coord.first, coord.second }, { nPathWidth_, nCellHeight_ }, color);
		}
	}
};
olc::Pixel const Maze::SPACE_PIXEL = olc::VERY_DARK_BLUE;
olc::Pixel const Maze::PATH_PIXEL = olc::RED;
olc::Pixel const Maze::CELL_PIXEL = olc::GREEN;
olc::Pixel const Maze::HEAD_PIXEL = olc::DARK_MAGENTA;
int const Maze::INDEX_DIRECTION[] = {0x01, 0x02, 0x04, 0x08, 0x10};

int main() {
	Maze game;
	if (game.Construct(game.nScreenWidth, game.nScreenHeight, 4, 4))
		game.Start();
}