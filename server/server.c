#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include "protocol.h"
#include <netinet/ip.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <sqlite3.h> //包含sqlite3数据库的头文件
#include <poll.h>
#include <errno.h>
#include <time.h>

#define USRSIZE 100
static void *send_online_job(void *arg);  //向每一个用户发送在线列表的线程
static void *client_job(void *arg);	 	//客户端线程
int login_ok(struct login_st *buf);   //判断是否登录成功
int register_ok(struct login_st *buf); //判断是否注册成功
int get_client_socket(const char *count); //获取在线用户的套接字

pthread_mutex_t mtex = PTHREAD_MUTEX_INITIALIZER; //初始化全局锁
pthread_cond_t cond = PTHREAD_COND_INITIALIZER; //初始化全局条件变量
pthread_mutex_t mutex_db;//操作数据库的锁

int flag_list = -1; // -1不用发列表 1 代表发列表
client_online client_arr[USRSIZE]; //存储在线用户的用户名和socket
void client_arr_init(client_online *arr); 
int get_clientArr_pos();  //获取数组可用位置
int is_double_login(const char *name);//判断是否二次登录

int main(int argc, char *argv[])
{
	int tcp_socket;
	int new_socket;			  // 存储accept(2)成功之后的描述符
	struct sockaddr_in laddr; // 存储本地地址信息
	pid_t pid;
	char buf[MSGSIZE] = {0};
	int count = 0;
	int fd;
	struct sigaction act;
	sqlite3 *db = NULL; // sqlite3数据库的句柄
	char *errmsg = NULL;
	pthread_mutex_init(&mutex_db,NULL); //初始化数据库的锁

	//[a]打开数据库
	if (sqlite3_open("./server.db", &db) != SQLITE_OK) // 打开数据库
	{
		fprintf(stderr, "splite3_open() is Failed!\n");
		return -1; // 由于打开数据库失败结束程序，并且返回-1
	}

	//[b]如果没有登陆表则建立登陆表
	if(sqlite3_exec(db,"create table if not exists loginTable(account text primary key not null,password text not null)",NULL,NULL,&errmsg)!=SQLITE_OK)
	{
		fprintf(stderr,"sqlite3_exec():%s\n",errmsg);
		sqlite3_close(db);
		return -1;
	}

	//[c]如果登录成功，关闭数据库
	sqlite3_close(db);

	act.sa_handler = SIG_DFL;
	act.sa_flags = SA_NOCLDWAIT; // 加入子进程不会变为僵尸进程的要求
	sigaction(SIGCHLD, &act, NULL);

	// 创建套接字
	tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (tcp_socket == -1)
	{
		perror("socket()"); // 打印错误信息
		return -1;			// 由于创建套接字程序失败结束程序
	}

	laddr.sin_family = AF_INET;
	laddr.sin_port = htons(SERVER_PORT);
	// inet_aton("0.0.0.0",&laddr.sin_addr);
	laddr.sin_addr.s_addr = INADDR_ANY; // 使用宏来制定本地IP
	// bind绑定本地
	if (bind(tcp_socket, (struct sockaddr *)&laddr, sizeof(laddr)) == -1)
	{
		perror("bind()"); // 打印错误信息
		close(tcp_socket);
		return -2;
	}

	// 监听
	if (listen(tcp_socket, 20) == -1)
	{
		perror("listen()");
		close(tcp_socket);
		return -3;
	}
	/******************/
	pthread_t online_list_tid = 0;
	pthread_create(&online_list_tid,NULL,send_online_job,NULL);
	pthread_detach(online_list_tid);

	/*****************/
	pthread_t tid;
	client_arr_init(client_arr);//把这个数组进行初始化
	// 接收请求
	while (1)
	{
		new_socket = accept(tcp_socket, NULL, NULL);
		if (new_socket == -1)
		{
			perror("accept()");
			close(tcp_socket);
			return -4;
		}
		printf("main()-----new_socket = %d\n", new_socket);

		pthread_create(&tid, NULL, client_job, (void *)&new_socket);
		pthread_detach(tid);

	}

	pthread_mutex_destroy(&mtex); //销毁锁
	pthread_mutex_destroy(&mutex_db);//销毁锁
	pthread_cond_destroy(&cond); //销毁信号量
	close(tcp_socket);
	return 0;
}

/*检测数据库*/ // 登录检测
int login_ok(struct login_st *buf)
{
	sqlite3 *db = NULL;		// 数据库句柄
	const char *sql = NULL; //
	sqlite3_stmt *stmt = NULL;
	int ret = 0;
	const char *get_passwd = NULL; // 存储从表中获取到的密码
	int error_flag = 0;

	//[1]打开数据库
	if (sqlite3_open("./server.db", &db) != SQLITE_OK) // 打开数据库
	{
		fprintf(stderr, "splite3_open() is Failed!\n");
		return -1; // 由于打开数据库失败结束程序，并且返回-1
	}
	//[2]把sql语句的字符串转换成aqlite3—stmt结构
	sql = "select *from loginTable where account=?"; //?是一占位符，是一个不确定的值
	sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	// stmt结构中有一个不确定值 ？，需要给这个不确定值进行绑定
	sqlite3_bind_text(stmt, 1, buf->count, -1, NULL); // 绑定表中第一列数据
	// 检索登录表，判断buf->count是否在表内
	ret = sqlite3_step(stmt);
	if (ret == SQLITE_ROW) // 判断是否找到了
	{
		get_passwd = sqlite3_column_text(stmt, 1);	// 获取表中的第一列数据，从0计数
		if (strcmp(get_passwd, buf->password) == 0) // 判断密码是否相等
			buf->login_state = LOGIN_SUCCESS;
		else
			buf->login_state = LOGIN_ERROR_PASSWD;
	}
	else if (ret == SQLITE_DONE)
	{
		buf->login_state = LOGIN_COUT_NOT_EXIST;
		// buf->login_state = REGISTER_IN;
	}
	else
	{
		error_flag = -1;
	}
	// 释放stmt结构
	sqlite3_finalize(stmt);
	// 关闭数据库
	sqlite3_close(db);

	return error_flag;
}

// 注册
int register_ok(struct login_st *buf)
{
	sqlite3 *db = NULL; // 数据库句柄
	const char *sql = NULL;
	const char *sql1 = NULL; //
	sqlite3_stmt *stmt = NULL;
	int ret = 0;
	char *errmsg = NULL;

	//[1]打开数据库
	if (sqlite3_open("./server.db", &db) != SQLITE_OK) // 打开数据库
	{
		fprintf(stderr, "splite3_open() is Failed!\n");
		return -1; // 由于打开数据库失败结束程序，并且返回-1
	}

	sql = "select *from loginTable where account=?"; //?是一占位符，是一个不确定的值
	sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	sqlite3_bind_text(stmt, 1, buf->count, -1, NULL); // 绑定表中第一列数据
	ret = sqlite3_step(stmt);
	if (ret == SQLITE_ROW) // 判断账户是否重复
	{
		buf->login_state = REGISTER_ERROR;
		printf("注册失败，用户重复 regret--------%d\n", buf->login_state);
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return -1;
	}
	//[最后]插入
	sql1 = sqlite3_mprintf("INSERT INTO loginTable(account,password) VALUES('%s','%s')", buf->count, buf->password);
	if(sqlite3_exec(db, sql1, NULL, NULL, &errmsg)!=SQLITE_OK)
	{
		fprintf(stderr,"-------插入失败\n");
		fprintf(stderr,"sqlite3_exec():%s\n",errmsg);
	}

	// 关闭数据库
	sqlite3_close(db);

	return 0;
}

static void *client_job(void *arg)
{
	int new_socket = *(int *)arg;
	int read_cont = 0;
	struct login_st buf1;
	int get_socket = 0;

	pthread_mutex_lock(&mtex);//加锁
	int a = get_clientArr_pos(); //获取数组可用最小位置 
	if(a==-1)
		fprintf(stderr,"------服务器虚了，不能再为更多客户服务了\n");
	client_arr[a].new_socket = new_socket;
	pthread_mutex_unlock(&mtex);//解锁
	printf("a = %d\n",a);

	while (1)
	{
		read_cont = read(new_socket, &buf1, sizeof(buf1));	
		if(read_cont == 0)
		{
			fprintf(stderr,"%s 客户端断开连接\n",buf1.count);
			pthread_mutex_lock(&mtex);
			client_arr[a].new_socket = -1;//说明断开链接
			memset(client_arr[a].count,'\0',CNTSIZE);
			flag_list = 1;
			pthread_mutex_unlock(&mtex);
			pthread_cond_broadcast(&cond); //通知 向每个客户发送在线列表
			close(new_socket);
			pthread_exit(0);
		}

		printf("new_socket=%d--account:%s\tpassward:%s\n", new_socket, client_arr[a].count, buf1.password);
		printf("--(%d)假 发给 %s 的消息为 : %s\n",buf1.login_state,buf1.tocount,buf1.message);

		if (buf1.login_state == MESSAGE) //发消息
		{
			pthread_mutex_lock(&mtex);//加锁
			get_socket = get_client_socket(buf1.tocount);
			if(get_socket == -1)
			{
				pthread_mutex_unlock(&mtex);//解锁
				fprintf(stderr,"---%s 客户端不在线\n",buf1.tocount);
				continue;
			}
			pthread_mutex_unlock(&mtex);//解锁
			write(get_socket,&buf1,sizeof(buf1)); //给对方发消息
			printf("%s 给 %s 发 : %s",buf1.count,buf1.tocount,buf1.message);
			memset(buf1.message,'\0',MESSIZE);
			continue;
		}
		else if (buf1.login_state == REGISTER_IN) //注册
		{
			printf("--------正在注册\n");
			pthread_mutex_lock(&mutex_db);
			register_ok(&buf1);
			pthread_mutex_unlock(&mutex_db);
		}
		else //登录
		{
			printf("---------正在登录\n");
			pthread_mutex_lock(&mutex_db);
			if (login_ok(&buf1) == -1)
			{
				fprintf(stderr, "SQLITE Search Is Failed!\n");
			}
			if (is_double_login(buf1.count)==1)
			{
				buf1.login_state = LOGIN_DOUBLE;
				printf("用户登录重复\n");
			}
			pthread_mutex_unlock(&mutex_db);
		}
		printf("发送登录或者注册状态\n");
		fflush(NULL);
		printf("client_job--------%d\n", buf1.login_state);

		write(new_socket, &buf1, sizeof(buf1));

		if(buf1.login_state == LOGIN_SUCCESS) //如果登录成功就更新列表
		{
			pthread_mutex_lock(&mtex);
			memcpy(client_arr[a].count,buf1.count,CNTSIZE); //添加到在线列表中
			flag_list = 1;
			pthread_mutex_unlock(&mtex);
			sleep(3);
			pthread_cond_broadcast(&cond); //通知 向每个客户发送在线列表
		}
	}
	close(new_socket);
	return NULL;
}

void client_arr_init(client_online *arr) //给数组进行初始化
{
	int i = 0;
	for(i=0;i<USRSIZE;i++)
	{
		arr[i].new_socket = -1;
		memset(arr[i].count,'\0',CNTSIZE);
	}
}

int get_clientArr_pos() //获取可用的位置
{
	int i = 0;
	for(i=0;i<USRSIZE;i++)
	{
		if(client_arr[i].new_socket==-1)
			return i;
	}
	return -1;
}

int get_client_socket(const char *count) //获取和用户通信的套接字描述符
{
	int i = 0;
	for(i=0;i<USRSIZE;i++)
	{
		if(strcmp(count,client_arr[i].count)==0)
		{
			if(client_arr[i].new_socket != -1)
				return client_arr[i].new_socket;
		}
	}
	return -1;
}

static void *send_online_job(void *arg)  //发送在线列表的线程
{
	int i = 0;
	struct login_st buf;
	buf.login_state = LIST;
	//buf5.login_state = -1;
	while(1)
	{
		memset(buf.message,'\0',MESSIZE);
		pthread_mutex_lock(&mtex);
		while (flag_list == -1)
		{
			pthread_cond_wait(&cond,&mtex);
		}
		//sleep(1);
		for(i=0;i<USRSIZE;i++)
		{
			if(client_arr[i].new_socket != -1)
			{
				strncat(buf.message,client_arr[i].count,MESSIZE);
				strcat(buf.message," ");
			}
		}
		flag_list = -1;
		pthread_mutex_unlock(&mtex);
		for(i=0;i<USRSIZE;i++)
		{
			
			if(client_arr[i].new_socket != -1)
			{
				pthread_mutex_lock(&mtex);
				printf("send %s 成功在线列表,当前状态是：%d\n",client_arr[i].count,buf.login_state);
				write(client_arr[i].new_socket,&buf,sizeof(buf));
				pthread_mutex_unlock(&mtex);
			}
		}
		printf("send_online_job 有在线列表= %s\n",buf.message);
		fflush(NULL);
	}
	
}

int is_double_login(const char *name) //判断是否重复登录
{
	int i = 0;
	for(i=0;i<USRSIZE;i++)
	{
		if(client_arr[i].new_socket != -1)
		{
			if(strcmp(name,client_arr[i].count)==0)
			{
				return 1;
			}
		}
	}
	return -1;
}

