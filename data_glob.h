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
typedef struct {
	int type;
	int id;
	char name[N];
	int  age;
	float score;
}MSG;

#define  A  1
#define  D  2
#define  M  3
#define  Q  4
#define  R  5
#define  L  6


#endif
