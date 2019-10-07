#include<stdio.h>
#include<winsock2.h>
#include<stdlib.h>
#pragma comment(lib,"ws2_32.lib")
#include<math.h>
#define CCITT 0x11021//0x11021 
#define buf 100
unsigned int crcrecv(unsigned int x);
int main()
{
	char filename[100] = {0};  //文件名
    printf("请输入接收文件名: ");
    scanf("%s",filename);
    FILE *fp =fopen(filename,"wb");  
    if(fp==NULL){
        printf("无法打开文件\n");
        system("pause");
        exit(0);
    }//这一段基本模仿教程上面的 ，不过这一段里面用到的东西也都大致了解了 
    
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2),&wsaData);//初始化DLL
	
	SOCKET f_sock=socket(PF_INET,SOCK_DGRAM,0);//套接字 
	
	sockaddr_in myaddr;
	memset(&myaddr,0,sizeof(myaddr));
	myaddr.sin_family=PF_INET;
	myaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	myaddr.sin_port=htons(1234);
	bind(f_sock,(sockaddr*)&myaddr,sizeof(myaddr));//绑定sock与IP地址、端口号 
	
	struct _BUF_ {
		int message; 
		int num;
		int crc;	
	};
	struct _BUF_ recv[buf];
	sockaddr to;
	int n=sizeof(to);
	char buffer[buf]={0};
	char ack[1]={0};
	int base=1;
	int strlen;
	while((strlen=recvfrom(f_sock,buffer,buf,0,&to,&n))>0){
		recvfrom(f_sock,buffer,buf,0,&to,&n);
		memcpy(&recv[base],buffer,buf);
		recv[base].crc=(unsigned int) recv[base].crc;
		if(recv[base].num==base&&crcrecv(recv[base].crc)==0){
			sendto(f_sock,ack,1,0,&to,n); 
			base+=1;
			ack[0]^=1;
		}
		else{
			memset(buffer,0,buf);
		}
		int n2=sizeof(recv[base].num),n3=sizeof(recv[base].crc);
		int n1=strlen-n2-n3;
		int NUM=0;
		char anotherbuffer[n1]={0};
		for(;NUM<n1;NUM++){
			anotherbuffer[NUM]=buffer[NUM+n2+n3];
		}
		fwrite(anotherbuffer,1,n1,fp);
	}

	printf("成功！\n");
	fclose(fp); 
	closesocket(f_sock);
	WSACleanup();
	return 0;
} 

unsigned int crcrecv(unsigned int x)
{
	int juice=1;
	int n=0,m=0;
	while(x!=0){
		int y=x;
		y/=2;
		n++;
	}
	if(n<16){
		printf("请加长报文长度！"); 
		}
	int x1=x/pow(2,n-16);
	x1=x1^CCITT;
	do{
		while((x1/pow(2,16))==0){
		x1<<=1;
		m++;	
	    }
		x1=x1+(x/pow(2,n-16-m)-x/pow(2,n-16));
		x1^=CCITT;
	}while(m<(n-16));
	if(x1==0){
		juice=0;//juice为0表示传输正确 
	} 
	return juice;
}
