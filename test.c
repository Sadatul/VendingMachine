#include <stdio.h>

void numberToBytes(unsigned long long int num, unsigned char s[8])
{
    for (int i = 0; i < 8; i++)
    {
        int tmp = num % 256;
        s[i] = (unsigned char)tmp;
        num = num >> 8;
    }
}
unsigned long long int bytesToNumber(unsigned char s[8])
{
    unsigned long long int num = 0;
    for (int i = 7; i >= 0; i--)
    {
        num = num << 8;
        num = num | (unsigned long long int)s[i];
    }
    return num;
}
int main(void)
{
    unsigned char s[8];
    numberToBytes(25537, s);
    for (int i = 7; i >= 0; i--)
    {
        printf("%d ", (int)s[i]);
    }
    printf("\n");
    unsigned long long int result = bytesToNumber(s);
    printf("%lld\n", result);
}