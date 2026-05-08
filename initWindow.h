#pragma once

#include"GameOfLife.h"

using namespace std;

class initWindow
{
private:
	int cellSize;
	int gridWidth;
	int gridHeight;
	const int windowWidth = 500;
	const int windowHeight = 500;
	const int maxGridWidth = 160;
	const int minGridWidth = 60;
	const int maxGridHeight = 90;
	const int minGridHeight = 20;
	COLORREF gridColor;

	int StartButtonx = windowWidth / 4;
	int StartButtony = windowHeight / 4;
	int StartButtonWidth = windowWidth / 2;
	int StartButtonHeight = windowHeight / 16;

	bool isInSetColor = false;
	bool isInSetSize = false;
	bool isInMenu = true;
	bool isPainting = false;

public:
	//构造函数
	initWindow(int cellSize = DEFAULTCELLSIZE, int gridWidth = DEFAULTGRIDWIDTH, int gridHeight = DEFAULTGRIDHEIGHT) :
		cellSize(cellSize), gridWidth(gridWidth), gridHeight(gridHeight), gridColor(DEFAULTGRIDCOLOR)
	{
		initgraph(windowWidth, windowHeight, EX_DBLCLKS);
		setbkcolor(BLACK);
		cleardevice();
		BeginBatchDraw();
	}

	//析构函数
	~initWindow(void)
	{
		EndBatchDraw();
		closegraph();
	}

	//绘制按钮（文本居中显示于矩形上）
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

	//绘制方格背景
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

	//绘制目录界面
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

	//以下四个函数用于判断鼠标点击位置
	bool isHangingOnStart(ExMessage& msg)
	{
		if (!isInMenu)
			return false;
		if (msg.x >= StartButtonx && msg.x <= StartButtonx + StartButtonWidth && msg.y >= StartButtony && msg.y <= StartButtonHeight + StartButtony)
			return true;
		return false;
	}

	bool isHangingOnSetColor(ExMessage& msg)
	{
		if (!isInMenu)
			return false;
		if (msg.x >= StartButtonx && msg.x <= StartButtonx + StartButtonWidth && msg.y >= StartButtony * 2 && msg.y <= StartButtonHeight + StartButtony * 2)
			return true;
		return false;
	}

	bool isHangingOnReturn(ExMessage& msg)
	{
		if (isInMenu)
			return false;
		if (msg.x <= windowWidth / 8 && msg.y <= windowHeight / 16)
			return true;
		return false;
	}

	bool isHangingOnSetSize(ExMessage& msg)
	{
		if (!isInMenu)
			return false;
		if (msg.x >= StartButtonx && msg.x <= StartButtonx + StartButtonWidth && msg.y >= StartButtony * 3 && msg.y <= StartButtonHeight + StartButtony * 3)
			return true;
		return false;
	}

	//绘制颜色设置界面
	void drawSetColor(void)
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

		TCHAR UI[100] = _T("Click the right side of button to add,left to reduce.");
		outtextxy(0, windowHeight * 15 / 16, UI);

		FlushBatchDraw();
	}

	//绘制方格数量设置界面
	void drawSetSize(void)
	{
		cleardevice();
		drawBackground();
		drawButton(0, 0, windowWidth / 8, windowHeight / 16, _T("Return"));
		
		TCHAR gridWidthMsg[100];
		_stprintf_s(gridWidthMsg, _T("Horizontal grid nums:%d"), gridWidth);
		drawButton(StartButtonx, windowHeight * 29 / 96, StartButtonWidth, StartButtonHeight, gridWidthMsg);

		TCHAR gridHeightMsg[100];
		_stprintf_s(gridHeightMsg, _T("Vertical grid nums:%d"), gridHeight);
		drawButton(StartButtonx, windowHeight * 61 / 96, StartButtonWidth, StartButtonHeight, gridHeightMsg);

		TCHAR UI[100] = _T("Click the right side of button to add,left to reduce.");
		outtextxy(0, windowHeight * 15 / 16, UI);

		FlushBatchDraw();
	}

	//运行游戏
	void runGame(void)
	{
		EndBatchDraw();
		closegraph();
		GameOfLife game(cellSize, gridWidth, gridHeight, gridColor);
		game.run();
	}

	//处理鼠标键盘消息
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
			if (isPainting)
				break;
			else
				isPainting = true;
			if (isHangingOnStart(msg))
				runGame();
			else if (isHangingOnSetColor(msg))
			{
				isInSetColor = true;
				isInSetSize = false;
				isInMenu = false;
				drawSetColor();
			}
			else if (isHangingOnSetSize(msg))
			{
				isInSetSize = true;
				isInSetColor = false;
				isInMenu = false;
				drawSetSize();
			}
			else if (isHangingOnReturn(msg))
			{
				isInSetSize = false;
				isInSetColor = false;
				isInMenu = true;
				drawMenu();
			}
			else if(isInSetSize)
			{
				if (msg.y <= windowHeight / 2)
				{
					if (msg.x <= windowWidth / 4)
					{
						gridWidth -= 10;
						if (gridWidth < minGridWidth)
							gridWidth += (maxGridWidth - minGridWidth);
					}
					else if (msg.x <= windowWidth / 2)
					{
						gridWidth -= 5;
						if (gridWidth < minGridWidth)
							gridWidth += (maxGridWidth - minGridWidth);
					}
					else if (msg.x <= windowWidth * 3 / 4)
					{
						gridWidth += 5;
						if (gridWidth > maxGridWidth)
							gridWidth -= (maxGridWidth - minGridWidth);
					}
					else
					{
						gridWidth += 10;
						if (gridWidth > maxGridWidth)
							gridWidth -= (maxGridWidth - minGridWidth);
					}
				}
				else
				{
					if (msg.x <= windowWidth / 4)
					{
						gridHeight -= 10;
						if (gridHeight < minGridHeight)
							gridHeight += (maxGridHeight - minGridHeight);
					}
					else if (msg.x <= windowWidth / 2)
					{
						gridHeight -= 5;
						if (gridHeight < minGridHeight)
							gridHeight += (maxGridHeight - minGridHeight);
					}
					else if (msg.x <= windowWidth * 3 / 4)
					{
						gridHeight += 5;
						if (gridHeight > maxGridHeight)
							gridHeight -= (maxGridHeight - minGridHeight);
					}
					else
					{
						gridHeight += 10;
						if (gridHeight > maxGridHeight)
							gridHeight -= (maxGridHeight - minGridHeight);
					}
				}
				drawSetSize();
			}
			else if (isInSetColor)
			{
				if (msg.y <= windowHeight * 3 / 8)
				{
					if (msg.x <= windowWidth / 4)
						gridColor = RGB((GetRValue(gridColor) + 245) % 255, GetGValue(gridColor), GetBValue(gridColor));
					else if (msg.x <= windowWidth / 2)
						gridColor = RGB((GetRValue(gridColor) + 250) % 255, GetGValue(gridColor), GetBValue(gridColor));
					else if (msg.x <= windowWidth * 3 / 4)
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
			else {}
			break;
		case WM_LBUTTONUP:
			isPainting = false;
		}
	}

	//运行主程序
	void run(void)
	{
		drawMenu();
		while (true)
		{
			handleMessage();
		}
	}

};
