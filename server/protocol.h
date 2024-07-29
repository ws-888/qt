#ifndef __PROTOCOL_H
#define __PROTOCOL_H
#include <stdint.h>
#include <pthread.h>

//#define SERVER_IP "10.11.100.225"
#define SERVER_IP "192.168.245.128"

#define SERVER_PORT 9527

#define MSGSIZE 128

#define CNTSIZE  64
#define PWDSIZE  512
#define MESSIZE  64

enum
{
    LOGIN_SUCCESS,
    LOGIN_COUT_NOT_EXIST,
    LOGIN_ERROR_PASSWD,
    LOGIN_DOUBLE,
    REGISTER_IN ,//注册
    REGISTER_ERROR ,
    MESSAGE,
    LIST
};//登录状态    

struct login_st
{
    char count[CNTSIZE];//存储账号网路编程不能用指针
    char password[PWDSIZE];//存储密码，密码需要加密
    char tocount[CNTSIZE];//要发送的用户
    char message[MESSIZE];//要发送的内容
    uint8_t login_state;//记录登录状态
}__attribute__((packed));//结构体指令单字节对齐

typedef struct client_online  //在线表
{
    int new_socket;
    char count[CNTSIZE]; //用户名
}client_online;


#endif



