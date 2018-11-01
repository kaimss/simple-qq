#include <WinSock2.h>
#include <stdio.h> 
#include <conio.h>
#pragma comment(lib, "ws2_32.lib")
#include<iostream>
#define namesize 16
#define passwordsize 10
#pragma warning(disable:4996)
using namespace std;
char name[namesize];
char password[passwordsize];
char selectflag[10][2];
char selectname[10][16];
char selectdialog[10][16];
int sel;
//*************************************************************************************
//截取字符串ch从pos位置开始的length长度的字符串并返回新字符串的地址 
char* substring(char* ch, int pos, int length)
{
	char* pch = ch;
	//定义一个字符指针，指向传递进来的ch地址。
	char* subch = (char*)calloc(sizeof(char), length + 1);
	//通过calloc来分配一个length长度的字符数组，返回的是字符指针。
	int i;
	//只有在C99下for循环中才可以声明变量，这里写在外面，提高兼容性。
	pch = pch + pos;
	//是pch指针指向pos位置。
	for (i = 0; i<length; i++)
	{
		subch[i] = *(pch++);
		//循环遍历赋值数组。
	}
	subch[length] = '\0';//加上字符串结束符。
	return subch;		//返回分配的字符数组地址。
}
//*************************************************************************************
//从字符串buf中寻找第n次出现字符c的位置并返回，没有则返回-1 
int find(char *buf, char c, int n)
{
	if (n<0)
		return -1;
	int poisition = -1, tempn = 1;
	int len = strlen(buf);
	for (int i = 0; i<len; i++) {
		if (buf[i] == c) {
			if (tempn == n) {
				poisition = i;
				break;
			}
			tempn++;
		}
	}
	return poisition;

}
int select(char *buff)
{
	//4#0.black#1.tony#
	printf(buff);
	printf("\n");
	int num = 0;//在线用户人数
	int numberofdian = 1;
	int numberofjing = 1;
	int m = find(buff, '#', 1), n = find(buff, '#', 2);
	for (int i = 0; m != -1 && n != -1 && i < 10; i++)
	{//
		char *sub1 = substring(buff, m + 1, find(buff, '.', numberofdian) - m - 1);//
		char *sub2 = substring(buff, find(buff, '.', numberofdian) + 1, find(buff, '.', numberofdian + 1) - find(buff, '.', numberofdian) - 1);
		char *sub3 = substring(buff, find(buff, '.', numberofdian + 1) + 1, n - find(buff, '.', numberofdian + 1) - 1);
		numberofdian += 2;
		numberofjing += 1;

		strcpy_s(selectflag[num], 5, sub1);
		strcpy_s(selectname[num], 20, sub2);
		strcpy_s(selectdialog[num], 20, sub3);
		

		num++;
		m = n;
		n = find(buff, '#', numberofjing + 1);
	}

	return num;
}
void refresh()
{
	printf("");
}

DWORD WINAPI  recv(PVOID p)
{
	SOCKET sockConing = *((SOCKET*)p);
	char buff[1024];
	memset(buff, 0, sizeof(buff));

	while (1)//3#0.tony#hello
	{

		memset(buff, 0, sizeof(buff));
		recv(sockConing, buff, sizeof(buff), 0);
		//printf("收到buff=%s\n", buff);
		char *operation = substring(buff, 0, find(buff, '#', 1));
		if (*operation == '4') {
			
			int num = select(buff); 
			printf("当前在线用户为：\n");
			for (int i = 0; i < num; i++)
			{
				//printf(selectid[i]);
				printf("%d", i);
				printf(".");
				printf(selectname[i]);
				if (strcmp(selectflag[i], "2")==0) {
					printf("  状态：在线");
				}
				else if (strcmp(selectflag[i], "3")==0) {
					printf("  状态：通话中（与%s）",selectdialog[i]);
				}
				printf("        ");
				

			}
			printf("\n");
		}
		else if (*operation == '3') {
			char *name = substring(buff, find(buff, '#', 1) + 1, find(buff, '#', 2) - find(buff, '#', 1) - 1);
			char *word = substring(buff, find(buff, '#', 2) + 1, strlen(buff) - find(buff, '#', 2) - 1);
			//if()
			printf("%s说：%s\n", name, word);
			//puts(buff);
		}
		
	}
}

int main()
{
	
	WORD sockVision = MAKEWORD(2, 2);
	//加载套接字
	WSADATA wsadata;
	if (WSAStartup(sockVision, &wsadata) != 0)
	{
		printf("WSAStartup failed\n");
		return 0;
	}
	if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wVersion) != 2) { // 检测是否2.2版本的socket库  
		WSACleanup();
		return 0;
	}

	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
	{
		printf("Failed to load Winsock");
		return 0;
	}

	SOCKADDR_IN addrSrv; //服务端地址
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(5099);
	addrSrv.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//192.168.43.11

	//创建客户端套接字
	SOCKET sockClient = socket(AF_INET, SOCK_STREAM, 0);//流步套接字
	if (SOCKET_ERROR == sockClient) {
		printf("Socket() error:%d", WSAGetLastError());
		return 0;
	}

	//向服务器发出连接请求
	if (connect(sockClient, (struct  sockaddr*)&addrSrv, sizeof(addrSrv)) == INVALID_SOCKET) {
		printf("Connect failed:%d", WSAGetLastError());
		return 0;
	}

	//登录或注册   
	
	char buff[1024];
	memset(buff, 0, sizeof(buff));
	char operation;
	while (1)
	{
		printf("1.登录 2.注册\n");

		scanf_s(" %c", &operation, 2);
		if (operation == '1') {
			//登录部分 
			printf("登录,请输入用户名和密码：");
			scanf_s(" %s", &name, namesize);
			scanf_s(" %s", &password, passwordsize);
			sprintf_s(buff, sizeof(buff), "%c#%s#%s", operation, name, password);
			//printf(buff);
			send(sockClient, buff, sizeof(buff), 0);
			memset(buff, 0, sizeof(buff));
			recv(sockClient, buff, sizeof(buff), 0);
			//printf(buff);
			if (strcmp(buff, "s") == 0) {//连接成功，进行通信
				printf("登录成功！\n");
				memset(buff, 0, sizeof(buff));
				break;
				
			}
			else if (strcmp(buff, "f") == 0) {//用户名或密码错误
				printf("用户名或密码错误！请重输：\n");
				memset(buff, 0, sizeof(buff));
			}
			else if (strcmp(buff, "a") == 0){//已经登录
				printf("你已经登录！请重输：\n");
			}
			else {//其他错误，断开连接
				printf("其他错误，断开连接\n");
				memset(buff, 0, sizeof(buff));
				return 0;
			}
		}
		//注册		
		else if (operation == '2') {
			printf("注册,请输入用户名和密码：");
			scanf_s(" %s", &name, namesize);
			scanf_s(" %s", &password, passwordsize);
			sprintf_s(buff, sizeof(buff), "2#%s#%s",  name, password);
			printf(buff);
			send(sockClient, buff, sizeof(buff), 0);
			memset(buff, 0, sizeof(buff));
			recv(sockClient, buff, sizeof(buff), 0);
			if (strcmp(buff, "r") == 0) {
				printf("成功注册\n");
			}
		}
		else {
			printf("operation=%c.",operation);
			printf("输入错误\n");
			
		}
		//printf("请选择：1.登录 2.注册\n");
	}
	HANDLE recieve = CreateThread(NULL, 0, recv, &sockClient, 0, 0);//启动接受信息的函数




	



	
	while (1) 
	{
		printf("请选择你要进行的操作：3.发送 4.查询\n");
		scanf(" %s", &operation);
		char *online[1024];
		if (operation == '3'){
			//puts(buff); //1#2.tony#5.lingling#

			printf("请选择一位进行通信");
			cin >> sel;

			printf("\n已经进入与%s的聊天，输入'q'退出\n", selectname[sel]);
			//给服务端发消息表明进入与sel的聊天状态中
			memset(buff, 0, sizeof(buff));
			sprintf_s(buff,1024,"6#%s",selectname[sel]);
			send(sockClient, buff, sizeof(buff), 0);


			char word[1024];
			char temp[] = "q" ;

			while (1) 
			{
				memset(buff, 0, sizeof(buff));
				memset(word, 0, sizeof(word));
				cin.getline(word, 1024);//读入char数组
				//gets_s(word, 1024);
				if (strcmp(word, temp) == 0){
					memset(buff, 0, sizeof(buff));
					sprintf_s(buff, 1024, "7#");
					send(sockClient, buff, sizeof(buff), 0);
					break;
				}
					
				printf("\n");
				sprintf_s(buff, sizeof(buff), "3#%s#%s", selectname[sel], word);
				//cout << "包装好的消息：" << buff << endl;
				//cout << "word=" << word <<" strlen(word)="<<strlen(word)<<endl;
				if (strlen(word) > 0){
					if (send(sockClient, buff, sizeof(buff), 0) <= 0) {
						cout << "发送失败\n";
					}
				}
					
				
			} 
			printf("已经退出与%s的聊天\n", selectname[sel]);
			




		}
		else if (operation == '4') {
			memset(buff, 0, sizeof(buff));
			sprintf_s(buff, 1024, "4#");

			send(sockClient, buff, sizeof(buff), 0);
			

		}



		
	}
	closesocket(sockClient);
	WSACleanup();
}