#include <stdio.h>

int factorial(int n) {
    if (n == 1) {
        return n;
    }
    return n * factorial(n-1);
}

int main() {
    int num;
    
    printf("enter num: ");
    scanf("%d", num);

    printf("\n%d", factorial(num));

    return 0;
}