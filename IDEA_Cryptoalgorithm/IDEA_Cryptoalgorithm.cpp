﻿#include <iostream>
#include<fstream>
#include <io.h>
#include <fcntl.h>

#define ENCRYPT true
#define DECRYPT false


#define HEX 16
#define DEC 10

#define WITH_SPACES  1
#define WITHOUT_SPACES  0

#define O std::wcout<<
#define I std::wcin>>
#define E <<std::endl;

wchar_t** allocate2Darray(int8_t, int8_t);
void deallocate2Darray(wchar_t**, int8_t);
void randomlyfill2Darray(wchar_t**, int8_t, int8_t);
void printWstringAs_UINT16_T(std::wstring, int8_t);
void printWstringAs_UINT16_T(wchar_t*, int8_t, int8_t);
void print1Darray(wchar_t*);
void print2Darray(wchar_t**, int8_t, int8_t, int8_t);
std::wstring inHex(int);

#pragma region mathematical operations
#define MATH_TYPE int
uint64_t mod_val = pow(2, 16) + 1;
MATH_TYPE _mul(MATH_TYPE x, MATH_TYPE y) {
    if (x == 0)
        x = mod_val - 1;
    if (y == 0)
        y = mod_val - 1;

    int xx = x;
    int yy = y;
    int64_t rt = (xx * yy) % mod_val;
    return rt;
}
MATH_TYPE _sum(MATH_TYPE x, MATH_TYPE y) {
    int xx = x;
    int yy = y;
    int64_t rt = (xx + yy) % (mod_val - 1);
    return rt;
}
MATH_TYPE _xor(MATH_TYPE x, MATH_TYPE y) {
    MATH_TYPE rt = x ^ y;
    return rt % (mod_val);
}
wchar_t _mulInv(int b, int n = mod_val) {
    int r1 = n;
    int r2 = b;
    int r;

    int t1 = 0;
    int t2 = 1;
    int t;

    int q;

    while (r2 > 0) {
        q = r1 / r2;

        r = r1 - q * r2;
        r1 = r2; r2 = r;

        t = t1 - q * t2;
        t1 = t2; t2 = t;
    }
    if (r1 == 1) {

        if (t1 * b % n != 1) {
            return t1 + 1;
        }
        return t1;
    }
}
wchar_t  _addInv(int16_t b, int16_t n = mod_val) {
    return n - b;
}
#pragma endregion


int length(wchar_t* message) {
    int16_t i = 0;
    while (message[i]) {
        i++;
    }
    return i;
}
void padding(wchar_t* plain_text) {
    int _txtLen = length(plain_text);
    int8_t requiredforPad = 4 - _txtLen % 4;

    int newsize = _txtLen + requiredforPad;
    if (requiredforPad > 0) {
        for (int8_t i = 0; i < requiredforPad; i++) {
            *(plain_text + _txtLen + 2 * i) = L'0';
        }
    }
}


void keygen(wchar_t* key) {
    *(key + 0) = 1;
    *(key + 1) = 2;
    *(key + 2) = 3;
    *(key + 3) = 4;
    *(key + 4) = 5;
    *(key + 5) = 6;
    *(key + 6) = 7;
    *(key + 7) = 8;
}
void key_rounds_gen_enc(wchar_t* encryptionkey, wchar_t** key_rounds) {
    wchar_t key[9];
    for (int8_t i = 0; i < 8; i++) {
        key[i] = encryptionkey[i];
    }
    int8_t counter = 0;
    int8_t key_ptr = 0;
    for (int8_t i = 0; i < 9; i++) {
        for (int8_t j = 0; j < 6; j++) {
            *(*(key_rounds + i) + j) = 0;
        }
    }
    for (int8_t i = 0; i < 9; i++) {
        for (int8_t j = 0; j < 6; j++) {
            counter++;
            *(*(key_rounds + i) + j) = *(key + key_ptr);
            key_ptr++;
            if (key_ptr == 8) {

                #pragma region shift 15 positions
                wchar_t key0 = *(key);
                for (int8_t k = 0; k < 7; k++) {
                    *(key + k) = *(key + k + 1);
                }
                *(key + 7) = key0;
                #pragma endregion

                #pragma region shift 10 positions
                key0 = *(key + 0);
                for (int8_t k = 0; k < 7; k++) {
                    *(key + k) = *(key + k) << 9 xor *(key + k + 1) >> 7;
                }
                *(key + 7) = *(key + 7) << 9 xor key0 >> 7;
                #pragma endregion

                key_ptr = 0;
            }
        }
    }
}
void key_rounds_gen_dec(wchar_t* key, wchar_t** key_rounds) {
    key_rounds_gen_enc(key, key_rounds);
    #define KEY_PARAM key_rounds, 9, 6
    O std::endl;
    print2Darray(KEY_PARAM, HEX);
    O std::endl;
    wchar_t first;
    wchar_t last;

    for (int8_t i = 0; i < 5; i++) {
        for (int8_t j = 0; j < 6; j++) {
            if (j == 0 || j == 3) {
                if (i < 4) {
                    first = key_rounds[i][j];
                    last = key_rounds[8 - i][j];
                    key_rounds[i][j] = _mulInv(last);
                    key_rounds[8 - i][j] = _mulInv(first);
                }
                else {
                    key_rounds[i][j] = _mulInv(key_rounds[i][j]);
                }

            }
            if (j == 1 || j == 2) {
                first = key_rounds[i][j];
                last = key_rounds[8 - i][j];
                key_rounds[i][j] = _addInv(last) - 1;
                key_rounds[8 - i][j] = _addInv(first) - 1;
            }
            if ((j == 4 || j == 5)&&(i<4)) {
                first = key_rounds[i][j];
                last = key_rounds[7 - i][j];
                key_rounds[i][j] = last;
                key_rounds[7 - i][j] = first;
            }
        }
    }
    std::swap(key_rounds[7][1], key_rounds[7][2]);
    for (int8_t i = 1; i < 7; i++) {
        std::swap(key_rounds[i][1], key_rounds[i][2]);
    }
}

void round(wchar_t* buffer, wchar_t** key_rounds, int8_t iterationNmb) {
#define RNDS_TYPE uint16_t
#define K key_rounds[iterationNmb]
    RNDS_TYPE A = _mul(buffer[0], K[0]);
    RNDS_TYPE B = _sum(buffer[1], K[1]);
    RNDS_TYPE C = _sum(buffer[2], K[2]);
    RNDS_TYPE D = _mul(buffer[3], K[3]);
    RNDS_TYPE _E = _xor(A, C);
    RNDS_TYPE F = _xor(B, D);


    RNDS_TYPE q1 = _mul(_E, K[4]);
    RNDS_TYPE q2 = _sum(F, q1);
    RNDS_TYPE q3 = _mul(q2, K[5]);
    RNDS_TYPE q4 = _xor(A, q3);

    buffer[0] = _xor(A, _mul(_sum(F, _mul(_E, K[4])), K[5]));
    buffer[1] = _xor(C, _mul(_sum(F, _mul(_E, K[4])), K[5]));
    buffer[2] = _xor(B, _sum(_mul(_E, K[4]), _mul(_sum(F, _mul(_E, K[4])), K[5])));
    buffer[3] = _xor(D, _sum(_mul(_E, K[4]), _mul(_sum(F, _mul(_E, K[4])), K[5])));

    if (iterationNmb == 7) {
        int c = buffer[1];
        buffer[1] = buffer[2];
        buffer[2] = c;
    }
}
std::wstring encrypt_decrypt(wchar_t* key, wchar_t* plain_text, bool enc_dec) {
    wchar_t** key_rounds = allocate2Darray(9, 6);
#define KEY_PARAM key_rounds, 9, 6
    if (enc_dec == ENCRYPT) {
        key_rounds_gen_enc(key, key_rounds);
        print2Darray(KEY_PARAM, DEC);
        O std::endl E;
        print2Darray(KEY_PARAM, HEX);
        O std::endl E;
    }
    else {
        key_rounds_gen_dec(key, key_rounds);
        print2Darray(KEY_PARAM, DEC);
        O std::endl E;
        print2Darray(KEY_PARAM, HEX);
        O std::endl E;
    }
    int _txtLen = length(plain_text);
    int requiredforPad = 4 - _txtLen % 4;

    int inxbuffer = 0;
    wchar_t buffer[4];
    std::wstring cipher_text = L"";
     for (int cnt = 0; cnt < _txtLen + requiredforPad; cnt++) {
         if (cnt % 4 == 0) {
             buffer[0] = *(plain_text + cnt);
             buffer[1] = *(plain_text + cnt + 1);
             buffer[2] = *(plain_text + cnt + 2);
             buffer[3] = *(plain_text + cnt + 3);
             for (int8_t i = 0; i < 8; i++) {
                 round(buffer, key_rounds, i);
             }
             buffer[0] = _mul(buffer[0], key_rounds[8][0]);
             buffer[1] = _sum(buffer[1], key_rounds[8][1]);
             buffer[2] = _sum(buffer[2], key_rounds[8][2]);
             buffer[3] = _mul(buffer[3], key_rounds[8][3]);

             cipher_text = cipher_text + buffer[0] + buffer[1] + buffer[2] + buffer[3];
         }
     }
     cipher_text = cipher_text.erase(_txtLen+requiredforPad, 1024 - _txtLen + requiredforPad);
     return cipher_text;
     deallocate2Darray(key_rounds, 9);
}
#pragma region array functions
wchar_t** allocate2Darray(int8_t n, int8_t m) {
    wchar_t** key = new wchar_t* [n];
    for (int8_t i = 0; i < n; i++) {
        *(key + i) = new wchar_t[m];
    }
    return key;
}
void deallocate2Darray(wchar_t** key, int8_t n) {
    for (int8_t i = 0; i < n; i++) {
        delete[] * (key + i);
    }
    delete[] key;
}
void randomlyfill2Darray(wchar_t** key, int8_t n, int8_t m) {
    for (int8_t i = 0; i < n; i++) {
        for (int8_t j = 0; j < m; j++) {
            *(*(key + i) + j) = (wchar_t)(rand() % mod_val);
        }
    }
}
void print2Darray(wchar_t** key, int8_t n, int8_t m, int8_t CS) {
    for (int8_t i = 0; i < n; i++) {
        for (int8_t j = 0; j < m; j++) {
            if (CS == HEX) {
                std::wcout << inHex(*(*(key + i) + j)) << "\t";
            }
            else {
                std::cout << *(*(key + i) + j) << "\t";
            }

        }
        std::cout << "\n";
    }
}
void printWstringAs_UINT16_T(wchar_t* plain_text, int8_t CS, int8_t spaces) {
    std::wstring str(plain_text);
    O "\n";
    O "\n";
    int devider = 16;
    if (spaces == WITHOUT_SPACES)
        O "0x";
    for (int i = 0; i < str.length(); i++) {
        if (CS == HEX) {
            if(spaces==WITHOUT_SPACES)
                O inHex((uint16_t)str[i]);
            else
                O inHex((uint16_t)str[i]) << "\t";
        }
        else {
            if (spaces == WITHOUT_SPACES)
                O(uint16_t)str[i];
            else
                O(uint16_t)str[i] << "\t";
        }
        if (i % devider == 0 && i != 0) {
            O "\n";
        }
    }
    O "\n";
    O "\n";
}
void printWstringAs_UINT16_T(std::wstring str, int8_t CS, int8_t spaces) {
    O "\n";
    O "\n";
    int devider = 16;
    if (spaces == WITHOUT_SPACES)
        O "0x";
    for (int i = 0; i < str.length(); i++) {
        if (CS == HEX) {
            if (spaces == WITHOUT_SPACES)
                O inHex((uint16_t)str[i]);
            else
                O inHex((uint16_t)str[i]) << "\t";
        }
        else {
            if (spaces == WITHOUT_SPACES)
                O(uint16_t)str[i];
            else
                O(uint16_t)str[i] << "\t";
        }
        if (i % devider == 0 && i != 0) {
            O "\n";
        }
    }
    O "\n";
    O "\n";
}
#pragma endregion


void reverseStr(std::wstring& str)
{
    int n = str.length();

    for (int i = 0; i < n / 2; i++)
        std::swap(str[i], str[n - i - 1]);
}
std::wstring inHex(int number) {
    int x = number;
    int y = 0;

    std::wstring result = L"";

    while (x > 0) {
        y = x % 16;
        switch (y) {
        case 0:
            result = result + L"0";
            break;
        case 1:
            result = result + L"1";
            break;
        case 2:
            result = result + L"2";
            break;
        case 3:
            result = result + L"3";
            break;
        case 4:
            result = result + L"4";
            break;
        case 5:
            result = result + L"5";
            break;
        case 6:
            result = result + L"6";
            break;
        case 7:
            result = result + L"7";
            break;
        case 8:
            result = result + L"8";
            break;
        case 9:
            result = result + L"9";
            break;
        case 10:
            result = result + L"a";
            break;
        case 11:
            result = result + L"b";
            break;
        case 12:
            result = result + L"c";
            break;
        case 13:
            result = result + L"d";
            break;
        case 14:
            result = result + L"e";
            break;
        case 15:
            result = result + L"f";
            break;
        }
        x = x / 16;
    }
    reverseStr(result);
    return result;
}


int main()
{

    wchar_t key[9];
    wchar_t message[1024];;

    std::cout << "secret key:\t";
    std::wcin.getline(key, 9);
    std::cout << "message:\t";
    std::wcin.getline(message, 1024);
    O "Plain text\t:" E;

    std::wstring cipher_text = encrypt_decrypt(key, message, ENCRYPT);
    std::wstring decrypted_text = encrypt_decrypt(key, &cipher_text[0], DECRYPT);
    O "Ciphered text\t:" E;
    printWstringAs_UINT16_T(cipher_text, HEX, WITHOUT_SPACES);

    O "Decrypted text\t:" E;
    O decrypted_text.length() E;
    int c=0;
    while (decrypted_text[c]!='\0') {
        O decrypted_text[c];
        c++;
    }
    
    std::wcout <<L"\n\n"<< std::endl;
    system("pause");
}