#include <iostream>
#include <string>
#include <vector>
#include <deque>

using namespace std;

class ISLAND{
public:
	vector<pair<ISLAND*, unsigned int> > neighbours; // ptr to neighbour and count of bridges to this neighbour
	unsigned int x;
	unsigned int y;
	unsigned int capacity;
	
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
	void addSubBridgeTo(const unsigned int& neighbourIdx, const int& increment);
	void addBridges(const unsigned int& bridges);
};

// To add bridge - increment = 1, to delete - increment = -1
void ISLAND::addSubBridgeTo(const unsigned int& neighbourIdx, const int& increment){
	auto neighbour = neighbours.at(neighbourIdx);
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
	deque<ISLAND*> nonZeroCapacity;
public:
	unsigned int width;
	unsigned int height;
	ISLANDS_MATR();
	void easySolver();
	int checkEasySolver() const;
	int betterSolver();
	void printBridges() const;
};

ISLANDS_MATR::ISLANDS_MATR(){
	
	// the number of cells on the X axis
	cin >> width; cin.ignore();
	// the number of cells on the Y axis
	cin >> height; cin.ignore();
	
	// init islands matrix
	
	for(int i = 0 ; i < height ; ++i)
		islandsMatr.push_back(move(vector<ISLAND>(width)));
		
	//	islandsMatr initialization with capacity
	
	for (int i = 0; i < height; ++i) {
		string line; // width characters, each either a number or a '.'
		getline(cin, line);
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
			}
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

int ISLANDS_MATR::checkEasySolver() const{
	for(const auto& island : nonZeroCapacity){
		if(island->capacity != 0)
			return 1;
	}
	return 0;
}

int ISLANDS_MATR::betterSolver(){
	// try to solve pazzle by easySolver
	easySolver();
	for(const auto& island : islandsQueue){
		if(island->capacity != 0)
			nonZeroCapacity.push_back(island);
	}
	
	// check if easySolver solved pazzle
	if(checkEasySolver() == 0)
		return 0;
	
	// try to add one bridge	
	ISLANDS_POOL islandsPool;
	for(auto& island : nonZeroCapacity){
		unsigned int neighbourIdx = 0;
		for(auto& neighbour : island->neighbours){
			if(neighbour.second < 2 && neighbour.first->capacity && 
			!islandsPool.checkUsing(island, neighbour.first)){
				islandsPool.addSrcDst(island, neighbour.first);
				auto tempMatr = islandsMatr;
				island->addSubBridgeTo(neighbourIdx, 1);
				easySolver();
				if(checkEasySolver()){
					islandsMatr = tempMatr;
					continue;
				}
				return 0;
			}
			neighbourIdx++;
		}
	}
	// try to add two bridges
	islandsPool.clear();
	for(auto& island : nonZeroCapacity){
		unsigned int neighbourIdx = 0;
		for(auto& neighbour : island->neighbours){
			if(neighbour.second == 0 && neighbour.first->capacity >= 2 && 
			!islandsPool.checkUsing(island, neighbour.first)){
				islandsPool.addSrcDst(island, neighbour.first);
				auto tempMatr = islandsMatr;
				island->addSubBridgeTo(neighbourIdx, 2);
				easySolver();
				if(checkEasySolver()){
					islandsMatr = tempMatr;
					continue;
				}
				return 0;
			}
			neighbourIdx++;
		}
	}
	return 1;
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