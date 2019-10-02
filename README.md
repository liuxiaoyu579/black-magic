## server
`#include<stdio.h>
#include<winsock2.h>

#pragma comment(lib,"ws2_32.lib")

#define buf 100

int main()
{
	 
	 WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2),&wsaData);//初始化DLL
	
	SOCKET f_sock=socket(AF_INET,SOCK_DGRAM,0);//套接字 
	
	sockaddr_in myaddr;
	memset(&myaddr,0,sizeof(myaddr));
	myaddr.sin_family=AF_INET;
	myaddr.sin_addr.s_addr=inet_addr("127.0.0.1");
	myaddr.sin_port=htons(1234);
	bind(f_sock,(sockaddr*)&myaddr,sizeof(myaddr));//绑定sock与IP地址、端口号 
	

	listen(f_sock,buf);
	
	SOCKET s_sock;
	SOCKADDR otheraddr; 
	int n=sizeof(otheraddr);
	s_sock=accept(f_sock,(sockaddr*)&otheraddr,&n); //这个套接字似乎没必要绑定？ 
	
	char bufrecv[buf]={0};
	char bufsend[buf]={0};
	int strlen2=recv(s_sock,bufrecv,buf,0)-1;
	for(;strlen2>=0;strlen2--){
		bufsend[strlen2]=bufrecv[strlen2]-32;
	}
	send(s_sock,bufsend,buf,0);

	closesocket(s_sock);
	closesocket(f_sock);
	WSACleanup();
	return 0;
} 
## client
`#include<stdio.h>
#include<winsock2.h>
#include<windows.h>
#define buf 100

int main()
{
	 
	 LoadLibrary("dllDemo.dll");
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2,2),&WSAData);
	
	sockaddr_in myaddr;
	memset(&myaddr,0,sizeof(myaddr));
	myaddr.sin_family=AF_INET;
	myaddr.sin_port=htons(1234);
	myaddr.sin_addr.s_addr=inet_addr("127.0.0.1");
	
	SOCKET sock=socket(AF_INET,SOCK_DGRAM,0);
	int n=sizeof(myaddr);
	connect(sock,(SOCKADDR*)&myaddr,n);
	
	char bufcsend[buf]={0};
	char bufcrecv[buf]={0};
	
	printf("请输入一组小写字母：\n");
	scanf("%s",bufcsend);
	int strlen1=send(sock,bufcsend,buf,0);                                                                                                                                                                                                                                                                           
	recv(sock,bufcrecv,buf,0);
	printf("得到的大写字母为：%s",bufcrecv);
	
	closesocket(sock);
	WSACleanup();
	return 0; 
}
