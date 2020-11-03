#include "data_glob.h"

int do_add(int sockfd,MSG *msg);
int do_del(int sockfd,MSG *msg);
int do_modify(int sockfd,MSG *msg);
int do_query(int sockfd,MSG *msg);
int do_quit(int sockfd,MSG *msg);

int main(int argc, char *argv[])
{
	int sockfd;
	struct sockaddr_in serveraddr;
	int n;
	MSG msg;

    if((sockfd = socket(AF_INET,SOCK_STREAM,0)) <0)
	{
		perror("fail to socket.\n");
		return -1;
	}
	bzero(&serveraddr,sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr("192.168.5.178");
	serveraddr.sin_port = htons(8888);
	
	if(connect(sockfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr)) < 0)
	{
		perror("fail to connect");
		return -1;
	}
	
	while(1)
	{
		printf("*******Welcome to the Student Information management system*******\n");
		printf("1.add  2.del  3.modify 4.query 5.quit\n");
		printf("*****************\n");
		scanf("%d", &n);
		
		switch(n)
		{
			case 1:
				do_add(sockfd,&msg);
				break;
			case 2:
				do_del(sockfd,&msg);
				break;
			case 3:
				do_modify(sockfd,&msg);
				break;
			case 4:
				do_query(sockfd,&msg);
				break;
			case 5:
				do_quit(sockfd,&msg);
				break;
			default:
				printf("invalid data cmd.\n");
		}

	}

	return 0;
}

int do_add(int sockfd,MSG *msg)
{
	printf("%s\n",__func__);

	//input # 退回上一级菜单

	msg->type = A;
	printf("input Student id:");
	scanf("%d",&msg->id);
	getchar();
	printf("input name:");
	scanf("%s",msg->name);
	getchar();
	printf("input age:");
	scanf("%d",&msg->age);
	getchar();
	printf("input score:");
	scanf("%f",&msg->score);
	getchar();

	if(send(sockfd,msg,sizeof(MSG),0)<0)
	{
		printf("fail to send.\n");
		return -1;
	}
	// recv ok or faile
	if(recv(sockfd,msg,sizeof(MSG),0) < 0)
	{
		printf("fail to recv \n");
		return -1;
	}
	
	printf("%s\n",msg->name); 
	
	return 0;

}
int do_del(int sockfd,MSG *msg)
{

	printf("%s\n",__func__);
	msg->type = D;
	printf("input id:");
	scanf("%d",&msg->id);


	send(sockfd,msg,sizeof(MSG),0);
	if(recv(sockfd,msg,sizeof(MSG),0) < 0)
	{
		printf("fail to recv \n");
		return -1;
	}
	printf("name=%s\n",msg->name); 
	return 0;
}
int do_modify(int sockfd,MSG *msg)
{
	
	printf("%s\n",__func__);
	
	msg->type = M;

	printf("input Student id:");
	scanf("%d",&msg->id);
	getchar();
	printf("input name:");
	scanf("%s",msg->name);
	getchar();
	printf("input age:");
	scanf("%d",&msg->age);
	getchar();
	printf("input score:");
	scanf("%f",&msg->score);
	getchar();
	send(sockfd,msg,sizeof(MSG),0);
	if(recv(sockfd,msg,sizeof(MSG),0) < 0)
	{
		printf("fail to recv \n");
		return -1;
	}
	printf("name=%s\n",msg->name); 
	return 0;
}
int do_query(int sockfd,MSG *msg)
{
	printf("%s\n",__func__);
	
	msg->type = Q;
	printf("input query id:");
	scanf("%d",&msg->id);
	getchar();
	
	//input # 退回上一级菜单
//	if(strncmp(msg->id,#,1) == 0)
//		break;

	if(send(sockfd,msg,sizeof(MSG),0) <0)
	{
		printf("faile to send\n");
		return -1;
	}

	if(recv(sockfd,msg,sizeof(MSG),0) < 0)
	{
		printf("fail to recv \n");
		return -1;
	}
	
	printf("name=%s score=%f\n",msg->name,msg->score); 

	return 0;
}
int do_quit(int sockfd,MSG *msg)
{	
 	printf("%s\n",__func__);
	close(sockfd);
	exit(0);
	return 0;
}
