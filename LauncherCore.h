#define _CRT_SECURE_NO_WARNINGS
#define VALUE 1
#undef UNICODE
#undef _UNICODE
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <string>
#include <io.h>
#include <vector>
#include <fstream>
#include <tchar.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>
#include <limits.h>
#include <ctype.h>
#include "CJson.h"
using namespace std;



char version[100];//要求用户输入版本
vector<string>versions;//定义版本数组
char json_string[70000] = {};//保存游戏json的字符串，也是库地址的储存地方
char opath[5000] = {};
char ddd[] = "111";
#pragma region json各级定义
cJSON* cjson_skill_item = NULL;
cJSON* cs;
cJSON* cs2;
cJSON* cs3;
cJSON* cs4;
int ren2;
int arr_size;
char* cs5;
char a5[3000] = {};
char d[10000];
char* b5;
char a[600] = {",\""};
char c[3000] = {};
char *cc;
char lancher[30000] = "cd /D \"";
char neicun[50]="";

#pragma endregion

void getFileNames(string path, vector<string>& files)
{
	intptr_t hFile = 0;
	struct _finddata_t fileinfo;string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			files.push_back(p.assign(fileinfo.name));
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}
//定义一个获取目录下所有文件名并保存到数组的函数

void getopath(char opath0[5000]) {
	char fopath[5000] = {};
	sprintf(fopath, "%s", __targv[0]);//为opath(original path根目录)赋值
	int time1 = 0;
	int time2 = 0;
	for (int i = 0;i < strlen(fopath);i++) {
		if (fopath[i] == '\\')
		{
			time1++;
		}
	}
	for (int i = 0;time2 < time1;i++) {
		if (fopath[i] == '\\')
		{
			time2++;
		}
		opath0[i] = fopath[i];
	}
	opath0[strlen(opath) - 1] = '\0';
}


int launch() {
#pragma region 获取启动所需信息
	char versionopath[5000];
	strcpy(versionopath, opath);
	strcat(versionopath, "\\.minecraft\\versions");
	getFileNames(versionopath, versions);
	cout << "版本列表：";
	for (int a = 2; a < versions.size(); a++) { std::cout << versions[a] << ","; }//获取单个文件
	cout << "你想要启动哪个版本?\n";
	cin >> version;
	printf("请输入java路径");
	char javapath1[1000] = "";
	char javapath[1000] = "\"";
	std::cin >> javapath1;
	printf("你想要分配多少内存？");
	cin >> neicun;
	strcat(javapath, javapath1);
	strcat(javapath, "\"");
#pragma endregion
#pragma region 重命名
	int ren;
	char jp1[150]= ".minecraft\\versions\\", jsonpath1[150] = ".minecraft\\versions\\";
	char jp2[150] = "\\", jsonpath2[150] = "\\";
	char jp3[150] = ".txt", jsonpath3[150] = ".json";
	char version2[150];
	char* on=strcat(strcat(strcat(strcat(jsonpath1, version),jsonpath2),version),jsonpath3);
	char* nn =strcat(strcat(strcat(strcat(jp1, version), jp2), version),jp3);
	ren = rename(on, nn);
#pragma endregion
#pragma region 获取.json文件的内容
	char* filename = ".minecraft\\versions\\1.12.2\\1.12.2.txt";
	FILE* json;
	char name[500];
	json = fopen(nn, "r");
	while (!feof(json))
	{
		fgets(name, 500, json);
		int i = strlen(name);
		strcat(json_string, name);
	}
	std::fclose(json);
#pragma endregion
#pragma region 重命名回去
	int ren2;
	ren2 = rename(nn, on);
#pragma endregion
#pragma region 一步步解析json
	char* jss = json_string;//以char指针保存json
	cJSON* cjson = cJSON_Parse(jss);//解析json为cjson格式
	cJSON* test_1_string = cJSON_GetObjectItem(cjson, "patches");//解析patches数组
	cjson_skill_item = cJSON_GetArrayItem(test_1_string, 0);//获取patches数组子项
	cs = cJSON_GetObjectItem(cjson_skill_item, "libraries");//获取libraries数组
	arr_size = cJSON_GetArraySize(cs);//获取数组项数
	char* str;
	char opath1[1000]="";
	for (int i = 0; i < arr_size; i++)//循环获取libraries各项
	{
		cs2 = cJSON_GetArrayItem(cs, i);//从cs数组（libraries的变量）中提取第i项
		str=strstr(cJSON_Print(cs2), "artifact");//定义一个判断项中是否有artifact项的变量
		cs3 = cJSON_GetObjectItem(cs2, "downloads");//从artifact中提取downloads
		if (str) //如果str为真（有artifact）
		{
			strcpy(opath1, opath);//再次为opath赋值
			strcpy(a, strcat(opath1,"\\.minecraft\\libraries\\"));//重新为a（保存"的值）赋值
			cs4 = cJSON_GetObjectItem(cs3, "artifact");//获取artifact键
			cs5 = cJSON_GetObjectItem(cs4, "path")->valuestring;//获取path值并指向一个字符串
			strcpy(a5, cs5);//将a5后面加上path值
			b5 = strcat(a5, ";");//再在a5后加入;
			strcat(d, strcat(a ,b5));//拼接完成加在d的后面
		}
	}
	d[strlen(d) - 1] = '\0';
	int jsonlen = strlen(d);
	char jsoni;
	for (int i = 0; i < jsonlen; i++) { jsoni = d[i]; if (jsoni == '/') { d[i] = '\\'; } }//Replace "/" to "\"
	cJSON_Delete(cjson);
	d[strlen(d) - 1] = '\0';
	std::cout << d<<"\n";
#pragma endregion
	strcat(strcat(strcat(strcat(lancher, opath),"\\minecraft\"\n"),javapath)," -Dfile.encoding=GB18030 -Dminecraft.client.jar=.minecraft\\versions\\");
	strcat(strcat(strcat(strcat(lancher, version),"\\"),version),".jar -XX:+UnlockExperimentalVMOptions -XX:+UseG1GC -XX:G1NewSizePercent=20 -XX:G1ReservePercent=20 -XX:MaxGCPauseMillis=50 -XX:G1HeapRegionSize=16m -XX:-UseAdaptiveSizePolicy -XX:-OmitStackTraceInFastThrow -XX:-DontCompileHugeMethods -Xmn128m -Xmx");
	strcat(strcat(lancher, neicun),"m -Dfml.ignoreInvalidMinecraftCertificates=true -Dfml.ignorePatchDiscrepancies=true -Djava.rmi.server.useCodebaseOnly=true -Dcom.sun.jndi.rmi.object.trustURLCodebase=false -Dcom.sun.jndi.cosnaming.object.trustURLCodebase=false -Dlog4j2.formatMsgNoLookups=true \" - Dlog4j.configurationFile = ");
	strcat(strcat(strcat(strcat(lancher, opath),"\\.minecraft\\versions\\"),version),"\\log4j2.xml\" -\nXX:HeapDumpPath=MojangTricksIntelDriversForPerformance_javaw.exe_minecraft.exe.heapdump \" - Djava.library.path = ");
	strcat(strcat(strcat(strcat(lancher, opath),"\\.minecraft\\versions\\"),version),"\\natives-windows-x86_64\" - Dminecraft.launcher.brand = PR_Minecraft - Dminecraft.launcher.version = 3.5.3 - cp ");
	strcat(lancher, d);
	cout << lancher;
#pragma region 停止
	int stop;
	std::cin >> stop;
	return 0;
#pragma endregion
}









	




