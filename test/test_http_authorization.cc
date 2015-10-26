
#include "../include/Http/HttpAuthorization.h"
#include <string>
#include <vector>
#include <iostream>

using namespace std;

int main()
{
    string auth_field;
    
    auth_field += " Digest";
    auth_field += " username=\"dinosaur\",";
    auth_field += " realm=\"Shopping Cart\",";
    auth_field += " nonce=\"66C4EF58DA7CB956BD04233FBB64E0A4\",";
    auth_field += " uri=\"/cgi-bin/checkout?cart=17854\",";
    auth_field += " qop=\"auth\",";
    auth_field += " nc=\"00000001\",";
    auth_field += " cnonce=\"CFA9207102EA210EA210FFC1120F6001110D073\",";
    auth_field += " response=\"E483C94FO83CA29109A7BA83D10FE519\"";
    // 3347bbe30c00207bf86eb03b4b79038e
    // a2641e89a2dfbc0536c785408b10c64f
    
    string user;
    string realm;
    string nonce;
    string uri;
    string qop;
    string nc;
    string cnonce;
    string response;
    
    DigestAuth digestAuth;
    
    digestAuth.extract_auth_field(auth_field, user, realm, nonce,
        uri, qop, nc, cnonce, response);
    
    cout << "user:" << user << endl;
    cout << "realm:" << realm << endl;
    cout << "nonce:" << nonce << endl;
    cout << "uri:" << uri << endl;
    cout << "qop:" << qop << endl;
    cout << "nc:" << nc << endl;
    cout << "cnonce:" << cnonce << endl;
    cout << "response:" << response << endl;

    string response2 = digestAuth.digest(
        "GET", user, "12345", realm, nonce, uri, qop, nc, cnonce);
    
    cout << "response2:" << response2 << endl;

    return 0;
}
