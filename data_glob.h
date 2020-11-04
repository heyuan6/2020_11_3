#ifndef __DATA_GLOB_H__
#define __DATA_GLOB_H__


#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>

#define N 32
//TCP传送的结构体类型.
typedef struct {
	int type;     //用于区分不同的功能
	int id;       //序号或学号
	char name[N]; //姓名
	int  age;     //年龄
	float score;  //分数
}MSG;

#define  A  1  //增加学生信息
#define  D  2  //删除学生信息
#define  M  3  //修改学生信息
#define  Q  4  //查询学生信息
#define  R  5  //注册用户名和密码
#define  L  6  //用户登陆


#endif
