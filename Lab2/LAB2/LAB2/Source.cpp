#include <windows.h>
#include <stdio.h>
#include"resource2.h"

#define IDM_SYS_ABOUT 1
#define IDM_SYS_HELP 2
#define IDM_SYS_REMOVE 3

struct
{
	int iStyle;
	TCHAR * szText;
}
button[] =
{
	BS_DEFPUSHBUTTON, TEXT("Press Me!"),
	BS_DEFPUSHBUTTON, TEXT("Customize"),
};

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ColorDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
void FillListBox(HWND hwndList);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT("LAB2");
	HACCEL hAccel;
	HMENU hMenu;
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wndclass.hCursor = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_CURSOR1));
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
		TEXT("Lab #2"), // window caption
		WS_OVERLAPPEDWINDOW, // window style
		CW_USEDEFAULT, // initial x position
		CW_USEDEFAULT, // initial y position
		CW_USEDEFAULT, // initial x size
		CW_USEDEFAULT, // initial y size
		NULL, // parent window handle
		NULL, // window menu handle
		hInstance, // program instance handle
		NULL); // creation parameters
	hMenu = GetSystemMenu(hwnd, FALSE);
	AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
	AppendMenu(hMenu, MF_STRING, IDM_SYS_ABOUT, TEXT("About..."));
	AppendMenu(hMenu, MF_STRING, IDM_SYS_HELP, TEXT("Help..."));
	AppendMenu(hMenu, MF_STRING, IDM_SYS_REMOVE, TEXT("Remove Additions"));
	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);
	hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));
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
	static HWND hwndButton[2];
	static HINSTANCE hInstance;
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
	static int color_r = 255, color_g = 255, color_b = 255;
	static int Color[3] = { 0, 0, 0 };
	static int font_size = 46;
	static char str[256]; 
	static char helloMessage[256];
	static HWND dialogWindow = NULL;
	static HWND customizeDialog = NULL;
	HWND editText;
	HWND scrollColor;
	static HICON hIcon;
	static HWND hwndList, hwndText;
	int cxIcon, cyIcon;
	int length;
	//srand(time(NULL));   // should only be called once
	//sprintf_s(str, "Message %f \n", (float) message);
	//OutputDebugString(str);
	switch (message)
	{
	case WM_CREATE:
		GetClientRect(hwnd, &rect);
		cxChar = LOWORD(GetDialogBaseUnits());
		cyChar = HIWORD(GetDialogBaseUnits());
		for (i = 0; i < 2; i++) {
			hwndButton[i] = CreateWindow(TEXT("button"),
				button[i].szText,
				WS_CHILD | WS_VISIBLE | button[i].iStyle,
				(rect.right - 20 * cxChar) / 2,
				(rect.bottom / 2) + cyChar * (1 + 3 * i),
				20 * cxChar,
				7 * cyChar / 4,
				hwnd, (HMENU)i,
				((LPCREATESTRUCT)lParam)->hInstance, NULL);
		}
		hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
		hwndList = CreateWindow(TEXT("listbox"), NULL,
			WS_CHILD | WS_VISIBLE | LBS_STANDARD,
			(rect.right - 20 * cxChar) / 2,
			cyChar * 3,
			cxChar * 18 + GetSystemMetrics(SM_CXVSCROLL),
			cyChar * 10,
			hwnd, (HMENU)44,
			(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),
			NULL);
		hwndText = CreateWindow(TEXT("static"), NULL,
			WS_CHILD | WS_VISIBLE | SS_LEFT,
			cxChar, cyChar,
			GetSystemMetrics(SM_CXSCREEN), cyChar,
			hwnd, (HMENU)45,
			(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),
			NULL);
		FillListBox(hwndList);
		hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
		return 0;
	case WM_PAINT:
		GetClientRect(hwnd, &rect);
		cxIcon = GetSystemMetrics(SM_CXICON);
		cyIcon = GetSystemMetrics(SM_CYICON);
		if (customizeDialog != NULL)
		{
			for (i = 10; i < 13; i++)
			{
				scrollColor = GetDlgItem(customizeDialog, i);
				Color[i - 10] = GetScrollPos(scrollColor, SB_CTL);
			}
		}
		GetClientRect(hwnd, &rect);
		hdc = BeginPaint(hwnd, &ps);
		SetTextColor(hdc, RGB(Color[0], Color[1], Color[2]));
		GetTextExtentPoint32(hdc, helloMessage, strlen(helloMessage), &lp_Size);
		TextOut(hdc, (rect.right - lp_Size.cx) / 2, (rect.bottom - lp_Size.cy) / 2, helloMessage, strlen(helloMessage));
		DrawIcon(hdc, (rect.right - cxIcon) / 2, (rect.bottom - cyIcon) / 2, hIcon);
		EndPaint(hwnd, &ps);
		for (i = 0; i < 2; i++) {
			GetClientRect(hwnd, &rect);
			MoveWindow(hwndButton[i],
				(rect.right - 20 * cxChar) / 2,
				(rect.bottom / 2) + cyChar * (1 + 3 * i),
				20  * cxChar,
				7 * cyChar / 4,
				1);
		}
		return 0;
	case WM_COMMAND:
		GetWindowRect(hwnd, &rect);
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		switch (LOWORD(wParam))
		{
		case 0:
			dialogWindow = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), hwnd, AboutDlgProc);
			break;
		case 1:
			customizeDialog = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG2), hwnd, ColorDlgProc);
			break;
		case 2:
			strcpy_s(helloMessage, "Hi, ");
			editText = GetDlgItem(dialogWindow, 0);
			GetWindowText(editText, str, sizeof(str));
			OutputDebugString(str);
			strcat_s(str, " ! Welcome to the LAB 2 Program");
			strcat_s(helloMessage, str);
			EndDialog(dialogWindow, 0);
			InvalidateRect(hwnd, &rect, TRUE);
			break;
		case 44:
			i = SendMessage(hwndList, LB_GETCURSEL, 0, 0);
			length = SendMessage(hwndList, LB_GETTEXT, i,
				(LPARAM)str);
			OutputDebugString(str);
			strcpy_s(helloMessage, "You clicked on ");
			strcat_s(helloMessage, str);
			InvalidateRect(hwnd, &rect, TRUE);
			break;
		case ID_ACCELERATOR40002:
			rect.left += rect.right / 20;
			rect.right -= rect.right / 20;
			rect.top += rect.bottom / 20;
			rect.bottom -= rect.bottom / 20;
			MoveWindow(hwnd, rect.left, rect.top, rect.right - rect.left,
				rect.bottom - rect.top, TRUE);
			break;
		case ID_ACCELERATOR40001:
			rect.left -= rect.right / 20;
			rect.right += rect.right / 20;
			rect.top -= rect.bottom / 20;
			rect.bottom += rect.bottom / 20;
			MoveWindow(hwnd, rect.left, rect.top, rect.right - rect.left,
				rect.bottom - rect.top, TRUE);
			break;

		}
		return 0;
	case WM_DRAWITEM:
		return 0;
	case WM_SIZE:
		GetClientRect(hwnd, &rect);
		MoveWindow(hwndList,
			(rect.right - 20 * cxChar) / 2,
			rect.bottom / 2 - cyChar * 11,
			cxChar * 18 + GetSystemMetrics(SM_CXVSCROLL),
			cyChar * 10,
			1);
		return 0;
	case WM_SYSCOMMAND:
		GetWindowRect(hwnd, &rect); 
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		switch (LOWORD(wParam))
		{
		case IDM_SYS_ABOUT:
			MessageBox(hwnd, TEXT("LAB #2 on Windows Programming\n")
				TEXT("\n\nYou Should Cry, This is Windows API!"),
				TEXT("LAB2"), MB_OK | MB_ICONINFORMATION);
			return 0;
		case IDM_SYS_HELP:
			MessageBox(hwnd, TEXT("This is LAB at Tehnical University Of Moldova")
				TEXT("\n\n No one will help you"),
				TEXT("LAB2"), MB_OK | MB_ICONINFORMATION);
			return 0;
		case IDM_SYS_REMOVE:
			GetSystemMenu(hwnd, TRUE);
			return 0;
		}
		break;
	case WM_CLOSE:
		//SendMessage(hwnd, WM_COMMAND, 2, lParam);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

BOOL CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HFONT new_font;
	HFONT old_font;
	switch (message) {
	case WM_INITDIALOG:
		SendMessage(hDlg, WM_SETFONT, wParam, lParam);
		return TRUE;
	case  WM_SETFONT:
		/*new_font = CreateFont(100, 0, 0, 0,
			FW_NORMAL, TRUE, FALSE, FALSE,
			ANSI_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_ROMAN,
			"Times New Roman");
		//GetDlgItem(hDlg,0)
		old_font = (HFONT)SelectObject(GetDC(hDlg), new_font);*/
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			SendMessage(GetParent(hDlg), WM_COMMAND, 2, lParam);
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg, 0);
			return	TRUE;
		}
		break;
	} 
	return FALSE;
}

BOOL CALLBACK ColorDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int iColor[3];
	HWND hwndParent, hCtrl;
	int iCtrlID, iIndex;
	switch (message)
	{
	case WM_INITDIALOG:
		for (iCtrlID = 10; iCtrlID < 13; iCtrlID++)
		{
			hCtrl = GetDlgItem(hDlg, iCtrlID);
			SetScrollRange(hCtrl, SB_CTL, 0, 255, FALSE);
			SetScrollPos(hCtrl, SB_CTL, 0, FALSE);
		}
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			EndDialog(hDlg, 0);
			return TRUE;
		}
		break;
	case WM_VSCROLL:
		hCtrl = (HWND)lParam;
		iCtrlID = GetWindowLong(hCtrl, GWL_ID);
		iIndex = iCtrlID - 10;
		hwndParent = GetParent(hDlg);
		switch (LOWORD(wParam))
		{
		case SB_PAGEDOWN:
			iColor[iIndex] += 15; // fall through
		case SB_LINEDOWN:
			iColor[iIndex] = min(255, iColor[iIndex] + 1);
			break;
		case SB_PAGEUP:
			iColor[iIndex] -= 15; // fall through
		case SB_LINEUP:
			iColor[iIndex] = max(0, iColor[iIndex] - 1);
			break;
		case SB_TOP:
			iColor[iIndex] = 0;
			break;
		case SB_BOTTOM:
			iColor[iIndex] = 255;
			break;
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			iColor[iIndex] = HIWORD(wParam);
			break;
		default:
			return FALSE;
		}
		SetScrollPos(hCtrl, SB_CTL, iColor[iIndex], TRUE);
		SetDlgItemInt(hDlg, iCtrlID + 3, iColor[iIndex], FALSE);
		/*DeleteObject((HGDIOBJ)SetClassLong(hwndParent, GCL_HBRBACKGROUND,
			(LONG)CreateSolidBrush(
			RGB(iColor[0], iColor[1], iColor[2]))));*/
		InvalidateRect(hwndParent, NULL, TRUE);
		return TRUE;
	}
	return FALSE;
}

void FillListBox(HWND hwndList)
{
	int i;
	TCHAR str[256];
	for (i = 0; i < 20; ++i)
	{
		sprintf_s(str, "List Element #%d", i + 1);
		SendMessage(hwndList, LB_INSERTSTRING, i, (LPARAM)str);
	}
	//TCHAR element[] = TEXT("Laboratory Work #2");
	
}