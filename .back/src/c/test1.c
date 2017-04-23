#include <stdio.h>

int k()
{
    return 8;
}

int main()
{
    int i;
    int j = i = k();
    printf("i = %d, j = %d, k = %d\n", i, j, k());
    return 0;
}
