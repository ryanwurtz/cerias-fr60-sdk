// factorial @ 0x00010000
int factorial(int param_1)
{
  int iVar1;
  
  if (param_1 == 1) {
    param_1 = 1;
  }
  else {
    iVar1 = factorial(param_1 + -1);
    param_1 = param_1 * iVar1;
  }
  return param_1;
}

// main @ 0x0001004c
/* WARNING: Removing unreachable block (ram,0x00010064) */

undefined1 main(void)
{
  return 1;
}

