#include <WinSock2.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#pragma comment(lib, "ws2_32.lib")
#include<iostream>
#include <fstream>
using namespace std;
#define number 10   //���ͻ���������  
#define namesize 16
#define pwdsize 10
#define buffsize 1024
using namespace std;


WSADATA wsaData;
SOCKET sockSrv;//�������׽��� 

int port = 5099;//����˶˿� 
char USERNAME[20][namesize];// = { "tony" ,"black" };//�û������� 
char PASSWORD[20][pwdsize];// = { "123456","555555" };//�������� 
int pt = 0;
//int pt = 2;
//1.��¼  2.ע��   3.ת��   4.���������û�����   5.Ⱥ��   6.�ͻ��˷�����״̬��Ϣ

typedef struct Client
{
	SOCKET csocket;      //�ͻ����׽���
	char username[namesize];   //�ͻ����û���
	char password[pwdsize];    //�ͻ������� 
	int flag;     //��Ǹ�λ�õ�״̬��0��ʾ���У�1��ʾ�����ӵ�δ��¼�ɹ���δ��¼��2��ʾ���������ѳɹ���¼��
				  //3��ʾ�����У���ʱdialog��Ч�ұ�ʾ��Ҫ��������ͨ�����û�
	char dialog[namesize];
}Client;
Client client[number];//�ͻ��� 
HANDLE hThread[number];//�ͻ����߳�
					   //pthread_t pthread_id[number]; 
int ret;
//int coutn = 0;//��¼���Ӹ��� 


//*************************************************************************************
//��ȡ�ַ���ch��posλ�ÿ�ʼ��length���ȵ��ַ������������ַ����ĵ�ַ 
char* substring(char* ch, int pos, int length)
{
	char* pch = ch;
	//����һ���ַ�ָ�룬ָ�򴫵ݽ�����ch��ַ��
	char* subch = (char*)calloc(sizeof(char), length + 1);
	//ͨ��calloc������һ��length���ȵ��ַ����飬���ص����ַ�ָ�롣
	int i;
	//ֻ����C99��forѭ���вſ�����������������д�����棬��߼����ԡ�
	pch = pch + pos;
	//��pchָ��ָ��posλ�á�
	for (i = 0; i < length; i++)
	{
		subch[i] = *(pch++);
		//ѭ��������ֵ���顣
	}
	subch[length] = '\0';//�����ַ�����������
	return subch;		//���ط�����ַ������ַ��
}
//*************************************************************************************
//���ַ���buf��Ѱ�ҵ�n�γ����ַ�c��λ�ò����أ�û���򷵻�-1 
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
//��������
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
//�ӿ�����Ѱ�û�name��������λ��
int search(char *name)
{
	/*for (int i = 0; i < number; i++)
	{
	//if((client[i].flag==2|| client[i].flag == 3)&&strcmp(client[i]))
	}*/
	return 0;
}
//*************************************************************************************
//���ڵ�¼��ƥ�亯������ע�����Ӻ��� 
//��¼�ɹ�����1����¼ʧ�ܣ��û�����������󣩷���2����¼ʧ�ܣ��Ѿ���¼������3������ԭ���½ʧ�ܷ���4��ע��ɹ�����5��ע��ʧ�ܷ���6
//poiΪ��Ӧ�Ĵ洢λ�� 
int logreg(char*buf, int*poi)
{
	//cout << "fangfali " << strlen(buf) << endl;
	printf("�Ѿ�����logreg������buf=%s\n", buf);
	if (strlen(buf) <= 3)
		return 4;

	char *operation = substring(buf, 0, find(buf, '#', 1));
	if (strcmp(operation, "1") == 0) {
		//��¼ 
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
		//���бȽϣ�֪���û������ݿ��е�λ��
		for (int i = 0; i < number; i++) {
			if (strcmp(name, USERNAME[i]) == 0 && strcmp(pass, PASSWORD[i]) == 0) {
				*poi = i;
				return 1;


			}
		}
		return 2;
	}
	else if (strcmp(operation, "2") == 0) {
		//ע�� 
		char *name = substring(buf, find(buf, '#', 1) + 1, find(buf, '#', 2) - find(buf, '#', 1) - 1);
		char *pass = substring(buf, find(buf, '#', 2) + 1, strlen(buf) - find(buf, '#', 1) - 1);
		strcpy_s(USERNAME[pt], name);
		strcpy_s(PASSWORD[pt], pass);
		pt++;
		ofstream in;
		in.open("username&pwd.txt", ios::app);//��record.txt�ļ�����ios::app׷�ӵķ�ʽ����
											  //ios::trunc��ʾ�ڴ��ļ�ǰ���ļ����,������д��,�ļ��������򴴽�
		in << pt << "#" << USERNAME[pt - 1] << "#" << PASSWORD[pt - 1] << "\n";
		in.close();//�ر��ļ�
		return 5;
	}
}
//*************************************************************************************
//Ⱥ�����������͸�����m���������û������󷵻�-1���ɹ�����1
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
//����û����ߵĺ����������������������������û�����Ϣ������online��
int Scan(char *online)
{
	char temp[1024];
	memset(temp, 0, sizeof(temp));
	strcpy_s(online, 3, "4#");
	int num = 0;
	for (int i = 0; i < number; i++)
	{
		if (client[i].flag == 2)//���״̬Ϊ���ߵ��û�
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
	//printf("�������ӡ����������%s\n",online);
	return num;
}
//*************************************************************************************
//���ͻ����Ƿ�Ͽ����ӵĺ���
DWORD WINAPI manager(PVOID p)
{
	while (1)
	{
		Sleep(1000); //1s���һ��
					 //printf("��һ�ֵļ��\n");
		for (int i = 0; i < number; i++)
		{
			if (client[i].flag == 2 || client[i].flag == 3)
			{
				//printf("��%d��λ�ñ�ռ�ã������.\n",i);

				int temp = send(client[i].csocket, "", 0, 0);
				//printf("������%d�����ݸ�%d��λ��\n",temp,i);
				if (temp == SOCKET_ERROR)
				{
					printf("��⵽�û���%s����λ��=%d��SOCKET=%d���Ѿ����ߡ�\n", client[i].username, i, client[i].csocket);

					error(i, client[i].csocket);
					char online[buffsize];
					memset(online, 0, sizeof(online));
					//ĳ�û����ߣ������������û���
					int num = Scan(online);//���������û���Ⱥ��
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
					printf("����������%d  ���ݣ�%s\n", num, online);

				}
			}
			else if (client[i].flag == 1)
			{
				int temp = send(client[i].csocket, "", 0, 0);
				//printf("������%d�����ݸ�%d��λ��\n",temp,i);
				if (temp == SOCKET_ERROR)
				{
					printf("��⵽SOCKET=%d�ͻ���������ֹ\n", client[i].csocket);
					error(i, client[i].csocket);
				}
			}
		}

	}

	return 0;
}

//*************************************************************************************
//�߳�
DWORD WINAPI handle(PVOID p)
{

	SOCKET sockConing = *((SOCKET*)p);
	int m;//��ѭ���ķ�ʽ�鿴�½�������������һ��λ�� 
	for (m = 0; m < number; m++) {
		if (sockConing == client[m].csocket) {
			break;
		}
	}

	printf("�ͻ���SOCKET=%d������\n", sockConing);
	char buffCon[buffsize];
	while (1)
	{

		memset(buffCon, 0, sizeof(buffCon));
		if (recv(sockConing, buffCon, sizeof(buffCon), 0) <= 0) {
			memset(buffCon, 0, sizeof(buffCon));
			printf("SOCKET=%d�ͻ���������ֹ\n", sockConing);
			error(m, sockConing);
			return 0;
		}
		//�յ���Ϣ����¼����ע��

		int poi;
		//puts(buffCon);
		//printf("buffCon=%s\n",buffCon);
		//printf("strlen(buffCon)=%d\n",strlen(buffCon));


		ret = logreg(buffCon, &poi);  //
									  //printf("ret=%d", ret); 
		if (ret == 1) {//�����¼�ɹ�����¼�ɹ�ʱ��poi>=0�� 
			if (poi >= 0) {
				//printf("�ͻ���SOCKET=%d��¼�ɹ���λ��Ϊ%d\n", sockConing, m);

				client[m].flag = 2;//�޸�״̬Ϊ�ɹ���¼
				strcpy_s(client[m].username, USERNAME[poi]);//װ���û���
				strcpy_s(client[m].password, PASSWORD[poi]);//װ������
				printf("�û���%s����λ��=%d��SOCKET=%d����¼�ɹ���\n", client[m].username, m, client[m].csocket);
				memset(buffCon, 0, sizeof(buffCon));
				strcpy_s(buffCon, "s");//��¼�ɹ���
				if (send(sockConing, buffCon, sizeof(buffCon), 0) <= 0) {
					memset(buffCon, 0, sizeof(buffCon));
					printf("�û���%s����λ��=%d��SOCKET=%d���Ѿ����ߣ�\n", client[m].username, m, client[m].csocket);
					error(m, sockConing);

					return 0;
				}
				memset(buffCon, 0, sizeof(buffCon));

				break;


			}




		}
		else if (ret == 2) {//�������Ϊ�û���������������¼���ɹ� 
			memset(buffCon, 0, sizeof(buffCon));
			strcpy_s(buffCon, "f");//�û������������
			if (send(sockConing, buffCon, sizeof(buffCon), 0) <= 0) {
				memset(buffCon, 0, sizeof(buffCon));
				printf("SOCKET=%d�ͻ���������ֹ\n", sockConing);
				error(m, sockConing);
				return 0;
			}
			memset(buffCon, 0, sizeof(buffCon));
			//closesocket(client[m].csocket);
			//client[m].csocket = 0;
			//client[m].flag = 0;
			printf("SOCKET=%d�ͻ��˵�¼ʧ��\n", sockConing);
			memset(buffCon, 0, sizeof(buffCon));


		}
		else if (ret == 3) {
			memset(buffCon, 0, sizeof(buffCon));
			strcpy_s(buffCon, "a");//���Ѿ���¼��
			if (send(sockConing, buffCon, sizeof(buffCon), 0) <= 0) {
				memset(buffCon, 0, sizeof(buffCon));
				printf("SOCKET=%d�ͻ���������ֹ\n", sockConing);
				error(m, sockConing);
				return 0;
			}
			memset(buffCon, 0, sizeof(buffCon));
			printf("SOCKET=%d�ͻ��˵�¼ʧ��\n", sockConing);
			memset(buffCon, 0, sizeof(buffCon));
		}
		else if (ret == 4)
		{
			memset(buffCon, 0, sizeof(buffCon));
			strcpy_s(buffCon, "o");//��������\n
			if (send(sockConing, buffCon, sizeof(buffCon), 0) <= 0) {
				memset(buffCon, 0, sizeof(buffCon));
				printf("SOCKET=%d�ͻ���������ֹ\n", sockConing);
				error(m, sockConing);
				return 0;
			}
			memset(buffCon, 0, sizeof(buffCon));
			printf("SOCKET=%d�ͻ��˵�¼ʧ��\n", sockConing);
			memset(buffCon, 0, sizeof(buffCon));
		}
		else if (ret == 5) {
			memset(buffCon, 0, sizeof(buffCon));
			strcpy_s(buffCon, "r");//��������\n
			if (send(sockConing, buffCon, sizeof(buffCon), 0) <= 0) {
				memset(buffCon, 0, sizeof(buffCon));
				printf("SOCKET=%d�ͻ���������ֹ\n", sockConing);
				error(m, sockConing);
				return 0;
			}
			memset(buffCon, 0, sizeof(buffCon));
			printf("ע��ɹ����û���:%s ���룺%s\n", USERNAME[pt - 1], PASSWORD[pt - 1]);
			memset(buffCon, 0, sizeof(buffCon));
		}
	}








	memset(buffCon, 0, sizeof(buffCon));
	//��¼�ɹ������Ƚ���ǰ�����������͸��ͻ���
	int num = Scan(buffCon);//���������û���Ⱥ��


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
	printf("����������%d  ���ݣ�%s\n", num, buffCon);


	while (1)
	{
		//printf("�յ�����Ϣ��");
		memset(buffCon, 0, sizeof(buffCon));
		if (recv(sockConing, buffCon, sizeof(buffCon), 0) <= 0) {
			printf("��⵽�û���%s����λ��=%d��SOCKET=%d���Ѿ����ߡ�\n", client[m].username, m, client[m].csocket);
			memset(buffCon, 0, sizeof(buffCon));
			error(m, sockConing);
			//ĳ�û����ߣ������������û���
			int num = Scan(buffCon);//���������û���Ⱥ��
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
			printf("����������%d  ���ݣ�%s\n", num, buffCon);
			
			return 0;
		}
		//��ʼ���  3#tony#hello  4#     5#hello,everyone!
		//���ַ���buf��Ѱ�ҵ�n�γ����ַ�c��λ�ò����أ�û���򷵻�-1 
		//cout << "�յ�����Ϣ��" << buffCon << endl;
		char *operation = substring(buffCon, 0, find(buffCon, '#', 1));
		if (strcmp(operation, "3") == 0) {//ת����Ϣ
			char *name = substring(buffCon, find(buffCon, '#', 1) + 1, find(buffCon, '#', 2) - find(buffCon, '#', 1) - 1);
			char *word = substring(buffCon, find(buffCon, '#', 2) + 1, strlen(buffCon) - find(buffCon, '#', 2) - 1);
			//cout << "name=" << name << "   word=" << word << endl;
			int sendflag = 0;//�Ƿ�ת���ɹ��ı�־
							 //cout << client[m].username << "������Ϣ��"<<name<<"��" << word<<endl;
			for (int i = 0; i < number; i++)
			{
				if (client[i].flag == 3 && strcmp(name, client[i].username) == 0) {
					memset(buffCon, 0, sizeof(buffCon));
					sprintf_s(buffCon, buffsize, "3#%s#%s", client[m].username, word);

					//ͨ���û�����ѯ��Ӧ��SOCKET����
					if (send(client[i].csocket, buffCon, sizeof(buffCon), 0) <= 0) {
						printf("��⵽�û���%s����λ��=%d��SOCKET=%d���Ѿ����ߡ�\n", client[i].username, i, client[i].csocket);
						memset(buffCon, 0, sizeof(buffCon));
						error(i, client[i].csocket);
						//ĳ�û����ߣ������������û���
						int num = Scan(buffCon);//���������û���Ⱥ��
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
						printf("����������%d  ���ݣ�%s\n", num, buffCon);
						memset(buffCon, 0, sizeof(buffCon));
						printf("ת����Ϣ��%s->%s��ʧ�ܣ�%s\n", client[m].username, name, word);
						//error(m, sockConing);
						//return 0;
					}
					else {
						printf("ת����Ϣ��%s->%s����%s\n", client[m].username, name, word);
						sendflag = 1;
					}

				}
			}
		}
		else if (strcmp(operation, "4") == 0) {//��ָ���Ŀͻ��˷��͵�ǰ�����û�����
			memset(buffCon, 0, sizeof(buffCon));
			Scan(buffCon);
			printf("�û���%s����ѯ������������%s\n", client[m].username, buffCon);
			if (send(sockConing, buffCon, sizeof(buffCon), 0) <= 0) {
				printf("��⵽�û���%s����λ��=%d��SOCKET=%d���Ѿ����ߡ�\n", client[m].username, m, client[m].csocket);
				memset(buffCon, 0, sizeof(buffCon));
				error(m, sockConing);
				//ĳ�û����ߣ������������û���
				int num = Scan(buffCon);//���������û���Ⱥ��
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
				printf("����������%d  ���ݣ�%s\n", num, buffCon);
				
				return 0;
			}
		}
		else if (strcmp(operation, "5") == 0) {//Ⱥ����Ϣ
			char *word = substring(buffCon, find(buffCon, '#', 1) + 1, strlen(buffCon) - find(buffCon, '#', 1) - 1);
			memset(buffCon, 0, sizeof(buffCon));
			sprintf_s(buffCon, buffsize, "5#%s", word);
			printf("�û���%s��Ⱥ����Ϣ��%s\n", client[m].username, buffCon);
			sendtogroup(word, m);
		}
		else if (strcmp(operation, "6") == 0) {//һЩ��Ϣ
											   //6#black ׼��Ҫ��black����
			char *name = substring(buffCon, find(buffCon, '#', 1) + 1, strlen(buffCon) - find(buffCon, '#', 1) - 1);
			client[m].flag = 3;
			strcpy_s(client[m].dialog, name);
			printf("�û�%sѡ���%s����\n", client[m].username, client[m].dialog);
		}
		else if (strcmp(operation, "7") == 0) {
			client[m].flag = 2;
			printf("�û�%s�˳���%s����\n", client[m].username, client[m].dialog);
			memset(client[m].dialog,0,sizeof(client[m].dialog));


		}
	}

	;

}




//*************************************************************************************
int main()
{

	/*ofstream in;
	in.open("username&pwd.txt", ios::app);//��record.txt�ļ�����ios::app׷�ӵķ�ʽ����
	//ios::trunc��ʾ�ڴ��ļ�ǰ���ļ����,������д��,�ļ��������򴴽�

	char USERNAME[20][namesize] = { "tony" ,"black" };//�û�������
	char PASSWORD[20][pwdsize] = { "123456","555555" };//��������
	int pt = 2;
	char tname[10][16];
	char tpwd[10][16];

	for (int i = 0; i < pt; i++) {
	in << i + 1 << "#" << USERNAME[i] << "#" << PASSWORD[i] << "\n";
	}
	in.close();//�ر��ļ�*/
	//cout << "��ʼ�����û�...\n";
	char nampwd[256];
	fstream out;
	out.open("username&pwd.txt");
	while (!out.eof())
	{
		out.getline(nampwd, 256, '\n');//getline(char *,int,char) ��ʾ�����ַ��ﵽ256�����������оͽ���
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
	//cout << "���سɹ�\n";
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) //�����׽��ֿ�
	{
		printf("Failed to load Winsock");
		return 0;
	}

	//�������ڼ������׽���
	sockSrv = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN addrSrv;
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(port); //1024���ϵĶ˿ں�
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY); //INADDR_ANY ��������ip


	if (bind(sockSrv, (LPSOCKADDR)&addrSrv, sizeof(SOCKADDR_IN)) == SOCKET_ERROR) {
		printf("Failed bind:%d\n", WSAGetLastError());
		return 0;
	}

	if (listen(sockSrv, 10) == SOCKET_ERROR) {//10�����������ӵĸ���
		printf("Listen failed:%d", WSAGetLastError());
		return 0;
	}



	printf("�ȴ��ͻ��˽���...\n");
	//HANDLE scan = CreateThread(NULL, 0, manager, NULL, 0, 0);//�����̼߳��ͻ����Ƿ�ر�������
	while (1) {
		SOCKADDR_IN addrClient;
		memset(&addrClient, 0, sizeof(addrClient));
		int len = sizeof(SOCKADDR);

		SOCKET sockConn = accept(sockSrv, (SOCKADDR *)&addrClient, &len);

		for (int i = 0; i < number; i++) {
			if (client[i].flag == 0) {
				client[i].csocket = sockConn;
				client[i].flag = 1;//����1
				hThread[i] = CreateThread(NULL, 0, handle, &sockConn, 0, 0);//�����߳̽���ͨ��
				break;
			}
			if (i == number - 1) {
				char temp[buffsize] = "�������������ܽ�������\n";
				send(sockConn, temp, sizeof(temp), 0);
				closesocket(sockConn);
			}
		}



	}



	WSACleanup();
	system("pause");
}