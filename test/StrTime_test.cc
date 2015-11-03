
#include "../include/Base/StrTime.h"
#include <iostream>
using namespace std;

int main(int ac, char** av)
{
    cout << StrTime().to_string("%d") << endl;
    return 0;
}
