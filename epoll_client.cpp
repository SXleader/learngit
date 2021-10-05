#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
int main()
{
    int sockfd = socket(AF_INET ,SOCK_STREAM,0);
    assert(sockfd != -1);
    struct sockaddr_in ser,cli;
    ser.sin_family =AF_INET;ser.sin_port = htons(7000);
    ser.sin_addr.s_addr = inet_addr( "127.0.0.1");
    int res = connect(sockfd, (struct sockaddr*)&ser,sizeof(ser));
    assert(res != -1);
    
    while(1){
        char buff[128] ={0};
        printf("INPUT :: ");
        fflush(stdout);
        fgets(buff,127,stdin);
        //if(strncmp(buff , "end" ,3) == 0){
        //    break;
        //}
        send ( sockfd, buff, strlen(buff)-1,0);
        //recv ( sockfd, buff,127,0);
        printf("buff :: %s\n", buff);
        if(strncmp(buff , "end" ,3) == 0){
            break;
        }
        recv ( sockfd, buff,127,0);
    }
    close( sockfd);
    return 0;
}

