#include "Base64.h"
#include <assert.h>
//#include <string>

using namespace std;

char Base64::_encode_map[64] = 
{
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '+', '/'
};

std::unordered_map<char, char> Base64::_decode_map = 
{
    {'A', 0x00},    {'Q', 0x10},    {'g', 0x20},    {'w', 0x30},
    {'B', 0x01},    {'R', 0x11},    {'h', 0x21},    {'x', 0x31},
    {'C', 0x02},    {'S', 0x12},    {'i', 0x22},    {'y', 0x32},
    {'D', 0x03},    {'T', 0x13},    {'j', 0x23},    {'z', 0x33},
    {'E', 0x04},    {'U', 0x14},    {'k', 0x24},    {'0', 0x34},
    {'F', 0x05},    {'V', 0x15},    {'l', 0x25},    {'1', 0x35},
    {'G', 0x06},    {'W', 0x16},    {'m', 0x26},    {'2', 0x36},
    {'H', 0x07},    {'X', 0x17},    {'n', 0x27},    {'3', 0x37},
    {'I', 0x08},    {'Y', 0x18},    {'o', 0x28},    {'4', 0x38},
    {'J', 0x09},    {'Z', 0x19},    {'p', 0x29},    {'5', 0x39},
    {'K', 0x0a},    {'a', 0x1a},    {'q', 0x2a},    {'6', 0x3a},
    {'L', 0x0b},    {'b', 0x1b},    {'r', 0x2b},    {'7', 0x3b},
    {'M', 0x0c},    {'c', 0x1c},    {'s', 0x2c},    {'8', 0x3c},
    {'N', 0x0d},    {'d', 0x1d},    {'t', 0x2d},    {'9', 0x3d},
    {'O', 0x0e},    {'e', 0x1e},    {'u', 0x2e},    {'+', 0x3e},
    {'P', 0x0f},    {'f', 0x1f},    {'v', 0x2f},    {'/', 0x3f}
};

string Base64::encode(const string plain)
{
    return encode(plain.data(), plain.data() + plain.size());
}

string Base64::encode(const char* beg, const char* end)
{
    string ciphertext;
    const char* cur = beg;
    int i;
    
    while (cur != end)
    {
        beg = cur;
        for (i = 0; i != 3 && cur != end; i++)
            cur++;
        
        if (cur != end || i == 3)
            ciphertext.append(do_encode3(beg));
        else if (i == 1)
            ciphertext.append(do_encode1(beg));
        else if (i == 2)
            ciphertext.append(do_encode2(beg));
    }
     
    return ciphertext;
}
        
string Base64::decode(const string cipher)
{
    return decode(cipher.data(), cipher.data() + cipher.size());
}

string Base64::decode(const char* beg, const char* end)
{
    string plaintext;
    while (beg < end)
    {
        plaintext.append(do_decode(beg));
        beg += 4;
    }
    return plaintext;
}


string Base64::do_decode(const char* beg)
{
    string plain;

    char x1 = _decode_map[beg[0]] << 2;
    x1 += (_decode_map[beg[1]] >> 4);
    plain.push_back(x1);

    if (beg[2] != '=')
    {
        char x2 = (_decode_map[beg[1]] << 4);
        x2 += (_decode_map[beg[2]] >> 2);
        plain.push_back(x2);
    }
    if (beg[3] != '=')
    {
        char x3 = (_decode_map[beg[2]] << 6);
        x3 += _decode_map[beg[3]];
        plain.push_back(x3);
    }
    return plain;
}

string Base64::do_encode1(const char* beg)
{
    string cipher;
    
    char x1 = (0xfc & *beg) >> 2;
    char x2 = (0x03 & *beg) << 4;
    
    cipher.push_back(_encode_map[static_cast<int>(x1)]);
    cipher.push_back(_encode_map[static_cast<int>(x2)]);
    cipher.push_back('=');
    cipher.push_back('=');
    
    return cipher;
}

string Base64::do_encode2(const char* beg)
{
    string cipher;

    char x1 = (0xfc & *beg) >> 2;
    char x2 = (0x03 & *beg) << 4;
    x2 += (0xf0 & *(beg+1)) >> 4;
    char x3 = (0x0f & *(beg+1)) << 2;
    
    cipher.push_back(_encode_map[static_cast<int>(x1)]);
    cipher.push_back(_encode_map[static_cast<int>(x2)]);
    cipher.push_back(_encode_map[static_cast<int>(x3)]);
    cipher.push_back('=');

    return cipher;
}

string Base64::do_encode3(const char* beg)
{
    string cipher;
    
    char x1 = (0xfc & *beg) >> 2;
    char x2 = (0x03 & *beg) << 4;
    x2 += (0xf0 & *(beg+1)) >> 4;
    char x3 = (0x0f & *(beg+1)) << 2;
    x3 += (0xc0 & *(beg+2)) >> 6;
    char x4 = 0x3f & *(beg+2);
    
    cipher.push_back(_encode_map[static_cast<int>(x1)]);
    cipher.push_back(_encode_map[static_cast<int>(x2)]);
    cipher.push_back(_encode_map[static_cast<int>(x3)]);
    cipher.push_back(_encode_map[static_cast<int>(x4)]);
    
    return cipher;
}
