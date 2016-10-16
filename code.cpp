#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <algorithm>

using namespace std;

class ISLAND{
	public:
	bool isIsland = false;
	unsigned int freeBridges;
	//unsigned int maxBridges;
	pair<unsigned int , unsigned int> indxs;
	list<ISLAND* > neighborsPtrs;
	
	void addNeighbors(int iIndx , int jIndx , int width , int height , ISLAND** islandsMatr){
		for(int i = iIndx + 1 ; i < width ; ++i){
			if(islandsMatr[i][jIndx].isIsland){
				neighborsPtrs.push_back( &(islandsMatr[i][jIndx]) ); break;
			}
		}
		for(int i = iIndx - 1 ; i >= 0 ; --i){
			if(islandsMatr[i][jIndx].isIsland){
				neighborsPtrs.push_back( &(islandsMatr[i][jIndx]) ); break;
			}
		}
		for(int i = jIndx + 1 ; i < height ; ++i){
			if(islandsMatr[iIndx][i].isIsland){
				neighborsPtrs.push_back( &(islandsMatr[iIndx][i]) ); break;
			}
		}
		for(int i = jIndx - 1 ; i >= 0 ; --i){
			if(islandsMatr[iIndx][i].isIsland){
				neighborsPtrs.push_back( &(islandsMatr[iIndx][i]) ); break;
			}
		}
	}
};
int main()
{
    int width; // the number of cells on the X axis
    cin >> width; cin.ignore();
    int height; // the number of cells on the Y axis
    cin >> height; cin.ignore();
	
	ISLAND** islandsMatr = new ISLAND*[height];
		for(int i = 0 ; i < height ; ++i)
			islandsMatr[i] = new ISLAND[width];
		
    for (int i = 0; i < height; i++) {
        string line; // width characters, each either a number or a '.'
        getline(cin, line);
		for(int j = 0 ; j < width ; ++j){
			if(line[j] != '.'){
				islandsMatr[i][j].isIsland = true;
				islandsMatr[i][j].freeBridges = (int)(line[j]) - 48;
			}
		}
    }
	
	for (int i = 0; i < height; i++) {
		for(int j = 0 ; j < width ; ++j){
			if(islandsMatr[i][j].isIsland){
				islandsMatr[i][j].addNeighbors(i , j , width , height , islandsMatr);
			}
		}
    }
    cout << "0 0 2 0 1" << endl;
}