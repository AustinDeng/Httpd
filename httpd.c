/* J. David's webserver */
/* This is a simple webserver.
 * Created November 1999 by J. David Blackstone.
 * CSE 4344 (Network concepts), Prof. Zeigler
 * University of Texas at Arlington
 */
/* This program compiles for Sparc Solaris 2.6.
 * To compile for Linux:
 *  1) Comment out the #include <pthread.h> line.
 *  2) Comment out the line that defines the variable newthread.
 *  3) Comment out the two lines that run pthread_create().
 *  4) Uncomment the line that runs accept_request().
 *  5) Remove -lsocket from the Makefile.
 */
#include <stdio.h>
// void perror(char *string);
// int sprintf(char *str, const char *format, ...) 发送格式化输出到 str 所指向的字符串。

// FILE *fopen(const char *filename, const char *mode)
// 使用给定的模式 mode 打开 filename 所指向的文件。
// "r"	打开一个用于读取的文件。该文件必须存在。
// "w"	创建一个用于写入的空文件。如果文件名称与已存在的文件相同，则会删除已有文件的内容，文件被视为一个新的空文件。
// "a"	追加到一个文件。写操作向文件末尾追加数据。如果文件不存在，则创建文件。
// "r+"	打开一个用于更新的文件，可读取也可写入。该文件必须存在。
// "w+"	创建一个用于读写的空文件。
// "a+"	打开一个用于读取和追加的文件。


#include <sys/socket.h>
// int socket(int af, int type, int protocol);
// 1) af 为地址族（Address Family），也就是 IP 地址类型，常用的有 AF_INET 和 AF_INET6。
// AF 是“Address Family”的简写，INET是“Inetnet”的简写。AF_INET 表示 IPv4 地址，
// 你也可以使用 PF 前缀，PF 是“Protocol Family”的简写，它和 AF 是一样的。
// 例如，PF_INET 等价于 AF_INET，PF_INET6 等价于 AF_INET6。
// 2) type 为数据传输方式/套接字类型，
// 常用的有 SOCK_STREAM（流格式套接字/面向连接的套接字） 和 SOCK_DGRAM（数据报套接字/无连接的套接字
// 3) protocol 表示传输协议，常用的有 IPPROTO_TCP 和 IPPTOTO_UDP，分别表示 TCP 传输协议和 UDP 传输协议。

// https://www.cnblogs.com/cthon/p/9270778.html
// int setsockopt( int socket, int level, int option_name,const void *option_value, size_t ，ption_len);

// int bind ( int sockfd, struct sockaddr * addr, socklen_t addrlen )
// 返回： 0 ──成功， - 1 ──失败
// 参数sockfd：指定地址与哪个套接字绑定，这是一个由之前的socket函数调用返回的套接字。
// 调用bind的函数之后，该套接字与一个相应的地址关联，发送到这个地址的数据可以通过这个套接字来读取与使用。
// 参数addr：指定地址。这是一个地址结构，并且是一个已经经过填写的有效的地址结构。调用bind之后这个地址与参数sockfd指定的套接字关联，
// 从而实现上面所说的效果。
// 参数addrlen：正如大多数socket接口一样，内核不关心地址结构，当它复制或传递地址给驱动的时候，它依据这个值来确定需要复制多少数据。

// int listen(int s, int backlog);
// listen()用来等待参数s 的socket 连线. 参数backlog 指定同时能处理的最大连接要求, 
// 如果连接数目达此上限则client 端将收到ECONNREFUSED 的错误. 
// Listen()并未开始接收连线, 只是设置socket 为listen 模式, 真正接收client 端连线的是accept(). 
// 通常listen()会在socket(), bind()之后调用, 接着才调用accept().

// https://blog.csdn.net/Z_Stand/article/details/102535706
// int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
// 函数功能：
// 被动监听客户端发起的tcp连接请求，三次握手后连接建立成功。客户端connect函数请求发起连接。
// 连接成功后服务器的tcp协议会记录客端的ip和端口，如果是跨网通信，记录ip的就是客户端所在路由器的公网ip
// 返回值：
// 成功：返回一个通信描述符，专门用于与连接成功的客户端进行通信。
// 失败：返回-1 ，并设置errno
// 函数参数：
// a. sockfd 已经被listen转为了被动描述符的“套接字文件描述符”，专门用于客户端的监听，
// 入股sockfs没有被listen函数转为被动描述符，则accept是无法将其用来监听客户端连接的。
// 套接字文件描述符默认是阻塞的，即如果没有客户端请求连接的时候，此时accept会阻塞，直到有客户端连接；
// 如果不想套接字文件描述符阻塞，则可以创建套接字 socket函数 时指定type为SOCK_NOBLOCK
// b. addrlen表示第二个参数addr的大小，不顾要求给定地址
// c. addr: 用于记录发起连接请求的那个客户端的IP端口

// 定义函数：int send(int s, const void * msg, int len, unsigned int falgs);
// 函数说明：send()用来将数据由指定的socket 传给对方主机. 参数s 为已建立好连接的socket. 参数msg 指向欲连线的数据内容, 参数len 则为数据长度. 参数flags 一般设0, 其他数值定义如下：
//    MSG_OOB 传送的数据以out-of-band 送出.
//    MSG_DONTROUTE 取消路由表查询
//    MSG_DONTWAIT 设置为不可阻断运作
//    MSG_NOSIGNAL 此动作不愿被SIGPIPE 信号中断.
// 返回值：成功则返回实际传送出去的字符数, 失败返回-1. 错误原因存于errno
// 错误代码：
//    EBADF 参数s 非合法的socket 处理代码.
//    EFAULT 参数中有一指针指向无法存取的内存空间
//    ENOTSOCK 参数s 为一文件描述词, 非socket.
//    EINTR 被信号所中断.
//    EAGAIN 此操作会令进程阻断, 但参数s 的socket 为不可阻断.
//    ENOBUFS 系统的缓冲内存不足
//    ENOMEM 核心内存不足
//    EINVAL 传给系统调用的参数不正确.



#include <netinet/in.h>
// struct sockaddr_in{
//   short sin_family;           /*Address family一般来说AF_INET（地址族）PF_INET（协议族）*/
//   unsigned short sin_port;    /*Port number(必须要采用网络数据格式,普通数字可以用htons()函数转换成网络数据格式的数字)*/
//   struct in_addr sin_addr;    /*IP address in network byte order（Internet address）*/
//   unsigned char sin_zero[8];  /*Same size as struct sockaddr没有实际意义,只是为了　跟SOCKADDR结构在内存中对齐*/
// };


#include <arpa/inet.h>
#include <unistd.h>
// socklen_t


#include <ctype.h>
// isspace()


#include <strings.h>
// memset(void *s,int ch,size_t n);

#include <string.h>
// int strcasecmp (const char *s1, const char *s2);
// 函数说明：strcasecmp()用来比较参数s1 和s2 字符串，比较时会自动忽略大小写的差异。
// 若参数s1 和s2 字符串相同则返回0。s1 长度大于s2 长度则返回大于0 的值，s1 长度若小于s2 长度则返回小于0 的值。

// char *strcat(char *dest, const char *src) 把 src 所指向的字符串追加到 dest 所指向的字符串的结尾。

#include <sys/types.h>
#include <sys/stat.h>
//  int stat(const char *file_name, struct stat *buf);
// 函数说明:    通过文件名filename获取文件信息，并保存在buf所指的结构体stat中
// 返回值:      执行成功则返回0，失败返回-1，错误代码存于errno

// struct stat {
//     dev_t         st_dev;       //文件的设备编号
//     ino_t         st_ino;       //节点
//     mode_t        st_mode;      //文件的类型和存取的权限
//     nlink_t       st_nlink;     //连到该文件的硬连接数目，刚建立的文件值为1
//     uid_t         st_uid;       //用户ID
//     gid_t         st_gid;       //组ID
//     dev_t         st_rdev;      //(设备类型)若此文件为设备文件，则为其设备编号
//     off_t         st_size;      //文件字节数(文件大小)
//     unsigned long st_blksize;   //块大小(文件系统的I/O 缓冲区大小)
//     unsigned long st_blocks;    //块数
//     time_t        st_atime;     //最后一次访问时间
//     time_t        st_mtime;     //最后一次修改时间
//     time_t        st_ctime;     //最后一次改变时间(指属性)
// };


#include <pthread.h>
// int pthread_create(pthread_t *tidp, const  pthread_attr_t *attr, ( void *)(*start_rtn)( void *), void  *arg);
// 第一个参数为指向线程 标识符的 指针。
// 第二个参数用来设置线程属性。
// 第三个参数是线程运行函数的起始地址。
// 最后一个参数是运行函数的参数。

#include <sys/wait.h>
#include <stdlib.h>
#include <stdint.h>

#define ISspace(x) isspace((int)(x))

#define SERVER_STRING "Server: jdbhttpd/0.1.0\r\n"
#define STDIN   0
#define STDOUT  1
#define STDERR  2

// 处理从套接字上监听到的一个 HTTP 请求，在这里可以很大一部分地体现服务器处理请求流程。
void accept_request(void *);
// 返回给客户端这是个错误请求，HTTP 状态吗 400 BAD REQUEST.
void bad_request(int);
// 读取服务器上某个文件写到 socket 套接字。
void cat(int, FILE *);
// 主要处理发生在执行 cgi 程序时出现的错误。
void cannot_execute(int);
// 把错误信息写到 perror 并退出。
void error_die(const char *);
// 运行 cgi 程序的处理，也是个主要函数。
void execute_cgi(int, const char *, const char *, const char *);
// 读取套接字的一行，把回车换行等情况都统一为换行符结束。
int get_line(int, char *, int);
// 把 HTTP 响应的头部写到套接字。
void headers(int, const char *);
// 主要处理找不到请求的文件时的情况。
void not_found(int);
// 调用 cat 把服务器文件返回给浏览器。
void serve_file(int, const char *);
// 初始化 httpd 服务，包括建立套接字，绑定端口，进行监听等。
int startup(u_short *);
// 返回给浏览器表明收到的 HTTP 请求所用的 method 不被支持。
void unimplemented(int);

/**********************************************************************/
/* A request has caused a call to accept() on the server port to
 * return.  Process the request appropriately.
 * Parameters: the socket connected to the client */
/**********************************************************************/
void accept_request(void *arg)
{
    int client = (intptr_t)arg;
    char buf[1024];

    // 请求的数据长度
    size_t numchars;

    // GET or POST
    char method[255];

    char url[255];
    // 服务器的文件路径，默认在htdocs文件夹下
    char path[512];
    
    // 这里的 j 一直指向请求的数据
    // i 则用于需要填充的部分
    size_t i, j;

    // 操作的文件的结构体
    struct stat st;

    // 判断是否执行CGI程序，当Post请求以及Get请求携带参数的时候会调用CGI
    int cgi = 0;      

    // 对于 GET 方法，如果有携带参数，则 query_string 指针指向 url 中 ？ 后面的 GET 参数。                  
    char *query_string = NULL;

    numchars = get_line(client, buf, sizeof(buf));

    i = 0; j = 0;
    // 取 method 放到method 数组
    while (!ISspace(buf[i]) && (i < sizeof(method) - 1))
    {
        method[i] = buf[i];
        i++;
    }
    j=i;
    method[i] = '\0';

    if (strcasecmp(method, "GET") && strcasecmp(method, "POST"))
    {
        // 不支持除了 get 和 post 以外的方法 
        unimplemented(client);
        return;
    }

    // 如果是 post 方法，则需要执行 CGI 程序
    if (strcasecmp(method, "POST") == 0)
        cgi = 1;

    i = 0;
    // 忽略空白符
    while (ISspace(buf[j]) && (j < numchars))
        j++;

    // 取 url 放到 url 数组
    while (!ISspace(buf[j]) && (i < sizeof(url) - 1) && (j < numchars))
    {
        url[i] = buf[j];
        i++; j++;
    }
    url[i] = '\0';

    // query_string 指针指向 url 中 ？ 后面的 GET 参数
    if (strcasecmp(method, "GET") == 0)
    {
        query_string = url;
        while ((*query_string != '?') && (*query_string != '\0'))
            query_string++;
        if (*query_string == '?')
        {
            cgi = 1;
            *query_string = '\0';
            query_string++;
        }
    }

    // 记录服务器的文件路径
    sprintf(path, "htdocs%s", url);

    // 当 url 以 / 结尾，或 url 是个目录，则默认在 path 中加上 index.html，表示访问主页。
    if (path[strlen(path) - 1] == '/')
        strcat(path, "index.html");

    // 通过文件名 path 获取文件信息，并保存在st所指的结构体stat中
    if (stat(path, &st) == -1) {
        while ((numchars > 0) && strcmp("\n", buf))  /* read & discard headers */
            numchars = get_line(client, buf, sizeof(buf));
        not_found(client);
    } else {
        // 如果 st 指代的是一个文件目录，则加上 index.html
        if ((st.st_mode & S_IFMT) == S_IFDIR)
            strcat(path, "/index.html");

        // S_IXUSR：用户可执行
        // S_IXGRP： 组可执行
        // S_IXOTH： 其他可执行
        if ((st.st_mode & S_IXUSR) || (st.st_mode & S_IXGRP) || (st.st_mode & S_IXOTH))
            cgi = 1;

        if (!cgi)
            // 调用 cat 把服务器文件返回给浏览器。
            serve_file(client, path);
        else
            execute_cgi(client, path, method, query_string);
    }

    close(client);
}

/**********************************************************************/
/* Inform the client that a request it has made has a problem.
 * Parameters: client socket */
/**********************************************************************/
void bad_request(int client)
{
    char buf[1024];

    sprintf(buf, "HTTP/1.0 400 BAD REQUEST\r\n");
    send(client, buf, sizeof(buf), 0);
    sprintf(buf, "Content-type: text/html\r\n");
    send(client, buf, sizeof(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, sizeof(buf), 0);
    sprintf(buf, "<P>Your browser sent a bad request, ");
    send(client, buf, sizeof(buf), 0);
    sprintf(buf, "such as a POST without a Content-Length.\r\n");
    send(client, buf, sizeof(buf), 0);
}

/**********************************************************************/
/* Put the entire contents of a file out on a socket.  This function
 * is named after the UNIX "cat" command, because it might have been
 * easier just to do something like pipe, fork, and exec("cat").
 * Parameters: the client socket descriptor
 *             FILE pointer for the file to cat */
/**********************************************************************/
void cat(int client, FILE *resource)
{
    char buf[1024];

    // 从指定的流中读取数据，每次读取一行
    fgets(buf, sizeof(buf), resource);

    // feof 检测流上的文件结束符，如果文件结束，则返回非0值，否则返回0
    while (!feof(resource))
    {
        send(client, buf, strlen(buf), 0);
        fgets(buf, sizeof(buf), resource);
    }
}

/**********************************************************************/
/* Inform the client that a CGI script could not be executed.
 * Parameter: the client socket descriptor. */
/**********************************************************************/
void cannot_execute(int client)
{
    char buf[1024];

    sprintf(buf, "HTTP/1.0 500 Internal Server Error\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-type: text/html\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<P>Error prohibited CGI execution.\r\n");
    send(client, buf, strlen(buf), 0);
}

/**********************************************************************/
/* Print out an error message with perror() (for system errors; based
 * on value of errno, which indicates system call errors) and exit the
 * program indicating an error. */
/**********************************************************************/
void error_die(const char *sc)
{
    // 函数perror()用于抛出最近的一次系统错误信息，其原型如下：
    // void perror(char *string);
    perror(sc);
    exit(1);
}

/**********************************************************************/
/* Execute a CGI script.  Will need to set environment variables as
 * appropriate.
 * Parameters: client socket descriptor
 *             path to the CGI script */

// 读取整个 HTTP 请求并丢弃，如果是 POST 则找出 Content-Length. 把 HTTP 200  状态码写到套接字。

// 建立两个管道，cgi_input 和 cgi_output, 并 fork 一个进程。

// 在子进程中，把 STDOUT 重定向到 cgi_outputt 的写入端，把 STDIN 重定向到 cgi_input 的读取端
// 关闭 cgi_input 的写入端 和 cgi_output 的读取端
// 设置 request_method 的环境变量，GET 的话设置 query_string 的环境变量
// POST 的话设置 content_length 的环境变量，这些环境变量都是为了给 cgi 脚本调用，接着用 execl 运行 cgi 程序。

// 在父进程中，关闭 cgi_input 的读取端 和 cgi_output 的写入端，
// 如果 POST 的话，把 POST 数据写入 cgi_input，已被重定向到 STDIN，
// 读取 cgi_output 的管道输出到客户端，该管道输入是 STDOUT。接着关闭所有管道，等待子进程结束。

/**********************************************************************/
void execute_cgi(int client, const char *path, const char *method, const char *query_string)
{
    char buf[1024];
    int cgi_output[2];
    int cgi_input[2];
    pid_t pid;
    int status;
    int i;
    char c;
    int numchars = 1;
    int content_length = -1;

    buf[0] = 'A'; buf[1] = '\0';
    if (strcasecmp(method, "GET") == 0)
        while ((numchars > 0) && strcmp("\n", buf))  /* read & discard headers */
            numchars = get_line(client, buf, sizeof(buf));
    else if (strcasecmp(method, "POST") == 0) /*POST*/
    {
        numchars = get_line(client, buf, sizeof(buf));
        while ((numchars > 0) && strcmp("\n", buf))
        {
            buf[15] = '\0';
            if (strcasecmp(buf, "Content-Length:") == 0)
                content_length = atoi(&(buf[16]));
            numchars = get_line(client, buf, sizeof(buf));
        }
        if (content_length == -1) {
            bad_request(client);
            return;
        }
    }
    else/*HEAD or other*/ {

    }


    if (pipe(cgi_output) < 0) {
        cannot_execute(client);
        return;
    }
    if (pipe(cgi_input) < 0) {
        cannot_execute(client);
        return;
    }

    if ( (pid = fork()) < 0 ) {
        cannot_execute(client);
        return;
    }
    sprintf(buf, "HTTP/1.0 200 OK\r\n");
    send(client, buf, strlen(buf), 0);
    if (pid == 0)  /* child: CGI script */
    {
        char meth_env[255];
        char query_env[255];
        char length_env[255];

        dup2(cgi_output[1], STDOUT);
        dup2(cgi_input[0], STDIN);
        close(cgi_output[0]);
        close(cgi_input[1]);
        sprintf(meth_env, "REQUEST_METHOD=%s", method);
        putenv(meth_env);
        if (strcasecmp(method, "GET") == 0) {
            sprintf(query_env, "QUERY_STRING=%s", query_string);
            putenv(query_env);
        }
        else {   /* POST */
            sprintf(length_env, "CONTENT_LENGTH=%d", content_length);
            putenv(length_env);
        }
        execl(path, NULL);
        exit(0);
    } else {    /* parent */
        close(cgi_output[1]);
        close(cgi_input[0]);
        if (strcasecmp(method, "POST") == 0)
            for (i = 0; i < content_length; i++) {
                recv(client, &c, 1, 0);
                write(cgi_input[1], &c, 1);
            }
        while (read(cgi_output[0], &c, 1) > 0)
            send(client, &c, 1, 0);

        close(cgi_output[0]);
        close(cgi_input[1]);
        waitpid(pid, &status, 0);
    }
}

/**********************************************************************/
/* Get a line from a socket, whether the line ends in a newline,
 * carriage return, or a CRLF combination.  Terminates the string read
 * with a null character.  If no newline indicator is found before the
 * end of the buffer, the string is terminated with a null.  If any of
 * the above three line terminators is read, the last character of the
 * string will be a linefeed and the string will be terminated with a
 * null character.
 从套接字获取一行，无论该行以换行符、回车符还是 CRLF 组合结尾。 用空字符终止读取的字符串。 
 如果在缓冲区结束之前没有找到换行符，则字符串以空值终止。 
 如果读取了上述三个行终止符中的任何一个，则字符串的最后一个字符将是换行符，并且字符串将以空字符终止。
 * Parameters: the socket descriptor
 *             the buffer to save the data in
 *             the size of the buffer
 * Returns: the number of bytes stored (excluding null) */
/**********************************************************************/

// 读取套接字的一行，把回车换行等情况都统一为换行符结束。
int get_line(int sock, char *buf, int size)
{
    int i = 0;
    char c = '\0';
    int n;

    while ((i < size - 1) && (c != '\n'))
    {
        n = recv(sock, &c, 1, 0);
        /* DEBUG printf("%02X\n", c); */
        if (n > 0)
        {
            if (c == '\r')
            {   
                // 把flags设置为MSG_PEEK，仅把tcp buffer中的数据读取到buf中，
                // 并不把已读取的数据从tcp buffer中移除，再次调用recv仍然可以读到刚才读到的数据。
                n = recv(sock, &c, 1, MSG_PEEK);
                /* DEBUG printf("%02X\n", c); */
                if ((n > 0) && (c == '\n'))
                    recv(sock, &c, 1, 0);
                else
                    c = '\n';
            }
            buf[i] = c;
            i++;
        }
        else
            c = '\n';
    }
    buf[i] = '\0';

    return(i);
}

/**********************************************************************/
/* Return the informational HTTP headers about a file. */
/* Parameters: the socket to print the headers on
 *             the name of the file */
/**********************************************************************/
void headers(int client, const char *filename)
{
    char buf[1024];
    (void)filename;  /* could use filename to determine file type */

    strcpy(buf, "HTTP/1.0 200 OK\r\n");
    send(client, buf, strlen(buf), 0);
    strcpy(buf, SERVER_STRING);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html\r\n");
    send(client, buf, strlen(buf), 0);
    strcpy(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
}

/**********************************************************************/
/* Give a client a 404 not found status message. */
/**********************************************************************/
void not_found(int client)
{
    char buf[1024];

    sprintf(buf, "HTTP/1.0 404 NOT FOUND\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, SERVER_STRING);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<HTML><TITLE>Not Found</TITLE>\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<BODY><P>The server could not fulfill\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "your request because the resource specified\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "is unavailable or nonexistent.\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "</BODY></HTML>\r\n");
    send(client, buf, strlen(buf), 0);
}

/**********************************************************************/
/* Send a regular file to the client.  Use headers, and report
 * errors to client if they occur.
 * Parameters: a pointer to a file structure produced from the socket
 *              file descriptor
 *             the name of the file to serve */
/**********************************************************************/
void serve_file(int client, const char *filename)
{
    FILE *resource = NULL;
    int numchars = 1;
    char buf[1024];

    buf[0] = 'A'; buf[1] = '\0';
    while ((numchars > 0) && strcmp("\n", buf))  /* read & discard headers */
        numchars = get_line(client, buf, sizeof(buf));

    resource = fopen(filename, "r");

    if (resource == NULL)
        not_found(client);
    else {
        headers(client, filename);
        cat(client, resource);
    }
    fclose(resource);
}

/**********************************************************************/
/* This function starts the process of listening for web connections
 * on a specified port.  If the port is 0, then dynamically allocate a
 * port and modify the original port variable to reflect the actual
 * port.
 * Parameters: pointer to variable containing the port to connect on
 * Returns: the socket */
//  此函数启动侦听指定端口上的 Web 连接的过程。 如果端口为0，则动态分配端口并修改原始端口变量以反映实际端口。
/**********************************************************************/
int startup(u_short *port)
{
    int httpd = 0;
    int on = 1;

    struct sockaddr_in name;

    // PF_INET:IPV4
    // SOCK_STREAM（流格式套接字/面向连接的套接字）
    // http://c.biancheng.net/view/2131.html
    httpd = socket(PF_INET, SOCK_STREAM, 0);
    if (httpd == -1)
        error_die("socket");
    // 初始化
    memset(&name, 0, sizeof(name));
    name.sin_family = AF_INET;
    name.sin_port = htons(*port);
    // INADDR_ANY就是指定地址为0.0.0.0的地址
    // 本函数将一个32位数从主机字节顺序转换成网络字节顺序。
    name.sin_addr.s_addr = htonl(INADDR_ANY);
    // struct sockaddr_in{
    //   short sin_family;           /*Address family一般来说AF_INET（地址族）PF_INET（协议族）*/
    //   unsigned short sin_port;    /*Port number(必须要采用网络数据格式,普通数字可以用htons()函数转换成网络数据格式的数字)*/
    //   struct in_addr sin_addr;    /*IP address in network byte order（Internet address）*/
    //   unsigned char sin_zero[8];  /*Same size as struct sockaddr没有实际意义,只是为了　跟SOCKADDR结构在内存中对齐*/
    // };
    // struct in_addr {
    //     in_addr_t s_addr;
    // };
    // in_addr_t一般为32位的unsigned int，其字节顺序为网络字节序
    // 即该无符号数采用大端字节序。其中每8位表示一个IP地址中的一个数值。

    // 设置打开端口复用：端口复用就是字面意思
    // https://www.cnblogs.com/cthon/p/9270778.html
    if ((setsockopt(httpd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0)  
    {  
        error_die("setsockopt failed");
    }
    // 在套接口中，一个套接字只是用户程序与内核交互信息的枢纽，它自身没有太多的信息，也没有网络协议地址和 端口号等信息
    // 在进行网络通信的时候，必须把一个套接字与一个地址相关联，这个过程就是地址绑定的过程。
    if (bind(httpd, (struct sockaddr *)&name, sizeof(name)) < 0)
        error_die("bind");
    // 如果端口是零的话，就动态分配一个端口
    if (*port == 0)  /* if dynamically allocating a port */
    {
        socklen_t namelen = sizeof(name);
        // getsockname()函数用于获取一个套接字的名字。
        if (getsockname(httpd, (struct sockaddr *)&name, &namelen) == -1)
            error_die("getsockname");
        // 本函数将一个16位数由网络字节顺序转换为主机字节顺序。
        // 字节顺序是指占内存多于一个字节类型的数据在内存中的存放顺序，通常有小端、大端两种字节顺序。
        *port = ntohs(name.sin_port);
    }
    // 最大连接要求为5
    if (listen(httpd, 5) < 0)
        error_die("listen");
    return(httpd);
}

/**********************************************************************/
/* Inform the client that the requested web method has not been
 * implemented.
 * Parameter: the client socket */
/**********************************************************************/
void unimplemented(int client)
{
    char buf[1024];

    sprintf(buf, "HTTP/1.0 501 Method Not Implemented\r\n");
    // send()用来将数据由指定的socket 传给对方主机. 
    // 参数 client 为已建立好连接的socket.
    send(client, buf, strlen(buf), 0);
    sprintf(buf, SERVER_STRING);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<HTML><HEAD><TITLE>Method Not Implemented\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "</TITLE></HEAD>\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<BODY><P>HTTP request method not supported.\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "</BODY></HTML>\r\n");
    send(client, buf, strlen(buf), 0);
}

/**********************************************************************/

int main(void)
{
    int server_sock = -1;
    u_short port = 4000;
    int client_sock = -1;
    struct sockaddr_in client_name;
    socklen_t  client_name_len = sizeof(client_name);
    pthread_t newthread;  // typedef unsigned long int pthread_t;

    // 建立监听
    server_sock = startup(&port);
    printf("httpd running on port %d\n", port);

    while (1)
    {
        // server_sock：被listen转为了被动描述符的“套接字文件描述符”
        // client_name：记录发起连接请求的那个客户端的IP端口
        // client_name_len： 表示第二个参数addr的大小，不顾要求给定地址
        // accept会一直阻塞直到客户端连接
        // 返回值：
        // 成功：返回一个通信描述符，专门用于与连接成功的客户端进行通信。
        // 失败：返回-1 ，并设置errno
        client_sock = accept(server_sock, (struct sockaddr *)&client_name, &client_name_len);
        if (client_sock == -1)
            error_die("accept");
        
        /* accept_request(&client_sock); */

        // 第一个参数为指向线程 标识符的 指针。
        // 第二个参数用来设置线程属性。
        // 第三个参数是线程运行函数的起始地址。
        // 最后一个参数是运行函数的参数。
        // 将client_sock参数传到accept_request
        if (pthread_create(&newthread , NULL, (void *)accept_request, (void *)(intptr_t)client_sock) != 0)
            perror("pthread_create");
    }

    close(server_sock);

    return(0);
}
