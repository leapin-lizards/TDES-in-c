#include <stdio.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
typedef unsigned char BYTE; //1 바이트 unsigned char 를 이용해 각각의 비트 조작
typedef struct 
{
    int r;
    int c;
} RC;
RC getRC(int n) //바이트로 구성된 비트 배열에서 특정 비트를 찾아주는 함수. r 은 바이트의 위치, c 는 바이트 내 비트 위치
{
    RC ret;
    ret.r = n / 8;
    ret.c = 7 - n % 8;
    return ret;
}
int ipow(int base, int exp) //거듭제곱
{
    int result = 1;
    for (;;)
    {
        if (exp & 1)
            result *= base;
        exp >>= 1;
        if (!exp)
            break;
        base *= base;
    }

    return result;
}
int getBit(BYTE bit, int n) //바이트 내에서 비트 하나를 가져옴
{
    BYTE ret = 0x00;
    ret = bit & (0x01 << n);
    if (ret != 0x00)
    {
        ret = 0x01;
    }
    return ret;
}
void setBit(BYTE *bit, int val, int n)//바이트 내에서 비트 하나를 설정
{
    if (val == 1)
    {
        *bit = (*bit) | 0x01 << n;
    }
    else
    {
        *bit = (*bit) | 0x00 << n;
    }
}
int checkParity(BYTE *bits) //패리티 비트 확인. 사용하지 않음
{
    for (int i = 0; i < 8; i++)
    {
        int ones = 0;
        for (int j = 7; j >= 1; j--)
        {
            if (getBit(*(bits + i), j) == 0x01)
            {
                ones++;
            }
        }
        if (ones % 2 == 0)
        {
            if (getBit(*(bits + i), 0) == 0x00)
            {
                return 0;
            }
        }
        else
        {
            if (getBit(*(bits + i), 0) == 0x01)
            {
                return 0;
            }
        }
    }
    return 1;
}
void printBits(BYTE *bits, int bytes) //개별 비트 출력 함수
{
    printf("==========\n");
    for (int i = 0; i < bytes; i++)
    {
        for (int j = 7; j >= 0; j--)
        {

            printf("%d ", getBit(*(bits + i), j));
        }
        printf("\n");
    }
}
void printHex(BYTE *bits, int bytes) //16진수로 바이트 출력
{
    for (int i = 0; i < bytes; i++)
    {
        printf("%02x", bits[i]);
    }
    printf("\n");
}
void PBox(BYTE *inp, int size_bytes, int *table) //목표블록, 블록크기.테이블을 입력받으면 P-Box 전치함수 기능을 수행. DES 에서 E, Q, Initial Permutation, Inverse Initial Permutation, PC1, PC2 함수에서 이용
{
    BYTE dest[8] = {0};
    for (int i = 0; i < size_bytes * 8; i++)
    {
        int k = table[i];
        int a = getRC(k - 1).r;
        int b = getRC(k - 1).c;
        int set = getBit(*(inp + a), b);
        setBit(dest + getRC(i).r, set, getRC(i).c);
    }
    memcpy(inp, dest, sizeof(BYTE) * size_bytes);
}
void RHL(BYTE *inp, int round) //key scheduling 시 이용하는 함수. 블록을 반으로 나눠 비트를 몇칸씩 옮김. round 에 따라 몇칸 이동하는지 달라짐
{
    int R[16] = {1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};
    int n = R[round];
    if (n == 1)
    {
        BYTE dest[7] = {0};
        int temp1 = getBit(*(inp + getRC(0).r), getRC(0).c); 
        int temp2 = getBit(*(inp + getRC(28).r), getRC(28).c);
        for (int i = 0; i < 27; i++)
        {
            int a = getRC(i + 1).r;
            int b = getRC(i + 1).c;
            int set = getBit(*(inp + a), b);
            setBit(dest + getRC(i).r, set, getRC(i).c);
            // printBits(dest,7);
        }
        setBit(dest + getRC(27).r, temp1, getRC(27).c);
        // printBits(dest,7);
        for (int i = 28; i < 55; i++)
        {
            int a = getRC(i + 1).r;
            int b = getRC(i + 1).c;
            int set = getBit(*(inp + a), b);
            setBit(dest + getRC(i).r, set, getRC(i).c);
        }
        setBit(dest + getRC(55).r, temp2, getRC(55).c);
        // printBits(dest,7);
        memcpy(inp, dest, sizeof(dest));
    }
    if (n == 2)
    {
        BYTE dest[7] = {0};
        int temp1 = getBit(*(inp + getRC(0).r), getRC(0).c);
        int temp2 = getBit(*(inp + getRC(1).r), getRC(1).c);
        int temp3 = getBit(*(inp + getRC(28).r), getRC(28).c);
        int temp4 = getBit(*(inp + getRC(29).r), getRC(29).c);
        for (int i = 0; i < 26; i++)
        {
            int a = getRC(i + 2).r;
            int b = getRC(i + 2).c;
            int set = getBit(*(inp + a), b);
            setBit(dest + getRC(i).r, set, getRC(i).c);
        }
        setBit(dest + getRC(26).r, temp1, getRC(26).c);
        setBit(dest + getRC(27).r, temp2, getRC(27).c);
        for (int i = 28; i < 54; i++)
        {
            int a = getRC(i + 2).r;
            int b = getRC(i + 2).c;
            int set = getBit(*(inp + a), b);
            setBit(dest + getRC(i).r, set, getRC(i).c);
        }
        setBit(dest + getRC(54).r, temp3, getRC(54).c);
        setBit(dest + getRC(55).r, temp4, getRC(55).c);
        memcpy(inp, dest, sizeof(dest));
    }
}
void SBox(BYTE *inp) //S-Box 함수. 48비트를 입력받아 S1,S2,S3....S8 까지 모두 수행하고 결과를 종합해 32비트로 출력 
{
    BYTE sn[8][4][16] = {{{14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7}, {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8}, {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0}, {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}},
                         {{15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10}, {3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5}, {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15}, {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}},
                         {{10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8}, {13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1}, {13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7}, {1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}},
                         {{7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15}, {13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9}, {10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4}, {3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}},
                         {{2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9}, {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6}, {4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14}, {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}},
                         {{12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11}, {10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8}, {9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6}, {4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}},
                         {{4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1}, {13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6}, {1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2}, {6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}},
                         {{13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7}, {1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2}, {7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8}, {2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}}};

    BYTE dest[6] = {0};
    BYTE s[8];
    for (int i = 0; i < 48; i += 6)
    {
        int r[3];
        r[0] = getBit(*(inp + getRC(i).r), getRC(i).c);
        r[1] = getBit(*(inp + getRC(i + 5).r), getRC(i + 5).c);
        r[2] = r[0] * 2 + r[1];
        int c[5] = {0};
        for (int j = 0; j < 4; j++)
        {
            c[j] = getBit(*(inp + getRC(i + j + 1).r), getRC(i + j + 1).c);
            c[4] += ipow(2, 3 - j) * c[j];
        }
        s[i / 6] = sn[i / 6][r[2]][c[4]];
    }
    for (int i = 0; i < 4; i++)
    {
        dest[i] = (s[i * 2] << 4) + s[i * 2 + 1];
    }
    memcpy(inp, dest, sizeof(dest));
}
void DES(BYTE *input, BYTE *key, int mode) //DES 수행. mode 가 0 이면 encrypt, 1 이면 decrypt
{
    //필요한 P-Box 테이블 정의
    int PC1Table[56] = {57, 49, 41, 33, 25, 17, 9,
                        1, 58, 50, 42, 34, 26, 18,
                        10, 2, 59, 51, 43, 35, 27,
                        19, 11, 3, 60, 52, 44, 36,
                        63, 55, 47, 39, 31, 23, 15,
                        7, 62, 54, 46, 38, 30, 22,
                        14, 6, 61, 53, 45, 37, 29,
                        21, 13, 5, 28, 20, 12, 4};

    int PC2Table[48] = {14, 17, 11, 24, 1, 5, 3, 28,
                        15, 6, 21, 10, 23, 19, 12, 4,
                        26, 8, 16, 7, 27, 20, 13, 2,
                        41, 52, 31, 37, 47, 55, 30, 40,
                        51, 45, 33, 48, 44, 49, 39, 56,
                        34, 53, 46, 42, 50, 36, 29, 32};

    int Q[32] = {16, 7, 20, 21,
                 29, 12, 28, 17,
                 1, 15, 23, 26,
                 5, 18, 31, 10,
                 2, 8, 24, 14,
                 32, 27, 3, 9,
                 19, 13, 30, 6,
                 22, 11, 4, 25};

    int E[48] = {32, 1, 2, 3, 4, 5,
                 4, 5, 6, 7, 8, 9,
                 8, 9, 10, 11, 12, 13,
                 12, 13, 14, 15, 16, 17,
                 16, 17, 18, 19, 20, 21,
                 20, 21, 22, 23, 24, 25,
                 24, 25, 26, 27, 28, 29,
                 28, 29, 30, 31, 32, 1};

    int IPRevTable[64] = {40, 8, 48, 16, 56, 24, 64, 32,
                          39, 7, 47, 15, 55, 23, 63, 31,
                          38, 6, 46, 14, 54, 22, 62, 30,
                          37, 5, 45, 13, 53, 21, 61, 29,
                          36, 4, 44, 12, 52, 20, 60, 28,
                          35, 3, 43, 11, 51, 19, 59, 27,
                          34, 2, 42, 10, 50, 18, 58, 26,
                          33, 1, 41, 9, 49, 17, 57, 25};

    int IPTable[64] = {58, 50, 42, 34, 26, 18, 10, 2,
                       60, 52, 44, 36, 28, 20, 12, 4,
                       62, 54, 46, 38, 30, 22, 14, 6,
                       64, 56, 48, 40, 32, 24, 16, 8,
                       57, 49, 41, 33, 25, 17, 9, 1,
                       59, 51, 43, 35, 27, 19, 11, 3,
                       61, 53, 45, 37, 29, 21, 13, 5,
                       63, 55, 47, 39, 31, 23, 15, 7};

    PBox(input, 8, IPTable); //Initial Permutation 수행
    BYTE LPT[6];
    BYTE RPT[6];
    memcpy(LPT, input, sizeof(LPT)); 
    memcpy(RPT, input + 4, sizeof(RPT));
    BYTE keys[16][6];
    BYTE key1_temp[8];

    BYTE L[6];
    BYTE R[6];

    memcpy(L, LPT, sizeof(L));  //결과를 32비트씩 L 과 R 로 나눠 저장
    memcpy(R, RPT, sizeof(RPT));
   

    //Key scheduler (key generator) 부분
    memcpy(key1_temp, key, sizeof(key1_temp));
    PBox(key1_temp, 7, PC1Table); //key scheduling 에 필요한 PC1 수행

    printf("\ninput after initial permutation : ");
    printHex(input,8);
    for (int i = 0; i < 16; i++)
    {
        RHL(key1_temp, i); //순환이동
        BYTE buffer[7];
        memcpy(buffer, key1_temp, sizeof(buffer)); 
        PBox(buffer, 6, PC2Table); //key scheduling 에 필요한 PC2 수행
        memcpy(keys[i], buffer, sizeof(keys[i]));
    }

    //실제 DES 수행
    for (int i = 0; i < 16; i++)
    {
        int a;
        if (mode == 0)
            a = i;
        else
            a = 15 - i;
        printf("\nRound %d.\nUsing key : ", i + 1);
        printHex(keys[a], 7);
        BYTE temp[6];
        memcpy(temp, R, sizeof(L));
        PBox(temp, 6, E); //E 수행
        for (int j = 0; j < 6; j++) //키값과 E의 결과값 XOR
        {
            temp[j] ^= keys[a][j]; 
        }
        SBox(temp); //S-Box 수행
        PBox(temp, 4, Q); //Q (문서에는 P 라 나옴) 수행. 이 결과값이 F 함수의 결과값
        for (int j = 0; j < 6; j++) // F 함수의 결과값과 Left block XOR
        {
            temp[j] ^= L[j];
        }
        if (i == 15) //특수케이스 마지막 단계일 경우 f 값만 L 에 저장하고 끝
        {
            memcpy(L, temp, sizeof(L));
                            
            printf("Left block after round:%d : ",i+1);
            printHex(L,4);
            printf("Right block after round:%d : ",i+1);
            printHex(R,4);
            break;
        }
        //L 에는 이전 R 저장
        memcpy(L, R, sizeof(L));
        //F 함수의 결과값과 Left block XOR 한 값 R 에 저장
        memcpy(R, temp, sizeof(R));
                
        printf("Left block after round %d : ",i+1);
        printHex(L,4);
        printf("Right block after round %d : ",i+1);
        printHex(R,4);
    }
    memcpy(input, L, sizeof(BYTE[4]));
    memcpy(input + 4, R, sizeof(BYTE[4]));
    PBox(input, 8, IPRevTable); //마지막으로 Inverse Initial Permutation 수행
    printf("\nDone after reverse permutation. Block result is :");
    printHex(input,8);
}
int isValidSize(FILE *f) //입력파일이 8바이트인지 확인
{
    fseek(f, 0, SEEK_END); // seek to end of file
    int size = ftell(f);
    fseek(f,0,SEEK_SET);
    if (size != 8)
    {
        return 0;
    }
    return 1;
}
int main()
{
    FILE *finput;
    FILE *fkey1;
    FILE *fkey2;
    FILE *foutput;
    int m1, m2;
    finput = fopen("plaintext.txt", "rb");
    if (finput == NULL)
    {
        finput = fopen("ciphertext.txt", "rb");
        if (finput == NULL)
        {
            printf("input file missing or invalid\n");
            return 0;
        }
        if (!isValidSize(finput))
        {
            printf("invalid input block size.\n");
            return 0;
        }
        printf("performing decryption...\n");
        m1 = 1;
        m2 = 0;
    }
    else
    {
        if (!isValidSize(finput))
        {
            printf("invalid input block size.\n");
            return 0;
        }
        printf("performing encryption...\n");
        m1 = 0;
        m2 = 1;
    }
    fkey1 = fopen("key1.txt", "rb");
    if (fkey1 == NULL)
    {
        printf("KEY1 file is missing or invalid.\n");
        return 0;
    }

    if (!isValidSize(fkey1))
    {
        printf("invalid key1 block size.\n");
        return 0;
    }
    fkey2 = fopen("key2.txt", "rb");
    if (fkey2 == NULL)
    {
        printf("KEY2 file is missing or invalid.\n");
        return 0;
    }

    if (!isValidSize(fkey2))
    {
        printf("invalid key2 block size.\n");
        return 0;
    }
    foutput = fopen("output.txt", "wb");
    if (foutput == NULL)
    {
        printf("cannot open to write to the output file.\n");
        return 0;
    }
    BYTE result[16];
    BYTE input[8];
    BYTE key1[8];
    BYTE key2[8];
    fread(input, sizeof(input), 1, finput);
    fread(key1, sizeof(input), 1, fkey1);
    fread(key2, sizeof(input), 1, fkey2);

    printf("input:");
    for (int j = 0; j < 8; j++)
        printf("%02x", input[j]);
    printf("\n");

    printf("\nkey1:");
    for (int j = 0; j < 8; j++)
        printf("%02x", key1[j]);
    printf("\n");

    printf("\nkey2:");
    for (int j = 0; j < 8; j++)
        printf("%02x", key2[j]);

    printf("\nCalculations for input\n"); //3-DES. 라서 3번 수행

    printf("\nDES1\n");

    DES(input, key1, m1);
    printf("\nDES2\n");

    DES(input, key2, m2);
    printf("\nDES3\n");

    DES(input, key1, m1);
    memcpy(result, input, sizeof(BYTE) * 8);

    for (int i = 0; i < 8; i++)
        input[i] = 0x08;

    
    printf("\nCalculations for padding\n"); //패딩에도 3-DES 수행
    printf("\nDES1\n");

    DES(input, key1, m1);
    printf("\nDES2\n");

    DES(input, key2, m2);
    printf("\nDES3\n");
    DES(input, key1, m1);
    
    memcpy(result + 8, input, sizeof(BYTE) * 8); 

    
    printf("\nFinal result:\n");
    for (int j = 0; j < 16; j++)
        printf("%02x", result[j]);
    printf("\n");

    fwrite(result, sizeof(BYTE), 16, foutput);
    fclose(finput);
    fclose(fkey1);
    fclose(fkey2);
    fclose(foutput);
    return 0;
}
