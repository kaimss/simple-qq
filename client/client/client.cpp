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
	for (i = 0; i<length; i++)
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
	int num = 0;//�����û�����
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
		//printf("�յ�buff=%s\n", buff);
		char *operation = substring(buff, 0, find(buff, '#', 1));
		if (*operation == '4') {
			
			int num = select(buff); 
			printf("��ǰ�����û�Ϊ��\n");
			for (int i = 0; i < num; i++)
			{
				//printf(selectid[i]);
				printf("%d", i);
				printf(".");
				printf(selectname[i]);
				if (strcmp(selectflag[i], "2")==0) {
					printf("  ״̬������");
				}
				else if (strcmp(selectflag[i], "3")==0) {
					printf("  ״̬��ͨ���У���%s��",selectdialog[i]);
				}
				printf("        ");
				

			}
			printf("\n");
		}
		else if (*operation == '3') {
			char *name = substring(buff, find(buff, '#', 1) + 1, find(buff, '#', 2) - find(buff, '#', 1) - 1);
			char *word = substring(buff, find(buff, '#', 2) + 1, strlen(buff) - find(buff, '#', 2) - 1);
			//if()
			printf("%s˵��%s\n", name, word);
			//puts(buff);
		}
		
	}
}

int main()
{
	
	WORD sockVision = MAKEWORD(2, 2);
	//�����׽���
	WSADATA wsadata;
	if (WSAStartup(sockVision, &wsadata) != 0)
	{
		printf("WSAStartup failed\n");
		return 0;
	}
	if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wVersion) != 2) { // ����Ƿ�2.2�汾��socket��  
		WSACleanup();
		return 0;
	}

	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
	{
		printf("Failed to load Winsock");
		return 0;
	}

	SOCKADDR_IN addrSrv; //����˵�ַ
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(5099);
	addrSrv.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//192.168.43.11

	//�����ͻ����׽���
	SOCKET sockClient = socket(AF_INET, SOCK_STREAM, 0);//�����׽���
	if (SOCKET_ERROR == sockClient) {
		printf("Socket() error:%d", WSAGetLastError());
		return 0;
	}

	//�������������������
	if (connect(sockClient, (struct  sockaddr*)&addrSrv, sizeof(addrSrv)) == INVALID_SOCKET) {
		printf("Connect failed:%d", WSAGetLastError());
		return 0;
	}

	//��¼��ע��   
	
	char buff[1024];
	memset(buff, 0, sizeof(buff));
	char operation;
	while (1)
	{
		printf("1.��¼ 2.ע��\n");

		scanf_s(" %c", &operation, 2);
		if (operation == '1') {
			//��¼���� 
			printf("��¼,�������û��������룺");
			scanf_s(" %s", &name, namesize);
			scanf_s(" %s", &password, passwordsize);
			sprintf_s(buff, sizeof(buff), "%c#%s#%s", operation, name, password);
			//printf(buff);
			send(sockClient, buff, sizeof(buff), 0);
			memset(buff, 0, sizeof(buff));
			recv(sockClient, buff, sizeof(buff), 0);
			//printf(buff);
			if (strcmp(buff, "s") == 0) {//���ӳɹ�������ͨ��
				printf("��¼�ɹ���\n");
				memset(buff, 0, sizeof(buff));
				break;
				
			}
			else if (strcmp(buff, "f") == 0) {//�û������������
				printf("�û�����������������䣺\n");
				memset(buff, 0, sizeof(buff));
			}
			else if (strcmp(buff, "a") == 0){//�Ѿ���¼
				printf("���Ѿ���¼�������䣺\n");
			}
			else {//�������󣬶Ͽ�����
				printf("�������󣬶Ͽ�����\n");
				memset(buff, 0, sizeof(buff));
				return 0;
			}
		}
		//ע��		
		else if (operation == '2') {
			printf("ע��,�������û��������룺");
			scanf_s(" %s", &name, namesize);
			scanf_s(" %s", &password, passwordsize);
			sprintf_s(buff, sizeof(buff), "2#%s#%s",  name, password);
			printf(buff);
			send(sockClient, buff, sizeof(buff), 0);
			memset(buff, 0, sizeof(buff));
			recv(sockClient, buff, sizeof(buff), 0);
			if (strcmp(buff, "r") == 0) {
				printf("�ɹ�ע��\n");
			}
		}
		else {
			printf("operation=%c.",operation);
			printf("�������\n");
			
		}
		//printf("��ѡ��1.��¼ 2.ע��\n");
	}
	HANDLE recieve = CreateThread(NULL, 0, recv, &sockClient, 0, 0);//����������Ϣ�ĺ���




	



	
	while (1) 
	{
		printf("��ѡ����Ҫ���еĲ�����3.���� 4.��ѯ\n");
		scanf(" %s", &operation);
		char *online[1024];
		if (operation == '3'){
			//puts(buff); //1#2.tony#5.lingling#

			printf("��ѡ��һλ����ͨ��");
			cin >> sel;

			printf("\n�Ѿ�������%s�����죬����'q'�˳�\n", selectname[sel]);
			//������˷���Ϣ����������sel������״̬��
			memset(buff, 0, sizeof(buff));
			sprintf_s(buff,1024,"6#%s",selectname[sel]);
			send(sockClient, buff, sizeof(buff), 0);


			char word[1024];
			char temp[] = "q" ;

			while (1) 
			{
				memset(buff, 0, sizeof(buff));
				memset(word, 0, sizeof(word));
				cin.getline(word, 1024);//����char����
				//gets_s(word, 1024);
				if (strcmp(word, temp) == 0){
					memset(buff, 0, sizeof(buff));
					sprintf_s(buff, 1024, "7#");
					send(sockClient, buff, sizeof(buff), 0);
					break;
				}
					
				printf("\n");
				sprintf_s(buff, sizeof(buff), "3#%s#%s", selectname[sel], word);
				//cout << "��װ�õ���Ϣ��" << buff << endl;
				//cout << "word=" << word <<" strlen(word)="<<strlen(word)<<endl;
				if (strlen(word) > 0){
					if (send(sockClient, buff, sizeof(buff), 0) <= 0) {
						cout << "����ʧ��\n";
					}
				}
					
				
			} 
			printf("�Ѿ��˳���%s������\n", selectname[sel]);
			




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