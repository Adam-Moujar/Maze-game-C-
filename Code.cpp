#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <chrono>
#include <stdio.h>
#include <Windows.h>


#define mazeLength 23
#define mazeWidth 79

int nScreenWidth = 120;			
int nScreenHeight = 40;			
int nMapWidth = mazeLength;
int nMapHeight = mazeWidth;

float fPlayerX = 1.0f;			
float fPlayerY = 1.0f;
float fPlayerA = 0.0f;			
float fFOV = 3.14159f / 4.0f;	
float fDepth = 16.0f;			
float fSpeed = 5.0f;			

using namespace std;

int returnIndex(int x, int y)
{
    return y * mazeWidth + x;
}

int inBounds(int x, int y)
{
    if (x < 0 || x >= mazeWidth)
    {
        return false;
    }
    if (y < 0 || y >= mazeLength)
    {
        return false;
    }
    return true;

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
                cout << grid[returnIndex(j,i)];
            }
            cout << " " << endl;
        }
    }
    void generateMaze(int x, int y)
    {
        grid[returnIndex(x, y)] = ' ';

        int dirs[4];
        dirs[0] = 0;
        dirs[1] = 1;
        dirs[2] = 2;
        dirs[3] = 3;

        for (int i = 0; i < 4; ++i)
        {
            int r = rand() & 3;
            int temp = dirs[r];
            dirs[r] = dirs[i];
            dirs[i] = temp;
        }

        for (int i = 0; i < 4; ++i)
        {
            int dx = 0, dy = 0;
            switch (dirs[i])
            {
            case 0: dy = -1; break;
            case 2: dy = 1; break;
            case 1: dx = 1; break;
            case 3: dx = -1; break;
            }
            int x2 = x + (dx << 1);
            int y2 = y + (dy << 1);
            if (inBounds(x2, y2))
            {
                if (grid[returnIndex(x2, y2)] == '#')
                {
                    grid[returnIndex(x2 - dx, y2 - dy)] = ' ';
                    generateMaze(x2, y2);
                }
            }
        }
    }

};

int main()
{
    wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;


    grid mygrid;
    srand(time(0));
    mygrid.initGrid();
    mygrid.generateMaze(1,1);

    wstring map;

    for (int i = 0; i < mazeWidth; i++)
    {
        for (int j = 0; j < mazeLength; j++)
        {
            map += mygrid.grid[returnIndex(j, i)];
        }
    }

	auto tp1 = chrono::system_clock::now();
	auto tp2 = chrono::system_clock::now();

	while (1)
	{

		tp2 = chrono::system_clock::now();
		chrono::duration<float> elapsedTime = tp2 - tp1;
		tp1 = tp2;
		float fElapsedTime = elapsedTime.count();


		if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
			fPlayerA -= (fSpeed * 0.75f) * fElapsedTime;

		if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
			fPlayerA += (fSpeed * 0.75f) * fElapsedTime;


		if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
		{
			fPlayerX += sinf(fPlayerA) * fSpeed * fElapsedTime;;
			fPlayerY += cosf(fPlayerA) * fSpeed * fElapsedTime;;
			if (map.c_str()[(int)fPlayerX * nMapWidth + (int)fPlayerY] == '#')
			{
				fPlayerX -= sinf(fPlayerA) * fSpeed * fElapsedTime;;
				fPlayerY -= cosf(fPlayerA) * fSpeed * fElapsedTime;;
			}
		}

		if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
		{
			fPlayerX -= sinf(fPlayerA) * fSpeed * fElapsedTime;;
			fPlayerY -= cosf(fPlayerA) * fSpeed * fElapsedTime;;
			if (map.c_str()[(int)fPlayerX * nMapWidth + (int)fPlayerY] == '#')
			{
				fPlayerX += sinf(fPlayerA) * fSpeed * fElapsedTime;;
				fPlayerY += cosf(fPlayerA) * fSpeed * fElapsedTime;;
			}
		}

		for (int x = 0; x < nScreenWidth; x++)
		{
			float fRayAngle = (fPlayerA - fFOV / 2.0f) + ((float)x / (float)nScreenWidth) * fFOV;


			float fStepSize = 0.1f;		 
			float fDistanceToWall = 0.0f;

			bool bHitWall = false;		
			bool bBoundary = false;		

			float fEyeX = sinf(fRayAngle); 
			float fEyeY = cosf(fRayAngle);

			while (!bHitWall && fDistanceToWall < fDepth)
			{
				fDistanceToWall += fStepSize;
				int nTestX = (int)(fPlayerX + fEyeX * fDistanceToWall);
				int nTestY = (int)(fPlayerY + fEyeY * fDistanceToWall);

				if (nTestX < 0 || nTestX >= nMapWidth || nTestY < 0 || nTestY >= nMapHeight)
				{
					bHitWall = true;			
					fDistanceToWall = fDepth;
				}
				else
				{
					if (map.c_str()[nTestX * nMapWidth + nTestY] == '#')
					{
						bHitWall = true;

						vector<pair<float, float>> p;
						for (int tx = 0; tx < 2; tx++)
							for (int ty = 0; ty < 2; ty++)
							{
								float vy = (float)nTestY + ty - fPlayerY;
								float vx = (float)nTestX + tx - fPlayerX;
								float d = sqrt(vx * vx + vy * vy);
								float dot = (fEyeX * vx / d) + (fEyeY * vy / d);
								p.push_back(make_pair(d, dot));
							}

						sort(p.begin(), p.end(), [](const pair<float, float>& left, const pair<float, float>& right) {return left.first < right.first; });
						float fBound = 0.01;
						if (acos(p.at(0).second) < fBound) bBoundary = true;
						if (acos(p.at(1).second) < fBound) bBoundary = true;
						if (acos(p.at(2).second) < fBound) bBoundary = true;
					}
				}
			}

			int nCeiling = (float)(nScreenHeight / 2.0) - nScreenHeight / ((float)fDistanceToWall);
			int nFloor = nScreenHeight - nCeiling;

			short nShade = ' ';
			if (fDistanceToWall <= fDepth / 4.0f)			
				nShade = 0x2588;		
			else if (fDistanceToWall < fDepth / 3.0f)		
				nShade = 0x2593;
			else if (fDistanceToWall < fDepth / 2.0f)		
				nShade = 0x2592;
			else if (fDistanceToWall < fDepth)				
				nShade = 0x2591;
			else											
				nShade = ' ';		

			if (bBoundary)		nShade = ' '; 

			for (int y = 0; y < nScreenHeight; y++)
			{
				if (y <= nCeiling)
					screen[y * nScreenWidth + x] = ' ';
				else if (y > nCeiling && y <= nFloor)
					screen[y * nScreenWidth + x] = nShade;
				else 
				{
					float b = 1.0f - (((float)y - nScreenHeight / 2.0f) / ((float)nScreenHeight / 2.0f));
					if (b < 0.25)
						nShade = '#';
					else if (b < 0.5)
						nShade = 'x';
					else if (b < 0.75)
						nShade = '.';
					else if (b < 0.9)
						nShade = '-';
					else
						nShade = ' ';
					screen[y * nScreenWidth + x] = nShade;
				}
			}
		}


		screen[nScreenWidth * nScreenHeight - 1] = '\0';
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
	}

	return 0;
}
