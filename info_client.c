#include "data_glob.h"

int do_add(int sockfd,MSG *msg);
int do_del(int sockfd,MSG *msg);
int do_modify(int sockfd,MSG *msg);
int do_query(int sockfd,MSG *msg);
int do_quit(int sockfd,MSG *msg);

int do_register(int sockfd,MSG *msg);
int do_login(int sockfd,MSG *msg);

int main(int argc, char *argv[])
{
	int sockfd;
	struct sockaddr_in serveraddr;
	int n;
	MSG msg;
  //建立网络IPv4,流式套接字 
    if((sockfd = socket(AF_INET,SOCK_STREAM,0)) <0)
	{
		perror("fail to socket.\n");
		return -1;
	}
	//为internet 协议地址结构体赋值
	bzero(&serveraddr,sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr("192.168.5.178");
	serveraddr.sin_port = htons(8888);
	//链接服务器
	if(connect(sockfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr)) < 0)
	{
		perror("fail to connect");
		return -1;
	}
	while(1)
	{
		//1先注册用户名和密码 2再进行登陆
		printf("*******Welcome to the Student Information management system*******\n");
	   	printf("* 1.register    2.login   3.quit *\n");
		printf("please choose:");
		scanf("%d",&n);
		getchar();
		switch(n)
		{
			case 1:
				do_register(sockfd,&msg);//注册用户名和密码
				break;
			case 2:
				if(do_login(sockfd,&msg)==1) //从客户端数据库中查询是否有该用户和密码
				{
					goto next;  //下一个菜单进行处理
				}
			case 3:
				close(sockfd);    //关闭退出
				exit(0);
				break;
			default:
				printf("input number is invalid\n:");
				break;
		}
	}

next:
	while(1)
	{   /***增删改查**/
		printf("*******Welcome to the Student Information management system*******\n");
		printf("1.add  2.del  3.modify 4.query 5.quit\n");
		printf("*****************\n");
		scanf("%d", &n);
		
		switch(n)
		{
			case 1:
				do_add(sockfd,&msg);//增加学生信息
				break;
			case 2:
				do_del(sockfd,&msg); //删除学生信息
				break;
			case 3:
				do_modify(sockfd,&msg);//修改学生信息
				break;
			case 4:
				do_query(sockfd,&msg);//查询学生信息 
				break;
			case 5:
				do_quit(sockfd,&msg); //退出
				break;
			default:
				printf("invalid data cmd.\n");
		}

	}

	return 0;
}
/***登陆***/
int do_login(int sockfd,MSG *msg)
{
	printf("%s\n",__func__);
	msg->type = L;   //标志位
	printf("input username:"); //输入用户
	scanf("%s",msg->name);
	getchar();
	printf("input passwd:"); //密码
	scanf("%d",&msg->id);
	getchar();

	if((send(sockfd,msg,sizeof(MSG),0)) <0) //发送给服务器
	{
		printf("send is failed\n");
		return -1;
	}
	if(recv(sockfd,msg,sizeof(MSG),0) <0)  //接受服务器的返回信息
	{
		printf("recv is failed\n");
		return -1;
	}
	printf("%s\n",msg->name);
	if(strncmp(msg->name,"ok",2)==0) //查询到信息返回1
	{
		return 1;        
	}
	return 0;

}
/**注册用户名和密码***/
int do_register(int sockfd,MSG *msg)
{
	printf("%s\n",__func__);
	msg->type = R;
	printf("input username:");
	scanf("%s",msg->name);
	getchar();
	printf("input passwd:");
	scanf("%d",&msg->id);
	getchar();

	if((send(sockfd,msg,sizeof(MSG),0)) <0)
	{
		printf("send is failed\n");
		return -1;
	}
	if(recv(sockfd,msg,sizeof(MSG),0) <0)
	{
		printf("recv is failed\n");
		return -1;
	}
	printf("%s\n",msg->name);
	return 0;

}
/*增加学生信息**/
int do_add(int sockfd,MSG *msg)
{
	printf("%s\n",__func__);


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
/**删除学生信息**/
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
/**更新学生信息***/
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
		bzero(msg,sizeof(MSG));
	if(recv(sockfd,msg,sizeof(MSG),0) < 0)
	{
		printf("fail to recv \n");
		return -1;
	}
	printf("%s\n",msg->name); 
	return 0;
}
/**查询学生信息*/
int do_query(int sockfd,MSG *msg)
{
	printf("%s\n",__func__);
	
	msg->type = Q;
	printf("input query id:");
	scanf("%d",&msg->id);
	getchar();
	

	if(send(sockfd,msg,sizeof(MSG),0) <0)
	{
		printf("faile to send\n");
		return -1;
	}
		bzero(msg,sizeof(MSG));

	if(recv(sockfd,msg,sizeof(MSG),0) < 0)
	{
		printf("fail to recv \n");
		return -1;
	}
	printf("%s\n",msg->name);
	if(strncmp(msg->name,"faile",5)==0)
	{
		printf("not the id\n");
		return 0;
	}
	else{

		printf("name=%s score=%f\n",msg->name,msg->score); 
	}

		bzero(msg,sizeof(MSG));

	return 0;
}
/***退出**/
int do_quit(int sockfd,MSG *msg)
{	
 	printf("%s\n",__func__);
	close(sockfd);
	exit(0);
	return 0;
}
