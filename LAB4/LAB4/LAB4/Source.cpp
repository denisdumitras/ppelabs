#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "Source1.cpp"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HBRUSH CreateGradientBrush(COLORREF top, COLORREF bottom, HDC hdc, RECT rc);
void updateMargins(FIGUREINFO *figure);
void UpdateFigures(FIGUREINFO *figures, int figures_amount, int MOVE_DELTA, RECT* rect);
int checkCollision(FIGUREINFO **figures, int *figures_amount, HBRUSH **figureBrushes, RECT rect, int prev_colision);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT("LAB4");
	HWND hwnd;
	HACCEL hAccel;
	MSG msg;
	WNDCLASS wndclass;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;
	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("Program requires Windows NT!"),
			szAppName, MB_ICONERROR);
		return 0;
	}
	hwnd = CreateWindow(szAppName, TEXT("LAB4"),
		WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);
	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);
	hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(101));
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(hwnd, hAccel, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int cxClient, cyClient;
	HDC hdc;
	static FIGUREINFO *figures;
	static int figures_amount = 3;
	long x_Margin, y_Margin;
	int i;
	static int j = 0;
	int width = 30, height = 30;
	static int MOVE_DELTA = 2;
	const int ID_TIMER = 1;
	char debug[50];
	static int colision = 0;
	static int velocity = 10;
	HDC hdcBuff;
	PAINTSTRUCT ps;
	//POINT apt[NUM];
	static RECT rect, redrawRect;
	static HPEN pens[5];
	int color_r = 255, color_g, color_b;
	static POINT bezier1[4], bezier2[4];
	static POINT polygon[3];
	static POINT circleCenter, squareCenter;
	static RECT circleRect, square;
	static int rePaint = 0;
	static HDC memoryHDC;
	static HBITMAP bitmapBuff;
	HANDLE oldBuff;
	static HINSTANCE hInstance;
	BITMAP bitmap;
	static TCHAR quote[] = TEXT("FAF");
	SIZE  lp_Size;
	static HBRUSH selectbrush = NULL;
	static HPEN circlePen;
	static HBRUSH *figureBrushes;
	static int changeCirclePen = 0, chnageSquareBrush = 0, drawBezier = 0, delta = 0, erase = 0;
	LOGPEN logpen;
	srand(time(NULL));
	switch (message)
	{
	case WM_CREATE:
		GetClientRect(hwnd, &rect);
		figures = (FIGUREINFO*)malloc(sizeof(FIGUREINFO)* figures_amount);
		figureBrushes = (HBRUSH*)malloc(sizeof(HBRUSH)* figures_amount);
		x_Margin = rect.right - width;
		y_Margin = rect.bottom - height;
		hdc = GetDC(hwnd);
		for (i = 0; i < figures_amount; ++i)
		{	
			figures[i].width = width;
			figures[i].height = height;
			figures[i].x = (rand() % x_Margin) + width / 2;
			figures[i].y = (rand() % y_Margin) + height / 2;
			figures[i].dx = MOVE_DELTA;
			figures[i].dy = MOVE_DELTA;
			updateMargins(&figures[i]);
			figureBrushes[i] = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));//CreateGradientBrush(RGB(rand() % 256, rand() % 256, rand() % 256), RGB(rand() % 256, rand() % 256, rand() % 256), hdc, redrawRect);
			figures[i].figure_type = 0;
		}
		if (SetTimer(hwnd, ID_TIMER, 10, NULL) == 0)
		{
			MessageBox(hwnd, "Could not SetTimer()!", "Error", MB_OK | MB_ICONEXCLAMATION);
		}
		MOVE_DELTA = 0;
		ReleaseDC(hwnd, hdc);
		/*for (i = 0; i < 5; i++)
		{
			color_r = rand() % 256;
			color_g = rand() % 256;
			color_b = rand() % 256;
			pens[i] = CreatePen(PS_SOLID, (i + 1) * 5 + (delta / 5), RGB(color_r, color_g, color_b));
		}
		hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
		hdc = GetDC(hwnd);
		memoryHDC = CreateCompatibleDC(hdc);
		bitmapMem = LoadBitmap(hInstance, MAKEINTRESOURCE(112));
		SelectObject(memoryHDC, bitmapMem);
		GetObject(bitmapMem, sizeof(bitmap), &bitmap);
		GetTextExtentPoint32(memoryHDC, quote, strlen(quote), &lp_Size);
		SetTextColor(memoryHDC, RGB(255, 255, 255));
		SetBkColor(memoryHDC, RGB(0, 0, 0));
		TextOut(memoryHDC, (bitmap.bmWidth - lp_Size.cx) / 2, (bitmap.bmHeight - lp_Size.cy) / 2, quote, strlen(quote));
		SetScrollRange(hwnd, SB_VERT, 0, 10, TRUE);
		SetScrollPos(hwnd, SB_VERT, 0, TRUE);
		ReleaseDC(hwnd, hdc);
		circlePen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
		squareBrush = (HBRUSH)GetStockObject(NULL_BRUSH);*/
		return 0;
	case WM_PAINT:
		GetClientRect(hwnd, &rect);
		hdc = BeginPaint(hwnd, &ps);
		hdcBuff = CreateCompatibleDC(hdc);
		bitmapBuff = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
		oldBuff = SelectObject(hdcBuff, bitmapBuff);
		memoryHDC = CreateCompatibleDC(hdc);
		FillRect(hdcBuff, &rect, (HBRUSH) GetStockObject(WHITE_BRUSH));
		//BitBlt(hdcBuff, g_ballInfo.x, g_ballInfo.y, g_ballInfo.width, g_ballInfo.height, hdcMem, 0, 0, SRCPAINT);
		for (i = 0; i < figures_amount; i++)
		{
			SelectObject(hdc, figureBrushes[i]);
			sprintf_s(debug, "x%d=%d y%d=%d colision %d\n", i, figures[i].x, i, figures[i].y, colision);
			OutputDebugString(debug);
			if (figures[i].figure_type == 0)
			{
				Ellipse(hdcBuff, figures[i].left, figures[i].top, figures[i].right, figures[i].bottom);
			}
			else
			{
				Rectangle(hdcBuff, figures[i].left, figures[i].top, figures[i].right, figures[i].bottom);
			}
			
		}
		//BitBlt(hdc, 0.65 * cxClient, 0.6 * cyClient, 0.45 * cxClient, 0.5 * cyClient, memoryHDC, 0, 0, SRCCOPY);
		BitBlt(hdc, 0, 0, rect.right, rect.bottom, hdcBuff, 0, 0, SRCCOPY);
		SelectObject(hdcBuff, oldBuff);
		DeleteObject(hdcBuff);
		DeleteDC(hdcBuff);
		EndPaint(hwnd, &ps);
		return 0;
	case WM_TIMER:
		hdc = GetDC(hwnd);

		GetClientRect(hwnd, &rect);

		UpdateFigures(figures, figures_amount, MOVE_DELTA, &rect);
		colision = checkCollision(&figures, &figures_amount, &figureBrushes,rect, colision);
		//SendMessage(hwnd, WM_PAINT, wParam, lParam);
		RedrawWindow(hwnd, &rect, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
		ReleaseDC(hwnd, hdc);
		return 0;
	case WM_COMMAND:
		GetClientRect(hwnd, &rect);
		switch (LOWORD(wParam))
		{
		case 40001:
			for (i = 0; i < figures_amount; i++)
			{
				if (figures[i].dx > 0)
				{ figures[i].dx += 2; }
				else { figures[i].dx -= 2; }
				if (figures[i].dy > 0)
				{ figures[i].dy += 2; }
				else { figures[i].dy -= 2; }
			}

			/*KillTimer(hwnd, ID_TIMER);
			velocity -= 10;
			SetTimer(hwnd, ID_TIMER, velocity, NULL);*/
			break;
		case 40002:
			for (i = 0; i < figures_amount; i++)
			{
				if (figures[i].dx > 3)
				{
					figures[i].dx -= 2;
				}
				else if (figures[i].dx < -3)
				{ figures[i].dx += 2; }
				if (figures[i].dy > 3)
				{
					figures[i].dy -= 2;
				}
				else if (figures[i].dy < -3)
				{ figures[i].dy += 2; }
			}
			break;
		}
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

HBRUSH CreateGradientBrush(COLORREF top, COLORREF bottom, HDC hdc, RECT rc)
{
	HBRUSH Brush = NULL;
	HDC hdcmem = CreateCompatibleDC(hdc);
	HBITMAP hbitmap = CreateCompatibleBitmap(hdc, rc.right - rc.left, rc.bottom - rc.top);
	SelectObject(hdcmem, hbitmap);

	int r1 = GetRValue(top), r2 = GetRValue(bottom), g1 = GetGValue(top), g2 = GetGValue(bottom), b1 = GetBValue(top), b2 = GetBValue(bottom);
	for (int i = 0; i < rc.bottom - rc.top; i++)
	{
		RECT temp;
		int r, g, b;
		r = int(r1 + double(i * (r2 - r1) / rc.bottom - rc.top));
		g = int(g1 + double(i * (g2 - g1) / rc.bottom - rc.top));
		b = int(b1 + double(i * (b2 - b1) / rc.bottom - rc.top));
		Brush = CreateSolidBrush(RGB(r, g, b));
		temp.left = 0;
		temp.top = i;
		temp.right = rc.right - rc.left;
		temp.bottom = i + 1;
		FillRect(hdcmem, &temp, Brush);
		DeleteObject(Brush);
	}
	HBRUSH pattern = CreatePatternBrush(hbitmap);

	DeleteDC(hdcmem);
	DeleteObject(Brush);
	DeleteObject(hbitmap);

	return pattern;
}

void updateMargins(FIGUREINFO *figure)
{
	long x_Margin = figure->width / 2;
	long y_Margin = figure->height / 2;
	figure->left = figure->x - x_Margin;
	figure->right = figure->x + x_Margin;
	figure->bottom = figure->y - y_Margin;
	figure->top = figure->y + y_Margin;
}

void UpdateFigures(FIGUREINFO *figures,int figures_amount,int MOVE_DELTA,RECT* rect)
{
	int i,j;
	for (i = 0; i < figures_amount; i++)
	{
		figures[i].x += figures[i].dx;
		figures[i].y += figures[i].dy;

		if (figures[i].x < 0)
		{
			figures[i].x = 0;
			figures[i].dx = -figures[i].dx;
			figures[i].figure_type = !figures[i].figure_type;
		}
		else if (figures[i].x + figures[i].width > rect->right)
		{
			figures[i].x = rect->right - figures[i].width;
			figures[i].dx = -figures[i].dx;
			figures[i].figure_type = !figures[i].figure_type;
		}

		if (figures[i].y < 0)
		{
			figures[i].y = 0;
			figures[i].dy = -(figures[i].dy - 1) + MOVE_DELTA;
		}
		else if (figures[i].y + figures[i].height > rect->bottom)
		{
			figures[i].y = rect->bottom - figures[i].height;
			figures[i].dy = -(figures[i].dy + 1) - MOVE_DELTA;
		}

		updateMargins(&figures[i]);
	}
}

double getDistance(long x1, long y1, long x2, long y2)
{
	long x_distance, y_distance;
	x_distance = pow((double) abs(x1 - x2),2);
	y_distance = pow((double)abs(y1 - y2), 2);
	return sqrt((double)(x_distance + y_distance));
}

int checkCollision(FIGUREINFO **figures, int *figures_amount, HBRUSH **figureBrushes, RECT rect, int prev_colision)
{
	int i, j, newItem, x_Margin, y_Margin, size, colision = 0;
	size = *figures_amount;
	for (i = 0; i < size; i++)
	{
		for (j = 0; j < size; j++)
		{
			if (i == j) { continue; }
			if (getDistance((*figures)[i].x, (*figures)[i].y, (*figures)[j].x, (*figures)[j].y) <= (*figures)[i].width 
				&& (*figures)[i].figure_type == (*figures)[j].figure_type)
			{
				colision = 1;
				if (prev_colision == 0)
				{
					x_Margin = rect.right - (*figures)[i].width;
					y_Margin = rect.bottom - (*figures)[i].height;
					(*figureBrushes)[i] = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));
					(*figureBrushes)[j] = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));
					*figures_amount = (*figures_amount) + 1;
					(*figures) = (FIGUREINFO*)realloc(*figures, sizeof(FIGUREINFO)* (*figures_amount));
					(*figureBrushes) = (HBRUSH*)realloc(*figureBrushes, sizeof(HBRUSH)* (*figures_amount));
					newItem = (*figures_amount) - 1;
					(*figures)[newItem].width = (*figures)[i].width;
					(*figures)[newItem].height = (*figures)[i].height;
					(*figures)[newItem].x = (rand() % x_Margin) + (*figures)[i].width / 2;
					(*figures)[newItem].y = (rand() % y_Margin) + (*figures)[i].height / 2;
					(*figures)[newItem].dx = (*figures)[i].dx;
					(*figures)[newItem].dy = (*figures)[i].dy;
					updateMargins(&(*figures)[newItem]);
					(*figureBrushes)[newItem] = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));
					(*figures)[newItem].figure_type = (*figures)[i].figure_type;


					(*figures)[i].dx = -(*figures)[i].dx;
					(*figures)[j].dx = -(*figures)[j].dx;
					(*figures)[i].dy = -(*figures)[i].dy;
					(*figures)[j].dy = -(*figures)[j].dy;
				}
			}
		}
	}

	return colision;
}