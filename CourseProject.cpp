#include<iostream>
#include<easyx.h>
#include<vector>
#include<thread>
#include<chrono>
#include<tchar.h>

using namespace std;

const int DEFAULTCELLSIZE = 8;			//默认方格大小
const int DEFAULTGRIDWIDTH = 120;		//默认方格宽度（列数）
const int DEFAULTGRIDHEIGHT = 60;		//默认方格高度（行数）
const int DEFAULTSPEED = 100;			//默认速度
const COLORREF DEFAULTGRIDCOLOR = RED;

class GameOfLife
{
private:
	int cellSize;
	int gridWidth;
	int gridHeight;
	int windowWidth;
	int windowHeight;
	COLORREF gridColor;

	vector<vector<bool>>currentGrid;
	vector<vector<bool>>nextGrid;

	bool running;
	bool drawing;
	int generation;
	int population;
	int speed;

	//计算周围存活细胞数
	int countNeighbors(int row, int col)
	{
		int neighbors = 0;
		for (int i = -1; i <= 1; i++)
			for (int j = -1; j <= 1; j++)
			{
				if (i == 0 && j == 0)
					continue;
				int nrow = row + i, ncol = col + j;
				if (nrow < 0)
					nrow = gridHeight - 1;
				else if (nrow >= gridHeight)
					nrow = 0;
				if (ncol < 0)
					ncol = gridWidth - 1;
				else if (ncol >= gridWidth)
					ncol = 0;

				if (currentGrid[nrow][ncol])
					neighbors++;
			}
		return neighbors;
	}

public:
	//构造函数
	GameOfLife(int cellSize = DEFAULTCELLSIZE, int gridWidth = DEFAULTGRIDWIDTH, int gridHeight = DEFAULTGRIDHEIGHT, COLORREF gridColor = DEFAULTGRIDCOLOR) :
		cellSize(cellSize), gridWidth(gridWidth), gridHeight(gridHeight), gridColor(gridColor), running(false), drawing(false), generation(0), population(0), speed(DEFAULTSPEED)
	{
		windowWidth = gridWidth * (cellSize + 1) - 1;
		windowHeight = gridHeight * (cellSize + 1) - 1;
		currentGrid.resize(gridHeight, vector<bool>(gridWidth));
		nextGrid.resize(gridHeight, vector<bool>(gridWidth));
		for (int i = 0; i < gridHeight; i++)
			for (int j = 0; j < gridWidth; j++)
			{
				currentGrid[i][j] = false;
				nextGrid[i][j] = false;
			}
		initgraph(windowWidth, windowHeight, EX_DBLCLKS);
		setbkcolor(BLACK);
		cleardevice();
		BeginBatchDraw();
	}

	//析构函数
	~GameOfLife(void)
	{
		EndBatchDraw();
		closegraph();
	}

	//随机生成方格
	void randomGrid(void)
	{
		population = 0;
		for (int i = 0; i < gridHeight; i++)
			for (int j = 0; j < gridWidth; j++)
			{
				currentGrid[i][j] = (rand() % 10 == 0);
				if (currentGrid[i][j])
					population++;
			}
		generation = 0;
		running = false;
	}

	//清空方格
	void clearGrid(void)
	{
		for (int i = 0; i < gridHeight; i++)
			for (int j = 0; j < gridWidth; j++)
			{
				currentGrid[i][j] = false;
			}
		population = 0;
		generation = 0;
		running = false;
	}

	//立即绘制
	void draw(void)
	{
		cleardevice();

		setlinecolor(RGB(250, 200, 220));
		for (int i = 1; i < gridWidth; i++)
			line(i * (cellSize + 1) - 1, 0, i * (cellSize + 1) - 1, windowHeight - 1);
		for (int i = 1; i < gridHeight; i++)
			line(0, i * (cellSize + 1) - 1, windowWidth - 1, i * (cellSize + 1) - 1);

		for (int i = 0; i < gridHeight; i++)
			for (int j = 0; j < gridWidth; j++)
			{
				if (currentGrid[i][j])
				{
					setfillcolor(gridColor);
					solidrectangle(j * (cellSize + 1), i * (cellSize + 1), (j + 1) * (cellSize + 1) - 2, (i + 1) * (cellSize + 1) - 2);
				}
			}

		TCHAR UI[200];
		_stprintf_s(UI, _T("Generation:%d Population:%d Speed:%dms Status:%s"), generation, population, speed, (running ? _T("RUNNING") : _T("PAUSED")));
		outtextxy(20, 10, UI);

		TCHAR help[200];
		_stprintf_s(help, _T("Space: Start/Pause  C: Clear  R: Random +/-: Speed  N: Next  ESC: Exit"));
		outtextxy(20, windowHeight - 30, help);

		FlushBatchDraw();
	}

	//处理键盘与鼠标信息
	void handleMessage(void)
	{
		ExMessage msg;
		while (peekmessage(&msg, EX_KEY | EX_MOUSE))
		{
			switch (msg.message)
			{
			case WM_KEYDOWN:
				switch (msg.vkcode)
				{
				case VK_SPACE:
					running = !running;
					draw();
					break;
				case VK_ESCAPE:
					exit(0);
					break;
				case 'R':
					randomGrid();
					draw();
					break;
				case 'C':
					clearGrid();
					draw();
					break;
				case VK_ADD:
					if (speed >= 20)
						speed -= 10;
					break;
				case VK_SUBTRACT:
					if (speed <= 190)
						speed += 10;
					break;
				case 'N':
					if(!running)
					{
						running = true;
						updateGrid();
						running = false;
						draw();
					}
					break;
				}
				break;
			case WM_LBUTTONDOWN:
			{
				drawing = true;
				int x = msg.x, y = msg.y;
				if (x >= 0 && x < windowWidth && y >= 0 && y < windowHeight)
					if (x % (cellSize + 1) != cellSize && y % (cellSize + 1) != cellSize)
					{
						currentGrid[y / (cellSize + 1)][x / (cellSize + 1)] = !currentGrid[y / (cellSize + 1)][x / (cellSize + 1)];
						population += (currentGrid[y / (cellSize + 1)][x / (cellSize + 1)] ? 1 : -1);
						draw();
					}
				break;
			}
			case WM_MOUSEMOVE:
			{
				if (!drawing)
					break;
				int x = msg.x, y = msg.y;
				if (x >= 0 && x < windowWidth && y >= 0 && y < windowHeight)
					if (x % (cellSize + 1) != cellSize && y % (cellSize + 1) != cellSize)
						if (!currentGrid[y / (cellSize + 1)][x / (cellSize + 1)])
						{
							currentGrid[y / (cellSize + 1)][x / (cellSize + 1)] = true;
							population++;
							draw();
						}
				break;
			}
			case WM_LBUTTONUP:
				drawing = false;
				break;
			}
		}
	}

	//更新方格状态
	void updateGrid(void)
	{
		if (!running)
			return;

		population = 0;
		for (int i = 0; i < gridHeight; i++)
			for (int j = 0; j < gridWidth; j++)
			{
				int neighbors = countNeighbors(i, j);
				if (currentGrid[i][j])
				{
					if (neighbors == 2 || neighbors == 3)
						nextGrid[i][j] = true;
					else
						nextGrid[i][j] = false;
				}
				else
				{
					if (neighbors == 3)
						nextGrid[i][j] = true;
					else
						nextGrid[i][j] = false;
				}
				if (nextGrid[i][j])
					population++;
			}
		swap(currentGrid, nextGrid);
		generation++;
	}

	//运行游戏
	void run(void)
	{
		randomGrid();
		draw();
		while (true)
		{
			handleMessage();
			updateGrid();
			if (running)
				draw();
			this_thread::sleep_for(chrono::milliseconds(speed));
		}
	}
};

class initWindow
{
private:
	int cellSize;
	int gridWidth;
	int gridHeight;
	const int windowWidth = 500;
	const int windowHeight = 500;
	COLORREF gridColor;

	int StartButtonx =  windowWidth / 4;
	int StartButtony = windowHeight / 4;
	int StartButtonWidth = windowWidth / 2;
	int StartButtonHeight = windowHeight / 16;

	bool isInSetColor = false;

public:
	initWindow(int cellSize = DEFAULTCELLSIZE, int gridWidth = DEFAULTGRIDWIDTH, int gridHeight = DEFAULTGRIDHEIGHT) :
		cellSize(cellSize), gridWidth(gridWidth), gridHeight(gridHeight), gridColor(DEFAULTGRIDCOLOR)
	{
		initgraph(windowWidth, windowHeight, EX_DBLCLKS);
		setbkcolor(BLACK);
		cleardevice();
		BeginBatchDraw();
	}

	~initWindow(void)
	{
		EndBatchDraw();
		closegraph();
	}

	void drawButton(int buttonx, int buttony, int buttonWidth, int buttonHeight, LPCTSTR text, int nHeight = 16, COLORREF buttoncolor = RED)
	{
		COLORREF textcolor = RGB(255 - GetRValue(buttoncolor), 255 - GetGValue(buttoncolor), 255 - GetBValue(buttoncolor));
		setfillcolor(buttoncolor);
		solidrectangle(buttonx, buttony, buttonx + buttonWidth, buttony + buttonHeight);
		settextcolor(textcolor);
		settextstyle(nHeight, 0, _T("Consolas"));
		int spaceWidth = (buttonWidth - textwidth(text)) / 2;
		int spaceHeight = (buttonHeight - textheight(text)) / 2;
		setbkmode(TRANSPARENT);
		outtextxy(buttonx + spaceWidth, buttony + spaceHeight, text);
	}

	void drawBackground(void)
	{
		setfillcolor(gridColor);
		srand((unsigned int)time(NULL));
		for (int i = 0; i < windowWidth; i += cellSize)
			for (int j = 0; j < windowHeight; j += cellSize)
			{
				if (rand() % 10 == 0)
					solidrectangle(i, j, i + cellSize - 1, j + cellSize - 1);
			}
		FlushBatchDraw();
	}

	void drawMenu(void)
	{
		cleardevice();
		drawBackground();
		drawButton(windowWidth / 4, windowHeight / 16, windowWidth / 2, windowHeight / 16, _T("Game of Life"), 32, BLACK);
		drawButton(StartButtonx, StartButtony, StartButtonWidth, StartButtonHeight, _T("Start"));
		drawButton(StartButtonx, StartButtony * 2, StartButtonWidth, StartButtonHeight, _T("Set Color"));
		drawButton(StartButtonx, StartButtony * 3, StartButtonWidth, StartButtonHeight, _T("Set Size"));
		FlushBatchDraw();
	}

	bool isHangingOnStart(ExMessage& msg)
	{
		if (msg.x >= StartButtonx && msg.x <= StartButtonx + StartButtonWidth && msg.y >= StartButtony && msg.y <= StartButtonHeight + StartButtony)
			return true;
		return false;
	}

	bool isHangingOnSetColor(ExMessage& msg)
	{
		if (msg.x >= StartButtonx && msg.x <= StartButtonx + StartButtonWidth && msg.y >= StartButtony * 2 && msg.y <= StartButtonHeight + StartButtony * 2)
			return true;
		return false;
	}

	bool isHangingOnReturn(ExMessage& msg)
	{
		if (msg.x <= windowWidth / 8 && msg.y <= windowHeight / 16)
			return true;
		return false;
	}

	void drawSetColor()
	{
		cleardevice();
		drawBackground();
		drawButton(0, 0, windowWidth / 8, windowHeight / 16, _T("Return"));

		TCHAR RValue[100];
		_stprintf_s(RValue, _T("R Value=%d"), GetRValue(gridColor));
		drawButton(StartButtonx, windowHeight * 7 / 32, StartButtonWidth, StartButtonHeight, RValue);

		TCHAR GValue[100];
		_stprintf_s(GValue, _T("G Value=%d"), GetGValue(gridColor));
		drawButton(StartButtonx, windowHeight * 15 / 32, StartButtonWidth, StartButtonHeight, GValue);

		TCHAR BValue[100];
		_stprintf_s(BValue, _T("B Value=%d"), GetBValue(gridColor));
		drawButton(StartButtonx, windowHeight * 23 / 32, StartButtonWidth, StartButtonHeight, BValue);

		FlushBatchDraw();
	}

	void runGame(void)
	{
		EndBatchDraw();
		closegraph();
		GameOfLife game(cellSize, gridWidth, gridHeight, gridColor);
		game.run();
	}

	void handleMessage(void)
	{
		ExMessage msg;
		peekmessage(&msg, EX_KEY | EX_MOUSE);
		switch (msg.message)
		{
		case WM_KEYDOWN:
			if (msg.vkcode == VK_RETURN)
				runGame();
			break;
		case WM_LBUTTONDOWN:
			if (!isInSetColor)
			{
				if (isHangingOnStart(msg))
					runGame();
				if (isHangingOnSetColor(msg))
				{
					isInSetColor = true;
					drawSetColor();
				}
			}
			else
			{
				if (isHangingOnReturn(msg))
				{
					isInSetColor = false;
					drawMenu();
				}
				else
				{
					if (msg.y <= windowHeight * 3 / 8)
					{
						if (msg.x <= windowWidth / 4)
							gridColor = RGB((GetRValue(gridColor) + 245) % 255, GetGValue(gridColor), GetBValue(gridColor));
						else if(msg.x<=windowWidth/2)
							gridColor = RGB((GetRValue(gridColor) + 250) % 255, GetGValue(gridColor), GetBValue(gridColor));
						else if(msg.x<=windowWidth*3/4)
							gridColor = RGB((GetRValue(gridColor) + 5) % 255, GetGValue(gridColor), GetBValue(gridColor));
						else
							gridColor = RGB((GetRValue(gridColor) + 10) % 255, GetGValue(gridColor), GetBValue(gridColor));
					}
					else if (msg.y <= windowHeight * 5 / 8)
					{
						if (msg.x <= windowWidth / 4)
							gridColor = RGB(GetRValue(gridColor), (GetGValue(gridColor) + 245) % 255, GetBValue(gridColor));
						else if (msg.x <= windowWidth / 2)
							gridColor = RGB(GetRValue(gridColor), (GetGValue(gridColor) + 250) % 255, GetBValue(gridColor));
						else if (msg.x <= windowWidth * 3 / 4)
							gridColor = RGB(GetRValue(gridColor), (GetGValue(gridColor) + 5) % 255, GetBValue(gridColor));
						else
							gridColor = RGB(GetRValue(gridColor), (GetGValue(gridColor) + 10) % 255, GetBValue(gridColor));
					}
					else
					{
						if (msg.x <= windowWidth / 4)
							gridColor = RGB(GetRValue(gridColor), GetGValue(gridColor), (GetBValue(gridColor) + 245) % 255);
						else if (msg.x <= windowWidth / 2)
							gridColor = RGB(GetRValue(gridColor), GetGValue(gridColor), (GetBValue(gridColor) + 250) % 255);
						else if (msg.x <= windowWidth * 3 / 4)
							gridColor = RGB(GetRValue(gridColor), GetGValue(gridColor), (GetBValue(gridColor) + 5) % 255);
						else
							gridColor = RGB(GetRValue(gridColor), GetGValue(gridColor), (GetBValue(gridColor) + 10) % 255);
					}
					drawSetColor();
				}
			}
			break;
		}
	}

	void run(void)
	{
		drawMenu();
		while (true)
		{
			handleMessage();
		}
	}

};

int main(void)
{
	initWindow window;
	window.run();

	return 0;
}