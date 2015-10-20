//
// Date: 2015/10/21
// Author: Dinosaur W.
//

#ifndef _MINIHTTPD_EXPECTION_H
#define _MINIHTTPD_EXPECTION_H

#include <string>
using std::string;

class BaseException
{
    public:
        BaseException(const string& f, int l, 
            const string& et, const string& ei):
                filename(f),
                line_num(l),
                error_type(et),
                error_info(ei)
            {
            }
        
        string filename;
        int line_num;
        string error_type;
        string error_info;
};

class ServeFileException : public BaseException 
{
    public:
        ServeFileException(const string& f, int l, 
            const string& et, const string& ei):
                BaseException(f, l, et, ei)
            {
            }
};

class ExecuteCGIException : public BaseException 
{
    public:
        ExecuteCGIException(const string& f, int l, 
            const string& et, const string& ei):
                BaseException(f, l, et, ei)
            {
            }
};

#endif
