#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <algorithm>

using namespace std;

class ISLAND{
public:
	vector<pair<ISLAND*, unsigned int> > neighbours; // ptr to neighbour and count of bridges to this neighbour
    unsigned int x;
    unsigned int y;
	unsigned int capacity;
	bool shown;
	unsigned int getMaxBridges() const;
	ISLAND():shown(false){};
	void addBridges(unsigned int bridges);
};

unsigned int ISLAND::getMaxBridges() const{
    unsigned int maxBridges = 0;
    for(const auto& neighbour : neighbours){
        //cerr << neighbour.first->capacity << neighbour.first->x << neighbour.first->y << endl;
        maxBridges += min(neighbour.first->capacity, 2 - neighbour.second);
    }
        
    return maxBridges;
}

void ISLAND::addBridges(unsigned int bridgesCount){
    for(auto& neighbour : neighbours){
        unsigned int bridges = min(min(2 - neighbour.second, bridgesCount), neighbour.first->capacity);
        cerr << "src: " << this->x << this->y << '('<<this->capacity<<')' \
        << " dst: " << neighbour.first->x << neighbour.first->y << '('<<neighbour.first->capacity<<')' \
        << " bridges: " << bridges << endl;
        
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

class ISLANDS_MATR{
public:
    unsigned int width;
    unsigned int height;
    vector<vector<ISLAND> > islandsMatr;
    deque<ISLAND*> islandsQueue;
    ISLANDS_MATR();
    void easySolver();
    void printBridges();
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
			auto island = &islandsMatr[y][x];
			if(island->capacity){
			    auto currentMaxBridges = island->getMaxBridges();
			    if(currentMaxBridges == island->capacity || 
			    ((currentMaxBridges - 1 == island->capacity) && (currentMaxBridges < island->neighbours.size()))){
			        islandsQueue.push_front(island);
			    }else{
			        islandsQueue.push_back(island);
			    }
			}
		}
    }
}

void ISLANDS_MATR::easySolver(){
    bool continueLoop = false;
    do{
        continueLoop = false;
        for(auto& island : islandsQueue){
            auto currentMaxBridges = island->getMaxBridges();
            if(island->capacity != 0){
                //cerr << currentMaxBridges << ' ' << island->capacity << endl;
                if(currentMaxBridges == island->capacity){
                    //cerr << "full; capacity: " << island->capacity << ' ' << island->x << island->y << endl;
    		        island->addBridges(2);
    		        continueLoop = true;
    		        continue;
                }
    		    if((currentMaxBridges - 1 == island->capacity) && (currentMaxBridges < island->neighbours.size())){
                    //cerr << "one " << island->x << island->y << endl;
    		        island->addBridges(1);
    		        continueLoop = true;
    		    }
            }
        }
    } while(continueLoop);
}

void ISLANDS_MATR::printBridges(){
    for(auto& island : islandsQueue){
        //cerr << island->x << island->y << ' ' << island->shown << endl;
        if(!island->shown){
            island->shown = true;
            for(auto& neighbour : island->neighbours){
                cerr << island->x << island->y << ' ' << island->shown << endl;
                if(!neighbour.first->shown && neighbour.second != 0){
                    //neighbour.first->shown = true;
                    cout << island->x << ' ' << island->y << ' ' << neighbour.first->x << ' ' << neighbour.first->y << ' ' << neighbour.second << endl;
                }
            }
        }
    }
    for(auto& island : islandsQueue){
        island->shown = false;
    }
}
int main()
{
	ISLANDS_MATR islands;
	islands.easySolver();
	islands.printBridges();
    //cout << "0 0 2 0 1" << endl;
}