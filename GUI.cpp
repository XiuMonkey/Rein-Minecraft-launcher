
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib,"pthread_lib.lib")
#include <pthread.h>  
#include <windows.h>
#include <stdio.h>
#include <ocidl.h>
#include <olectl.h>
#include <assert.h>
#include <direct.h>
#include <string>
#include <shlwapi.h>
#include <iostream>
#include "Function.h"
#include "LauncherCore.h"
#include "resource.h"
#include "wincodec.h"
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <mmsystem.h>//播放音频
#pragma comment(lib, "winmm.lib")//播放音频
#pragma  comment(lib,"Windowscodecs.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib,"Shlwapi.lib")
//#pragma comment(lib,"gdiplus.lib")
#define SAFE_RELEASE(p) if(p){p->Release() ; p = NULL ;} 
#define LAUNCH 150
#define SHIFT 151

#pragma region 变量声明
int CursorIsLeaveCount = 0;
int ad3 = 0;
int mbnum2 = 0;
int isBorderDown;
HDC mHDC;
HDC JavaPathEditHDC;
HWND MainWin;//主窗口句柄
HWND LaunchButton;//开始按钮句柄
HWND DownloadButton;//下载按钮句柄
HWND JavaPathEdit;//java路径编辑框
HWND SettingButton;
HWND mBorder;
HFONT hFont;//字体句柄
WNDCLASS wc = { 0 };//主窗口类
char javapath[1000];
HINSTANCE hInst;

ID2D1Factory* D2DFactory = NULL;
ID2D1HwndRenderTarget* pRenderTarget = NULL;//用来在窗口中进行渲染 
ID2D1SolidColorBrush* pBlackBrush = NULL;//定义画刷，用来绘制图形
ID2D1Layer* pLayer = NULL;
ID2D1Bitmap* m_pD2d1Bitmap;
IWICBitmap* m_pWicBitmap;
IWICImagingFactory* m_pWicImagingFactory;
IWICBitmapDecoder* m_pWicDecoder;
IWICBitmapFrameDecode* m_pWicFrameDecoder;

void* pImageFile;
HGLOBAL imageResDataHandle = NULL;
DWORD imageFileSize = 0;
IWICStream* pStream = NULL;

float dpi;
float ccc;

using namespace std;
#pragma endregion

VOID DrawPic(HWND hwnd, ID2D1Factory* fac, LONG facx, LONG facy, UINT resourceName,char* resourceType)
{
	float c;
	c = dpi;
	//dpi = GetDpiForWindow(MainWin);
	//MessageBox(MainWin, to_string(dpi).c_str(), "",NULL);
	RECT rc{ 0,0,facx,facy };
	UINT a;
	D2DFactory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
		D2D1::HwndRenderTargetProperties(
			hwnd,
			D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)),
		&pRenderTarget);
	pRenderTarget->CreateLayer(NULL, &pLayer);
    CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pWicImagingFactory));
	HRSRC imageResHandle = FindResource(GetModuleHandle(NULL), MAKEINTRESOURCE(resourceName), resourceType);
	HRESULT hr = imageResHandle ? S_OK : E_FAIL;
	imageResDataHandle = LoadResource(NULL, imageResHandle);
	pImageFile = LockResource(imageResDataHandle);
	imageFileSize = SizeofResource(NULL, imageResHandle);
	m_pWicImagingFactory->CreateStream(&pStream);
	pStream->InitializeFromMemory(
		reinterpret_cast<BYTE*>(pImageFile),
		imageFileSize
	);
	m_pWicImagingFactory->CreateDecoderFromStream(
		pStream,
		nullptr,
		WICDecodeMetadataCacheOnLoad,
		&m_pWicDecoder
	);
	IWICBitmapSource* pWicSource = nullptr;
	IWICFormatConverter* pCovert = nullptr;
	WICPixelFormatGUID pixelFormat;//位图像素格式
	if (!(m_pWicDecoder == nullptr)) {
		m_pWicDecoder->GetFrame(0, &m_pWicFrameDecoder);
		m_pWicFrameDecoder->GetPixelFormat(&pixelFormat);//获取位图格式
		m_pWicFrameDecoder->QueryInterface(IID_PPV_ARGS(&pWicSource));
		m_pWicImagingFactory->CreateFormatConverter(&pCovert);
		pCovert->Initialize(
			pWicSource,
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			NULL,
			0.f,
			WICBitmapPaletteTypeCustom
		);
	}

	m_pWicImagingFactory->CreateBitmapFromSource(pCovert, WICBitmapCacheOnDemand, &m_pWicBitmap);
	SAFE_RELEASE(pCovert);
	UINT pixelWidth = 0, pixelHeight = 0;
	m_pWicBitmap->GetSize(&pixelWidth, &pixelHeight);


	pRenderTarget->CreateBitmapFromWicBitmap(m_pWicBitmap, NULL, &m_pD2d1Bitmap);

	pRenderTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::GreenYellow),
		&pBlackBrush
	);

	pRenderTarget->BeginDraw();
	pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
	pRenderTarget->PushLayer(D2D1::LayerParameters(),pLayer);

	/*****************绘制图片*************************/
	D2D1_SIZE_F rtSize = pRenderTarget->GetSize();
	D2D1_SIZE_U sizeU = m_pD2d1Bitmap->GetPixelSize();
	D2D1_RECT_F rectangle3 = D2D1::RectF(
		(rtSize.width - sizeU.width) * 0.5f,
		(rtSize.height - sizeU.height) * 0.5f,
		sizeU.width + (rtSize.width - sizeU.width) * 0.5f,
		sizeU.height + (rtSize.height - sizeU.height) * 0.5f
	);

	pRenderTarget->DrawBitmap(m_pD2d1Bitmap, &rectangle3);
	pRenderTarget->PopLayer();
	pRenderTarget->EndDraw();

	SAFE_RELEASE(pStream);
	SAFE_RELEASE(m_pWicBitmap);//这个图片资源记得释放，不然会不断增加内存，不然，你可以把它创建完后，就不要再创建，到程序结束后再释放。
	SAFE_RELEASE(pBlackBrush);
	SAFE_RELEASE(m_pD2d1Bitmap);
	SAFE_RELEASE(pRenderTarget);
	SAFE_RELEASE(m_pWicFrameDecoder);
	SAFE_RELEASE(m_pWicDecoder);
	SAFE_RELEASE(m_pWicImagingFactory);
};

//释放资源文件

void OnPaintBack() {
	DrawPic(MainWin, D2DFactory, 810 , 540 , BACK, "JPG");
}
void OnPaintBorder() {
	DrawPic(mBorder, D2DFactory, 810, 23, Border, "PNG");
}
void OnPaintLaunchNormal() {
	DrawPic(LaunchButton, D2DFactory,340 ,200, LAUNCHNORMAL, "PNG");
}
void OnPaintLaunchFocus() {
	DrawPic(LaunchButton, D2DFactory, 340, 200, LAUNCHFOCUS, "PNG");
}
void OnPaintDownload() {
	DrawPic(DownloadButton, D2DFactory, 106, 146, IDB_PNG5, "PNG");
}
void OnPaintSetting() {
	DrawPic(SettingButton, D2DFactory, 106, 146, IDB_PNG5, "PNG");
}
void OnPaintJavaPathEdit() {
	DrawPic(JavaPathEdit, D2DFactory, 237, 25, IDR_JPG4, "JPG");
}

LRESULT CALLBACK DownloadProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
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
	default:
		return DefWindowProc(hwnd, Message, wParam, lParam);   //让系统处理消息，这条语句一定要加上
	}
	return 0;
}

int CursorIsFirstPaintCount = 0;//光标是否是首次渲染计数

LRESULT CALLBACK SettingProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) 
{
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
			SetLayeredWindowAttributes(SettingButton, RGB(0, 0, 0), 130, LWA_COLORKEY | LWA_ALPHA);
		}
		break;
	default:
		return DefWindowProc(hwnd, Message, wParam, lParam);   //让系统处理消息，这条语句一定要加上
	}
	return 0;
}

LRESULT CALLBACK mBorderProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	POINT pt1;
	pt1.y = 0;
	pt1.x = 0;
	POINT pt2;
	RECT rt1;
	pthread_t tr2;
	PAINTSTRUCT ps;
	TRACKMOUSEEVENT tme;
	UINT_PTR mTimer = 0;   // 定时器ID
	tme.cbSize = sizeof(tme);
	tme.dwFlags = TME_LEAVE;
	tme.dwHoverTime = HOVER_DEFAULT;
	tme.hwndTrack = hwnd;
	switch (Message) {
	case WM_PAINT:
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
		ReleaseCapture();
		break;
	case WM_MOUSEMOVE:
		if (isBorderDown == 1) {
			GetCursorPos(&pt2);
			GetWindowRect(MainWin, &rt1);
			SetWindowPos(MainWin, MainWin, pt2.x-300, pt2.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			GetCursorPos(&pt1);
			TrackMouseEvent(&tme);
		}
		default:
		    return DefWindowProc(hwnd, Message, wParam, lParam);   //让系统处理消息，这条语句一定要加上
     }
	return 0;
}

LRESULT CALLBACK LaunchProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	pthread_t tr1;
	pthread_t tr2;
	PAINTSTRUCT ps;
	RECT rt;
	POINT pt;
	POINT pt2;
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(tme);
	tme.dwFlags = TME_LEAVE;
	tme.dwHoverTime = HOVER_DEFAULT;
	tme.hwndTrack = hwnd;
	switch (Message)
	{
	case WM_MOUSEMOVE:	
		mHDC = BeginPaint(LaunchButton, &ps);
		if (ad3 == 0) {
			pthread_create(&tr1, NULL, playclick, NULL);
			ad3++;
		}
		CursorIsLeaveCount=1;     
		GetCursorPos(&pt);
		TrackMouseEvent(&tme);
	case WM_MOUSELEAVE:
		GetCursorPos(&pt2);
		if (CursorIsLeaveCount == 0) {
			if (CursorIsFirstPaintCount == 0) {
				CursorIsFirstPaintCount++;
			}
			else {
				pthread_create(&tr2, NULL, playclick, NULL);
				ad3 = 0;
			}
		}
		if (!((pt.x == pt2.x)&&(pt.y == pt2.y))){	
			CursorIsLeaveCount = 0;   
		}
	case WM_PAINT:
	    FreeMyResource(IDR_MP31, "MP3", "data//01//Click.mp3");
		if (CursorIsLeaveCount == 0) {
			if (CursorIsFirstPaintCount < 2) {
				CursorIsFirstPaintCount++;
			}
			else {
				pthread_create(&tr2, NULL, playclick, NULL);
				ad3 = 0;
			}
			mHDC = BeginPaint(LaunchButton, &ps);
			OnPaintLaunchNormal();
			EndPaint(LaunchButton, &ps);
		}
		else {
			OnPaintLaunchFocus();
			EndPaint(LaunchButton, &ps);
		}
		break;
	case WM_LBUTTONDOWN://鼠标点击
		GetWindowText(JavaPathEdit, javapath, 1000);//获取编辑框内容
	default:
		return DefWindowProc(hwnd, Message, wParam, lParam);   //让系统处理消息，这条语句一定要加上
	}
	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msgID, WPARAM wParam, LPARAM lParam);

int CALLBACK WinMain(HINSTANCE hIns, HINSTANCE hPreIns, LPSTR lpCmdLine, int nCmdShow) {
	DWORD c;
	BOOL rd;
	ULONG_PTR token;
#pragma region 创建主窗口
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
	getopath(opath);
	_mkdir("data");
	_mkdir("data\\01");
	_mkdir("data\\02");
	FreeMyResource(IDR_JAR1, "JAR", "data\\02\\log4j-patch-agent-1.0.jar");
	MainWin = CreateWindow("main", "ReM Alpha1.0", WS_CLIPCHILDREN | WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX| WS_BORDER| WS_EX_LAYERED| WS_EX_COMPOSITED, 100, 100, 810,540, NULL, NULL, hIns, NULL);//创建主窗口
	dpi = GetDpiForWindow(MainWin); //获取DPI
#pragma endregion
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
	RECT rc;
	static int x = 0;
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	switch (msgID)
	{
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_CREATE:
		D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &D2DFactory);
		ShowWindow(hWnd, SW_SHOW);
		LaunchButton = CreateWindowEx(0, TEXT("LaunchButton"), NULL, WS_CHILD| WS_CLIPSIBLINGS | WS_CLIPCHILDREN| WS_EX_TOOLWINDOW | WS_VISIBLE |  WS_EX_LAYERED ,40 , 410,136, 80,hWnd, (HMENU)(x++), hInst, NULL);
		DownloadButton = CreateWindowEx(0, TEXT("DownloadButton"), NULL, WS_CHILD | WS_VISIBLE,240 , 230 ,106, 146,hWnd, (HMENU)(x++), hInst, NULL);
		SettingButton = CreateWindowEx(0, TEXT("SettingButton"), NULL, WS_CHILD | WS_VISIBLE, 630, 320, 166, 146, hWnd, (HMENU)(x++), hInst, NULL);
		mBorder = CreateWindowEx(0, TEXT("mBorder"), NULL, WS_CHILD | WS_VISIBLE, 00, 00, 810, 23, hWnd, (HMENU)(x++), hInst, NULL);
		SetWindowLong(LaunchButton, GWL_EXSTYLE, GetWindowLong(LaunchButton, GWL_EXSTYLE) | WS_EX_LAYERED);
		SetLayeredWindowAttributes(LaunchButton, RGB(255, 255, 255), 255, LWA_COLORKEY|LWA_ALPHA);
		SetWindowLong(DownloadButton, GWL_EXSTYLE, GetWindowLong(LaunchButton, GWL_EXSTYLE) | WS_EX_LAYERED);
		SetLayeredWindowAttributes(DownloadButton, RGB(0, 0, 0), 1, LWA_COLORKEY | LWA_ALPHA);
		SetWindowLong(SettingButton, GWL_EXSTYLE, GetWindowLong(LaunchButton, GWL_EXSTYLE) | WS_EX_LAYERED);
		SetLayeredWindowAttributes(SettingButton, RGB(0, 0, 0), 1, LWA_COLORKEY | LWA_ALPHA);
#pragma region 创建编辑框并上位图
		JavaPathEdit = CreateWindow("edit", NULL, WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL| ES_NOHIDESEL , 450, 10, 237, 25, hWnd, NULL, NULL, NULL);
		SetWindowLong(JavaPathEdit, GWL_EXSTYLE, GetWindowLong(LaunchButton, GWL_EXSTYLE) | WS_EX_LAYERED);
		SetLayeredWindowAttributes(JavaPathEdit, RGB(0, 0, 0), 1, LWA_COLORKEY | LWA_ALPHA);
		hFont = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,DEFAULT_PITCH | FF_SWISS, "等线");
		SendMessage(JavaPathEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
#pragma endregion
		break;
		//销毁窗口
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		mHDC = BeginPaint(hWnd, &ps);
		OnPaintBack();
		mHDC = BeginPaint(JavaPathEdit, &ps);
		OnPaintJavaPathEdit();
		EndPaint(hWnd, &ps);

	case WM_NCCALCSIZE:
		return 0;
		break;
	default:
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		DefWindowProc(hWnd, msgID, wParam, lParam);
	}
	return 1;
}



