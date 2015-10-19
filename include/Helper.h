//
// Date: 2015/10/09
// Author: Dinosaur W.
//

#ifndef MINIHTTPD_HELPER_H
#define MINIHTTPD_HELPER_H

#include <sys/types.h>
#include <sys/socket.h>

#include <string>
#include <iostream>

class Helper
{
    public:
        static int get_line(int sock, char *buf, int size)
        {
            int i = 0;
            char c = '\0';
            int n;
            
            while ((i < size - 1) && (c != '\n'))
            {
                n = recv(sock, &c, 1, 0);
                if (n > 0)
                {
                    if (c == '\r')
                    {
                        n = recv(sock, &c, 1, MSG_PEEK);
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
            return i;
        }
};

#endif
