#include <unzip.h>
#include <zconf.h>
#include <string.h>
#include <zlib.h>
#include <string>
#include "PaintingFunction.h"
extern WNDCLASS wc;
extern LRESULT CALLBACK JPEBackProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
extern HINSTANCE hInst;
extern HDC mHDC;
void minWin() ;


using namespace std;

void MsgInt(int mi) {
	MessageBox(MainWin, to_string(mi).c_str(), "", NULL);
}

class mEditor {//用两个编辑框和一个背景图合成一个编辑框
public:
	static WNDPROC oldProc1;//第一个编辑框
	static WNDPROC oldProc2;//第二个
	static UINT rs;//背景资源
	char name[15];//注册背景时的名称
	static HFONT hFont;//字体
	static HWND ed1, ed2, eBack;
	static char edt[200];
	static int x1, y1, x2, y2;//起始坐标和长宽
	inline void edRegister(char name1[15],UINT rs2) {
		strcpy(name, name1);
		wc.cbClsExtra = sizeof(long);
		wc.hIcon = NULL;
		wc.lpfnWndProc = mEditor::eProc;
		wc.lpszClassName = name;
		RegisterClass(&wc);
		rs = rs2;
	}//注册背景，并设置背景资源

	inline void edBuild(HWND hWnd, int x/*注册的数字id*/, WPARAM wParam, LPARAM lParam, int cx1, int cy1, int cx2,int cy2) {
		x1 = cx1;
		y1 = cy1;
		x2 = cx2;
		y2 = cy2;
		ed1 = CreateWindow(TEXT("edit"), TEXT("请输入文本："), WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_NOHIDESEL| ES_AUTOHSCROLL, x1, y1, x2, y2, hWnd, (HMENU)(x++), ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		eBack = CreateWindowEx(0, name, NULL, WS_CHILD | WS_VISIBLE, x1, y1-7, x2, y2, hWnd, (HMENU)(x++), hInst, NULL);
	    ed2 = CreateWindow(TEXT("edit"), TEXT("请输入:"), WS_CHILD | WS_VISIBLE | ES_MULTILINE| ES_NOHIDESEL| ES_AUTOHSCROLL, x1, y1, x2, y2, hWnd, (HMENU)(x++), ((LPCREATESTRUCT)lParam)->hInstance, NULL);

		SetWindowLong(ed1, GWL_EXSTYLE, GetWindowLong(ed1, GWL_EXSTYLE) | WS_EX_LAYERED);
		SetWindowLong(ed2, GWL_EXSTYLE, GetWindowLong(ed2, GWL_EXSTYLE) | WS_EX_LAYERED);

		SendMessage(ed1, WM_GETTEXT, 200, (LPARAM)edt);
		SetWindowText(ed2, edt);

		oldProc1 = (WNDPROC)SetWindowLongPtrA(ed1, -4, (LONG_PTR)eProc1);//子类化编辑框
		oldProc2 = (WNDPROC)SetWindowLongPtrA(ed2, -4, (LONG_PTR)eProc2);

		SetLayeredWindowAttributes(ed1, RGB(255, 255, 255), 1, LWA_ALPHA);//第一个编辑框几乎透明，负责获取焦点
		SetLayeredWindowAttributes(ed2, RGB(255, 255, 255), 1, LWA_COLORKEY);//第二个编辑框将背景色透明，用于展示文本内容

		hFont = CreateFont(25, 0, 0, 0, 700, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, NONANTIALIASED_QUALITY, DEFAULT_PITCH | FF_SWISS, "等线");
		SendMessage(ed1, WM_SETFONT, (WPARAM)hFont, TRUE);
		SendMessage(ed2, WM_SETFONT, (WPARAM)hFont, TRUE);
	}

	static void edSHOW(int mState) {
		ShowWindow(eBack, mState);
		ShowWindow(ed2, mState);
		ShowWindow(ed1, mState);
		SetWindowPos(ed1, HWND_TOP, 0, 0, 0, 0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOREDRAW);
	}//隐藏，展示编辑框

	static void Synchronize() {

		LRESULT rst;
		SendMessage(ed1, WM_GETTEXT, 1000, (LPARAM)edt);
		SetWindowText(ed2, edt);
		rst = SendMessage(ed1, EM_GETSEL, 0, 0);
		CreateCaret(ed2, NULL, 2, 25);
		ShowCaret(ed2);
		SendMessage(ed2, EM_SETSEL, HIWORD(rst), LOWORD(rst));
		SendMessageA(ed2, EM_LINESCROLL, -200, 0);
		SendMessageA(ed2, EM_SCROLLCARET,0, 0);
		SendMessageA(ed1, EM_LINESCROLL, -200, 0);
		SendMessageA(ed1, EM_SCROLLCARET, 0, 0);
	}//将两个编辑框同步方法

	inline static void EonPaint() {
		DrawPic(eBack, x2, y2, rs, "PNG");
	}//绘制背景

	static LRESULT CALLBACK eProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
		PAINTSTRUCT ps;
		switch (Message)
		{

		case WM_PAINT:
			mHDC = BeginPaint(eBack, &ps);

			EonPaint();
			EndPaint(eBack, &ps);
			break;
		default:
			return DefWindowProc(hwnd, Message, wParam, lParam);   //让系统处理消息，这条语句一定要加上
		case WM_ERASEBKGND:
			return TRUE;
		}
		return 0;
	}//背景回调函数

	static LRESULT CALLBACK eProc1(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam) {
		switch (Message)
		{
		case WM_MOUSEMOVE:
			Synchronize();
		default:
			return CallWindowProc(oldProc1, hWnd, Message, wParam, lParam);
		}
	}

	static LRESULT CALLBACK eProc2(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam) {
		switch (Message)
		{
		default:
			return CallWindowProc(oldProc1, hWnd, Message, wParam, lParam);
		}
	}
};
UINT mEditor::rs = 0;
int mEditor::x1 = 0;
int mEditor::x2 = 0;
int mEditor::y1 = 0;
int mEditor::y2 = 0;
HWND mEditor::eBack = 0;
HWND mEditor::ed1 = 0;
HWND mEditor::ed2 = 0;
HFONT mEditor::hFont = 0;
WNDPROC mEditor::oldProc1=0;
WNDPROC mEditor::oldProc2 = 0;
char mEditor::edt[200] = "";


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
}//释放资源文件

void playclick2() {
	mciSendString("play data\\01\\Click.mp3", NULL, 0, NULL);
}//播放按钮声音

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



