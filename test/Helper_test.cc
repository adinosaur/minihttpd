
#include "../include/Base/Helper.h"
#include <assert.h>
#include <iostream>

using namespace std;

void date_test()
{
    cout << Helper::date() << endl;
}

void time_test()
{
    cout << Helper::time() << endl;
}

void urlcheck_test()
{
    cout << Helper::urlUnderRootDir("/www/cgi-bin/dinosaur/test.cgi") << endl;
    cout << Helper::urlUnderRootDir("/../www/cgi-bin/dinosaur/test.cgi") << endl;
    cout << Helper::urlUnderRootDir("/www/cgi-bin/dinosaur/../../../test.cgi") << endl;
    cout << Helper::urlUnderRootDir("/www/../../../dinosaur/test.cgi") << endl;
    
    assert(Helper::urlUnderRootDir("/www/cgi-bin/dinosaur/test.cgi") == true);
    assert(Helper::urlUnderRootDir("/../www/cgi-bin/dinosaur/test.cgi") == false);
    assert(Helper::urlUnderRootDir("/www/cgi-bin/dinosaur/../../../test.cgi") == true);
    assert(Helper::urlUnderRootDir("/www/../../../dinosaur/test.cgi") == false);
}

void test()
{
    date_test();
    time_test();
    urlcheck_test();
}

int main(int ac, char ** av)
{
    test();
    return 0;
}
