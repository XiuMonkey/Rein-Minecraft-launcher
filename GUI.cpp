
#define _CRT_SECURE_NO_WARNINGS
#include <thread>  
#include <windows.h>
#include <stdio.h>
#include <ocidl.h>
#include <olectl.h>
#include <assert.h>
#include <direct.h>
#include <shlwapi.h>
#include <direct.h>
#include "Function.h"
#include "LauncherCore.h"
#include "resource.h"
#include "wincodec.h"
#include <dwrite.h>
#include <mmsystem.h>//播放音频
#pragma comment(lib, "winmm.lib")//播放音频
#pragma comment(lib,"Windowscodecs.lib")
#pragma comment(lib,"Shlwapi.lib")
#pragma comment(lib,"zlibstatic.lib")
#define LAUNCH 150
#define SHIFT 151
#pragma region 变量声明
int mbnum2 = 0;
int isCurInDestroy = 0;
int isCurInMin = 0;
int isBorderDown;
int WhichBack;
LaunchConfig mConfig;//启动配置对象
HDC mHDC;
HDC JavaPathEditHDC;
HWND MainWin;//主窗口句柄
HWND LaunchButton;//开始按钮句柄
HWND DownloadButton;//下载按钮句柄
HWND SettingButton;
HWND mBorder;
HWND mLOGO;
HWND DestroyButton;
HWND MinButton;
HWND GoBackButton;
mEditor JavaPathEditor;
WNDCLASS wc = { 0 };//窗口类
HINSTANCE hInst;

float dpi;
#pragma endregion

#pragma region OnPaint函数定义
inline void minWin() {
	ShowWindow(MainWin, 0);
	ShowWindow(GoBackButton, 0);
	ShowWindow(mBorder, 0);
	ShowWindow(LaunchButton, 0);
	ShowWindow(DownloadButton, 0);
	ShowWindow(SettingButton, 0);
	JavaPathEditor.edSHOW(0);
}
inline void OnPaintBack() {
	DrawPic(MainWin,  810 , 540 , BACK, "JPG");
	ShowWindow(mBorder, SW_SHOW);
	ShowWindow(LaunchButton, SW_SHOW);
	ShowWindow(DownloadButton, SW_SHOW);
	ShowWindow(SettingButton, SW_SHOW);
	ShowWindow(GoBackButton, SW_HIDE);
	JavaPathEditor.edSHOW(0);
}
inline void OnPaintBack2() {
	DrawPic(MainWin,  810, 540, BACK2, "PNG");
	ShowWindow(GoBackButton, SW_SHOW);
	ShowWindow(mBorder, SW_SHOW);
	ShowWindow(LaunchButton, SW_HIDE);
	ShowWindow(DownloadButton, SW_HIDE);
	ShowWindow(SettingButton, SW_HIDE);
}
inline void OnPaintBackSetting() {
	DrawPic(MainWin, 810, 540, BACK2, "PNG");
	JavaPathEditor.edSHOW(5);
	ShowWindow(GoBackButton, SW_SHOW);
	ShowWindow(mBorder, SW_SHOW);
	ShowWindow(LaunchButton, SW_HIDE);
	ShowWindow(DownloadButton, SW_HIDE);
	ShowWindow(SettingButton, SW_HIDE);
}
inline void OnPaintBorder() {
	DrawPic(mBorder,  810, 27, Border, "PNG");
}
inline void OnPaintLaunchNormal() {
	DrawPic(LaunchButton, 340 ,200, LAUNCHNORMAL, "PNG");
}
inline void OnPaintLaunchFocus() {
	DrawPic(LaunchButton, 340, 200, LAUNCHFOCUS, "PNG");
}
inline void OnPaintDownload() {
	DrawPic(DownloadButton,  106, 146, IDB_PNG5, "PNG");
}
inline void OnPaintSetting() {
	DrawPic(SettingButton,  106, 146, IDB_PNG5, "PNG");
}
inline void OnPaintLOGO() {
    DrawPic(mLOGO,  27, 27, LOGOPNG,"PNG");
}
inline void OnPaintDestroyNormal(){
	DrawPic(DestroyButton,  22, 23, mDESTROY, "PNG");
}
inline void OnPaintDestroyFocus() {
	DrawPicFocusDestroy(DestroyButton,  22, 23, mDESTROY, "PNG");
}
inline void OnPaintMinNormal() {
	DrawPic(MinButton,  22, 23, MIN, "PNG");
}
inline void OnPaintMinFocus() {
	DrawPicFocus(MinButton, 22, 23,MIN, "PNG");
}
inline void OnPaintGoBackNormal() {
	DrawPic(GoBackButton,  36, 36, GOBACK, "PNG");
}
inline void OnPaintGoBackFocus() {
	DrawPicFocus(GoBackButton,  36, 36, GOBACK, "PNG");
}
inline void OnPaintJPEBack() {
	DrawPic(JavaPathEditor.eBack, 500, 26, JPEBackPic, "PNG");
}
#pragma endregion

LRESULT CALLBACK DownloadProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{

	thread tt;
	POINT pt2;
	POINT pt;
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(tme);
	tme.dwFlags = TME_LEAVE;
	tme.dwHoverTime = HOVER_DEFAULT;
	tme.hwndTrack = hwnd;
	PAINTSTRUCT ps;
	switch (Message)
	{
	case WM_MOUSEMOVE:
		mbnum2++;
		GetCursorPos(&pt2);
		TrackMouseEvent(&tme);
	case WM_MOUSELEAVE:
		GetCursorPos(&pt);
		if (!((pt.x==pt2.x)&&(pt.y==pt2.y))) {
			mbnum2 = 0;
		}
	case WM_PAINT:
		if (mbnum2==0){
			mHDC = BeginPaint(DownloadButton, &ps);
			OnPaintDownload();
			EndPaint(DownloadButton, &ps);
			SetLayeredWindowAttributes(DownloadButton, RGB(0, 0, 0), 1, LWA_COLORKEY | LWA_ALPHA);
		}
		else{
			mHDC = BeginPaint(DownloadButton, &ps);
			SetLayeredWindowAttributes(DownloadButton, RGB(0, 0, 0), 130, LWA_COLORKEY | LWA_ALPHA);
		}
		break;
	case  WM_LBUTTONDOWN:
		WhichBack = 2;
		tt=thread(&playclick2);
		tt.join();
		Sleep(100);
		InvalidateRect(MainWin,NULL,TRUE);
		break;
	default:
		return DefWindowProc(hwnd, Message, wParam, lParam);   //让系统处理消息，这条语句一定要加上
	}
	return 0;
}

int CursorIsFirstPaintCount = 0;//光标是否是首次渲染计数
int isFirstMove = 0;

LRESULT CALLBACK SettingProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) 
{
	thread tt;
	POINT pt2;
	POINT pt;
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(tme);
	tme.dwFlags = TME_LEAVE;
	tme.dwHoverTime = HOVER_DEFAULT;
	tme.hwndTrack = hwnd;
	PAINTSTRUCT ps;
	LONG x;
	switch (Message)
	{
	case WM_MOUSEMOVE:
		mbnum2++;
		GetCursorPos(&pt2);
		TrackMouseEvent(&tme);
	case WM_MOUSELEAVE:
		GetCursorPos(&pt);
		if (!((pt.x == pt2.x) && (pt.y == pt2.y))) {
			mbnum2 = 0;
		}
	case WM_PAINT:
		if (mbnum2 == 0) {
			mHDC = BeginPaint(SettingButton, &ps);
			OnPaintSetting();
			EndPaint(SettingButton, &ps);
			SetLayeredWindowAttributes(SettingButton, RGB(0, 0, 0), 1, LWA_COLORKEY | LWA_ALPHA);
		}
		else {
			mHDC = BeginPaint(SettingButton, &ps);
			OnPaintSetting();
			EndPaint(SettingButton, &ps);
			SetLayeredWindowAttributes(SettingButton, RGB(0, 0, 0), 130, LWA_COLORKEY | LWA_ALPHA);
		}
		break;
	case  WM_LBUTTONDOWN:
		WhichBack = 3;
		tt = thread(&playclick2);
		tt.join();
		Sleep(100);
		InvalidateRect(MainWin, NULL, TRUE);
		break;
	default:
		return DefWindowProc(hwnd, Message, wParam, lParam);   //让系统处理消息，这条语句一定要加上
	}
	return 0;
}

POINT pt1;
LRESULT CALLBACK mBorderProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	POINT pt2;
	PAINTSTRUCT ps;
	TRACKMOUSEEVENT tme;
	UINT_PTR mTimer = 0;   // 定时器ID
	tme.cbSize = sizeof(tme);
	tme.dwFlags = TME_LEAVE;
	tme.dwHoverTime = HOVER_DEFAULT;
	tme.hwndTrack = hwnd;
	switch (Message) {
	case WM_PAINT:
		ShowWindow(mBorder, SW_HIDE);
		ShowWindow(mBorder, SW_SHOW);
		mHDC = BeginPaint(mBorder, &ps);
		OnPaintBorder();
		EndPaint(mBorder, &ps);
		break;
	case  WM_LBUTTONDOWN:
		isBorderDown = 1;
		SetCapture(mBorder);
		break;
	case  WM_LBUTTONUP:
		isBorderDown = 0;
		isFirstMove = 0;
		CursorIsFirstPaintCount = 0;
		ReleaseCapture();
		break;
	case WM_MOUSEMOVE:
		if (isBorderDown == 1) {
			if (isFirstMove == 0) {
				isFirstMove++;
				GetCursorPos(&pt1);
				ScreenToClient(MainWin, &pt1);
			}
			GetCursorPos(&pt2);
			SetWindowPos(MainWin, MainWin, pt2.x-pt1.x, pt2.y-pt1.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			TrackMouseEvent(&tme);
		}
	default:
		    return DefWindowProc(hwnd, Message, wParam, lParam);   //让系统处理消息，这条语句一定要加上
	case WM_ERASEBKGND:
		return TRUE;

     }
	return 0;
}

void Launch() {
	mConfig.LaunchGame();
}

LRESULT CALLBACK LaunchProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	static int CursorIsInLaunch = 0;
	thread tt;
	thread tt2;
	PAINTSTRUCT ps;
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(tme);
	tme.dwFlags = TME_LEAVE;
	tme.dwHoverTime = HOVER_DEFAULT;
	tme.hwndTrack = hwnd;
	switch (Message)
	{
	case WM_MOUSEMOVE:
		TrackMouseEvent(&tme);
		if (CursorIsInLaunch == 0) {
			InvalidateRect(LaunchButton, NULL, TRUE);
		}
		CursorIsInLaunch = 1;
		break;
	case WM_MOUSELEAVE:
		CursorIsInLaunch = 0;
		InvalidateRect(LaunchButton, NULL, TRUE);
		break;
	case WM_PAINT:
		FreeMyResource(IDR_MP31, "MP3", "data//01//Click.mp3");
		if (CursorIsInLaunch == 0) {
			mHDC = BeginPaint(LaunchButton, &ps);
			OnPaintLaunchNormal();
			EndPaint(LaunchButton, &ps);
			if (CursorIsFirstPaintCount < 1) {
				CursorIsFirstPaintCount++;
			}
			else {
				tt = thread(&playclick2);
				tt.join();
			}
		}
		else {
			tt = thread(&playclick2);
			tt.join();
			mHDC = BeginPaint(LaunchButton, &ps);
			OnPaintLaunchFocus();
			EndPaint(LaunchButton, &ps);
		}
		break;
	case WM_LBUTTONDOWN://鼠标点击
		mConfig.UserName = "1";
		mConfig.Memory = "4000";
		mConfig.JavaPath = "C:\\Program Files\\Java\\jre1.8.0_321\\bin\\java.exe";
		mConfig.versionName = "1.12.2";
		tt = thread(Launch);
		tt.detach();
		break;
		//GetWindowText(JavaPathEdit, javapath, 1000);//获取编辑框内容
	default:
		return DefWindowProc(hwnd, Message, wParam, lParam);   //让系统处理消息，这条语句一定要加上
	}
	return 0;
}

LRESULT CALLBACK mLOGOProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	switch (Message)
	{

	case WM_PAINT:
		mHDC = BeginPaint(mLOGO, &ps);
		OnPaintLOGO();
		EndPaint(mLOGO, &ps);
		break;
	default:
		return DefWindowProc(hwnd, Message, wParam, lParam);   //让系统处理消息，这条语句一定要加上
	}
}

LRESULT CALLBACK mDestroyProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static int isCurInDestroy = 0;
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(tme);
	tme.dwFlags = TME_LEAVE;
	tme.dwHoverTime = HOVER_DEFAULT;
	tme.hwndTrack = hwnd;
	PAINTSTRUCT ps;
	LONG x;
	switch (Message){
	case WM_MOUSEMOVE:
		TrackMouseEvent(&tme);
		if (isCurInDestroy == 0) {
			InvalidateRect(DestroyButton, NULL, TRUE);
		}
		isCurInDestroy = 1;
		break;
	case WM_MOUSELEAVE:
		isCurInDestroy = 0;
		InvalidateRect(DestroyButton, NULL, TRUE);
		break;
	case WM_PAINT:
		if (isCurInDestroy == 0) {
			mHDC = BeginPaint(DestroyButton, &ps);
			OnPaintDestroyNormal();
			EndPaint(DestroyButton, &ps);
		}
		else {
			mHDC = BeginPaint(DestroyButton, &ps);
			OnPaintDestroyFocus();
			EndPaint(DestroyButton, &ps);
		}
		break;
	case  WM_LBUTTONDOWN:
		SendMessage(MainWin, WM_CLOSE, 0, 0);
		break;
	case WM_ERASEBKGND:
		return TRUE;

	default:
		return DefWindowProc(hwnd, Message, wParam, lParam);   //让系统处理消息，这条语句一定要加上
	}
	return 0;
}

LRESULT CALLBACK MinProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(tme);
	tme.dwFlags = TME_LEAVE;
	tme.dwHoverTime = HOVER_DEFAULT;
	tme.hwndTrack = hwnd;
	PAINTSTRUCT ps;
	switch (Message) {
	case WM_MOUSEMOVE:
		TrackMouseEvent(&tme);
		if (isCurInMin == 0) {
			InvalidateRect(MinButton, NULL, TRUE);
		}
		isCurInMin = 1;
		break;
	case WM_MOUSELEAVE:
		isCurInMin = 0;
		InvalidateRect(MinButton, NULL, TRUE);
		break;
	case WM_PAINT:
		if (isCurInMin == 0) {
			mHDC = BeginPaint(MinButton, &ps);
			OnPaintMinNormal();
			EndPaint(MinButton, &ps);
		}
		else {
			mHDC = BeginPaint(MinButton, &ps);
			OnPaintMinFocus();
			EndPaint(MinButton, &ps);
		}
		break;
	case  WM_LBUTTONDOWN:
		CloseWindow(MainWin);
		break;
	case WM_ERASEBKGND:
		return TRUE;

	default:
		return DefWindowProc(hwnd, Message, wParam, lParam);   //让系统处理消息，这条语句一定要加上
	}
	return 0;
}

LRESULT CALLBACK GoBackProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static int isCurInGoBack = 0;
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(tme);
	tme.dwFlags = TME_LEAVE;
	tme.dwHoverTime = HOVER_DEFAULT;
	tme.hwndTrack = hwnd;
	PAINTSTRUCT ps;
	switch (Message) {
	case WM_MOUSEMOVE:
		TrackMouseEvent(&tme);
		if (isCurInGoBack == 0) {
			InvalidateRect(GoBackButton, NULL, TRUE);
		}
		isCurInGoBack = 1;
		break;
	case WM_MOUSELEAVE:
		isCurInGoBack = 0;
		InvalidateRect(GoBackButton, NULL, TRUE);
		break;
	case WM_PAINT:
		if (isCurInGoBack == 0) {
			mHDC = BeginPaint(GoBackButton, &ps);
			OnPaintGoBackNormal();
			EndPaint(GoBackButton, &ps);
		}
		else {
			mHDC = BeginPaint(GoBackButton, &ps);
			OnPaintGoBackFocus();
			EndPaint(GoBackButton, &ps);
		}
		break;
	case  WM_LBUTTONDOWN:
		WhichBack = 0;
		InvalidateRect(MainWin, NULL, TRUE);
		break;
	case WM_ERASEBKGND:
		return TRUE;

	default:
		return DefWindowProc(hwnd, Message, wParam, lParam);   //让系统处理消息，这条语句一定要加上
	}
	return 0;
}

LRESULT CALLBACK JPETag(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	switch (Message) {
	case WM_PAINT:
		break;
	case WM_ERASEBKGND:
		return TRUE;
	default:
		return DefWindowProc(hwnd, Message, wParam, lParam);   //让系统处理消息，这条语句一定要加上
	}
	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msgID, WPARAM wParam, LPARAM lParam);

int CALLBACK WinMain(HINSTANCE hIns, HINSTANCE hPreIns, LPSTR lpCmdLine, int nCmdShow) {

#pragma region 创建窗口
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.hCursor = NULL;
	wc.hIcon = LoadIcon(hIns, MAKEINTRESOURCE(IDI_ICON1));
	wc.hInstance = hIns;
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = "main";
	wc.lpszMenuName = NULL;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&wc);//写主窗口
	wc.cbClsExtra = sizeof(long);
	wc.hIcon = NULL;
	wc.lpfnWndProc = LaunchProc;
	wc.lpszClassName = TEXT("LaunchButton");
	RegisterClass(&wc);
	wc.cbClsExtra = sizeof(long);
	wc.hIcon = NULL;
	wc.lpfnWndProc = DownloadProc;
	wc.lpszClassName = TEXT("DownloadButton");
	RegisterClass(&wc);
	wc.cbClsExtra = sizeof(long);
	wc.hIcon = NULL;
	wc.lpfnWndProc = SettingProc;
	wc.lpszClassName = TEXT("SettingButton");
	RegisterClass(&wc);
	wc.cbClsExtra = sizeof(long);
	wc.hIcon = NULL;
	wc.lpfnWndProc = mBorderProc;
	wc.lpszClassName = TEXT("mBorder");
	RegisterClass(&wc);
	wc.cbClsExtra = sizeof(long);
	wc.hIcon = NULL;
	wc.lpfnWndProc = mLOGOProc;
	wc.lpszClassName = TEXT("mLOGO");
	RegisterClass(&wc);
	wc.cbClsExtra = sizeof(long);
	wc.hIcon = NULL;
	wc.lpfnWndProc = mDestroyProc;
	wc.lpszClassName = TEXT("mDestroy");
	RegisterClass(&wc);
	wc.cbClsExtra = sizeof(long);
	wc.hIcon = NULL;
	wc.lpfnWndProc = MinProc;
	wc.lpszClassName = TEXT("Min");
	RegisterClass(&wc);
	wc.cbClsExtra = sizeof(long);
	wc.hIcon = NULL;
	wc.lpfnWndProc = GoBackProc;
	wc.lpszClassName = TEXT("GoBack");
	RegisterClass(&wc);
	JavaPathEditor.edRegister("JPEback",JPEBackPic);
#pragma endregion

	Rootpath = GetRootPath();
	_mkdir("data");
	_mkdir("data\\01");
	_mkdir("data\\02");
	FreeMyResource(IDR_JAR1, "JAR", "data\\02\\log4j-patch-agent-1.0.jar");
	MainWin = CreateWindow("main", "ReM Alpha1.0", WS_CLIPCHILDREN | WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX| WS_BORDER| WS_EX_LAYERED| WS_EX_COMPOSITED, 200, 300,810,540, NULL, NULL, hIns, NULL);//创建主窗口
	WNDCLASS Bt = { 0 };
	UpdateWindow(MainWin);
	MSG nMsg = { 0 };
	hInst = hIns;
	while (GetMessage(&nMsg, NULL, 0, 0)) { //抓消息
		TranslateMessage(&nMsg);//翻译消息
		DispatchMessage(&nMsg);//派发消息：将消息交给窗口处理函数来处理。
	}
	return 0;     
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT msgID, WPARAM wParam, LPARAM lParam)
{
	DWORD nc;
	static HWND BT;
	DWORD mErr;
	static int x = 0;
	PAINTSTRUCT ps;
	switch (msgID)
	{
	case WM_CLOSE:

		DestroyWindow(hWnd);
		break;
	case WM_CREATE:
#pragma region 各窗口初始化
		D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &D2DFactory);
		LaunchButton = CreateWindowEx(0, TEXT("LaunchButton"), NULL, WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_EX_TOOLWINDOW | WS_VISIBLE | WS_EX_LAYERED, 40, 410, 136, 80, hWnd, (HMENU)(x++), hInst, NULL);
		DownloadButton = CreateWindowEx(0, TEXT("DownloadButton"), NULL, WS_CHILD | WS_VISIBLE, 240, 230, 106, 146, hWnd, (HMENU)(x++), hInst, NULL);
		SettingButton = CreateWindowEx(0, TEXT("SettingButton"), NULL, WS_CHILD | WS_VISIBLE, 630, 320, 166, 146, hWnd, (HMENU)(x++), hInst, NULL);
		mBorder = CreateWindowEx(0, TEXT("mBorder"), NULL, WS_CHILD | WS_VISIBLE, 00, 00, 810, 27, hWnd, (HMENU)(x++), hInst, NULL);
		mLOGO = CreateWindowEx(0, TEXT("mLOGO"), NULL, WS_CHILD | WS_VISIBLE, 2, 0, 27, 27, hWnd, (HMENU)(x++), hInst, NULL);
		DestroyButton = CreateWindowEx(0, TEXT("mDestroy"), NULL, WS_CHILD | WS_VISIBLE, 785, 2, 22, 23, hWnd, (HMENU)(x++), hInst, NULL);
		MinButton = CreateWindowEx(0, TEXT("Min"), NULL, WS_CHILD | WS_VISIBLE | WS_EX_TOPMOST, 760, 2, 22, 23, hWnd, (HMENU)(x++), hInst, NULL);
		GoBackButton = CreateWindowEx(0, TEXT("GoBack"), NULL, WS_CHILD | WS_VISIBLE | WS_EX_TOPMOST, 15, 30, 36, 36, hWnd, (HMENU)(x++), hInst, NULL);

		SetWindowLong(LaunchButton, GWL_EXSTYLE, GetWindowLong(LaunchButton, GWL_EXSTYLE) | WS_EX_LAYERED);
		SetLayeredWindowAttributes(LaunchButton, RGB(255, 255, 255), 255, LWA_COLORKEY | LWA_ALPHA);

		SetWindowLong(DownloadButton, GWL_EXSTYLE, GetWindowLong(DownloadButton, GWL_EXSTYLE) | WS_EX_LAYERED);
		SetLayeredWindowAttributes(DownloadButton, RGB(0, 0, 0), 1, LWA_COLORKEY | LWA_ALPHA);

        SetWindowLong(SettingButton, GWL_EXSTYLE, GetWindowLong(SettingButton, GWL_EXSTYLE) | WS_EX_LAYERED);
		SetLayeredWindowAttributes(SettingButton, RGB(0, 0, 0), 1, LWA_COLORKEY | LWA_ALPHA);

		SetWindowLong(mLOGO, GWL_EXSTYLE, GetWindowLong(mLOGO, GWL_EXSTYLE) | WS_EX_LAYERED);
		SetLayeredWindowAttributes(mLOGO, RGB(255, 255, 255), 255, LWA_COLORKEY | LWA_ALPHA);

		SetWindowPos(mBorder, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		ShowWindow(GoBackButton, SW_HIDE);
		ShowWindow(hWnd, SW_SHOW);
		mciSendString("open data\\01\\Click.mp3", NULL, 0, NULL);
#pragma endregion
		JavaPathEditor.edBuild(hWnd, x, wParam, lParam,230,80,540,40);
		JavaPathEditor.edSHOW(0);
		break;
		//销毁窗口
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		if (isBorderDown == 1) {
			break;
		}
		UpdateWindow(mBorder);
		mHDC = BeginPaint(hWnd, &ps);
		if (WhichBack == 0) {
		    OnPaintBack();
		}
		else if (WhichBack == 2) {
			OnPaintBack2();
			thread tt(playclick2);
			tt.detach();
			Sleep(10);
		}
		else if (3 == WhichBack) {
			OnPaintBackSetting();
			thread tt(playclick2);
			tt.detach();
			Sleep(10);
		}
		break;

	case WM_NCCALCSIZE:
		return 0;
		break;
	case WM_COMMAND:
		if ( HIWORD(wParam) == EN_CHANGE)
		{
			JavaPathEditor.Synchronize();
		}
		break;
	case WM_ERASEBKGND:
		return TRUE;
	default:
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		DefWindowProc(hWnd, msgID, wParam, lParam);
	}
	return 1;
}



