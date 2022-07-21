#include <stdio.h>

int main()
{
    int a = 6;
    int *p = &a;
    *p = 5656666;
    printf("Dia chi cua a: %u\n", &a);
    printf("Dia chi cua p: %u\n", &p);
    printf("Dia chi cua a: %d\n", p);
    printf("Gia tri cua P: %d\n", *p);

    int Mang[] = {3, 23, 45};
    printf("%d\n", *(Mang + 1));
}