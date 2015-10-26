//
// Date: 2015/10/26
// Author: Dinosaur W.
//

#ifndef RANDOM_STRING_H
#define RANDOM_STRING_H

#include "Base64.h"
#include <openssl/md5.h>
#include <time.h>
#include <string>
#include <vector>

class RandomString
{
    public:
        
        RandomString(std::string dft)
        {
            for (auto c : dft)
                default_random_string.push_back(static_cast<unsigned char>(dft[i]));
        }
        
        std::string gen()
        {
            time_t cur_time;
            time(&cur_time);
            
            string random_str = std::to_string(cur_time);
            random_str += default_random_string;
            MD5()
        }
    private:
        std::vector<unsigned char> default_random_string;
        unsigned char default_random_string[32];
};

RandomString randomEngine("uvmMi12NRtYpEj9nUQ6AaHx5scPbJF4e");

#endif
