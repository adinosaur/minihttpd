
#include "Base64.h"
#include <string>
#include <iostream>

using namespace std;

int main()
{
    char buf[] = "Man is distinguishedas";
    Base64 base64;

    string cipher = base64.encode(buf, buf+sizeof(buf));
    string plain = base64.decode(cipher);
    
    cout << "cipher: " << cipher << endl;
    cout << "plain: " << plain << endl;
    return 0;
}
