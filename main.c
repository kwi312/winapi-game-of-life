/*
	Игра "жизнь" на чистом СИ и WinAPI

	для сборки: cl.exe main.c  /link /DEFAULTLIB:user32.lib /DEFAULTLIB:kernel32.lib /DEFAULTLIB:Gdi32.lib /SUBSYSTEM:WINDOWS
*/
#include <Windows.h>
#include <wingdi.h>
#include <stdlib.h>
#include <WinUser.h>
#include <windowsx.h>
#pragma comment(lib, "user32.lib")
#pragma commemt(lib, "Gdi32.lib")
#pragma comment(lib, "kernel32.lib")
#define BOARDSIZE 100 // размер поля (обе стороны)
#define MAX_INDEX 99 // значение для итераторов, должно быть равно (BOARDSIZE - 1)
#define ubyte unsigned char
static unsigned char board[BOARDSIZE][BOARDSIZE];
static unsigned char mouseDown = 0;
static unsigned long long mouseMoves = 0;
static HCURSOR cursor;
void initBoard() {
	for (char iy = 0; iy < BOARDSIZE; iy++) {
		for (char ix = 0; ix < BOARDSIZE; ix++) {
			board[iy][ix] = 0;
		}
	}
}

unsigned int check(ubyte y, ubyte x) { //проверка соседних клеток
    int rt = 0;
    if (board[y][x+1] == 1) {rt++;}
    if (board[y][x-1] == 1) {rt++;}
    if (board[y+1][x] == 1) {rt++;}
    if (board[y-1][x] == 1) {rt++;}
    if (board[y-1][x+1] == 1) {rt++;}
    if (board[y-1][x-1] == 1) {rt++;}
    if (board[y+1][x+1] == 1) {rt++;}
    if (board[y+1][x-1] == 1) {rt++;}
    return rt;
}
void gameStep() {
    unsigned char nboard[BOARDSIZE][BOARDSIZE];
	unsigned int around;
	for (ubyte iy =1; iy<MAX_INDEX; iy++) {
		for (ubyte ix =1; ix<MAX_INDEX; ix++) {
            around = check(iy, ix);
            if (board[iy][ix] == 0) {
                if (around == 3) {
                    nboard[iy][ix] = 1;
                } else {
                    nboard[iy][ix] = 0;
                }
            } else if (board[iy][ix] == 1) {
                if (around < 2 || around > 3) {
                    nboard[iy][ix] = 0;
                } else {
                    nboard[iy][ix] = 1;
                }
            }
		}
	}
    for (char iy = 1; iy < BOARDSIZE; iy++) {
		for (char ix = 1; ix < BOARDSIZE; ix++) {
			board[iy][ix] = nboard[iy][ix];
		}
	}
}





void drawBlocks(HDC mhdc) {
	HBRUSH blockBrush = CreateSolidBrush(RGB(50, 50, 50));
	for (unsigned char y = 1; y < MAX_INDEX; y++) {
			for (unsigned char x = 1; x < MAX_INDEX; x++) {
				if (board[y][x] > 0) {
					RECT nrct = {10*(x - 1), 10*(y - 1), 10*(x - 1)+10, 10*(y - 1)+10};
					FillRect(mhdc, &nrct, blockBrush);
				}
			}
		}
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg)
	{
	case WM_CLOSE:
		DestroyWindow(hwnd);
		ExitProcess((UINT)0);
		break;
	case WM_TIMER:

		break;
	case WM_PAINT:
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
		drawBlocks(hdc);
		EndPaint(hwnd, &ps);
		break;
	case WM_SIZE:
		break;
    case WM_GETMINMAXINFO: //вызывается при установке размеров окна
		MINMAXINFO *mminf = (MINMAXINFO *)lParam;
		POINT min = {BOARDSIZE * 10, BOARDSIZE * 10}, max = {BOARDSIZE * 10, BOARDSIZE * 10};
		mminf->ptMinTrackSize = min;
		mminf->ptMaxTrackSize = max;  
		return 0;
        break;
	case WM_KEYDOWN:
		
		switch (wParam)
		{
		case VK_SPACE:
			gameStep();
			RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_INTERNALPAINT);
			break;
		case VK_RETURN:
			gameStep();
			RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_INTERNALPAINT);
			break;
		case VK_F1:
			MessageBoxEx(hwnd, (LPCTSTR)"ESC - Exit\nR - Clear\nSPACE\\RETURN - Step\nF1 - About\nMADE BY: D.Borunov\n                                                                              ", (LPCSTR)"About", (UINT)MB_OK, 0);
			break;
		case 0x52: //сканкод клавиши "R"
			initBoard();
			RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_INTERNALPAINT);
			break;
		case VK_ESCAPE:
			DestroyWindow(hwnd);
			ExitProcess((UINT)0);
			break;
		default:
			break;
		}
		
		break;
	case WM_LBUTTONDOWN:
		POINT mpt;
		mpt.x = GET_X_LPARAM(lParam);
		mpt.y = GET_Y_LPARAM(lParam);
		if (mpt.x < (BOARDSIZE * 10) && mpt.y < (BOARDSIZE * 10)) {
			board[mpt.y / 10][mpt.x / 10] = 1;
		}
		mouseDown = 1;
		break;
	case WM_MOUSEMOVE:
		if (mouseDown == 1) {
			POINT mpt;
			mpt.x = GET_X_LPARAM(lParam);
			mpt.y = GET_Y_LPARAM(lParam);
			if (mpt.x < (BOARDSIZE * 10) && mpt.y < (BOARDSIZE * 10)) {
				board[mpt.y / 10][mpt.x / 10] = 1;
				mouseMoves++;
				if (mouseMoves % 30 == 0) { //это некий костыль, нужный чтобы уменьшить количество перерисовок окна при рисовании линий а то уж очень сильно мерцало (так как все примитивы рисуются через GDI и никакой вертикалки или чего-то подобного тут вроде нет)
					RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_INTERNALPAINT);
				} 
			}
		}
		break;
	case WM_LBUTTONUP:
		mouseDown = 0;
		mouseMoves = 0;
		RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_INTERNALPAINT);
		break;
	case WM_SETCURSOR:
		SetCursor(cursor);
		break;
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
		break;
	}
}





int WINAPI WinMain(HINSTANCE argHinst, HINSTANCE argPHinst, LPCSTR argCmd, int argCmdShow) {
    WNDCLASS windClass = { 0 };
	windClass.lpfnWndProc = WindowProc;
	windClass.hInstance = argHinst;
	windClass.lpszClassName = (LPCSTR)"GOLW";
	RegisterClass(&windClass);
	HWND hwnd = CreateWindowEx(
		0,
		(LPCSTR)"GOLW",
		(LPCSTR)"game of life",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL,
		NULL,
		argHinst,
		NULL
		);
	if (hwnd == 0) {
		MessageBoxA(NULL, (LPCSTR)"cannot open window", (LPCSTR)"error", MB_ICONERROR);
		return 1;
	}
	cursor  = LoadCursor(NULL, IDC_ARROW);
	ShowWindow(hwnd, argCmdShow);

	initBoard();

	MSG msg;
	while (1) {
		GetMessage(&msg, NULL, 0, 0);
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
    return 0;
}