#include "data_glob.h"

#define DATABASE  "my.db"

int do_client(int acceptfd ,sqlite3 *db);

int main(int argc,char *argv[])
{

	int sockfd;
	int acceptfd;
	sqlite3 *db;
	int pid;
	char sql[128];
	char *errmsg;

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

	}


}
