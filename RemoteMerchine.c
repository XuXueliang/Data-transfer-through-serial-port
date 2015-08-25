#include<stdio.h>
#include<windows.h>
#include<stdlib.h>
#include <WINSOCK2.H>



int main()
{
   SOCKADDR_IN clientsocket;
   SOCKET serConn;
   WORD myVersionRequest;
   WSADATA wsaData;
   myVersionRequest=MAKEWORD(1,1);
   int err;
   int len=sizeof(SOCKADDR);
   err=WSAStartup(myVersionRequest,&wsaData);
   if (err)  
   {
	   return -1;
   }
   SOCKET serSocket=socket(AF_INET,SOCK_STREAM,0);
   SOCKADDR_IN addr;
   addr.sin_family=AF_INET;
   addr.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
   addr.sin_port=htons(6000);
   
   bind(serSocket,(SOCKADDR*)&addr,sizeof(SOCKADDR));
   listen(serSocket,5);
   
   
   printf("Machine B...\n");
  
   char sendBuf[50];
   serConn=accept(serSocket,(SOCKADDR*)&clientsocket,&len);
   sprintf(sendBuf,"welcome!",inet_ntoa(clientsocket.sin_addr));
   send(serConn,sendBuf,strlen(sendBuf)+1,0);
   
   char recvBuf[100];
   recv(serConn,recvBuf,101,0);
   printf("Data:%s\n",recvBuf);	
   
   close(serSocket);	
   return 0;
}
