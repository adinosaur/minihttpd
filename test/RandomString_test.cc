
#include "../include/Base/Helper.h"
#include <iostream>
#include <string>

using namespace std;

void test()
{
    cout << Helper::random_string(2) << endl;
}

int main(int ac, char** av)
{
    test();
    return 0;
}
