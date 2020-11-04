#include "data_glob.h"

#define DATABASE  "my.db"

int do_client(int acceptfd ,sqlite3 *db);

int do_add(int acceptfd,MSG *msg,sqlite3 *db);
int do_del(int acceptfd,MSG *msg,sqlite3 *db);
int do_modify(int acceptfd,MSG *msg,sqlite3 *db);
int do_query(int acceptfd,MSG *msg,sqlite3 *db);

int do_login(int acceptfd,MSG *msg,sqlite3 *db);
int do_register(int acceptfd,MSG *msg,sqlite3 *db);

int main(int argc,char *argv[])
{

	int sockfd;
	int acceptfd;
	sqlite3 *db;
	int pid;
	char sql[128];
	char *errmsg;
	 int b_reuse =1;

	struct sockaddr_in serveraddr;

	//建立或打开数据库my.db
	if(sqlite3_open(DATABASE,&db) != SQLITE_OK)
	{
		printf("%s\n",sqlite3_errmsg(db));
		return -1;
	}
	else
	{
		printf("open DATABASE success.\n");
	}
	//
	//数据库中创建学生信息表
	
	sprintf(sql,"create table stuinfo(id integer,name text,age integer,score float)");
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) !=SQLITE_OK)
	{
		printf("%s\n",errmsg);
	}
	else{
		printf("creat table stuinfo done\n");
	}
	//数据库中创建登陆用户记录表
	sprintf(sql,"create table record(name text,passwd integer)");
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) !=SQLITE_OK)
	{
		printf("%s\n",errmsg);
	}
	else{
		printf("create table passwd done\n");
	}

	//建立套接字
	 if((sockfd = socket(AF_INET,SOCK_STREAM,0))<0)
	{
		perror("faile to sockfd.\n");
		return -1;
	}
	 //设置端口快速重用
	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&b_reuse,sizeof(int));
	//填充网络数据结构体
	bzero(&serveraddr,sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr("192.168.5.178");
	serveraddr.sin_port = htons(8888);
	//绑定结构体
	if(bind(sockfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr)) < 0)
	{
		perror("fail  to bind.\n");
		return -1;
	}
	//设置监听数量
	if(listen(sockfd ,5) < 0)
	{
		printf("fail to listen.\n");
		return -1;
	}
	//退出程序后自动处理僵尸进程
	signal(SIGCHLD,SIG_IGN);

	while(1)
	{
		//待定链接
		 if((acceptfd = accept(sockfd,NULL,NULL))<0)
		 {
		 	perror("fail  to accept");
			return -1;
		 }
		 //建立多进程处理客户端请求
		 if((pid = fork()) < 0)
		 {
		 	perror("fail to fork");
			return -1;
		 }
		 else if(pid == 0) //儿子进程处理客户端请求
		 {
		 	close(sockfd);
			do_client(acceptfd,db);
		 }
		 else{
		 	close(acceptfd);
		 }
		
	}

	return 0;
}

int do_client(int acceptfd ,sqlite3 *db)
{
	MSG msg;
 //接受客户端发来的信息
	while(recv(acceptfd ,&msg,sizeof(msg),0) >0)
	{
		printf("type:%d \n" ,msg.type);
		switch(msg.type)
		{
			case A:
				do_add(acceptfd,&msg,db);
				break;
			case D:
				do_del(acceptfd,&msg,db);
				break;
			case M:
				do_modify(acceptfd,&msg,db);
				break;
			case Q:
				do_query(acceptfd,&msg,db);
				break;
			case R:
				do_register(acceptfd,&msg,db);
				break;
			case L:
				do_login(acceptfd,&msg,db);
				break;
			default:
				printf("Invalid data msg\n");
				break;
		}
	}
	printf("client exit.\n");
	close(acceptfd);
	exit(0);
}
int do_login(int acceptfd,MSG *msg,sqlite3 *db)
{
	char *errmsg;
	char sql[128];
	char **resultp;
	int nrow;
	int ncolumn;
	printf("%s\n",__func__);
	//查询表格中是否有对应的用户名和密码
	sprintf(sql,"select * from record where name ='%s' and passwd= %d;",msg->name,msg->id);
	if(sqlite3_get_table(db,sql, &resultp, &nrow, &ncolumn, &errmsg)!= SQLITE_OK)
	{
		printf("%s\n",errmsg);
	}
	else
	{
		printf("get_table ok\n");
	}
	if(nrow ==1)//有查询记录发送OK
	{
		strcpy(msg->name,"ok");
		if(send(acceptfd,msg,sizeof(MSG),0)< 0)
		{
			printf("send faile\n");
		}
		return 1;
	}
	if(nrow == 0)//无查询记录发送wrong
	{
		strcpy(msg->name,"user/passwd wrong");
		if(send(acceptfd,msg,sizeof(MSG),0) <0)
		{
			printf("send faile\n");
		}

	}
	return 0;
}
int do_register(int acceptfd,MSG *msg,sqlite3 *db)
{
	char *errmsg;
	char sql[128];
	printf("%s\n",__func__);
	//将用户名和密码插入表格
	sprintf(sql,"insert into record values('%s',%d);",msg->name,msg->id);
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) !=SQLITE_OK)
	{
		printf("%s\n",errmsg);
		strcpy(msg->name,"faile");
	}
	else
	{
		printf("do_register ok\n");
		strcpy(msg->name,"do_register ok\n");
	}
	   printf("%s\n",msg->name); //不加这个的时候msg->name 的内容发送不更新?

	if(send(acceptfd,msg,sizeof(MSG),0)<0)
	{
		printf("send failed\n");
	}
	return 0;
}

int do_add(int acceptfd,MSG *msg,sqlite3 *db)
{
	char *errmsg;
	char sql[128];
	printf("%s\n",__func__);
	
	printf(" id=%d name =%s age =%d score=%f\n",msg->id,msg->name,msg->age,msg->score);
//学生信息插入到表格中
	sprintf(sql,"insert into stuinfo values(%d,'%s',%d,%f);",msg->id,msg->name,msg->age,msg->score);
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) !=SQLITE_OK)
	{
		printf("%s\n",errmsg);
		strcpy(msg->name,"faile");
	}
	else
	{
		printf(" add Information ok\n");
		strcpy(msg->name,"OK!");
	}
	printf("%s\n",msg->name);
	if(send(acceptfd,msg,sizeof(MSG),0) < 0)
	{
		perror("fail to send");
	}
	
	return 0;
}
int do_del(int acceptfd,MSG *msg,sqlite3 *db)
{
	char sql[128];
	char *errmsg;

	printf("%s\n",__func__);
	//删除对应id 的学生信息
	sprintf(sql,"delete from stuinfo where id =%d;",msg->id);
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
	{
		printf("%s\n",errmsg);
	    strcpy(msg->name,"faile");
	}
	else
	{
		printf("Delete done \n");
	    strcpy(msg->name,"delete done");
	}
	printf("%s\n",msg->name);
	   if(send(acceptfd,msg,sizeof(MSG),0) < 0)
	     {
		    perror("fail to send");
	     }

	return 0;
}
int do_modify(int acceptfd,MSG *msg,sqlite3 *db)
{	
	char sql[128];
	char *errmsg;

	printf("%s\n",__func__);
	//修改信息
	sprintf(sql,"update stuinfo set name='%s',age=%d,score=%f where id =%d",msg->name,msg->age,msg->score,msg->id);
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
	{
		printf("%s\n",errmsg);
	    strcpy(msg->name,"faile");
	}
	else
	{
		printf("Update done \n");
	    strcpy(msg->name,"Update done");
	}
		printf("%s\n",msg->name);
	   if(send(acceptfd,msg,sizeof(MSG),0) < 0)
	     {
		    perror("fail to send");
	     }
	return 0;
}

int query_callback(void *arg,int f_num, char** f_value,char **f_name)
{
	int acceptfd;
	int i;
	MSG msg;
	acceptfd =*((int *)arg);
	
	printf("%s\n",__func__);
	for(i=0;i<f_num;i=i+4)
	{
		//一条信息的内容
		printf("%s ",f_value[i]);
		printf("%s ",f_value[i+1]);
		printf("%s ",f_value[i+2]);
		printf("%s \n",f_value[i+3]);		
	
		sprintf(msg.name,"%s",f_value[i+1]);
		//将字符数值转为float类型
      	msg.score = atof(f_value[i+3]); 

		
	  if(send(acceptfd,&msg,sizeof(MSG),0) < 0)
	  {
		perror("fail to send");
	   }
	}

	return 0;
}
int do_query(int acceptfd,MSG *msg,sqlite3 *db)
{
	char *errmsg;
	char sql[128];
	printf("%s\n",__func__);
	printf("id =%d\n",msg->id);
    //查询对应的id 记录信息
	sprintf(sql,"select * from stuinfo where id = %d",msg->id);
	//有一条记录就执行回调函数
	if(sqlite3_exec(db,sql,query_callback,(void *)&acceptfd,&errmsg)!=SQLITE_OK)
	{
		printf("%s\n",errmsg);
	}
	else{
	
	    	strcpy(msg->name,"faile");

	     if(send(acceptfd,msg,sizeof(MSG),0) < 0)
	      {
		    perror("fail to send");
	       }
		
	}
	printf("-------------\n");
	return 0;
}
