#include <windows.h>
#include <math.h>
#include <time.h>
#define NUM 1000
#define TWOPI (2 * 3.14159)
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HBRUSH CreateGradientBrush(COLORREF top, COLORREF bottom, HDC hdc, RECT rc);
HPEN * CreatePens(HWND hwnd, int delta);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT("SineWave");
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
	hwnd = CreateWindow(szAppName, TEXT("Sine Wave Using Polyline"),
		WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);
	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);
	hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(113));
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
	int i;
	static int j = 0;
	PAINTSTRUCT ps;
	POINT apt[NUM];
	static RECT rect;
	static HPEN pens[5];
	int color_r = 255, color_g, color_b;
	static POINT bezier1[4], bezier2[4];
	static POINT polygon[3];
	static POINT circleCenter, squareCenter;
	static RECT circleRect, square;
	static int rePaint = 0;
	static HDC memoryHDC;
	static HBITMAP bitmapMem;
	static HINSTANCE hInstance;
	BITMAP bitmap;
	static TCHAR quote[] = TEXT("FAF");
	SIZE  lp_Size;
	static HBRUSH selectbrush = NULL;
	static HPEN circlePen;
	static HBRUSH squareBrush;
	static int changeCirclePen = 0, chnageSquareBrush = 0, drawBezier = 0, delta = 0, erase = 0;
	LOGPEN logpen;
	srand(time(NULL));
	switch (message)
	{
	case WM_CREATE:
		GetClientRect(hwnd, &rect);
		for (i = 0; i < 5; i++)
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
		SetTextColor(memoryHDC, RGB(255,255,255));
		SetBkColor(memoryHDC, RGB(0,0,0));
		TextOut(memoryHDC, (bitmap.bmWidth - lp_Size.cx) / 2, (bitmap.bmHeight - lp_Size.cy) / 2, quote, strlen(quote));
		SetScrollRange(hwnd, SB_VERT, 0 ,10, TRUE);
		SetScrollPos(hwnd, SB_VERT, 0, TRUE);
		ReleaseDC(hwnd, hdc);
		circlePen = CreatePen(PS_SOLID,1,RGB(0,0,0));
		squareBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
		return 0;
	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		bezier1[0].x = cxClient / 4; bezier2[0].x = cxClient / 2;
		bezier1[0].y = cyClient / 2; bezier2[0].y = 3 * cyClient / 4;
		bezier1[1].x = cxClient / 2; bezier2[1].x = cxClient / 4;
		bezier1[1].y = cyClient / 4; bezier2[1].y = cyClient ;
		bezier1[2].x = cxClient / 2; bezier2[2].x = cxClient / 4;
		bezier1[2].y = 3 * cyClient / 4; bezier2[2].y = 3 * cyClient / 4;
		bezier1[3].x = 3 * cxClient / 4; bezier2[3].x = 4 * cxClient / 10;
		bezier1[3].y = cyClient / 2; bezier2[3].y = 3 * cyClient / 4;

		polygon[0].x = 0.2 * cxClient; polygon[0].y = 0.5 * cyClient;
		polygon[1].x = 0.15 * cxClient; polygon[1].y = 0.5  * cyClient + 0.1 * cxClient;
		polygon[2].x = 0.25 * cxClient; polygon[2].y = 0.5  * cyClient + 0.1 * cxClient;
		return 0;
	case WM_PAINT:
		GetClientRect(hwnd, &rect);
		hdc = BeginPaint(hwnd, &ps);
		PolyBezier(hdc, bezier1, 4);
		PolyBezier(hdc, bezier2, 4);
		Ellipse(hdc, 0.02 * cxClient, 0.25 * cyClient, 0.12 * cxClient , 0.25  * cyClient  + 0.1 * cxClient);
		SelectObject(hdc, pens[2]);
		Rectangle(hdc, 0.15 * cxClient, 0.25 * cyClient, 0.25 * cxClient, 0.25  * cyClient + 0.1 * cxClient);
		SelectObject(hdc, pens[0]);
		selectbrush = CreateGradientBrush(RGB(180, 0, 0), RGB(255, 180, 0), hdc, rect);
		SelectObject(hdc, selectbrush);
		Pie(hdc, 0.02 * cxClient, 0.5 * cyClient, 0.12 * cxClient, 0.5  * cyClient + 0.1 * cxClient, 0.07 * cxClient, 0.55 * cyClient, 0.035 * cxClient, 0.5  * cyClient + 0.07 * cxClient);
		SelectObject(hdc, GetStockObject(BLACK_PEN));
		selectbrush = CreateGradientBrush(RGB(0, 180, 0), RGB(255, 180, 0), hdc, rect);
		SelectObject(hdc, selectbrush);
		Polygon(hdc, polygon, 3);
		for (i = 0; i < 5; i++)
		{
			SelectObject(hdc, pens[i]);
			MoveToEx(hdc, 0 - delta, (i * (25 + (delta / 5))) - delta, NULL);
			LineTo(hdc, cxClient + delta, (i * (25 + (delta / 5))) - delta);
		}
		BitBlt(hdc, 0.65 * cxClient, 0.6 * cyClient, 0.45 * cxClient, 0.5 * cyClient, memoryHDC, 0, 0, SRCCOPY);
		EndPaint(hwnd, &ps);
		return 0;
	case WM_LBUTTONDOWN:
		circleCenter.x = LOWORD(lParam);
		circleCenter.y = HIWORD(lParam);
		return 0;
	case WM_RBUTTONDOWN:
		squareCenter.x = LOWORD(lParam);
		squareCenter.y = HIWORD(lParam);
		return 0;
	case WM_MOUSEMOVE:
		GetClientRect(hwnd, &rect); 
		if (wParam & MK_LBUTTON || wParam & MK_RBUTTON)
		{
			hdc = GetDC(hwnd);
			/*memoryHDC = CreateCompatibleDC(hdc);
			bitmapMem = CreateCompatibleBitmap(hdc, cxClient, cyClient);
			SelectObject(memoryHDC, bitmapMem);*/
			if (drawBezier)
			{
				SelectObject(hdc, GetStockObject(WHITE_PEN));
				PolyBezier(hdc, bezier1, 4);
				if (wParam & MK_LBUTTON)
				{
					bezier1[1].x = LOWORD(lParam);
					bezier1[1].y = HIWORD(lParam);
				}
				if (wParam & MK_RBUTTON)
				{
					bezier1[2].x = LOWORD(lParam);
					bezier1[2].y = HIWORD(lParam);
				}
				SelectObject(hdc, GetStockObject(BLACK_PEN));
				PolyBezier(hdc, bezier1, 4);
			}
			else
			{
				if (erase && (wParam & MK_LBUTTON))
				{
					SelectObject(hdc, GetStockObject(WHITE_BRUSH));
					SelectObject(hdc, GetStockObject(NULL_PEN));
					circleRect.left = circleCenter.x -20;
					circleRect.right = circleCenter.x + 20;
					circleRect.bottom = circleCenter.y + 20;
					circleRect.top = circleCenter.y -20;
					Rectangle(hdc, circleRect.left, circleRect.top, circleRect.right, circleRect.bottom);
					circleCenter.x = LOWORD(lParam);
					circleCenter.y = HIWORD(lParam);
				}
				else
				{
					if (wParam & MK_LBUTTON)
					{
						SelectObject(hdc, GetStockObject(NULL_BRUSH));
						SelectObject(hdc, circlePen);
						//InvalidateRect(hwnd, &circleRect, TRUE);
						//UpdateWindow(hwnd);
						RedrawWindow(hwnd, &circleRect, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
						if (LOWORD(lParam) < HIWORD(lParam))
						{
							circleRect.left = circleCenter.x - (HIWORD(lParam) - circleCenter.y);
							circleRect.right = circleCenter.x + (HIWORD(lParam) - circleCenter.y);
							circleRect.bottom = HIWORD(lParam);
							circleRect.top = 2 * circleCenter.y - HIWORD(lParam);
						}
						else
						{
							circleRect.left = 2 * circleCenter.x - LOWORD(lParam);
							circleRect.right = LOWORD(lParam);
							circleRect.bottom = circleCenter.y + (LOWORD(lParam) - circleCenter.x);
							circleRect.top = circleCenter.y - (LOWORD(lParam) - circleCenter.x);
						}
						Ellipse(hdc, circleRect.left, circleRect.top, circleRect.right, circleRect.bottom);
						//BitBlt(hdc, circleRect.left, circleRect.top, circleRect.right - circleRect.left, circleRect.bottom - circleRect.top, memoryHDC, circleRect.left, circleRect.top, SRCCOPY);
					}
					if (wParam & MK_RBUTTON)
					{
						SelectObject(hdc, squareBrush);
						SelectObject(hdc, GetStockObject(BLACK_PEN));
						RedrawWindow(hwnd, &square, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
						if (LOWORD(lParam) < HIWORD(lParam))
						{
							square.left = squareCenter.x - (HIWORD(lParam) - squareCenter.y);
							square.right = squareCenter.x + (HIWORD(lParam) - squareCenter.y);
							square.bottom = HIWORD(lParam);
							square.top = 2 * squareCenter.y - HIWORD(lParam);
						}
						else
						{
							square.left = 2 * squareCenter.x - LOWORD(lParam);
							square.right = LOWORD(lParam);
							square.bottom = squareCenter.y + (LOWORD(lParam) - squareCenter.x);
							square.top = squareCenter.y - (LOWORD(lParam) - squareCenter.x);
						}
						Rectangle(hdc, square.left, square.top, square.right, square.bottom);
					}
				}
			}
			ReleaseDC(hwnd, hdc);
		}
		return 0;
	case WM_COMMAND:
		GetClientRect(hwnd, &rect);
		switch (LOWORD(wParam))
		{
		case 40001:
			if (!changeCirclePen)
			{
				circlePen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
				changeCirclePen = 1;
			}
			else
			{
				circlePen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
				changeCirclePen = 0;
			}
			break;
		case 40003:
			hdc = GetDC(hwnd);
			GetClientRect(hwnd, &rect);
			if (!chnageSquareBrush)
			{
				squareBrush = CreateGradientBrush(RGB(rand() % 256, rand() % 256, rand() % 256), RGB(rand() % 256, rand() % 256, rand() % 256), hdc, rect);
				chnageSquareBrush = 1;
			}
			else
			{
				squareBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
				chnageSquareBrush = 0;
			}
			break;
		case 40005:
			if (!drawBezier)
			{
				drawBezier = 1;
			}
			else
			{
				drawBezier = 0;
			}
			break;
		case 40007:
			delta += 25;
			for (i = 0; i < 5; i++)
			{
				GetObject(pens[i], sizeof (LOGPEN), (LPVOID)&logpen);
				pens[i] = CreatePen(PS_SOLID, (i + 1) * 5 + (delta / 5), logpen.lopnColor);
			}
			InvalidateRect(hwnd,&rect,TRUE);
			break;
		case 40008:
			if (!erase)
			{
				erase = 1;
			}
			else
			{
				erase = 0;
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

HPEN * CreatePens(HWND hwnd, int delta)
{
	static HPEN pens[5];
	int i;

	srand(time(NULL));
	for (i = 0; i < 5; i++)
	{
		pens[i] = CreatePen(PS_SOLID, (i + 1) * 5 + (delta / 5), RGB(rand() % 256, rand() % 256, rand() % 256));
	}

	return pens;
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