//#include "../include/MD5.h"
#include "../include/md5/md5.h"

#include <iostream>
#include <string>

using namespace std;

int main()
{
    string ss;
    string s = MD5("a3842e2740bbe41c95cb2a02d8a8de2a:66C4EF58DA7CB956BD04233FBB64E0A4:00000001:CFA9207102EA210EA210FFC1120F6001110D073:auth:914f2894f60caebd226d85bbb6f39f48").toStr();
    cout << s << endl;
    return 0;
}
