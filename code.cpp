#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

/**
 * The machines are gaining ground. Time to show them what we're really made of...
 **/
int main()
{
    int width; // the number of cells on the X axis
    cin >> width; cin.ignore();
    int height; // the number of cells on the Y axis
    cin >> height; cin.ignore();
    for (int i = 0; i < height; i++) {
        string line; // width characters, each either a number or a '.'
        getline(cin, line);
    }

    // Write an action using cout. DON'T FORGET THE "<< endl"
    // To debug: cerr << "Debug messages..." << endl;


    // Two coordinates and one integer: a node, one of its neighbors, the number of links connecting them.
    cout << "0 0 2 0 1" << endl;
}