#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>

//[1]把server端的IP地址定义成宏
#define SERVER_IP	"10.11.100.6"

//[2]把server端的PORT端口号定义成宏
#define SERVER_PORT	9527

//[3]通信结构的定义
#define MSGSIZE		128

#define CNTSIZE 64
#define PWDSIZE 512//加密之后

enum {LOGIN_SUCCESS, LOGIN_COUT_NOT_EXIST, LOGIN_ERROR_PASSWD, LOGIN_DOUBLE, REGISTER_IN, REGISTER_ERROR, MESSAGE, LIST};

struct login_st
{
    char count[CNTSIZE];//存储账号(网络编程不能用指针)
    char passwd[PWDSIZE];//注意密码需要加密
    char tocount[CNTSIZE];//要发送的用户
    char message[CNTSIZE];//要发送的内容
    uint8_t login_state;//记录登录状态
}__attribute__((packed));//结构体制定单字节对齐

#endif // PROTOCOL_H
