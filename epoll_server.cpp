#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <mysql++.h>
#include <string>

int main()
{
    int listenfd = socket(AF_INET , SOCK_STREAM,0);
   // printf("%d\n", listenfd);

    assert(listenfd != -1);
    struct sockaddr_in ser,cli;
    memset(&ser,0,sizeof(ser));
    ser.sin_family =AF_INET;
    ser.sin_port = htons(7000);
    ser.sin_addr.s_addr = inet_addr("127.0.0.1");
    int res = bind(listenfd, (struct sockaddr*)&ser, sizeof(ser));
    assert(res != -1);
    listen(listenfd,5);
    int epfd = epoll_create(5);
    printf("%d\n", epfd);
    assert(epfd != -1);//,,
    struct epoll_event revents[128];
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = listenfd;

    epoll_ctl(epfd ,EPOLL_CTL_ADD,listenfd ,&event);

    /*connect db*/
    const char* db = 0, *server = 0, *user = 0, *password = "";
    db = "firstdb";
    server = "localhost";
    user = "root";
    password = "123456";

    mysqlpp::Connection conn(false);

    if(conn.connect(db, server, user, password))
    {
        std::cout << "connect db succeed!" << '\n';
    }
    else
    {
        std::cout << "connect db failed!" << '\n';
    }

    int w = 0;
    while(!w)
    {
        int n = epoll_wait(epfd, revents, 128, -1);
        printf("%d\n",n);
        
        assert(n != -1);
        if(n == 0)
        {
            printf("time out\n");
            continue;
        }
        int i = 0;
        for(;i<n;i++){
            if ( revents[i].events & EPOLLIN){   //have new connect
                int fd = revents[i].data.fd;
                if(fd == listenfd)
                {
                    printf("link\n");
                    int len = sizeof(cli);
                    int c = accept(fd, (struct sockaddr*)&cli, (socklen_t*)&len);
                    assert(c != -1);
                    event.data.fd = c;
                    epoll_ctl(epfd, EPOLL_CTL_ADD , c, &event);
                    continue;
                }
                else{
                    char buff [128] = {0};
                    int n = recv ( fd, buff,128,0);
		    if(strncmp(buff, "end", 3) == 0){
			    w = 1;
			    break;
		    }
                    printf("%d : : %s\n" , fd , buff);
                    mysqlpp::Query query = conn.query();
                    query << "insert into conversation(Nickname, Time, content) values(""'" + std::to_string(fd)  + "'"", now(), ""'" + buff + "'"");";
                    //query << "insert into conversation(name, time, content) values('songxuan', now(), 'I love lzr');";
                    query.execute();

                    send(fd, "OK", sizeof ("OK"), 0);
                }
            }
        }
    }
    return 0;
}
                
