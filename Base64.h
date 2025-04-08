#ifndef __BASE64_H_
#define __BASE64_H_

#include <tchar.h>
#include <string>

using namespace std;
typedef basic_string<TCHAR> tstring;
typedef basic_string<unsigned char> blob;

tstring base64encode(const TCHAR *input, int len);
tstring base64encode(const unsigned char *input, int len);

bool base64decode(blob &output, const TCHAR *input, int len);
bool base64decode(blob &output, const unsigned char *input, int len);

#endif __BASE64_H_
