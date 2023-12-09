
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib,"pthread_lib.lib")
//#pragma comment(lib, "msvcrtd.lib") 
//#pragma comment(lib, "vcruntimed.lib") 
//#pragma comment(lib, "ucrtd.lib") 
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
HDC MainHDC;
HDC LaunchHDC;
HDC DownloadHDC;
HDC JavaPathEditHDC;
HWND MainWin;//主窗口句柄
HWND LaunchButton;//开始按钮句柄
HWND DownloadButton;//下载按钮句柄
HWND JavaPathEdit;//java路径编辑框
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

VOID MyDraw(HWND hwnd, ID2D1Factory* fac, LONG facx, LONG facy, UINT resourceName,char* resourceType)
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
BOOL FreeMyResource(UINT uiResouceName, char* lpszResourceType, char* lpszSaveFileName)
{
	HRSRC hRsrc = ::FindResource(GetModuleHandle(NULL), MAKEINTRESOURCE(uiResouceName), lpszResourceType);
	LPTSTR szBuffer = new TCHAR[1024];
	if (hRsrc == NULL)
	{
		return FALSE;
	}
	DWORD dwSize = ::SizeofResource(NULL, hRsrc);
	if (0 >= dwSize)
	{
		return FALSE;
	}
	HGLOBAL hGlobal = ::LoadResource(NULL, hRsrc);
	if (NULL == hGlobal)
	{
		return FALSE;
	}
	LPVOID lpVoid = ::LockResource(hGlobal);
	if (NULL == lpVoid)
	{
		return FALSE;
	}
	FILE* fp = NULL;
	fopen_s(&fp, lpszSaveFileName, "wb+");
	if (NULL == fp)
	{
		return FALSE;
	}
	fwrite(lpVoid, sizeof(char), dwSize, fp);
	fclose(fp);
	return TRUE;
}

void OnPaintBack(HDC hdc) {
	MyDraw(MainWin, D2DFactory, 810 , 540 , BACK, "JPG");
}
void OnPaintLaunchNormal(HDC hdc) {
	MyDraw(LaunchButton, D2DFactory,340 ,200, LAUNCHNORMAL, "PNG");
}
void OnPaintLaunchFocus(HDC hdc) {
	MyDraw(LaunchButton, D2DFactory, 340, 200, LAUNCHFOCUS, "PNG");
}
void OnPaintDownload(HDC hdc) {
	MyDraw(DownloadButton, D2DFactory, 106, 146, IDB_PNG5, "PNG");
}
void OnPaintJavaPathEdit(HDC hdc) {
	MyDraw(JavaPathEdit, D2DFactory, 237, 25, IDR_JPG4, "JPG");
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
			DownloadHDC = BeginPaint(DownloadButton, &ps);
			OnPaintDownload(DownloadHDC);
			EndPaint(DownloadButton, &ps);
			SetLayeredWindowAttributes(DownloadButton, RGB(0, 0, 0), 1, LWA_COLORKEY | LWA_ALPHA);
		}
		else{
			DownloadHDC = BeginPaint(DownloadButton, &ps);
			SetLayeredWindowAttributes(DownloadButton, RGB(0, 0, 0), 130, LWA_COLORKEY | LWA_ALPHA);
		}
		break;
	default:
		return DefWindowProc(hwnd, Message, wParam, lParam);   //让系统处理消息，这条语句一定要加上
	}
	return 0;
}
void *playclick(void* args){
	mciSendString("play data\\01\\Click.mp3", NULL, 0, NULL);
	return 0;
}

int CursorIsFirstPaintCount = 0;//光标是否是首次渲染计数
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
		LaunchHDC = BeginPaint(LaunchButton, &ps);
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
			if (CursorIsFirstPaintCount == 0) {
				CursorIsFirstPaintCount++;
			}
			else {
				pthread_create(&tr2, NULL, playclick, NULL);
				ad3 = 0;
			}
			LaunchHDC = BeginPaint(LaunchButton, &ps);
			OnPaintLaunchNormal(LaunchHDC);
			EndPaint(LaunchButton, &ps);
		}
		else {
			OnPaintLaunchFocus(LaunchHDC);
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
	getopath(opath);
	_mkdir("data");
	_mkdir("data\\01");
	_mkdir("data\\02");
	FreeMyResource(IDR_JAR1, "JAR", "data\\02\\log4j-patch-agent-1.0.jar");
	MainWin = CreateWindow("main", "ReM Alpha1.0", WS_CLIPCHILDREN | WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX| WS_BORDER| WS_EX_LAYERED| WS_EX_COMPOSITED, 100, 100, 810,540, NULL, NULL, hIns, NULL);//创建主窗口
	dpi = GetDpiForWindow(MainWin); //获取DPI
	UpdateWindow(LaunchButton);//创建完主窗口让LaunchButton重绘
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
		LaunchButton = CreateWindowEx(0, TEXT("LaunchButton"), NULL, WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN| WS_EX_TOOLWINDOW | WS_VISIBLE |  WS_EX_LAYERED ,30 , 350 ,136, 80,hWnd, (HMENU)(x++), hInst, NULL);
		DownloadButton = CreateWindowEx(0, TEXT("DownloadButton"), NULL, WS_CHILD | WS_VISIBLE,235 , 205 ,106, 146,hWnd, (HMENU)(x++), hInst, NULL);
		SetWindowLong(LaunchButton, GWL_EXSTYLE, GetWindowLong(LaunchButton, GWL_EXSTYLE) | WS_EX_LAYERED);
		SetLayeredWindowAttributes(LaunchButton, RGB(255, 255, 255), 255, LWA_COLORKEY|LWA_ALPHA);
		SetWindowLong(DownloadButton, GWL_EXSTYLE, GetWindowLong(LaunchButton, GWL_EXSTYLE) | WS_EX_LAYERED);
		SetLayeredWindowAttributes(DownloadButton, RGB(0, 0, 0), 1, LWA_COLORKEY | LWA_ALPHA);
#pragma region 创建编辑框并上位图
		JavaPathEdit = CreateWindow("edit", NULL, WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL| ES_NOHIDESEL, 450, 10, 237, 25, hWnd, NULL, NULL, NULL);
		hFont = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,DEFAULT_PITCH | FF_SWISS, "等线");
		SendMessage(JavaPathEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
#pragma endregion
		break;
		//销毁窗口
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		MainHDC = BeginPaint(hWnd, &ps);
		OnPaintBack(MainHDC);
		MainHDC = BeginPaint(JavaPathEdit, &ps);
		OnPaintJavaPathEdit(JavaPathEditHDC);
		EndPaint(hWnd, &ps);
	case WM_MOVE:
		GetWindowRect(MainWin, &rc);
		SetWindowPos(LaunchButton, HWND_TOP, rc.left+50, rc.top+420, 0,0, SWP_SHOWWINDOW | SWP_NOSIZE);
	default:
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		DefWindowProc(hWnd, msgID, wParam, lParam);
	}
	return 1;
}



