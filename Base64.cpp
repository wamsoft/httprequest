#include <vector>

#include "Base64.h"

/*
 * Basic 認証で必要なBase64 のエンコード処理を実装しています。
 * デコードは現状必要ないので実装していません。
 */

using namespace std;

// Base64 変換コードテーブル
static TCHAR *code = _T("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");

#ifdef BASE64_TEST
#include <iostream>
#define TEST_STR "TESTTEST"
int _tmain(int argc, TCHAR * argv[]) {
	cout << base64encode((const unsigned TCHAR *)argv[1], tcslen(argv[1]));
	return 0;
}
#endif

template <typename SRC>
static inline void
encode_block(vector<TCHAR> &buf, const SRC *p) {
	buf.push_back(code[((unsigned int)p[0] & 0xfc) >> 2]);
	buf.push_back(code[((unsigned int)p[0] & 0x03) << 4 | ((unsigned int)p[1] & 0xf0) >> 4]);
	buf.push_back(code[((unsigned int)p[1] & 0x0f) << 2 | ((unsigned int)p[2] & 0xc0) >> 6]);
	buf.push_back(code[((unsigned int)p[2] & 0x3f)]);
}

template <typename SRC>
static inline tstring 
base64encode_tmpl(const SRC *input, int len) {
	vector<TCHAR> buf;
	if (len > 0) buf.reserve(len * 4 / 3 + 8); // サイズ予約
	const SRC *p = input;

	// 3文字ずつ変換
	if (len >= 3) {
		for (int i = 0; i <= len - 3; i += 3) {
			encode_block(buf, p);
			p += 3;
		}
	}

	// 3文字に満たない最後の余分を変換
	// 一旦3文字にして変換した後、余計な部分を終端'='で上書きする
	int odd = len % 3;
	if (odd != 0) {
		TCHAR last[3] = {0x0, 0x0, 0x0};
		for (int i = 0; i < odd; i++ ) {
			last[i] = *p++;
		}
		encode_block(buf, last);
		
		// 終端を追加
		for (int i = (2 - odd); i >= 0; i--) {
			buf[buf.size() - i - 1] = '=';
		}
	}
	buf.push_back('\0');
	
	return tstring((TCHAR *)&buf[0]);
}

tstring base64encode(const TCHAR         *input, int len) { return base64encode_tmpl(input, len); }
tstring base64encode(const unsigned char *input, int len) { return base64encode_tmpl(input, len); }


#define XX 255
static const unsigned char base64decode_table[] = {
//	 0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
	XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX, // 0x00
	XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX, // 0x10
	XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  62,  XX,  XX,  XX,  63, // 0x20 : +/
	52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  XX,  XX,  XX,  XX,  XX,  XX, // 0x30 : 0-9
	XX,   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14, // 0x40 : A-O
	15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  XX,  XX,  XX,  XX,  XX, // 0x50 : P-Z
	XX,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40, // 0x60 : a-o
	41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  XX,  XX,  XX,  XX,  XX, // 0x70 : p-z
	XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX, // 0x80
	XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX, // 0x90
	XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX, // 0xA0
	XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX, // 0xB0
	XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX, // 0xC0
	XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX, // 0xD0
	XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX, // 0xE0
	XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX,  XX  // 0xF0
	};
template <typename SRC>
static inline bool
decode_block(blob &output, const SRC *p) {
	const unsigned char b1 = base64decode_table[(unsigned char)p[0]];
	const unsigned char b2 = base64decode_table[(unsigned char)p[1]];
	const unsigned char b3 = base64decode_table[(unsigned char)p[2]];
	const unsigned char b4 = base64decode_table[(unsigned char)p[3]];
	if ((b1|b2|b3|b4) == XX) return false;

	output.push_back(((b1 & 0x3F) << 2) | ((b2 & 0x30) >> 4));
	output.push_back(((b3 & 0x3C) >> 2) | ((b2 & 0x0F) << 4));
	output.push_back( (b4 & 0x3F)       | ((b3 & 0x03) << 6));
	return true;
}
static inline bool
decode_term(blob &output, const unsigned char *p) {
	const unsigned char b1 = (p[0] == '=') ? 0 : base64decode_table[p[0]];
	const unsigned char b2 = (p[1] == '=') ? 0 : base64decode_table[p[1]];
	const unsigned char b3 = (p[2] == '=') ? 0 : base64decode_table[p[2]];
	const unsigned char b4 = (p[3] == '=') ? 0 : base64decode_table[p[3]];
	if ((b1|b2|b3|b4) == XX) return false;

	/**/             output.push_back(((b1 & 0x3F) << 2) | ((b2 & 0x30) >> 4));
	if (p[2] != '=') output.push_back(((b3 & 0x3C) >> 2) | ((b2 & 0x0F) << 4));
	if (p[3] != '=') output.push_back( (b4 & 0x3F)       | ((b3 & 0x03) << 6));
	return true;
}
#undef XX
template <typename SRC>
static inline bool
base64decode_tmpl(blob &output, const SRC *input, int len) {
	if (len > 0) output.reserve(len * 3 / 4 + 6); // サイズ予約
	const SRC *p = input;
	const SRC *const q = input + len;

	// 4文字ずつ変換
	if (len > 4) {
		const SRC *const end = p + len-4;
		for (; p < end; p+=4) {
			if (!decode_block(output, p)) return false;
		}
	}

	// 残り分(※最後の4文字も含む)
	if (p < q) {
		unsigned char work[] = {'=','=','=','='};
		for (int i = 0; i < 4 && p < q; ++i) work[i] = (unsigned char)*p++;
		if (!decode_term(output, work)) return false;
	}

	return true;
}

bool base64decode(blob &output, const TCHAR         *input, int len) { return base64decode_tmpl(output, input, len); }
bool base64decode(blob &output, const unsigned char *input, int len) { return base64decode_tmpl(output, input, len); }


