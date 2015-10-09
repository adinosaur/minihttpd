//
// Date: 2015/10/09
// Author: Dinosaur W.
//

#ifndef MINIHTTPD_HTTPBASE_H
#define MINIHTTPD_HTTPBASE_H

class HttpBase
{
    public:
        // http method.
        static const int METHOD_UNKNOWN;
        static const int METHOD_GET;
        static const int METHOD_POST;
        
        // http version.
        static const int HTTPV_UNKONWN;
        static const int HTTPV_09;
        static const int HTTPV_10;
        static const int HTTPV_11;
        static const int HTTPV_20;
        
        // http status code
        static const int HTTP_STATUS_CODE_UNKNOWN;
        static const int HTTP_STATUS_CODE_200;
        static const int HTTP_STATUS_CODE_301;
        static const int HTTP_STATUS_CODE_400;
        static const int HTTP_STATUS_CODE_404;
        static const int HTTP_STATUS_CODE_501;
        
};

// http method.
const int HttpBase::METHOD_UNKNOWN = 0;
const int HttpBase::METHOD_GET = 1;
const int HttpBase::METHOD_POST = 2;

// http version.
const int HttpBase::HTTPV_UNKONWN = 0;
const int HttpBase::HTTPV_09 = 1;
const int HttpBase::HTTPV_10 = 2;
const int HttpBase::HTTPV_11 = 3;
const int HttpBase::HTTPV_20 = 4;

// http status code
const int HttpBase::HTTP_STATUS_CODE_UNKNOWN = 0;
const int HttpBase::HTTP_STATUS_CODE_200 = 200;
const int HttpBase::HTTP_STATUS_CODE_301 = 301;
const int HttpBase::HTTP_STATUS_CODE_400 = 400;
const int HttpBase::HTTP_STATUS_CODE_404 = 404;
const int HttpBase::HTTP_STATUS_CODE_501 = 501;

#endif
