
#include "../include/UrlCheck.h"

#include <assert.h>

#include <string>
#include <iostream>

using namespace std;

void test()
{
    assert(urlUnderRootDir("/www/cgi-bin/dinosaur/test.cgi") == true);
    assert(urlUnderRootDir("/../www/cgi-bin/dinosaur/test.cgi") == false);
    assert(urlUnderRootDir("/www/cgi-bin/dinosaur/../../../test.cgi") == true);
    assert(urlUnderRootDir("/www/../../../dinosaur/test.cgi") == false);
}

int main()
{
    test();
    return 0;
}
