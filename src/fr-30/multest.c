int multiply(int b, int c)
{
    return b * c;
}

int main () {
    int a = 0;
    int b = 10;
    int c = 5;

    a = multiply(b, c);
    b = multiply(a,c);

    if (a > 0 || b > 0) {return 0;}
    else {return 1;}
}