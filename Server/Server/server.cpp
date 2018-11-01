#include <WinSock2.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#pragma comment(lib, "ws2_32.lib")
#include<iostream>
#include <fstream>
using namespace std;
#define number 10   //最大客户端连接数  
#define namesize 16
#define pwdsize 10
#define buffsize 1024
using namespace std;


WSADATA wsaData;
SOCKET sockSrv;//服务器套接字 

int port = 5099;//服务端端口 
char USERNAME[20][namesize];// = { "tony" ,"black" };//用户名数组 
char PASSWORD[20][pwdsize];// = { "123456","555555" };//密码数组 
int pt = 0;
//int pt = 2;
//1.登录  2.注册   3.转发   4.发送在线用户名单   5.群发   6.客户端发来的状态信息

typedef struct Client
{
	SOCKET csocket;      //客户端套接字
	char username[namesize];   //客户端用户名
	char password[pwdsize];    //客户端密码 
	int flag;     //标记该位置的状态，0表示空闲，1表示已连接但未登录成功或未登录，2表示已连接且已成功登录，
				  //3表示聊天中，此时dialog有效且表示想要或者正在通话的用户
	char dialog[namesize];
}Client;
Client client[number];//客户端 
HANDLE hThread[number];//客户端线程
					   //pthread_t pthread_id[number]; 
int ret;
//int coutn = 0;//记录连接个数 


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
	for (i = 0; i < length; i++)
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
	if (n < 0)
		return -1;
	int poisition = -1, tempn = 1;
	int len = strlen(buf);
	for (int i = 0; i < len; i++) {
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
//*************************************************************************************
//错误处理函数
void error(int m, SOCKET socket)
{
	memset(client[m].username, 0, sizeof(client[m].username));
	memset(client[m].password, 0, sizeof(client[m].password));
	client[m].csocket = 0;
	client[m].flag = 0;
	closesocket(client[m].csocket);
	CloseHandle(hThread[m]);
}
//*************************************************************************************
//从库中搜寻用户name，并返回位置
int search(char *name)
{
	/*for (int i = 0; i < number; i++)
	{
	//if((client[i].flag==2|| client[i].flag == 3)&&strcmp(client[i]))
	}*/
	return 0;
}
//*************************************************************************************
//用于登录的匹配函数或者注册的添加函数 
//登录成功返回1，登录失败（用户名或密码错误）返回2，登录失败（已经登录）返回3，其他原因登陆失败返回4，注册成功返回5，注册失败返回6
//poi为对应的存储位置 
int logreg(char*buf, int*poi)
{
	//cout << "fangfali " << strlen(buf) << endl;
	printf("已经进入logreg函数，buf=%s\n", buf);
	if (strlen(buf) <= 3)
		return 4;

	char *operation = substring(buf, 0, find(buf, '#', 1));
	if (strcmp(operation, "1") == 0) {
		//登录 
		*poi = -1;
		char *name = substring(buf, find(buf, '#', 1) + 1, find(buf, '#', 2) - find(buf, '#', 1) - 1);
		char *pass = substring(buf, find(buf, '#', 2) + 1, strlen(buf) - find(buf, '#', 1) - 1);
		//cout<<endl<<name<<"            "<<pass<<endl; 
		//printf("name=%s,password=%s",name,pass);
		for (int i = 0; i < number; i++)
		{
			if ((client[i].flag == 2 || client[i].flag == 3) && strcmp(name, client[i].username) == 0)
			{
				return 3;
			}
		}
		//进行比较，知道用户在数据库中的位置
		for (int i = 0; i < number; i++) {
			if (strcmp(name, USERNAME[i]) == 0 && strcmp(pass, PASSWORD[i]) == 0) {
				*poi = i;
				return 1;


			}
		}
		return 2;
	}
	else if (strcmp(operation, "2") == 0) {
		//注册 
		char *name = substring(buf, find(buf, '#', 1) + 1, find(buf, '#', 2) - find(buf, '#', 1) - 1);
		char *pass = substring(buf, find(buf, '#', 2) + 1, strlen(buf) - find(buf, '#', 1) - 1);
		strcpy_s(USERNAME[pt], name);
		strcpy_s(PASSWORD[pt], pass);
		pt++;
		ofstream in;
		in.open("username&pwd.txt", ios::app);//打开record.txt文件，以ios::app追加的方式输入
											  //ios::trunc表示在打开文件前将文件清空,由于是写入,文件不存在则创建
		in << pt << "#" << USERNAME[pt - 1] << "#" << PASSWORD[pt - 1] << "\n";
		in.close();//关闭文件
		return 5;
	}
}
//*************************************************************************************
//群发函数，发送给除了m处的所有用户，错误返回-1，成功返回1
int sendtogroup(char *word, int m)
{
	char buff[buffsize];
	sprintf_s(buff, buffsize, "5#%s#%s", client[m].username, word);
	for (int i = 0; i < number; i++)
	{
		if (i != m && client[i].flag == 2)
		{
			if (send(client[i].csocket, buff, buffsize, 0) <= 0)
			{
				return -1;
			}
		}
	}
	return 1;
}
//*************************************************************************************
//检测用户在线的函数，返回在线人数，并将在线用户的信息储存在online中
int Scan(char *online)
{
	char temp[1024];
	memset(temp, 0, sizeof(temp));
	strcpy_s(online, 3, "4#");
	int num = 0;
	for (int i = 0; i < number; i++)
	{
		if (client[i].flag == 2)//检查状态为在线的用户
		{
			num++;
			sprintf_s(temp, namesize + 5, "2.%s.#", client[i].username);//4#2.tony#5.tom#
			strcat_s(online, strlen(online) + strlen(temp) + 1, temp);
		}
		if (client[i].flag == 3) {
			num++;
			sprintf_s(temp, 1024, "3.%s.%s#", client[i].username, client[i].dialog);//4#2.tony#5.tom#
			strcat_s(online, strlen(online) + strlen(temp) + 1, temp);
		}
	}
	//printf("方法里打印在线人数：%s\n",online);
	return num;
}
//*************************************************************************************
//检测客户端是否断开连接的函数
DWORD WINAPI manager(PVOID p)
{
	while (1)
	{
		Sleep(1000); //1s检查一次
					 //printf("新一轮的检测\n");
		for (int i = 0; i < number; i++)
		{
			if (client[i].flag == 2 || client[i].flag == 3)
			{
				//printf("第%d个位置被占用，检测它.\n",i);

				int temp = send(client[i].csocket, "", 0, 0);
				//printf("发送了%d个数据给%d号位置\n",temp,i);
				if (temp == SOCKET_ERROR)
				{
					printf("检测到用户“%s”（位置=%d；SOCKET=%d）已经离线。\n", client[i].username, i, client[i].csocket);

					error(i, client[i].csocket);
					char online[buffsize];
					memset(online, 0, sizeof(online));
					//某用户离线，给其他在线用户发
					int num = Scan(online);//查找在线用户并群发
					for (int i = 0; i < number; i++)
					{
						if (client[i].flag == 2)
						{
							if (send(client[i].csocket, online, buffsize, 0) <= 0)
							{
								//printf("");
							}
						}
					}
					printf("在线人数：%d  内容：%s\n", num, online);

				}
			}
			else if (client[i].flag == 1)
			{
				int temp = send(client[i].csocket, "", 0, 0);
				//printf("发送了%d个数据给%d号位置\n",temp,i);
				if (temp == SOCKET_ERROR)
				{
					printf("检测到SOCKET=%d客户端连接中止\n", client[i].csocket);
					error(i, client[i].csocket);
				}
			}
		}

	}

	return 0;
}

//*************************************************************************************
//线程
DWORD WINAPI handle(PVOID p)
{

	SOCKET sockConing = *((SOCKET*)p);
	int m;//以循环的方式查看新建立的连接在哪一个位置 
	for (m = 0; m < number; m++) {
		if (sockConing == client[m].csocket) {
			break;
		}
	}

	printf("客户端SOCKET=%d已连接\n", sockConing);
	char buffCon[buffsize];
	while (1)
	{

		memset(buffCon, 0, sizeof(buffCon));
		if (recv(sockConing, buffCon, sizeof(buffCon), 0) <= 0) {
			memset(buffCon, 0, sizeof(buffCon));
			printf("SOCKET=%d客户端连接中止\n", sockConing);
			error(m, sockConing);
			return 0;
		}
		//收到信息，登录或者注册

		int poi;
		//puts(buffCon);
		//printf("buffCon=%s\n",buffCon);
		//printf("strlen(buffCon)=%d\n",strlen(buffCon));


		ret = logreg(buffCon, &poi);  //
									  //printf("ret=%d", ret); 
		if (ret == 1) {//如果登录成功（登录成功时，poi>=0） 
			if (poi >= 0) {
				//printf("客户端SOCKET=%d登录成功！位置为%d\n", sockConing, m);

				client[m].flag = 2;//修改状态为成功登录
				strcpy_s(client[m].username, USERNAME[poi]);//装入用户名
				strcpy_s(client[m].password, PASSWORD[poi]);//装入密码
				printf("用户“%s”（位置=%d；SOCKET=%d）登录成功！\n", client[m].username, m, client[m].csocket);
				memset(buffCon, 0, sizeof(buffCon));
				strcpy_s(buffCon, "s");//登录成功！
				if (send(sockConing, buffCon, sizeof(buffCon), 0) <= 0) {
					memset(buffCon, 0, sizeof(buffCon));
					printf("用户“%s”（位置=%d；SOCKET=%d）已经离线！\n", client[m].username, m, client[m].csocket);
					error(m, sockConing);

					return 0;
				}
				memset(buffCon, 0, sizeof(buffCon));

				break;


			}




		}
		else if (ret == 2) {//如果是因为用户名或密码错误而登录不成功 
			memset(buffCon, 0, sizeof(buffCon));
			strcpy_s(buffCon, "f");//用户名或密码错误！
			if (send(sockConing, buffCon, sizeof(buffCon), 0) <= 0) {
				memset(buffCon, 0, sizeof(buffCon));
				printf("SOCKET=%d客户端连接中止\n", sockConing);
				error(m, sockConing);
				return 0;
			}
			memset(buffCon, 0, sizeof(buffCon));
			//closesocket(client[m].csocket);
			//client[m].csocket = 0;
			//client[m].flag = 0;
			printf("SOCKET=%d客户端登录失败\n", sockConing);
			memset(buffCon, 0, sizeof(buffCon));


		}
		else if (ret == 3) {
			memset(buffCon, 0, sizeof(buffCon));
			strcpy_s(buffCon, "a");//你已经登录！
			if (send(sockConing, buffCon, sizeof(buffCon), 0) <= 0) {
				memset(buffCon, 0, sizeof(buffCon));
				printf("SOCKET=%d客户端连接中止\n", sockConing);
				error(m, sockConing);
				return 0;
			}
			memset(buffCon, 0, sizeof(buffCon));
			printf("SOCKET=%d客户端登录失败\n", sockConing);
			memset(buffCon, 0, sizeof(buffCon));
		}
		else if (ret == 4)
		{
			memset(buffCon, 0, sizeof(buffCon));
			strcpy_s(buffCon, "o");//其他错误！\n
			if (send(sockConing, buffCon, sizeof(buffCon), 0) <= 0) {
				memset(buffCon, 0, sizeof(buffCon));
				printf("SOCKET=%d客户端连接中止\n", sockConing);
				error(m, sockConing);
				return 0;
			}
			memset(buffCon, 0, sizeof(buffCon));
			printf("SOCKET=%d客户端登录失败\n", sockConing);
			memset(buffCon, 0, sizeof(buffCon));
		}
		else if (ret == 5) {
			memset(buffCon, 0, sizeof(buffCon));
			strcpy_s(buffCon, "r");//其他错误！\n
			if (send(sockConing, buffCon, sizeof(buffCon), 0) <= 0) {
				memset(buffCon, 0, sizeof(buffCon));
				printf("SOCKET=%d客户端连接中止\n", sockConing);
				error(m, sockConing);
				return 0;
			}
			memset(buffCon, 0, sizeof(buffCon));
			printf("注册成功，用户名:%s 密码：%s\n", USERNAME[pt - 1], PASSWORD[pt - 1]);
			memset(buffCon, 0, sizeof(buffCon));
		}
	}








	memset(buffCon, 0, sizeof(buffCon));
	//登录成功，首先将当前在线人数发送给客户端
	int num = Scan(buffCon);//查找在线用户并群发


	for (int i = 0; i < number; i++)
	{
		if (client[i].flag == 2)
		{
			if (send(client[i].csocket, buffCon, buffsize, 0) <= 0)
			{
				return -1;
			}
		}
	}
	printf("在线人数：%d  内容：%s\n", num, buffCon);


	while (1)
	{
		//printf("收到的消息：");
		memset(buffCon, 0, sizeof(buffCon));
		if (recv(sockConing, buffCon, sizeof(buffCon), 0) <= 0) {
			printf("检测到用户“%s”（位置=%d；SOCKET=%d）已经离线。\n", client[m].username, m, client[m].csocket);
			memset(buffCon, 0, sizeof(buffCon));
			error(m, sockConing);
			//某用户离线，给其他在线用户发
			int num = Scan(buffCon);//查找在线用户并群发
			for (int i = 0; i < number; i++)
			{
				if (client[i].flag == 2|| client[i].flag == 3)
				{
					if (send(client[i].csocket, buffCon, buffsize, 0) <= 0)
					{
						//printf("");
					}
				}
			}
			printf("在线人数：%d  内容：%s\n", num, buffCon);
			
			return 0;
		}
		//开始拆分  3#tony#hello  4#     5#hello,everyone!
		//从字符串buf中寻找第n次出现字符c的位置并返回，没有则返回-1 
		//cout << "收到的消息：" << buffCon << endl;
		char *operation = substring(buffCon, 0, find(buffCon, '#', 1));
		if (strcmp(operation, "3") == 0) {//转发消息
			char *name = substring(buffCon, find(buffCon, '#', 1) + 1, find(buffCon, '#', 2) - find(buffCon, '#', 1) - 1);
			char *word = substring(buffCon, find(buffCon, '#', 2) + 1, strlen(buffCon) - find(buffCon, '#', 2) - 1);
			//cout << "name=" << name << "   word=" << word << endl;
			int sendflag = 0;//是否转发成功的标志
							 //cout << client[m].username << "发送消息给"<<name<<"：" << word<<endl;
			for (int i = 0; i < number; i++)
			{
				if (client[i].flag == 3 && strcmp(name, client[i].username) == 0) {
					memset(buffCon, 0, sizeof(buffCon));
					sprintf_s(buffCon, buffsize, "3#%s#%s", client[m].username, word);

					//通过用户名查询对应的SOCKET函数
					if (send(client[i].csocket, buffCon, sizeof(buffCon), 0) <= 0) {
						printf("检测到用户“%s”（位置=%d；SOCKET=%d）已经离线。\n", client[i].username, i, client[i].csocket);
						memset(buffCon, 0, sizeof(buffCon));
						error(i, client[i].csocket);
						//某用户离线，给其他在线用户发
						int num = Scan(buffCon);//查找在线用户并群发
						for (int j = 0; j < number; j++)
						{
							if (client[j].flag == 2 || client[j].flag == 3)
							{
								if (send(client[j].csocket, buffCon, buffsize, 0) <= 0)
								{
									//printf("");
								}
							}
						}
						printf("在线人数：%d  内容：%s\n", num, buffCon);
						memset(buffCon, 0, sizeof(buffCon));
						printf("转发消息（%s->%s）失败：%s\n", client[m].username, name, word);
						//error(m, sockConing);
						//return 0;
					}
					else {
						printf("转发消息（%s->%s）：%s\n", client[m].username, name, word);
						sendflag = 1;
					}

				}
			}
		}
		else if (strcmp(operation, "4") == 0) {//给指定的客户端发送当前在线用户名单
			memset(buffCon, 0, sizeof(buffCon));
			Scan(buffCon);
			printf("用户“%s”查询了在线人数：%s\n", client[m].username, buffCon);
			if (send(sockConing, buffCon, sizeof(buffCon), 0) <= 0) {
				printf("检测到用户“%s”（位置=%d；SOCKET=%d）已经离线。\n", client[m].username, m, client[m].csocket);
				memset(buffCon, 0, sizeof(buffCon));
				error(m, sockConing);
				//某用户离线，给其他在线用户发
				int num = Scan(buffCon);//查找在线用户并群发
				for (int i = 0; i < number; i++)
				{
					if (client[i].flag == 2 || client[i].flag == 3)
					{
						if (send(client[i].csocket, buffCon, buffsize, 0) <= 0)
						{
							//printf("");
						}
					}
				}
				printf("在线人数：%d  内容：%s\n", num, buffCon);
				
				return 0;
			}
		}
		else if (strcmp(operation, "5") == 0) {//群发消息
			char *word = substring(buffCon, find(buffCon, '#', 1) + 1, strlen(buffCon) - find(buffCon, '#', 1) - 1);
			memset(buffCon, 0, sizeof(buffCon));
			sprintf_s(buffCon, buffsize, "5#%s", word);
			printf("用户（%s）群发消息：%s\n", client[m].username, buffCon);
			sendtogroup(word, m);
		}
		else if (strcmp(operation, "6") == 0) {//一些信息
											   //6#black 准备要与black聊天
			char *name = substring(buffCon, find(buffCon, '#', 1) + 1, strlen(buffCon) - find(buffCon, '#', 1) - 1);
			client[m].flag = 3;
			strcpy_s(client[m].dialog, name);
			printf("用户%s选择和%s聊天\n", client[m].username, client[m].dialog);
		}
		else if (strcmp(operation, "7") == 0) {
			client[m].flag = 2;
			printf("用户%s退出和%s聊天\n", client[m].username, client[m].dialog);
			memset(client[m].dialog,0,sizeof(client[m].dialog));


		}
	}

	;

}




//*************************************************************************************
int main()
{

	/*ofstream in;
	in.open("username&pwd.txt", ios::app);//打开record.txt文件，以ios::app追加的方式输入
	//ios::trunc表示在打开文件前将文件清空,由于是写入,文件不存在则创建

	char USERNAME[20][namesize] = { "tony" ,"black" };//用户名数组
	char PASSWORD[20][pwdsize] = { "123456","555555" };//密码数组
	int pt = 2;
	char tname[10][16];
	char tpwd[10][16];

	for (int i = 0; i < pt; i++) {
	in << i + 1 << "#" << USERNAME[i] << "#" << PASSWORD[i] << "\n";
	}
	in.close();//关闭文件*/
	//cout << "开始加载用户...\n";
	char nampwd[256];
	fstream out;
	out.open("username&pwd.txt");
	while (!out.eof())
	{
		out.getline(nampwd, 256, '\n');//getline(char *,int,char) 表示该行字符达到256个或遇到换行就结束
									   //cout << nampwd <<strlen(nampwd)<< endl;
		if (strlen(nampwd) == 0) {
			continue;
		}
		char *name = substring(nampwd, find(nampwd, '#', 1) + 1, find(nampwd, '#', 2) - find(nampwd, '#', 1) - 1);
		char *pwd = substring(nampwd, find(nampwd, '#', 2) + 1, strlen(nampwd) - find(nampwd, '#', 2) - 1);
		//cout << "name=" << name << "   pwd=" << pwd << endl;
		strcpy_s(USERNAME[pt], name);
		strcpy_s(PASSWORD[pt], pwd);
		pt++;

	}
	out.close();
	//system("pause");
	//cout << "加载成功\n";
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) //加载套接字库
	{
		printf("Failed to load Winsock");
		return 0;
	}

	//创建用于监听的套接字
	sockSrv = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN addrSrv;
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(port); //1024以上的端口号
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY); //INADDR_ANY 代表任意ip


	if (bind(sockSrv, (LPSOCKADDR)&addrSrv, sizeof(SOCKADDR_IN)) == SOCKET_ERROR) {
		printf("Failed bind:%d\n", WSAGetLastError());
		return 0;
	}

	if (listen(sockSrv, 10) == SOCKET_ERROR) {//10代表允许连接的个数
		printf("Listen failed:%d", WSAGetLastError());
		return 0;
	}



	printf("等待客户端接入...\n");
	//HANDLE scan = CreateThread(NULL, 0, manager, NULL, 0, 0);//启动线程检测客户端是否关闭了连接
	while (1) {
		SOCKADDR_IN addrClient;
		memset(&addrClient, 0, sizeof(addrClient));
		int len = sizeof(SOCKADDR);

		SOCKET sockConn = accept(sockSrv, (SOCKADDR *)&addrClient, &len);

		for (int i = 0; i < number; i++) {
			if (client[i].flag == 0) {
				client[i].csocket = sockConn;
				client[i].flag = 1;//先置1
				hThread[i] = CreateThread(NULL, 0, handle, &sockConn, 0, 0);//启动线程进行通信
				break;
			}
			if (i == number - 1) {
				char temp[buffsize] = "连接已满，不能进行连接\n";
				send(sockConn, temp, sizeof(temp), 0);
				closesocket(sockConn);
			}
		}



	}



	WSACleanup();
	system("pause");
}