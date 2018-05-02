#include <windows.h>
#include <Commctrl.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include"Source1.cpp"

struct
{
	int iStyle;
	TCHAR * szText;
}
button[] =
{
	BS_DEFPUSHBUTTON, TEXT("Default"),
	BS_OWNERDRAW, TEXT("Custom"),
};
#define NUM (sizeof button / sizeof button[0])

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HBRUSH CreateGradientBrush(COLORREF top, COLORREF bottom, HDC hdc, RECT rc);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT("HelloWin");
	HWND hwnd;
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
		MessageBox(NULL, TEXT("This program requires Windows NT!"),
			szAppName, MB_ICONERROR);
		return 0;
	}
	hwnd = CreateWindow(szAppName, // window class name
		TEXT("Lab #1"), // window caption
		WS_OVERLAPPEDWINDOW, // window style
		CW_USEDEFAULT, // initial x position
		CW_USEDEFAULT, // initial y position
		CW_USEDEFAULT, // initial x size
		CW_USEDEFAULT, // initial y size
		NULL, // parent window handle
		NULL, // window menu handle
		hInstance, // program instance handle
		NULL); // creation parameters
	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hwndButton[NUM];
	static int cxChar, cyChar;
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rect;
	int i;
	HBRUSH selectbrush = NULL;
	LPDRAWITEMSTRUCT draw_struct;
	HFONT new_font;
	HFONT old_font;
	SIZE  lp_Size;
	char *text_element;
	int cxClient, cyClient;
	static int random_color = 255;
	int random_r, random_g, random_b;
	static int color_r = 255, color_g, color_b;
	static int font_size = 46;

	srand(time(NULL));   // should only be called once
	
	switch (message)
	{
	case WM_CREATE:
		GetClientRect(hwnd, &rect);
		cxChar = LOWORD(GetDialogBaseUnits());
		cyChar = HIWORD(GetDialogBaseUnits());
		for (i = 0; i < NUM; i++) {
			hwndButton[i] = CreateWindow(TEXT("button"),
				button[i].szText,
				WS_CHILD | WS_VISIBLE | button[i].iStyle,
				(rect.right - (20 + (i * 10)) * cxChar) / 2,
				(rect.bottom / 2) + cyChar * (1 + 2 * i),
				(20 + (i * 10)) * cxChar,
				7 * (cyChar * (i + 1)) / 4,
				hwnd, (HMENU)i,
				((LPCREATESTRUCT)lParam)->hInstance, NULL);
		}
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		GetClientRect(hwnd, &rect);
		DrawText(hdc, TEXT("Hello world!"), -1, &rect,
			DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		SetTextColor(hdc, RGB(color_r, color_g, color_b));
		SetBkColor(hdc, RGB(0, 0, 0));
		new_font = CreateFont(font_size, 0, 0, 0,
			FW_NORMAL, TRUE, FALSE, FALSE,
			ANSI_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_ROMAN,
			"Times New Roman");
		old_font = (HFONT)SelectObject(hdc, new_font);
		text_element = TEXT("Text Element");
		GetTextExtentPoint32(hdc, text_element, strlen(text_element), &lp_Size);
		//DrawText(hdc, TEXT("Text Element"), -1, &rect, DT_LEFT | DT_TOP);
		TextOut(hdc, (rect.right - lp_Size.cx) / 2, (rect.bottom - lp_Size.cy) / 3, text_element, strlen(text_element));
		(HFONT)SelectObject(hdc, old_font);
		EndPaint(hwnd, &ps);
		for (i = 0; i < NUM; i++) {
			GetClientRect(hwnd, &rect);
			MoveWindow(hwndButton[i],
				(rect.right - (20 + (i * 10)) * cxChar) / 2,
				(rect.bottom / 2) + cyChar * (1 + 2 * i),
				(20 + (i * 10)) * cxChar,
				7 * (cyChar * (i + 1)) / 4,
				1);
		}
		return 0;
	case WM_COMMAND:
		GetWindowRect(hwnd, &rect);
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);

		switch (wParam)
		{
		case 0:
			rect.left += cxClient / 20;
			rect.right -= cxClient / 20;
			rect.top += cyClient / 20;
			rect.bottom -= cyClient / 20;
			MoveWindow(hwnd, rect.left, rect.top, rect.right - rect.left,
				rect.bottom - rect.top, TRUE);

			break;
		case 1:
			font_size++;
			GetClientRect(hwnd, &rect);
			InvalidateRect(hwnd, &rect, TRUE);
			break;
		case 2:
			GetClientRect(hwnd, &rect);
			random_r = rand() % 256;
			random_g = rand() % 256;
			random_b = rand() % 256;
			//random_color = random_int;
			color_r = random_r;
			color_g = random_g;
			color_b = random_b;
			GetClientRect(hwnd, &rect);
			InvalidateRect(hwnd, &rect, TRUE);
			break;
		case 3:
			rect.left -= cxClient / 20;
			rect.right += cxClient / 20;
			rect.top -= cyClient / 20;
			rect.bottom += cyClient / 20;
			MoveWindow(hwnd, rect.left, rect.top, rect.right - rect.left,
				rect.bottom - rect.top, TRUE);
			break;
		}



		return 0;
	case WM_DRAWITEM:
		draw_struct = (LPDRAWITEMSTRUCT)lParam;
		selectbrush = CreateGradientBrush(RGB(180, 0, 0), RGB(255, 180, 0), draw_struct->hDC, draw_struct->rcItem);
		//RECT some_rect = (RECT)some_item->rcItem;
		FillRect(draw_struct->hDC, &draw_struct->rcItem,
			selectbrush);
		FrameRect(draw_struct->hDC, &draw_struct->rcItem,
			(HBRUSH)GetStockObject(BLACK_BRUSH));
		SetTextColor(draw_struct->hDC, RGB(0, 0, 255));
		SetBkMode(draw_struct->hDC, TRANSPARENT);
		new_font = CreateFont(48, 0, 0, 0, FW_DONTCARE, FALSE, TRUE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
			CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Impact"));
		old_font = (HFONT)SelectObject(draw_struct->hDC, new_font);
		DrawText(draw_struct->hDC, TEXT("Custom"), -1, &draw_struct->rcItem, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		SelectObject(draw_struct->hDC, old_font);
		DeleteObject(new_font);
		return 0;
	case WM_SIZE:
		if (wParam == 100)
		{
			GetWindowRect(hwnd, &rect);
			cxClient = LOWORD(lParam);
			cyClient = HIWORD(lParam);

			rect.left -= cxClient / 20;
			rect.right += cxClient / 20;
			rect.top -= cyClient / 20;
			rect.bottom += cyClient / 20;

			MoveWindow(hwnd, rect.left, rect.top, rect.right - rect.left,
				rect.bottom - rect.top, TRUE);
		}
		return 0;
	case WM_CLOSE:
		SendMessage(hwnd, WM_COMMAND, 2, lParam);
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