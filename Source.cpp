#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#pragma comment(lib,"comctl32")
#pragma comment(lib,"gdiplus")

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <gdiplus.h>
#include "resource.h"

TCHAR szClassName[] = TEXT("Window");

Gdiplus::Bitmap* LoadBitmapFromResource(LPCTSTR pName, LPCTSTR pType, HMODULE hInst)
{
	Gdiplus::Bitmap* pImage = NULL;
	const HRSRC hResource = FindResource(hInst, pName, pType);
	if (!hResource)
		return NULL;
	const DWORD imageSize = SizeofResource(hInst, hResource);
	if (!imageSize)
		return NULL;
	const void* pResourceData = LockResource(::LoadResource(hInst,
		hResource));
	if (!pResourceData)
		return NULL;
	const HGLOBAL hBuffer = GlobalAlloc(GMEM_MOVEABLE, imageSize);
	if (hBuffer)
	{
		void* pBuffer = GlobalLock(hBuffer);
		if (pBuffer)
		{
			CopyMemory(pBuffer, pResourceData, imageSize);
			IStream* pStream = NULL;
			if (CreateStreamOnHGlobal(hBuffer, TRUE, &pStream) == S_OK)
			{
				pImage = Gdiplus::Bitmap::FromStream(pStream);
				pStream->Release();
				if (pImage)
				{
					if (pImage->GetLastStatus() != Gdiplus::Ok)
					{
						delete pImage;
						pImage = NULL;
					}
				}
			}
			GlobalUnlock(hBuffer);
		}
	}
	return pImage;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hEdit;
	static HWND hButton1;
	static HWND hButton2;
	static HWND hButton3;
	static HWND hButton4;
	static HWND hButton5;
	static HWND hButton6;
	static HWND hButton7;
	static HWND hButton8;
	static HWND hButton9;
	static HWND hScaleTrack;
	static HBITMAP hBitmap;
	static HDC hdcBitmap;
	static BOOL bMouseDown;
	static BOOL bTouchDown;
	static INT x;
	static INT y;
	static INT nSrcWidth;
	static INT nSrcHeight;
	static INT nDstWidth;
	static INT nDstHeight;
	static COLORREF color;
	static HPEN hPen;
	static HPEN hOldPen;
	static INT_PTR nScaleValue = 100;
	static Gdiplus::Bitmap *pBitmapBorderRight;
	static Gdiplus::Bitmap *pBitmapBorderBottom;
	static Gdiplus::Bitmap *pBitmapBorderCorner;
	switch (msg)
	{
	case WM_CREATE:
		InitCommonControls();
		hButton1 = CreateWindow(TEXT("BUTTON"), TEXT("クリア"), WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, (HMENU)100, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hButton2 = CreateWindow(TEXT("BUTTON"), TEXT("保存"), WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, (HMENU)101, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hButton3 = CreateWindow(TEXT("BUTTON"), TEXT("黒"), WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | BS_PUSHLIKE, 0, 0, 0, 0, hWnd, (HMENU)102, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hButton4 = CreateWindow(TEXT("BUTTON"), TEXT("赤"), WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | BS_PUSHLIKE, 0, 0, 0, 0, hWnd, (HMENU)103, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hButton5 = CreateWindow(TEXT("BUTTON"), TEXT("緑"), WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | BS_PUSHLIKE, 0, 0, 0, 0, hWnd, (HMENU)104, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hButton6 = CreateWindow(TEXT("BUTTON"), TEXT("青"), WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | BS_PUSHLIKE, 0, 0, 0, 0, hWnd, (HMENU)105, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hButton7 = CreateWindow(TEXT("BUTTON"), TEXT("灰"), WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | BS_PUSHLIKE, 0, 0, 0, 0, hWnd, (HMENU)106, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hButton8 = CreateWindow(TEXT("BUTTON"), TEXT("ピンク"), WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | BS_PUSHLIKE, 0, 0, 0, 0, hWnd, (HMENU)107, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hButton9 = CreateWindow(TEXT("BUTTON"), TEXT("オレンジ"), WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | BS_PUSHLIKE, 0, 0, 0, 0, hWnd, (HMENU)108, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT("16"), WS_VISIBLE | WS_CHILD | ES_NUMBER | ES_AUTOHSCROLL, 0, 0, 0, 0, hWnd, (HMENU)109, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hScaleTrack = CreateWindow(TRACKBAR_CLASS, 0, WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_ENABLESELRANGE | TBS_DOWNISLEFT, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		SendMessage(hScaleTrack, TBM_SETRANGE, TRUE, MAKELONG(10, 3200));
		SendMessage(hScaleTrack, TBM_SETPAGESIZE, 0, 20);
		SendMessage(hScaleTrack, TBM_SETTICFREQ, 20, 0);
		SendMessage(hScaleTrack, TBM_SETPOS, TRUE, 100);
		SendMessage(hButton3, BM_SETCHECK, BST_CHECKED, 0);

		pBitmapBorderRight = LoadBitmapFromResource(MAKEINTRESOURCE(IDB_BORDER_RIGHT), TEXT("PNG"), ((LPCREATESTRUCT)lParam)->hInstance);
		pBitmapBorderBottom = LoadBitmapFromResource(MAKEINTRESOURCE(IDB_BORDER_BOTTOM), TEXT("PNG"), ((LPCREATESTRUCT)lParam)->hInstance);
		pBitmapBorderCorner = LoadBitmapFromResource(MAKEINTRESOURCE(IDB_BORDER_CORNER), TEXT("PNG"), ((LPCREATESTRUCT)lParam)->hInstance);		
		
		{
			RECT rect;
			GetClientRect(hWnd, &rect);
			nDstWidth = nSrcWidth = rect.right;
			nDstHeight = nSrcHeight = rect.bottom;
			const HDC hdc = GetDC(hWnd);
			hdcBitmap = CreateCompatibleDC(hdc);
			hBitmap = CreateCompatibleBitmap(hdc, nSrcWidth, nSrcHeight);
			SelectObject(hdcBitmap, hBitmap);
			ReleaseDC(hWnd, hdc);
			PatBlt(hdcBitmap, 0, 0, nSrcWidth, nSrcHeight, WHITENESS);
		}

		break;
	case WM_SIZE:
		MoveWindow(hButton1, 0, 0, 64, 64, TRUE);
		MoveWindow(hButton2, 64, 0, 64, 64, TRUE);
		MoveWindow(hButton3, 128, 0, 64, 64, TRUE);
		MoveWindow(hButton4, 192, 0, 64, 64, TRUE);
		MoveWindow(hButton5, 256, 0, 64, 64, TRUE);
		MoveWindow(hButton6, 320, 0, 64, 64, TRUE);
		MoveWindow(hButton7, 384, 0, 64, 64, TRUE);
		MoveWindow(hButton8, 448, 0, 64, 64, TRUE);
		MoveWindow(hButton9, 512, 0, 64, 64, TRUE);
		MoveWindow(hEdit, 512 + 64, 0, 64, 64, TRUE);
		MoveWindow(hScaleTrack, 0, HIWORD(lParam) - 32, LOWORD(lParam), 32, TRUE);
		break;
	case WM_LBUTTONDOWN:
		if (!bMouseDown)
		{
			SetCapture(hWnd);
			const int nPenWidth = GetDlgItemInt(hWnd, 109, 0, 0);
			hPen = CreatePen(PS_SOLID, nPenWidth, color);
			hOldPen = (HPEN)SelectObject(hdcBitmap, hPen);
			MoveToEx(hdcBitmap, (int)((GET_X_LPARAM(lParam) - x)*100.0 / nScaleValue), (int)((GET_Y_LPARAM(lParam) - y)*100.0 / nScaleValue), 0);
			bMouseDown = TRUE;
		}
		break;
	case WM_MOUSEMOVE:
		if (bMouseDown)
		{
			LineTo(hdcBitmap, (int)((GET_X_LPARAM(lParam) - x)*100.0 / nScaleValue), (int)((GET_Y_LPARAM(lParam) - y)*100.0 / nScaleValue));
			InvalidateRect(hWnd, 0, 0);
		}
		break;
	case WM_LBUTTONUP:
		if (bMouseDown)
		{
			ReleaseCapture();
			SelectObject(hdcBitmap, hOldPen);
			bMouseDown = FALSE;
		}
		break;
	case WM_POINTERDOWN:
		{
			POINTER_INFO pinfo;
			GetPointerInfo(LOWORD(wParam), &pinfo);
			POINT p = { pinfo.ptPixelLocation.x, pinfo.ptPixelLocation.y };
			ScreenToClient(hWnd, &p);
			const int nPenWidth = GetDlgItemInt(hWnd, 109, 0, 0);
			hPen = CreatePen(PS_SOLID, nPenWidth, color);
			hOldPen = (HPEN)SelectObject(hdcBitmap, hPen);
			MoveToEx(hdcBitmap, p.x, p.y, 0);
			bTouchDown = TRUE;
		}
		break;
	case WM_POINTERUPDATE:
		if (bTouchDown)
		{
			POINTER_INFO pinfo;
			GetPointerInfo(LOWORD(wParam), &pinfo);
			POINT p = { pinfo.ptPixelLocation.x, pinfo.ptPixelLocation.y };
			ScreenToClient(hWnd, &p);
			LineTo(hdcBitmap, p.x, p.y);
			InvalidateRect(hWnd, 0, 0);
		}
		break;
	case WM_POINTERUP:
		if (bTouchDown)
		{
			POINTER_INFO pinfo;
			GetPointerInfo(LOWORD(wParam), &pinfo);
			POINT p = { pinfo.ptPixelLocation.x, pinfo.ptPixelLocation.y };
			ScreenToClient(hWnd, &p);
			SelectObject(hdcBitmap, hOldPen);
			bTouchDown = FALSE;
		}
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			const HDC hdc = BeginPaint(hWnd, &ps);
			RECT rect;
			GetClientRect(hWnd, &rect);
			if (hdcBitmap)
			{
				const double dScale = nScaleValue / 100.0;
				int nOldStretchBltMode;
				if (nScaleValue < 100)
					nOldStretchBltMode = SetStretchBltMode(hdc, HALFTONE);
				x = (int)((rect.right - nDstWidth) / 2.0);
				y = (int)((rect.bottom - nDstHeight) / 2.0);
				{
					Gdiplus::Graphics g(hdc);
					g.SetClip(Gdiplus::Rect(x, y, nDstWidth, nDstHeight), Gdiplus::CombineMode::CombineModeExclude);
					g.FillRectangle(&Gdiplus::SolidBrush(Gdiplus::Color(220, 230, 240)), Gdiplus::Rect(0, 0, rect.right, rect.bottom));
					g.DrawImage(pBitmapBorderRight, x + nDstWidth, y + 8, 8, nDstHeight - 8);
					g.DrawImage(pBitmapBorderBottom, x, y + nDstHeight, nDstWidth, 8);
					g.DrawImage(pBitmapBorderCorner, x + nDstWidth, y + nDstHeight, 8, 8);
				}
				StretchBlt(hdc, x, y, nDstWidth, nDstHeight, hdcBitmap, 0, 0, nSrcWidth, nSrcHeight, SRCCOPY);
				if (nScaleValue < 100)
					SetStretchBltMode(hdc, nOldStretchBltMode);
			}
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case 100:
			PatBlt(hdcBitmap, 0, 0, nSrcWidth, nSrcHeight, WHITENESS);
			InvalidateRect(hWnd, 0, 0);
			break;
		case 101:
			break;
		case 102:
			color = RGB(0, 0, 0);
			break;
		case 103:
			color = RGB(255, 0, 0);
			break;
		case 104:
			color = RGB(0, 255, 0);
			break;
		case 105:
			color = RGB(0, 0, 255);
			break;
		case 106:
			color = RGB(128, 128, 128);
			break;
		case 107:
			color = RGB(255, 128, 128);
			break;
		case 108:
			color = RGB(255, 165, 0);
			break;
		case ID_EXIT:
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			break;
		}
		break;
	case WM_MOUSEWHEEL:
		{
			nScaleValue += GET_WHEEL_DELTA_WPARAM(wParam) / 40;
			if (nScaleValue<10)nScaleValue = 10;
			if (nScaleValue>3200)nScaleValue = 3200;
			SendMessage(hScaleTrack, TBM_SETPOS, TRUE, nScaleValue);
			nDstWidth = (int)(nScaleValue * nSrcWidth / 100.0);
			nDstHeight = (int)(nScaleValue * nSrcHeight / 100.0);
			InvalidateRect(hWnd, 0, 1);
		}
		break;
	case WM_HSCROLL:
		{
			nScaleValue = SendMessage(hScaleTrack, TBM_GETPOS, 0, 0);
			nDstWidth = (int)(nScaleValue * nSrcWidth / 100.0);
			nDstHeight = (int)(nScaleValue * nSrcHeight / 100.0);
			InvalidateRect(hWnd, 0, 1);
		}
		break;
	case WM_APP:
		if (hdcBitmap)
		{
			DeleteDC(hdcBitmap);
			hdcBitmap = NULL;
			DeleteObject(hBitmap);
			hBitmap = NULL;
		}
		break;
	case WM_DESTROY:
		SendMessage(hWnd, WM_APP, 0, 0);
		delete pBitmapBorderRight;
		delete pBitmapBorderBottom;
		delete pBitmapBorderCorner;
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	ULONG_PTR gdiToken;
	Gdiplus::GdiplusStartupInput gdiSI;
	Gdiplus::GdiplusStartup(&gdiToken, &gdiSI, NULL);
	MSG msg = { 0 };
	const WNDCLASS wndclass = { CS_HREDRAW | CS_VREDRAW,WndProc,0,0,hInstance,0,LoadCursor(0,IDC_ARROW),0/*(HBRUSH)(COLOR_WINDOW + 1)*/,MAKEINTRESOURCE(IDR_MENU1),szClassName };
	RegisterClass(&wndclass);
	const HWND hWnd = CreateWindow(szClassName, TEXT("MyPaint"), WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, 0, 0, hInstance, 0);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	Gdiplus::GdiplusShutdown(gdiToken);
	return (int)msg.wParam;
}
