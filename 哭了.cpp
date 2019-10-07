#include<stdio.h>
#include<winsock2.h>
#include<stdlib.h> 
#include<string.h> 
#include<time.h> 
#include<math.h>
#define CCITT 0x11021//0x11021 
#define buf 100
#pragma comment(lib,"ws2_32.lib") 
void timeout(clock_t *start,int t[],int next,int base);
unsigned int crcsend(unsigned int y); 
int main()
{
	WSADATA WSAData;
	(WSAStartup(MAKEWORD(2,2),&WSAData));//初始化dll 
	
	sockaddr_in myaddr;
	memset(&myaddr,0,sizeof(myaddr));
	myaddr.sin_family=PF_INET;
	myaddr.sin_port=htons(1234);
	myaddr.sin_addr.s_addr=inet_addr("127.0.0.1");//接收方IP地址与端口号 
	
	SOCKET sock=socket(PF_INET,SOCK_DGRAM,0);//套接字 
	
	const char *filename="D:\\play1.cpp";  
    FILE *fp=fopen(filename,"rb");  
    if(fp==NULL){
        printf("无法打开文件\n");
        system("pause");
        exit(0);
    }//这一段基本模仿教程上面的 ，不过这一段里面用到的东西也都大致了解了 

	int base=1;
	clock_t start=clock();//计时 
//	int timeone;//计时启动时发送的数据包序号 
	char ack[1]={2};
    struct _BUF_ {
		int message; 
		int num;
		int crc;	
	};
	struct _BUF_ send[buf];
	char buffer[buf]={0};//缓冲区 
	int t[buf]={0};//改为1后，相当于该序号计时器已经停止 
	int ACK=0,next=1;
	int N=4;//慢启动以后再考虑 
	sockaddr* from=(sockaddr*)&myaddr;
	sockaddr to;
	int k=sizeof(to);
//	HANDLE timethread;//计时进程
//	DWORD timeID;
//	timethread=CreatThread(NULL,0,(LPTHREAD_START_ROUTINE)timeout,NULL,0,&timeID);**在网上搜到以为能用就
//依葫芦画瓢写上了，结果似乎编译都过不了，所以使用以后学东西还是要注重原理，把原理搞懂，*一味*模仿是没用的 
    while((send[next].message=fread(buffer,1,buf,fp))>0){
		send[next].num=next;
		send[next].crc=crcsend(send[next].message);
		int n1=sizeof(send[next].message),n2=sizeof(send[next].num),n3=sizeof(send[next].crc);//循环内定义 ..
		memcpy(buffer,&send[next].num,n2);
		memcpy(buffer,&send[next].crc,n3);
		memcpy(buffer,&send[next].message,n1);
		if(next<base+N){ 
			sendto(sock,buffer,n1+n2+n3,0,from,sizeof(myaddr)); 	
			next++;
			if(base==next) 
				start=clock(); 
			}
//		else{
//			wait
//		}
		recvfrom(sock,ack,1,0,&to,&k);
		if(ack[0]==ACK){
			base+=1;
			ACK^=ACK;
			if(next>base){
				start=clock();
			} 
			else if(next==base){
				t[base]=1;
			}
		}	
		else{
			send[base].num=next;
			send[base].crc=crcsend(send[base].message);
			n1=sizeof(send[base].message),n2=sizeof(send[base].num),n3=sizeof(send[base].crc);
			memcpy(buffer,&send[base].num,n2);
			memcpy(buffer,&send[base].crc,n3);
			memcpy(buffer,&send[base].message,n1);
			sendto(sock,buffer,n1+n2+n3,0,from,sizeof(myaddr));
		}
		if(clock()>start+25*CLOCKS_PER_SEC&&t[base]==0){
			start=clock();
			for(;base<=next-1;base++){
				send[base].num=next;
				send[base].crc=crcsend(send[base].message);
				n1=sizeof(send[base].message),n2=sizeof(send[base].num),n3=sizeof(send[base].crc);
				memcpy(buffer,&send[base].num,n2);
				memcpy(buffer,&send[base].crc,n3);
				memcpy(buffer,&send[base].message,n1);
				sendto(sock,buffer,n1+n2+n3,0,from,sizeof(myaddr));
			}
		}//既然不会多线程，只能每次循环都看一次超没超时（这样应该会降低程序运算性能与效率） 
	}
	
	fclose(fp); 
	closesocket(sock);
	WSACleanup();
    return 0;
}

unsigned int crcsend(unsigned int y)
{
	int n=0,m=0;
	while(y!=0){
		int x=y;
		x/=2;
		n++;
	}
	y=y<<16;
	int y1=y/(pow(2,n));
	y1=y1^CCITT;
	do{
		while((y1/pow(2,16))==0){
		y1<<=1;
		m++;
	    }
	    if(n>16){
			y1=y1+y/(pow(2,n-16-m)-y/pow(2,n-16));
		}
		y1=y1^CCITT;
	}while(y1>pow(2,16));
	
	return y1;
}
