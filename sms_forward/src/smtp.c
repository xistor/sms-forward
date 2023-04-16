

#include "sockets.h"
#include "dns.h"
#include "netdb.h"
#include "sms_forward_config.h"
#include "luat_debug.h"

// 查表法
#define BASE_TAB \
  "ABCDEFGHIJKLMNOPQRSTUVWXYZ" \
  "abcdefghijklmnopqrstuvwxyz" "0123456789+/"


union base64_t {    
    struct {
        unsigned c: 8;
        unsigned b: 8;
        unsigned a: 8;
    } src;
    struct {
        unsigned d: 6;
        unsigned c: 6;
        unsigned b: 6;
        unsigned a: 6;
    } res;
};

void _base64(char *res, char *src) {
    union base64_t data;
    data.src.a = (unsigned)src[0];
    data.src.b = (unsigned)src[1];
    data.src.c = (unsigned)src[2];
    res[0] = BASE_TAB[data.res.a];
    res[1] = BASE_TAB[data.res.b];
    res[2] = BASE_TAB[data.res.c];
    res[3] = BASE_TAB[data.res.d];
}

void base64(char *res, char *src) {
    int len = strlen(src);
    // src 中的每 3 个字符做一次操作
    while (*src != 0) {
        _base64(res, src);
        // 若 len 不是 3 的倍数, 则 res 末尾会有若干个 0
        src += 3, res += 4;
    }
    // 填充 '='
    if (len % 3 != 0) {
        // 先回退
        res -= (3 - len % 3);
        // 再填充
        memset(res, '=', 3 - len % 3);
    }
}

int mail_client(char* ip, int port) {
		int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    struct hostent *smtp_host = gethostbyname(SMTP_ADDR);
    if(!smtp_host){
        LUAT_DEBUG_PRINT("Get IP address error!");
        return -1;
    }
    inet_pton(AF_INET, inet_ntoa(*((struct in_addr *)smtp_host->h_addr)), &addr.sin_addr);
    addr.sin_port = htons(port);
    if(connect(sock, (const struct sockaddr *)(&addr), sizeof(addr)) <0) {
    	LUAT_DEBUG_PRINT("con fail\n");
    	return -1;
    }
    return sock;
}

void send_email(char *user, char *pass, char *addr, char *body) {
      int sock =  0;

    char buf[SMTP_BUF_SIZE]  = {0}; // 发送缓冲区
    char rbuf[SMTP_BUF_SIZE] = {0}; // 接收缓冲区
    char tmp[SMTP_BUF_SIZE]  = {0};
    int n = 0;

    sock = mail_client(SMTP_ADDR, SMTP_PORT);

    if(sock < 0){
        while(1);
    }
    n = recv(sock, rbuf, SMTP_BUF_SIZE, 0);
    LUAT_DEBUG_PRINT("rcv %d :%s\n", n, rbuf);

    // HELO & AUTH LOGIN
    LUAT_DEBUG_PRINT("HELO x\n"); 
    send(sock, "HELO x\r\n", 8, 0);
    memset(rbuf, 0, sizeof(rbuf));
    n = recv(sock, rbuf, SMTP_BUF_SIZE, 0);
    LUAT_DEBUG_PRINT("rcv %d :%s\n", n, rbuf);

    LUAT_DEBUG_PRINT("AUTH LOGIN\n"); 

    n = send(sock, "AUTH LOGIN\r\n", 12, 0);
    LUAT_DEBUG_PRINT("send AUTH LOGIN %d  \n",n);
    memset(rbuf, 0, sizeof(rbuf));
    n = recv(sock, rbuf, SMTP_BUF_SIZE, 0);
    LUAT_DEBUG_PRINT("rcv %d :%s\n",n, rbuf);



    // 用户名 & 密码
    base64(tmp, user);
   
    snprintf(buf, SMTP_BUF_SIZE, "%s\r\n", tmp);

    LUAT_DEBUG_PRINT("user: %s\n", buf); 
    send(sock, buf, strlen(buf), 0);
    memset(rbuf, 0, sizeof(rbuf));
    recv(sock, rbuf, SMTP_BUF_SIZE, 0);
    LUAT_DEBUG_PRINT("%s\n", rbuf);


    
    snprintf(buf, SMTP_BUF_SIZE, "%s\r\n", pass);
    LUAT_DEBUG_PRINT("pass: %s\n", buf);
    send(sock, buf, strlen(buf), 0);
    memset(rbuf, 0, sizeof(rbuf));
    recv(sock, rbuf, SMTP_BUF_SIZE, 0);
    LUAT_DEBUG_PRINT("%s", rbuf);
  
    // MAIL FROM & RCPT TO
    snprintf(buf, SMTP_BUF_SIZE, "mail from: <%s>\r\n", user);
    LUAT_DEBUG_PRINT("mail form send: %s", buf);
    send(sock, buf, strlen(buf), 0);
    memset(rbuf, 0, sizeof(rbuf));
    recv(sock, rbuf, SMTP_BUF_SIZE, 0);
    LUAT_DEBUG_PRINT("mail form rcv: %s", rbuf);


    snprintf(buf, SMTP_BUF_SIZE, "rcpt to: <%s>\r\n", addr);
    send(sock, buf, strlen(buf), 0);
    memset(rbuf, 0, sizeof(rbuf));
    recv(sock, rbuf, SMTP_BUF_SIZE, 0);
    LUAT_DEBUG_PRINT("rcpt to rcv: %s", rbuf);

    // DATA & 邮件正文

    send(sock, "DATA\r\n", 6, 0);
    memset(rbuf, 0, sizeof(rbuf));
    recv(sock, rbuf, SMTP_BUF_SIZE, 0);
    LUAT_DEBUG_PRINT("DATA to rcv: %s", rbuf);
    snprintf(buf, SMTP_BUF_SIZE, "%s\r\n.\r\n", body);
    LUAT_DEBUG_PRINT("mail body:%s", buf);
    send(sock, buf, strlen(buf), 0);
    memset(rbuf, 0, sizeof(rbuf));
    recv(sock, rbuf, SMTP_BUF_SIZE, 0);
    LUAT_DEBUG_PRINT("body rcv:%s", rbuf);

    // QUIT
    send(sock, "QUIT\r\n", 7, 0);
    recv(sock, rbuf, SMTP_BUF_SIZE, 0);
    LUAT_DEBUG_PRINT("%s", rbuf);
    close(sock);
}