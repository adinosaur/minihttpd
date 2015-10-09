//
// Date: 2015/10/10
// Author: Dinosaur W.
//

#ifndef MINIHTTPD_EVENTLOOP_H
#define MINIHTTPD_EVENTLOOP_H

#include "HttpServer.h"

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>

class EventLoop : public NonCopyable
{
    public:
        explicit EventLoop(uint32_t port):
            NonCopyable(),
            _port(port),
            _sockfd(setup(_port))
        {
        }
        
        ~EventLoop()
        {
            close(_sockfd);
        }
        
        //
        // accept request
        //
        void loop()
        {
            while (1)
            {
                struct sockaddr_in cliaddr;
                socklen_t length;
                //pthread_t pid;
                
                // accept
                int connfd = accept(_sockfd, (struct sockaddr*)(&cliaddr), &length);
                if (connfd < 0)
                    perror("accept error...");
                
                //if (pthread_create(&pid, NULL, accept_request, &connfd) != 0)
                //    perror("pthread_create error...");
                HttpServer http(connfd);
                http.accept_request();
                http.handle_request();
            }
        }
        
    private:
        //
        // setup the http server.
        // return socket fd.
        //
        int setup(uint32_t port)
        {
            struct sockaddr_in seraddr;
            
            // create socket
            int sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (sockfd < 0)
                perror("socket error...");
            
            // bind
            bzero(&seraddr, sizeof(seraddr));
            seraddr.sin_family = AF_INET;
            seraddr.sin_port = htons(port);
            seraddr.sin_addr.s_addr = htonl(INADDR_ANY);
            if (bind(sockfd, (struct sockaddr*)(&seraddr), sizeof(seraddr)) < 0)
                perror("bind error...");
            
            // listen
            if (listen(sockfd, 5) < 0)
                perror("listen error...");
            
            return sockfd;
        }
        
        uint32_t _port;
        int _sockfd;
};

#endif

