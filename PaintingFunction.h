#include <d2d1.h>
#include "wincodec.h"
#include <d2d1helper.h>
#include <windows.h>
#include <math.h>
#define SAFE_RELEASE(p) if(p){p->Release() ; p = NULL ;} 
#define DP_NORMAL 0
#define DP_FOCUS 1

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

extern HWND MainWin;
extern float dpi;

#pragma region 绘图函数定义
void DrawPic(HWND hwnd, float facx1, float facx2, float facy1, float facy2, UINT resourceName, char* resourceType,UINT PaintType)
{
	//dpi = GetDpiForWindow(MainWin);//GetDPI
	dpi = 96;
	RECT rc{ facx1 ,facy1, static_cast<int>(ceil(facx2 * dpi / 96.f)), static_cast<int>(ceil(facy2 * dpi / 96.f)) };

	D2DFactory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
		D2D1::HwndRenderTargetProperties(
			hwnd,
			D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)),
		&pRenderTarget);
	if (pRenderTarget == nullptr) {
		MessageBox(MainWin, "RenderTarget is nullptr", "Error", MB_ICONERROR);
		return;
	}
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

	pRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_FORCE_DWORD);
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
	if (PaintType == 1) {
		pRenderTarget->FillRectangle(D2D1::RectF(facx1, facy1, facx2, facy2), pBlackBrush);
	}
	else if (PaintType == 2) {
		pBlackBrush->SetColor(D2D1::ColorF(1, 0.3f, 0.3f, 1));
		pRenderTarget->FillRectangle(D2D1::RectF(facx1, facy1, facx2, facy2), pBlackBrush);
	}
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
VOID DrawPic(HWND hwnd, LONG facx2, LONG facy2, UINT resourceName, char* resourceType) {
	DrawPic(hwnd, 0, facx2, 0, facy2, resourceName, resourceType, DP_NORMAL);
}
VOID DrawPicFocus(HWND hwnd, LONG facx1, LONG facx2, LONG facy1, LONG facy2, UINT resourceName, char* resourceType)
{
	DrawPic(hwnd, facx1, facx2, facy1, facy2, resourceName, resourceType, DP_FOCUS );
};
VOID DrawPicFocus(HWND hwnd, LONG facx2, LONG facy2, UINT resourceName, char* resourceType)
{
	DrawPicFocus(hwnd, 0, facx2, 0, facy2, resourceName, resourceType);
};
VOID DrawPicFocusDestroy(HWND hwnd, LONG facx2, LONG facy2, UINT resourceName, char* resourceType)
{
	DrawPic(hwnd, 0, facx2, 0, facy2, resourceName, resourceType, 2);
};
#pragma endregion