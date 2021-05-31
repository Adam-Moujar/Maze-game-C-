#include <iostream>

#define mazeLength 20
#define mazeWidth 50

using namespace std;

int returnIndex(int x, int y)
{
    return y * 50 + x;
}

class grid
{
public:

    int gridElements = mazeLength * mazeWidth;
    char grid[mazeLength * mazeWidth];

    void initGrid()
    {
        for (int i = 0; i < gridElements; i++)
        {
            grid[i] = '#';
        }
    }

    void printGrid()
    {
        for (int i = 0; i < mazeLength; i++)
        {
            for (int j = 0; j < mazeWidth; j++)
            {
                cout << grid[i * mazeLength + j];
            }
            cout << " " << endl;
        }
    }
};

int main()
{
    grid mygrid;
    mygrid.initGrid();
    mygrid.printGrid();
}
