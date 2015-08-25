#include<stdio.h>
#include<windows.h>
#include<stdlib.h>
#include<WINSOCK2.H>

struct COMMTIMEOUTS {      
	DWORD ReadIntervalTimeout;     
	DWORD ReadTotalTimeoutMultiplier;     
	DWORD ReadTotalTimeoutConstant;     
	DWORD WriteTotalTimeoutMultiplier;     
	DWORD WriteTotalTimeoutConstant; 
};
HANDLE m_hIDComDev;
int delay = 5;

void initialSerial()
{
	char szComParams[50];
	DCB dcb;
	char *m_com; char *m_baud; char *m_jiaoyan;
	m_com="Com4";
	m_baud="1200";
	m_jiaoyan="E";
     
	COMMTIMEOUTS CommTimeOuts;
	m_hIDComDev = NULL;
	m_hIDComDev = CreateFile(m_com, GENERIC_READ|GENERIC_WRITE, 0, NULL,  
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL); 
    
	if(m_hIDComDev==INVALID_HANDLE_VALUE)
	{
		printf("打开串口错误0，请检查!");
		goto endd;
	}
	if(m_hIDComDev ==(HANDLE) -1)
	{
		printf("打开串口错误，请检查!"); 
		goto endd;
	}
    
	SetCommTimeouts(m_hIDComDev, &CommTimeOuts);
	CommTimeOuts. ReadIntervalTimeout= 0xFFFFFFFF;//"0xFFFFFFFF";
	CommTimeOuts. ReadTotalTimeoutMultiplier = 0;
	CommTimeOuts. ReadTotalTimeoutConstant =5000;
	CommTimeOuts. WriteTotalTimeoutMultiplier = 0;
	CommTimeOuts. WriteTotalTimeoutConstant = 5000;
	PurgeComm(m_hIDComDev, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR ) ; 
	m_com="Com4:38400,E,8,1";
	wsprintf(szComParams, m_com);
    
	dcb. DCBlength = sizeof(DCB);
	GetCommState(m_hIDComDev, &dcb);
   
	int baud;
	baud = atoi(m_baud);
	dcb. BaudRate = baud;
	dcb. ByteSize= 8;
    
    
    
	if ((!SetCommState(m_hIDComDev, &dcb))||(!SetupComm(m_hIDComDev,10000,10000)))//设置串口和收发缓冲器的大小
	{
		DWORD dwError = GetLastError();
		CloseHandle(m_hIDComDev);
	}
	PurgeComm(m_hIDComDev,PURGE_RXCLEAR|PURGE_TXCLEAR|PURGE_TXABORT|PURGE_RXABORT);//清收发缓冲器
endd:
	;
}

DWORD SendData( unsigned char buff[],int send_length)
{   
	int t;
	DWORD dwBytesWritten;
	if(!WriteFile(m_hIDComDev,buff,send_length,&dwBytesWritten,NULL))
	{
		return 0;
	}
	for (t=0;t<send_length;t++)
		fprintf(stdout," %2X ",buff[t]);
	printf("\n");
	PurgeComm(m_hIDComDev,PURGE_TXCLEAR); 
	return 1;
}

DWORD ReadData(unsigned char rebuff [],DWORD dwBytesRead )
{
	DWORD dwErrorFlags;
	COMSTAT stat;
	if(delay>0)                                                                                   
		ClearCommError(m_hIDComDev,&dwErrorFlags,&stat);  
	if(stat.cbInQue <= 0)  //"stat.cbInQue" is  bytes in input buffer
		return 0;
	dwBytesRead = min(dwBytesRead,(DWORD)stat.cbInQue); 
	
	if(!ReadFile(m_hIDComDev,rebuff,dwBytesRead,&dwBytesRead,NULL)) 
	{  
		return 0;
	}
	return dwBytesRead;
}
void tcpSend(unsigned char ch[])
{
    int err;
	char receiveBuf[100]={"0"};
	char ip_addr[16];
	WORD versionRequired;
	WSADATA wsaData;
	versionRequired=MAKEWORD(1,1);
	err=WSAStartup(versionRequired,&wsaData);
	SOCKET clientSocket;
    SOCKADDR_IN clientsock_in;

	if (err)
	{
		return;
	}
	clientSocket=socket(AF_INET,SOCK_STREAM,0);
	
	clientsock_in.sin_addr.S_un.S_addr=inet_addr("127.0.0.1");
	clientsock_in.sin_family=AF_INET;
	clientsock_in.sin_port=htons(6000);
	connect(clientSocket,(SOCKADDR*)&clientsock_in,sizeof(SOCKADDR));
	
    
    
    recv(clientSocket,receiveBuf,101,0);
    send(clientSocket,ch,101,0);
    close(clientSocket);
}
void comRead(unsigned char ch1[])
{
    int send_length  = 100; 
	printf("begin\n");
	initialSerial();
	printf("init end.\n");
	printf("begin send.\n");

    DWORD x ;
	while(1)
    {    
		x = ReadData(ch1,send_length);
		sleep(1);
		printf("waiting for data...\n");
		if(x) 
              break;
    }
}
main()
{
	int i,r;
	unsigned char ch[10] = "hello";
	unsigned char ch1[100] = "\0";
	
    comRead(ch1);
    
    printf("\n\n\n\n"); 
    printf("Machine A...\n\n");
	printf("From com:%s\n",ch1); 
    tcpSend(ch1); // 将串口数据发送到 远程机器 
    printf("Done...\n");
	system("pause");
}
