
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib,"pthread_lib.lib")
#include <pthread.h>  
#include <thread>  
#include <windows.h>
#include <stdio.h>
#include <ocidl.h>
#include <olectl.h>
#include <assert.h>
#include <direct.h>
#include <string>
#include <shlwapi.h>
#include <iostream>
#include <direct.h>
#include "Function.h"
#include "LauncherCore.h"
#include "resource.h"
#include "wincodec.h"
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <mmsystem.h>//播放音频
#include <unzip.h>
#include <zconf.h>
#include <zlib.h>
#pragma comment(lib, "winmm.lib")//播放音频
#pragma comment(lib,"Windowscodecs.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib,"Shlwapi.lib")
#pragma comment(lib,"zlibstatic.lib")
#define SAFE_RELEASE(p) if(p){p->Release() ; p = NULL ;} 
#define LAUNCH 150
#define SHIFT 151

#pragma region 变量声明
int CursorIsLeaveCount = 0;
int CursorIsInLaunch = 0;
int ad3 = 0;
int mbnum2 = 0;
int isCurInDestroy = 0;
int isCurInMin = 0;
int isCurInGoBack = 0;
int isBorderDown;
int WhichBack;
HDC mHDC;
HDC JavaPathEditHDC;
HWND MainWin;//主窗口句柄
HWND LaunchButton;//开始按钮句柄
HWND DownloadButton;//下载按钮句柄
HWND JavaPathEdit;//java路径编辑框
HWND SettingButton;
HWND mBorder;
HWND mLOGO;
HWND DestroyButton;
HWND MinButton;
HWND GoBackButton;
HANDLE Proc;
HFONT hFont;//字体句柄
WNDCLASS wc = { 0 };//窗口类
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

VOID DrawPic(HWND hwnd, ID2D1Factory* fac, LONG facx1, LONG facx2,  LONG facy1, LONG facy2, UINT resourceName,char* resourceType)
{
	//dpi = GetDpiForWindow(MainWin);
	//MessageBox(MainWin, to_string(dpi).c_str(), "",NULL);
	RECT rc{ facx1 ,facy1,facx2,facy2 };
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

	SAFE_RELEASE(pLayer);
	SAFE_RELEASE(pStream);
	SAFE_RELEASE(m_pWicBitmap);//这个图片资源记得释放，不然会不断增加内存，不然，你可以把它创建完后，就不要再创建，到程序结束后再释放。
	SAFE_RELEASE(pBlackBrush);
	SAFE_RELEASE(m_pD2d1Bitmap);
	SAFE_RELEASE(pRenderTarget);
	SAFE_RELEASE(m_pWicFrameDecoder);
	SAFE_RELEASE(m_pWicDecoder);
	SAFE_RELEASE(m_pWicImagingFactory);
};
VOID DrawPic(HWND hwnd, ID2D1Factory* fac, LONG facx2, LONG facy2, UINT resourceName, char* resourceType) {
	DrawPic(hwnd, fac, 0, facx2, 0, facy2, resourceName, resourceType);
}
VOID DrawPicFocus(HWND hwnd, ID2D1Factory* fac, LONG facx1, LONG facx2, LONG facy1, LONG facy2, UINT resourceName, char* resourceType)
{
	RECT rc{ facx1 ,facy1,facx2 ,facy2 };
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
		D2D1::ColorF(D2D1::ColorF::White),
		&pBlackBrush
	);


	pRenderTarget->BeginDraw();
	pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
	pRenderTarget->PushLayer(D2D1::LayerParameters(), pLayer);
	pBlackBrush->SetOpacity(0.5);
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
	pRenderTarget->FillRectangle(D2D1::RectF(facx1, facy1, facx2, facy2), pBlackBrush);
	pRenderTarget->PopLayer();
	pRenderTarget->EndDraw();

	SAFE_RELEASE(pLayer);
	SAFE_RELEASE(pStream);
	SAFE_RELEASE(m_pWicBitmap);//这个图片资源记得释放，不然会不断增加内存，不然，你可以把它创建完后，就不要再创建，到程序结束后再释放。
	SAFE_RELEASE(pBlackBrush);
	SAFE_RELEASE(m_pD2d1Bitmap);
	SAFE_RELEASE(pRenderTarget);
	SAFE_RELEASE(m_pWicFrameDecoder);
	SAFE_RELEASE(m_pWicDecoder);
	SAFE_RELEASE(m_pWicImagingFactory);
};
VOID DrawPicFocus(HWND hwnd, ID2D1Factory* fac, LONG facx2,  LONG facy2, UINT resourceName, char* resourceType)
{
	RECT rc{ 0 ,0,facx2 ,facy2 };
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
		D2D1::ColorF(D2D1::ColorF::White),
		&pBlackBrush
	);


	pRenderTarget->BeginDraw();
	pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
	pRenderTarget->PushLayer(D2D1::LayerParameters(), pLayer);
	pBlackBrush->SetOpacity(0.5);
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
	pRenderTarget->FillRectangle(D2D1::RectF(0, 0, facx2, facy2), pBlackBrush);
	pRenderTarget->PopLayer();
	pRenderTarget->EndDraw();

	SAFE_RELEASE(pLayer);
	SAFE_RELEASE(pStream);
	SAFE_RELEASE(m_pWicBitmap);//这个图片资源记得释放，不然会不断增加内存，不然，你可以把它创建完后，就不要再创建，到程序结束后再释放。
	SAFE_RELEASE(pBlackBrush);
	SAFE_RELEASE(m_pD2d1Bitmap);
	SAFE_RELEASE(pRenderTarget);
	SAFE_RELEASE(m_pWicFrameDecoder);
	SAFE_RELEASE(m_pWicDecoder);
	SAFE_RELEASE(m_pWicImagingFactory);
};
VOID DrawPicFocusDestroy(HWND hwnd, ID2D1Factory* fac, LONG facx2, LONG facy2, UINT resourceName, char* resourceType)

{
	RECT rc{ 0 ,0,facx2 ,facy2 };
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
		D2D1::ColorF(D2D1::ColorF(1,0.3f,0.3f,1)),
		&pBlackBrush
	);


	pRenderTarget->BeginDraw();
	pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
	pRenderTarget->PushLayer(D2D1::LayerParameters(), pLayer);
	pBlackBrush->SetOpacity(0.5);
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
	pRenderTarget->FillRectangle(D2D1::RectF(0, 0, facx2, facy2), pBlackBrush);
	pRenderTarget->PopLayer();
	pRenderTarget->EndDraw();

	SAFE_RELEASE(pLayer);
	SAFE_RELEASE(pStream);
	SAFE_RELEASE(m_pWicBitmap);//这个图片资源记得释放，不然会不断增加内存，不然，你可以把它创建完后，就不要再创建，到程序结束后再释放。
	SAFE_RELEASE(pBlackBrush);
	SAFE_RELEASE(m_pD2d1Bitmap);
	SAFE_RELEASE(pRenderTarget);
	SAFE_RELEASE(m_pWicFrameDecoder);
	SAFE_RELEASE(m_pWicDecoder);
	SAFE_RELEASE(m_pWicImagingFactory);
};

bool unzipCurrentFile(unzFile uf, const char* destFolder)
{
	char szFilePath[512];
	unz_file_info64 FileInfo;

	if (unzGetCurrentFileInfo64(uf, &FileInfo, szFilePath, sizeof(szFilePath), NULL, 0, NULL, 0) != UNZ_OK)
		return false;

	size_t len = strlen(szFilePath);
	if (len <= 0)
	{
		return false;
	}

	std::string fullFileName = destFolder;
	fullFileName = fullFileName + "\\" + szFilePath;
	if (szFilePath[len - 1] == '\\' || szFilePath[len - 1] == '/')
	{
		_mkdir(fullFileName.c_str());
		return true;
	}
	auto file = fopen(fullFileName.c_str(), "wb");

	if (file == nullptr)
	{
		return false;
	}

	const int BUFFER_SIZE = 4096;
	unsigned char byBuffer[BUFFER_SIZE];
	if (unzOpenCurrentFile(uf) != UNZ_OK)
	{
		fclose(file);
		return false;
	}

	while (true)
	{
		int nSize = unzReadCurrentFile(uf, byBuffer, BUFFER_SIZE);

		if (nSize < 0)
		{
			unzCloseCurrentFile(uf);
			fclose(file);
			return false;
		}
		else if (nSize == 0)
		{
			break;
		}
		else
		{
			size_t wSize = fwrite(byBuffer, 1, nSize, file);
			if (wSize != nSize)
			{
				unzCloseCurrentFile(uf);
				fclose(file);
				return false;
			}
		}
	}

	unzCloseCurrentFile(uf);
	fclose(file);
	return true;
}
bool unzipFile(std::string zipFileName, std::string goalPath)
{
	unzFile uf = unzOpen64(zipFileName.c_str());
	if (uf == NULL)
		return false;

	unz_global_info64 gi;
	if (unzGetGlobalInfo64(uf, &gi) != UNZ_OK)
	{
		unzClose(uf);
		return false;
	}

	std::string path = zipFileName;
	auto pos = path.find_last_of("/\\");
	if (pos != std::string::npos)
		path.erase(path.begin() + pos, path.end());

	for (int i = 0; i < gi.number_entry; ++i)
	{
		if (!unzipCurrentFile(uf, goalPath.c_str()))
		{
			unzClose(uf);
			return false;
		}
		if (i < gi.number_entry - 1)
		{
			if (unzGoToNextFile(uf) != UNZ_OK)
			{
				unzClose(uf);
				return false;
			}
		}
	}
	unzClose(uf);
	return true;
}

void OnPaintBack() {
	DrawPic(MainWin, D2DFactory, 810 , 540 , BACK, "JPG");
	ShowWindow(mBorder, SW_SHOW);
	ShowWindow(LaunchButton, SW_SHOW);
	ShowWindow(DownloadButton, SW_SHOW);
	ShowWindow(SettingButton, SW_SHOW);
	ShowWindow(GoBackButton, SW_HIDE);
}
void OnPaintBack2() {
	DrawPic(MainWin, D2DFactory, 810, 540, BACK2, "PNG");
	ShowWindow(GoBackButton, SW_SHOW);
	ShowWindow(mBorder, SW_SHOW);
	ShowWindow(LaunchButton, SW_HIDE);
	ShowWindow(DownloadButton, SW_HIDE);
	ShowWindow(SettingButton, SW_HIDE);
}
void OnPaintBorder() {
	DrawPic(mBorder, D2DFactory, 810, 27, Border, "PNG");
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
void OnPaintLOGO() {
    DrawPic(mLOGO, D2DFactory, 27, 27, LOGOPNG,"PNG");
}
void OnPaintDestroyNormal(){
	DrawPic(DestroyButton, D2DFactory, 22, 23, mDESTROY, "PNG");
}
void OnPaintDestroyFocus() {
	DrawPicFocusDestroy(DestroyButton, D2DFactory, 22, 23, mDESTROY, "PNG");
}
void OnPaintMinNormal() {
	DrawPic(MinButton, D2DFactory, 22, 23, MIN, "PNG");
}
void OnPaintMinFocus() {
	DrawPicFocus(MinButton, D2DFactory, 22, 23,MIN, "PNG");
}

void OnPaintGoBackNormal() {
	DrawPic(GoBackButton, D2DFactory, 36, 36, GOBACK, "PNG");
}
void OnPaintGoBackFocus() {
	DrawPicFocus(GoBackButton, D2DFactory, 36, 36, GOBACK, "PNG");
}

LRESULT CALLBACK DownloadProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	thread tt;
	int ss;
	pthread_t tr1;
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
	case  WM_LBUTTONDOWN:
		WhichBack = 1;
		tt=thread(&playclick2);
		tt.join();
		InvalidateRect(MainWin,NULL,TRUE);
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
			OnPaintSetting();
			EndPaint(SettingButton, &ps);
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
		ShowWindow(mBorder, SW_HIDE);
		ShowWindow(mBorder, SW_SHOW);
		mHDC = BeginPaint(mBorder, &ps);
		OnPaintBorder();
		EndPaint(mBorder, &ps);
		break;
	case  WM_LBUTTONDOWN:
		isBorderDown = 1;
		LockWindowUpdate(MainWin);
		SetCapture(mBorder);
		break;
	case  WM_LBUTTONUP:
		isBorderDown = 0;
		CursorIsFirstPaintCount = 0;
		LockWindowUpdate(NULL);
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
	case WM_ERASEBKGND:
		return TRUE;

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
				pthread_create(&tr2, NULL, playclick, NULL);
			}
		}
		else {
			pthread_create(&tr2, NULL, playclick, NULL);
			pthread_detach(tr2);
			mHDC = BeginPaint(LaunchButton, &ps);
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
	POINT pt2;
	POINT pt;
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(tme);
	tme.dwFlags = TME_LEAVE;
	tme.dwHoverTime = HOVER_DEFAULT;
	tme.hwndTrack = hwnd;
	PAINTSTRUCT ps;
	LONG x;
	switch (Message){
	case WM_MOUSEMOVE:
		isCurInDestroy++;
		GetCursorPos(&pt2);
		TrackMouseEvent(&tme);
	case WM_MOUSELEAVE:
		GetCursorPos(&pt);
		if (!((pt.x == pt2.x) && (pt.y == pt2.y))) {
			isCurInDestroy = 0;
		}
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
	POINT pt2;
	POINT pt;
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(tme);
	tme.dwFlags = TME_LEAVE;
	tme.dwHoverTime = HOVER_DEFAULT;
	tme.hwndTrack = hwnd;
	PAINTSTRUCT ps;
	LONG x;
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
	void* thread_result;
	pthread_t tr1;
	POINT pt2;
	POINT pt;
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(tme);
	tme.dwFlags = TME_LEAVE;
	tme.dwHoverTime = HOVER_DEFAULT;
	tme.hwndTrack = hwnd;
	PAINTSTRUCT ps;
	LONG x;
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

LRESULT CALLBACK WndProc(HWND hWnd, UINT msgID, WPARAM wParam, LPARAM lParam);

int CALLBACK WinMain(HINSTANCE hIns, HINSTANCE hPreIns, LPSTR lpCmdLine, int nCmdShow) {
	DWORD c;
	BOOL rd;
	ULONG_PTR token;
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
#pragma endregion

	Proc = GetCurrentProcess();
	//SetProcessWorkingSetSize(Proc, 0, 1024*1024*10);
	getopath(opath);
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
	pthread_t tr1;
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

		SetForegroundWindow(MinButton);
		SetWindowPos(MinButton, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		SetWindowPos(mBorder, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		ShowWindow(GoBackButton, SW_HIDE);
		ShowWindow(hWnd, SW_SHOW);
#pragma endregion
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
		UpdateWindow(mBorder);
		mHDC = BeginPaint(hWnd, &ps);
		if (WhichBack == 0) {
		    OnPaintBack();
		}
		else if (WhichBack == 1) {
			OnPaintBack2();
			thread tt(playclick2);
			tt.join();
		}
		mHDC = BeginPaint(JavaPathEdit, &ps);
		OnPaintJavaPathEdit();
		EndPaint(hWnd, &ps);
		break;
	case WM_NCCALCSIZE:
		return 0;
		break;
	case WM_ERASEBKGND:
		return TRUE;
	default:
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		DefWindowProc(hWnd, msgID, wParam, lParam);
	}
	return 1;
}



