#ifndef SMS_FORWARD_CONFIG_H
#define SMS_FORWARD_CONFIG_H

// 字符串和缓冲区大小
#define SMTP_BUF_SIZE  1024
// 邮箱服务器 IP 和端口
#define SMTP_ADDR "smtp.163.com"
#define SMTP_PORT 25

// 发送邮箱和接收邮箱
#define MAIL_USER  "xx@163.com"    // user account of mail sender
#define MAIL_PASS  "passbase64"   // password base64
#define MAIL_TO    "xx@qq.com"    // mail receiver

#endif // SMS_FORWARD_CONFIG_H