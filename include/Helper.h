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
        static int readline(int fd, char *buf, int size)
        {
            int n, rc;
            char c, *ptr;
            
            ptr = buf;
            
            for (n = 0; n < size-1; ++n)
            {
                rc = read(fd, &c, 1);
                if (rc == 1)
                {
                    if (c == '\n')
                        break;
                    *ptr++ = c;
                }
                else if (rc == 0)
                {
                    break;
                }
                else
                {
                    return -1;
                }
            }
            
            *ptr++ = '\n';
            *ptr = '\0';
            
            //std::cout << (ptr-buf) / sizeof(char*) << std::endl;
            return (ptr-buf);
        }
};

#endif
