#include "data_glob.h"

#define DATABASE  "my.db"

int do_client(int acceptfd ,sqlite3 *db);

int do_add(int acceptfd,MSG *msg,sqlite3 *db);
int do_del(int acceptfd,MSG *msg,sqlite3 *db);
int do_modify(int acceptfd,MSG *msg,sqlite3 *db);
int do_query(int acceptfd,MSG *msg,sqlite3 *db);



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
	//add cord 
	
	sprintf(sql,"create table stuinfo(id integer,name text,age integer,score float)");
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) !=SQLITE_OK)
	{
		printf("%s\n",errmsg);
	}
	else{
		printf("creat table stuinfo done\n");
	}

	//
	 if((sockfd = socket(AF_INET,SOCK_STREAM,0))<0)
	{
		perror("faile to sockfd.\n");
		return -1;
	}
	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&b_reuse,sizeof(int));

	bzero(&serveraddr,sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr("192.168.5.178");
	serveraddr.sin_port = htons(8888);
	
	if(bind(sockfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr)) < 0)
	{
		perror("fail  to bind.\n");
		return -1;
	}
	
	if(listen(sockfd ,5) < 0)
	{
		printf("fail to listen.\n");
		return -1;
	}
	signal(SIGCHLD,SIG_IGN);

	while(1)
	{
		 if((acceptfd = accept(sockfd,NULL,NULL))<0)
		 {
		 	perror("fail  to accept");
			return -1;
		 }
		 if((pid = fork()) < 0)
		 {
		 	perror("fail to fork");
			return -1;
		 }
		 else if(pid == 0)
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

			default:
				printf("Invalid data msg\n");
				break;
		}
	}
	printf("client exit.\n");
	close(acceptfd);
	exit(0);
}

int do_register(int acceptfd,MSG *msg,sqlite3 *db)
{
	char *errmsg;
	char sql[128];
	printf("%s\n",__func__);

}

int do_add(int acceptfd,MSG *msg,sqlite3 *db)
{
	char *errmsg;
	char sql[128];
	printf("%s\n",__func__);
	printf(" id=%d name =%s age =%d score=%f\n",msg->id,msg->name,msg->age,msg->score);

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
	sprintf(sql,"delete from stuinfo where id =%d",msg->id);
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
		printf("%s ",f_value[i]);
		printf("%s ",f_value[i+1]);
		printf("%s ",f_value[i+2]);
		printf("%s \n",f_value[i+3]);		
	
		sprintf(msg.name,"%s",f_value[i+1]);
		//
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

	sprintf(sql,"select * from stuinfo where id = %d",msg->id);
	
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
