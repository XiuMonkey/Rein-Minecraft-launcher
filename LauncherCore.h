#define _CRT_SECURE_NO_WARNINGS
#define VALUE 1
#undef UNICODE
#undef _UNICODE
#include <stdio.h>
#include <string>
#include <io.h>
#include <vector>
#include <fstream>
#include <tchar.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <ctype.h>
#include "CJson.h"
string Rootpath;
vector<string>versions;//定义版本数组

void getFileNames(string path, vector<string>& files)
{
	intptr_t hFile = 0;
	struct _finddata_t fileinfo; string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			files.push_back(p.assign(fileinfo.name));
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}

string GetRootPath() {
	char opath0[5000];
	char fopath[5000] = {};
	sprintf(fopath, "%s", __targv[0]);//为opath(original path根目录)赋值
	int time1 = 0;
	int time2 = 0;
	for (int i = 0; i < strlen(fopath); i++) {
		if (fopath[i] == '\\')
		{
			time1++;
		}
	}
	for (int i = 0; time2 < time1; i++) {
		if (fopath[i] == '\\')
		{
			time2++;
		}
		opath0[i] = fopath[i];
	}
	opath0[strlen(opath0) - 1] = '\0';
	return string(opath0);
}


class LaunchConfig {
public:
	string UserName;
	string versionName;
	string gameDir;
	string UUID;
	string Memory;
	string JavaPath;
	int LaunchGame() {
		vector<string>versions;//定义版本数组
		char* json_string;//保存游戏json的字符串，也是库地址的储存地方
		json_string= (char*)malloc(70000);
		strcpy(json_string, "");
		char RootPath[500] = {};
		const char* JavaPathStr = JavaPath.c_str();
		int arr_size;
		char filename2[200] = {};
		char* cs5;
		char a5[3000] = {};
		char *d;
		d=(char*)malloc(10000);
		strcpy(d, "");
		char* b5;
		char a[600] = {};
		char Launch[7000] = { "/c cd /D " };
		char neicun[50] = "";

		cJSON* cjson_skill_item = NULL;
		cJSON* cs;
		cJSON* cs2;
		cJSON* cs3;
		cJSON* cs4;

		strcpy(RootPath, Rootpath.c_str());

#pragma region 获取.json文件的内容
		char filename[300] = {};
		strcpy(filename, RootPath);
		strcat(filename, "\\.minecraft\\versions\\");
		strcat(filename, versionName.c_str());
		strcat(filename, "\\");
		gameDir = filename;
		strcat(filename, versionName.c_str());
		strcat(filename, ".json");
		FILE* json;
		char name[500];
		json = fopen(filename, "r");
		if (nullptr == json) {
			free(json_string);
			return 1;
		}
		while (!feof(json))
		{
			fgets(name, 500, json);
			int i = strlen(name);
			strcat(json_string, name);
		}
		std::fclose(json);
#pragma endregion
#pragma region 重命名回去

#pragma endregion
#pragma region 一步步解析json
		char* jss = json_string;//以char指针保存json
		cJSON* cjson = cJSON_Parse(jss);//解析json为cjson格式
		cs = cJSON_GetObjectItem(cjson, "libraries");//获取libraries数组
		char* AIID = cJSON_GetObjectItem(cjson, "assets")->valuestring;
		char cAIID[10];
		strcpy(cAIID, AIID);
		char* assetsversion0 = cJSON_GetObjectItem(cjson, "assets")->valuestring;
		arr_size = cJSON_GetArraySize(cs);//获取数组项数
		char* str;
		char opath1[1000] = "";
		for (int i = 0; i < arr_size; i++)//循环获取libraries各项
		{
			cs2 = cJSON_GetArrayItem(cs, i);//从cs数组（libraries的变量）中提取第i项
			str = strstr(cJSON_Print(cs2), "artifact");//定义一个判断项中是否有artifact项的变量
			cs3 = cJSON_GetObjectItem(cs2, "downloads");//从artifact中提取downloads
			if (str) //如果str为真（有artifact）
			{
				strcpy(opath1, RootPath);//再次为opath赋值
				strcpy(a, strcat(opath1, "\\.minecraft\\libraries\\"));//重新为a（保存"的值）赋值
				cs4 = cJSON_GetObjectItem(cs3, "artifact");//获取artifact键
				cs5 = cJSON_GetObjectItem(cs4, "path")->valuestring;//获取path值并指向一个字符串
				strcpy(a5, cs5);//将a5后面加上path值
				b5 = strcat(a5, ";");//再在a5后加入;
				strcat(d, strcat(a, b5));//拼接完成加在d的后面
			}
		}
		char versionid[10];
		char assetsversion[10];
		strcpy(assetsversion, assetsversion0);
		int jsonlen = strlen(d);
		char jsoni;
		for (int i = 0; i < jsonlen; i++) { jsoni = d[i]; if (jsoni == '/') { d[i] = '\\'; } }//Replace "/" to "\"
		cJSON_Delete(cjson);
#pragma endregion
		strcat(strcat(strcat(strcat(Launch, RootPath), "\\.minecraft\\&&\""), JavaPathStr), "\" -Xmn128m -Xmx");
		strcat(strcat(Launch, Memory.c_str()), "m -Djava.library.path=\"");
		strcat(strcat(strcat(strcat(strcat(strcat(Launch, gameDir.c_str()), "natives\" -cp \""), d), gameDir.c_str()), versionName.c_str()), ".jar\" ");
		strcat(Launch, " net.minecraft.client.main.Main --username ");
		strcat(strcat(Launch, UserName.c_str()), " --version 1.0 --gameDir \"");
		strcat(strcat(Launch, RootPath), "\\.minecraft\" --assetsDir \"");
		strcat(strcat(Launch, RootPath), "\\.minecraft\\assets\" --assetIndex ");
		strcat(Launch, AIID);
		strcat(Launch, " --uuid 15468a55d6633077a691aed0be0ffacf --accessToken 60752adf5e3149a3b1cf571c01f81a5f --userProperties {} --userType mojang --width 854 --height 480");
		ShellExecute(NULL, "runas", "cmd", Launch, NULL, SW_SHOW);
		cout << "";//在这个项目里面必须有一个cout，我也不知为啥，不然报错(或者cin等)
		free(json_string);
		return 0;
	}
};















