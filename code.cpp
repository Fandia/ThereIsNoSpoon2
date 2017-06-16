#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <algorithm>
#include <stdexcept>

using namespace std;
class ISLANDS_MATR;
class ISLAND{
public:
	vector<pair<ISLAND*, unsigned int> > neighbours; // ptr to neighbour and count of bridges to this neighbour
	unsigned int x;
	unsigned int y;
	unsigned int capacity;
	bool usedFlag;
	ISLAND() : capacity(0), usedFlag(false) {};
	
	inline unsigned int getMaxBridges() const{
		unsigned int maxBridges = 0;
		for(const auto& neighbour : neighbours)
			maxBridges += min(neighbour.first->capacity, 2 - neighbour.second);
		
		return maxBridges;
	};
	
	inline unsigned int getNeighboursCount() const{
		unsigned int neighboursCount = 0;
		for(const auto& neighbour : neighbours)
			if(neighbour.first->capacity)
				neighboursCount++;
				
		return neighboursCount;
	};
	void addSubBridgeTo(const unsigned int& neighbourIdx, const int& increment, ISLANDS_MATR* islandsMatr);
	void addBridges(const unsigned int& bridges);
};

// Add bridges to neighbours with bridgesCount
void ISLAND::addBridges(const unsigned int& bridgesCount){
	for(auto& neighbour : neighbours){
		unsigned int bridges = min(min(2 - neighbour.second, bridgesCount), neighbour.first->capacity);
		
		if(bridges == 0){
			continue;
		}
		neighbour.first->capacity -= bridges;
		neighbour.second += bridges;
		this->capacity -= bridges;
		
		for(auto& neighbourInNeighbours : neighbour.first->neighbours){
			if(neighbourInNeighbours.first == this){
				neighbourInNeighbours.second += bridges;
				break;
			}
		}
	}
}

// Created to check using pairs of islands
class ISLANDS_POOL{
	vector<pair<ISLAND*, ISLAND*> > pool;
public:
	inline void addSrcDst(ISLAND* srcIsland, ISLAND* dstIsland){
		pool.push_back(pair<ISLAND*, ISLAND*>(srcIsland, dstIsland));
	};
	inline bool checkUsing(const ISLAND* srcIsland, const ISLAND* dstIsland) const{
		for(const auto& bridge : pool){
			if((bridge.first == srcIsland && bridge.second == dstIsland) ||
			(bridge.first == dstIsland && bridge.second == srcIsland))
				return 1;
		}
		return 0;
	};
	inline void clear(){
		pool.clear();
	};
};

class ISLANDS_MATR{
	vector<vector<ISLAND> > islandsMatr;
	deque<ISLAND*> islandsQueue;
	vector<ISLAND*> bridgesQueue;
	vector<ISLAND*> nonZeroCapacity;
	unsigned int width;
	unsigned int height;
public:
	ISLANDS_MATR();
	void addSubObstacles(const ISLAND* src, const ISLAND* dst, const int& increment);
	void easySolver();
	bool checkSolver();
	bool checkSingleGroup();
	bool backtracking();
	bool betterSolver();
	void printBridges() const;
};

ISLANDS_MATR::ISLANDS_MATR(){
	
	// the number of cells on the X axis
	cin >> width; cin.ignore();
	cerr << width << endl;
	// the number of cells on the Y axis
	cin >> height; cin.ignore();
	cerr << height << endl;
	
	// init islands matrix
	for(int i = 0 ; i < height ; ++i)
		islandsMatr.push_back(move(vector<ISLAND>(width)));
		
	// islandsMatr initialization with capacity
	for (int i = 0; i < height; ++i) {
		string line; // width characters, each either a number or a '.'
		getline(cin, line);
		cerr << line << endl;
		for(int j = 0 ; j < width ; ++j){
			if(line[j] == '.'){
				islandsMatr[i][j].capacity = 0;
			}else{
				islandsMatr[i][j].capacity = (int)(line[j]) - 48;
				islandsMatr[i][j].x = j;
				islandsMatr[i][j].y = i;
			}
		}
	}
	
	// adding neighbors to every island
	for (int y = 0; y < height; y++) {
		for(int x = 0 ; x < width ; ++x){
			auto& island = islandsMatr[y][x];
			if(island.capacity){
				for(int i = y + 1 ; i < height ; ++i){
					if(islandsMatr[i][x].capacity){
						island.neighbours.push_back( make_pair<ISLAND*, unsigned int>(&(islandsMatr[i][x]), 0) ); 
						break;
					}
				}
				for(int i = y - 1 ; i >= 0 ; --i){ 
					if(islandsMatr[i][x].capacity){
						island.neighbours.push_back( make_pair<ISLAND*, unsigned int>(&(islandsMatr[i][x]), 0) ); 
						break;
					}
				}
				for(int i = x + 1 ; i < width ; ++i){
					if(islandsMatr[y][i].capacity){
						island.neighbours.push_back( make_pair<ISLAND*, unsigned int>(&(islandsMatr[y][i]), 0) ); 
						break;
					}
				}
				for(int i = x - 1 ; i >= 0 ; --i){
					if(islandsMatr[y][i].capacity){
						island.neighbours.push_back( make_pair<ISLAND*, unsigned int>(&(islandsMatr[y][i]), 0) ); 
						break;
					}
				}
			}
		}
	}
	
	// init priority queue of islands
	for (int y = 0; y < height; y++) {
		for(int x = 0 ; x < width ; ++x){
			const auto island = &islandsMatr[y][x];
			if(island->capacity){
				auto currentMaxBridges = island->getMaxBridges();
				if(currentMaxBridges == island->capacity || 
				((currentMaxBridges - 1 == island->capacity) && (island->capacity < island->getNeighboursCount()))){
					islandsQueue.push_front(island);
				}else{
					islandsQueue.push_back(island);
				}
			}else{
				bridgesQueue.push_back(island);
			}
		}
	}
}

// Add obstacles in islands matrix to avoid cross between bridges. increment = 1 - add, -1 - sub
void ISLANDS_MATR::addSubObstacles(const ISLAND* src, const ISLAND* dst, const int& increment){
	if(src->x == dst->x){
		if(src->y < dst->y)
			for(unsigned int i = src->y + 1; i < dst->y; i++)
				(islandsMatr.at(i)).at(src->x).capacity += increment;
		else
			for(unsigned int i = src->y - 1; i > dst->y; i--)
				(islandsMatr.at(i)).at(src->x).capacity += increment;
	}else if(src->y == dst->y){
		if(src->x < dst->x)
			for(unsigned int i = src->x + 1; i < dst->x; i++)
				(islandsMatr.at(src->y)).at(i).capacity += increment * 2;
		else
			for(unsigned int i = src->x - 1; i > dst->x; i--)
				(islandsMatr.at(src->y)).at(i).capacity += increment * 2;
	}else{
		throw logic_error("Can't add obstacles");
	}
}

// To add bridge - increment = 1, to delete - increment = -1
void ISLAND::addSubBridgeTo(const unsigned int& neighbourIdx, const int& increment, ISLANDS_MATR* islandsMatr){
	auto& neighbour = neighbours.at(neighbourIdx);
	islandsMatr->addSubObstacles(this, neighbour.first, increment);
	capacity -= increment;
	neighbour.first->capacity -= increment;
	neighbour.second += increment;
	
	for(auto& neighbourInNeighbours : neighbour.first->neighbours){
		if(neighbourInNeighbours.first == this){
			neighbourInNeighbours.second += increment;
			break;
		}
	}
}

// Iterative solver with simple rules
void ISLANDS_MATR::easySolver(){
	bool continueLoop = false;
	do{
		continueLoop = false;
		for(auto& island : islandsQueue){
			auto currentMaxBridges = island->getMaxBridges();
			if(island->capacity != 0){
				if(currentMaxBridges == island->capacity){
					island->addBridges(2);
					continueLoop = true;
				}
				else if((currentMaxBridges - 1 == island->capacity) && (island->capacity < island->getNeighboursCount())){
					island->addBridges(1);
					continueLoop = true;
				}
				else if(island->capacity == 1 &&  island->getNeighboursCount() == 1){
					island->addBridges(1);
					continueLoop = true;
				}
			}
		}
	} while(continueLoop);
}

// Check by BFS single group of islands
bool ISLANDS_MATR::checkSingleGroup(){
	queue<ISLAND*> bfsQueue;
	bfsQueue.push(islandsQueue.front());
	unsigned int groupSize = 0;
	do{
		auto island = bfsQueue.front();
		bfsQueue.pop();
		if(island->usedFlag)
			continue;
		island->usedFlag = true;
		
		for(auto& neighbour : island->neighbours)
			if(!neighbour.first->usedFlag && neighbour.second > 0)
				bfsQueue.push(neighbour.first);
		
		groupSize++;
	} while(bfsQueue.size() > 0);
	
	for(auto& island : islandsQueue)
		island->usedFlag = false;
	if(groupSize == islandsQueue.size())
		return true;
	else
		return false;
}

bool ISLANDS_MATR::checkSolver(){
	// Check to route all bridges
	for(const auto& island : nonZeroCapacity){
		if(island->capacity != 0)
			return false;
	}
	// Check to avoid cross bridges
	for(const auto& bridge : bridgesQueue){
		if(bridge->capacity > 1)
			if(bridge->capacity % 2 != 0)
				return false;
	}
	// Check to single group
	if(checkSingleGroup())
		return true;
		
	return false;
}

bool ISLANDS_MATR::backtracking(){
	for(auto& island : nonZeroCapacity){
		if(island->capacity != 0){
			unsigned int neighbourIdx = 0;
			for(auto& neighbour : island->neighbours){
				if(neighbour.second < 2 && neighbour.first->capacity){
					island->addSubBridgeTo(neighbourIdx, 1, this);
					if(checkSolver())
						return true;
					if(backtracking())
						return true;
					island->addSubBridgeTo(neighbourIdx, -1, this);
				}
				neighbourIdx++;
			}
		}
	}
	
	return false;
}

bool ISLANDS_MATR::betterSolver(){
	// try to solve pazzle by easySolver
	easySolver();
	for(const auto& island : islandsQueue){
		if(island->capacity != 0)
			nonZeroCapacity.push_back(island);
	}
	
	// check if easySolver solved pazzle
	if(checkSolver())
		return false;
	// backtracking enter point
	if(!backtracking())
		return true;
		
	return false;
}

void ISLANDS_MATR::printBridges() const{
	ISLANDS_POOL islandsPool;
	for(const auto& island : islandsQueue){
		for(const auto& neighbour : island->neighbours){
			if(!islandsPool.checkUsing(island, neighbour.first) && neighbour.second != 0){
				islandsPool.addSrcDst(island, neighbour.first);
				cout << island->x << ' ' << island->y << ' ' << neighbour.first->x << ' ' << neighbour.first->y << ' ' << neighbour.second << endl;
			}
		}
	}
}
int main()
{
	ISLANDS_MATR islands;
	if(islands.betterSolver()){
		cout << "Can't solve pazzle" << endl;
		return 0;
	}
	islands.printBridges();
}