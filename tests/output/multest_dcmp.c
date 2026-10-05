// multiply @ 0x00010000
int multiply(int param_1,int param_2)
{
  return param_2 * param_1;
}

// main @ 0x0001002c
bool main(void)
{
  int iVar1;
  
  iVar1 = multiply(10,5);
  return iVar1 < 1;
}

