// divide @ 0x00010000

undefined4 divide(undefined4 param_1,undefined4 param_2)
{
  undefined4 uVar1;
  
  uVar1 = __divsi3(param_1,param_2);
  return uVar1;
}


// main @ 0x00010038

undefined4 main(void)
{
  divide(10,5);
  return 0;
}


// __divsi3 @ 0x00010088

uint __divsi3(int param_1,uint param_2)
{
  longlong lVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  
  uVar2 = (uint)(param_1 < 0) << 9;
  uVar4 = (uint)((ulonglong)((longlong)param_1 << 1) >> 0x20);
  uVar5 = (uint)((longlong)param_1 << 1);
  if (param_1 < 0 == (int)param_2 < 0) {
    uVar2 = uVar2 | uVar4 < param_2;
    uVar3 = -param_2;
  }
  else {
    uVar2 = uVar2 | 0x400 | (uint)CARRY4(uVar4,param_2);
    uVar3 = param_2;
  }
  if ((byte)((byte)(uVar2 >> 9) & 1 ^ (byte)(uVar2 >> 10)) == (byte)uVar2) {
    uVar5 = uVar5 | 1;
    uVar4 = uVar4 + uVar3;
  }
  lVar1 = CONCAT44(uVar4,uVar5) << 1;
  uVar4 = (uint)((ulonglong)lVar1 >> 0x20);
  uVar5 = (uint)lVar1;
  if ((char)(uVar2 >> 10) == '\x01') {
    uVar3 = uVar2 & 0xfffffffa | (uint)CARRY4(uVar4,param_2);
    uVar2 = param_2;
  }
  else {
    uVar3 = uVar2 & 0xfffffffa | (uint)(uVar4 < param_2);
    uVar2 = -param_2;
  }
  if ((byte)((byte)(uVar3 >> 9) & 1 ^ (byte)(uVar3 >> 10)) == (byte)uVar3) {
    uVar5 = uVar5 | 1;
    uVar4 = uVar4 + uVar2;
  }
  lVar1 = CONCAT44(uVar4,uVar5) << 1;
  uVar5 = (uint)((ulonglong)lVar1 >> 0x20);
  uVar2 = (uint)lVar1;
  if ((char)(uVar3 >> 10) == '\x01') {
    uVar3 = uVar3 & 0xfffffffa | (uint)CARRY4(uVar5,param_2);
    uVar4 = param_2;
  }
  else {
    uVar3 = uVar3 & 0xfffffffa | (uint)(uVar5 < param_2);
    uVar4 = -param_2;
  }
  if ((byte)((byte)(uVar3 >> 9) & 1 ^ (byte)(uVar3 >> 10)) == (byte)uVar3) {
    uVar2 = uVar2 | 1;
    uVar5 = uVar5 + uVar4;
  }
  lVar1 = CONCAT44(uVar5,uVar2) << 1;
  uVar5 = (uint)((ulonglong)lVar1 >> 0x20);
  uVar2 = (uint)lVar1;
  if ((char)(uVar3 >> 10) == '\x01') {
    uVar3 = uVar3 & 0xfffffffa | (uint)CARRY4(uVar5,param_2);
    uVar4 = param_2;
  }
  else {
    uVar3 = uVar3 & 0xfffffffa | (uint)(uVar5 < param_2);
    uVar4 = -param_2;
  }
  if ((byte)((byte)(uVar3 >> 9) & 1 ^ (byte)(uVar3 >> 10)) == (byte)uVar3) {
    uVar2 = uVar2 | 1;
    uVar5 = uVar5 + uVar4;
  }
  lVar1 = CONCAT44(uVar5,uVar2) << 1;
  uVar5 = (uint)((ulonglong)lVar1 >> 0x20);
  uVar2 = (uint)lVar1;
  if ((char)(uVar3 >> 10) == '\x01') {
    uVar3 = uVar3 & 0xfffffffa | (uint)CARRY4(uVar5,param_2);
    uVar4 = param_2;
  }
  else {
    uVar3 = uVar3 & 0xfffffffa | (uint)(uVar5 < param_2);
    uVar4 = -param_2;
  }
  if ((byte)((byte)(uVar3 >> 9) & 1 ^ (byte)(uVar3 >> 10)) == (byte)uVar3) {
    uVar2 = uVar2 | 1;
    uVar5 = uVar5 + uVar4;
  }
  lVar1 = CONCAT44(uVar5,uVar2) << 1;
  uVar5 = (uint)((ulonglong)lVar1 >> 0x20);
  uVar2 = (uint)lVar1;
  if ((char)(uVar3 >> 10) == '\x01') {
    uVar3 = uVar3 & 0xfffffffa | (uint)CARRY4(uVar5,param_2);
    uVar4 = param_2;
  }
  else {
    uVar3 = uVar3 & 0xfffffffa | (uint)(uVar5 < param_2);
    uVar4 = -param_2;
  }
  if ((byte)((byte)(uVar3 >> 9) & 1 ^ (byte)(uVar3 >> 10)) == (byte)uVar3) {
    uVar2 = uVar2 | 1;
    uVar5 = uVar5 + uVar4;
  }
  lVar1 = CONCAT44(uVar5,uVar2) << 1;
  uVar5 = (uint)((ulonglong)lVar1 >> 0x20);
  uVar2 = (uint)lVar1;
  if ((char)(uVar3 >> 10) == '\x01') {
    uVar3 = uVar3 & 0xfffffffa | (uint)CARRY4(uVar5,param_2);
    uVar4 = param_2;
  }
  else {
    uVar3 = uVar3 & 0xfffffffa | (uint)(uVar5 < param_2);
    uVar4 = -param_2;
  }
  if ((byte)((byte)(uVar3 >> 9) & 1 ^ (byte)(uVar3 >> 10)) == (byte)uVar3) {
    uVar2 = uVar2 | 1;
    uVar5 = uVar5 + uVar4;
  }
  lVar1 = CONCAT44(uVar5,uVar2) << 1;
  uVar5 = (uint)((ulonglong)lVar1 >> 0x20);
  uVar2 = (uint)lVar1;
  if ((char)(uVar3 >> 10) == '\x01') {
    uVar3 = uVar3 & 0xfffffffa | (uint)CARRY4(uVar5,param_2);
    uVar4 = param_2;
  }
  else {
    uVar3 = uVar3 & 0xfffffffa | (uint)(uVar5 < param_2);
    uVar4 = -param_2;
  }
  if ((byte)((byte)(uVar3 >> 9) & 1 ^ (byte)(uVar3 >> 10)) == (byte)uVar3) {
    uVar2 = uVar2 | 1;
    uVar5 = uVar5 + uVar4;
  }
  lVar1 = CONCAT44(uVar5,uVar2) << 1;
  uVar5 = (uint)((ulonglong)lVar1 >> 0x20);
  uVar2 = (uint)lVar1;
  if ((char)(uVar3 >> 10) == '\x01') {
    uVar3 = uVar3 & 0xfffffffa | (uint)CARRY4(uVar5,param_2);
    uVar4 = param_2;
  }
  else {
    uVar3 = uVar3 & 0xfffffffa | (uint)(uVar5 < param_2);
    uVar4 = -param_2;
  }
  if ((byte)((byte)(uVar3 >> 9) & 1 ^ (byte)(uVar3 >> 10)) == (byte)uVar3) {
    uVar2 = uVar2 | 1;
    uVar5 = uVar5 + uVar4;
  }
  lVar1 = CONCAT44(uVar5,uVar2) << 1;
  uVar5 = (uint)((ulonglong)lVar1 >> 0x20);
  uVar2 = (uint)lVar1;
  if ((char)(uVar3 >> 10) == '\x01') {
    uVar3 = uVar3 & 0xfffffffa | (uint)CARRY4(uVar5,param_2);
    uVar4 = param_2;
  }
  else {
    uVar3 = uVar3 & 0xfffffffa | (uint)(uVar5 < param_2);
    uVar4 = -param_2;
  }
  if ((byte)((byte)(uVar3 >> 9) & 1 ^ (byte)(uVar3 >> 10)) == (byte)uVar3) {
    uVar2 = uVar2 | 1;
    uVar5 = uVar5 + uVar4;
  }
  lVar1 = CONCAT44(uVar5,uVar2) << 1;
  uVar5 = (uint)((ulonglong)lVar1 >> 0x20);
  uVar2 = (uint)lVar1;
  if ((char)(uVar3 >> 10) == '\x01') {
    uVar3 = uVar3 & 0xfffffffa | (uint)CARRY4(uVar5,param_2);
    uVar4 = param_2;
  }
  else {
    uVar3 = uVar3 & 0xfffffffa | (uint)(uVar5 < param_2);
    uVar4 = -param_2;
  }
  if ((byte)((byte)(uVar3 >> 9) & 1 ^ (byte)(uVar3 >> 10)) == (byte)uVar3) {
    uVar2 = uVar2 | 1;
    uVar5 = uVar5 + uVar4;
  }
  lVar1 = CONCAT44(uVar5,uVar2) << 1;
  uVar5 = (uint)((ulonglong)lVar1 >> 0x20);
  uVar2 = (uint)lVar1;
  if ((char)(uVar3 >> 10) == '\x01') {
    uVar3 = uVar3 & 0xfffffffa | (uint)CARRY4(uVar5,param_2);
    uVar4 = param_2;
  }
  else {
    uVar3 = uVar3 & 0xfffffffa | (uint)(uVar5 < param_2);
    uVar4 = -param_2;
  }
  if ((byte)((byte)(uVar3 >> 9) & 1 ^ (byte)(uVar3 >> 10)) == (byte)uVar3) {
    uVar2 = uVar2 | 1;
    uVar5 = uVar5 + uVar4;
  }
  lVar1 = CONCAT44(uVar5,uVar2) << 1;
  uVar5 = (uint)((ulonglong)lVar1 >> 0x20);
  uVar2 = (uint)lVar1;
  if ((char)(uVar3 >> 10) == '\x01') {
    uVar3 = uVar3 & 0xfffffffa | (uint)CARRY4(uVar5,param_2);
    uVar4 = param_2;
  }
  else {
    uVar3 = uVar3 & 0xfffffffa | (uint)(uVar5 < param_2);
    uVar4 = -param_2;
  }
  if ((byte)((byte)(uVar3 >> 9) & 1 ^ (byte)(uVar3 >> 10)) == (byte)uVar3) {
    uVar2 = uVar2 | 1;
    uVar5 = uVar5 + uVar4;
  }
  lVar1 = CONCAT44(uVar5,uVar2) << 1;
  uVar5 = (uint)((ulonglong)lVar1 >> 0x20);
  uVar2 = (uint)lVar1;
  if ((char)(uVar3 >> 10) == '\x01') {
    uVar3 = uVar3 & 0xfffffffa | (uint)CARRY4(uVar5,param_2);
    uVar4 = param_2;
  }
  else {
    uVar3 = uVar3 & 0xfffffffa | (uint)(uVar5 < param_2);
    uVar4 = -param_2;
  }
  if ((byte)((byte)(uVar3 >> 9) & 1 ^ (byte)(uVar3 >> 10)) == (byte)uVar3) {
    uVar2 = uVar2 | 1;
    uVar5 = uVar5 + uVar4;
  }
  lVar1 = CONCAT44(uVar5,uVar2) << 1;
  uVar5 = (uint)((ulonglong)lVar1 >> 0x20);
  uVar2 = (uint)lVar1;
  if ((char)(uVar3 >> 10) == '\x01') {
    uVar3 = uVar3 & 0xfffffffa | (uint)CARRY4(uVar5,param_2);
    uVar4 = param_2;
  }
  else {
    uVar3 = uVar3 & 0xfffffffa | (uint)(uVar5 < param_2);
    uVar4 = -param_2;
  }
  if ((byte)((byte)(uVar3 >> 9) & 1 ^ (byte)(uVar3 >> 10)) == (byte)uVar3) {
    uVar2 = uVar2 | 1;
    uVar5 = uVar5 + uVar4;
  }
  lVar1 = CONCAT44(uVar5,uVar2) << 1;
  uVar5 = (uint)((ulonglong)lVar1 >> 0x20);
  uVar2 = (uint)lVar1;
  if ((char)(uVar3 >> 10) == '\x01') {
    uVar3 = uVar3 & 0xfffffffa | (uint)CARRY4(uVar5,param_2);
    uVar4 = param_2;
  }
  else {
    uVar3 = uVar3 & 0xfffffffa | (uint)(uVar5 < param_2);
    uVar4 = -param_2;
  }
  if ((byte)((byte)(uVar3 >> 9) & 1 ^ (byte)(uVar3 >> 10)) == (byte)uVar3) {
    uVar2 = uVar2 | 1;
    uVar5 = uVar5 + uVar4;
  }
  lVar1 = CONCAT44(uVar5,uVar2) << 1;
  uVar5 = (uint)((ulonglong)lVar1 >> 0x20);
  uVar2 = (uint)lVar1;
  if ((char)(uVar3 >> 10) == '\x01') {
    uVar3 = uVar3 & 0xfffffffa | (uint)CARRY4(uVar5,param_2);
    uVar4 = param_2;
  }
  else {
    uVar3 = uVar3 & 0xfffffffa | (uint)(uVar5 < param_2);
    uVar4 = -param_2;
  }
  if ((byte)((byte)(uVar3 >> 9) & 1 ^ (byte)(uVar3 >> 10)) == (byte)uVar3) {
    uVar2 = uVar2 | 1;
    uVar5 = uVar5 + uVar4;
  }
  lVar1 = CONCAT44(uVar5,uVar2) << 1;
  uVar5 = (uint)((ulonglong)lVar1 >> 0x20);
  uVar2 = (uint)lVar1;
  if ((char)(uVar3 >> 10) == '\x01') {
    uVar3 = uVar3 & 0xfffffffa | (uint)CARRY4(uVar5,param_2);
    uVar4 = param_2;
  }
  else {
    uVar3 = uVar3 & 0xfffffffa | (uint)(uVar5 < param_2);
    uVar4 = -param_2;
  }
  if ((byte)((byte)(uVar3 >> 9) & 1 ^ (byte)(uVar3 >> 10)) == (byte)uVar3) {
    uVar2 = uVar2 | 1;
    uVar5 = uVar5 + uVar4;
  }
  lVar1 = CONCAT44(uVar5,uVar2) << 1;
  uVar5 = (uint)((ulonglong)lVar1 >> 0x20);
  uVar2 = (uint)lVar1;
  if ((char)(uVar3 >> 10) == '\x01') {
    uVar3 = uVar3 & 0xfffffffa | (uint)CARRY4(uVar5,param_2);
    uVar4 = param_2;
  }
  else {
    uVar3 = uVar3 & 0xfffffffa | (uint)(uVar5 < param_2);
    uVar4 = -param_2;
  }
  if ((byte)((byte)(uVar3 >> 9) & 1 ^ (byte)(uVar3 >> 10)) == (byte)uVar3) {
    uVar2 = uVar2 | 1;
    uVar5 = uVar5 + uVar4;
  }
  lVar1 = CONCAT44(uVar5,uVar2) << 1;
  uVar5 = (uint)((ulonglong)lVar1 >> 0x20);
  uVar2 = (uint)lVar1;
  if ((char)(uVar3 >> 10) == '\x01') {
    uVar3 = uVar3 & 0xfffffffa | (uint)CARRY4(uVar5,param_2);
    uVar4 = param_2;
  }
  else {
    uVar3 = uVar3 & 0xfffffffa | (uint)(uVar5 < param_2);
    uVar4 = -param_2;
  }
  if ((byte)((byte)(uVar3 >> 9) & 1 ^ (byte)(uVar3 >> 10)) == (byte)uVar3) {
    uVar2 = uVar2 | 1;
    uVar5 = uVar5 + uVar4;
  }
  lVar1 = CONCAT44(uVar5,uVar2) << 1;
  uVar5 = (uint)((ulonglong)lVar1 >> 0x20);
  uVar2 = (uint)lVar1;
  if ((char)(uVar3 >> 10) == '\x01') {
    uVar3 = uVar3 & 0xfffffffa | (uint)CARRY4(uVar5,param_2);
    uVar4 = param_2;
  }
  else {
    uVar3 = uVar3 & 0xfffffffa | (uint)(uVar5 < param_2);
    uVar4 = -param_2;
  }
  if ((byte)((byte)(uVar3 >> 9) & 1 ^ (byte)(uVar3 >> 10)) == (byte)uVar3) {
    uVar2 = uVar2 | 1;
    uVar5 = uVar5 + uVar4;
  }
  lVar1 = CONCAT44(uVar5,uVar2) << 1;
  uVar5 = (uint)((ulonglong)lVar1 >> 0x20);
  uVar2 = (uint)lVar1;
  if ((char)(uVar3 >> 10) == '\x01') {
    uVar3 = uVar3 & 0xfffffffa | (uint)CARRY4(uVar5,param_2);
    uVar4 = param_2;
  }
  else {
    uVar3 = uVar3 & 0xfffffffa | (uint)(uVar5 < param_2);
    uVar4 = -param_2;
  }
  if ((byte)((byte)(uVar3 >> 9) & 1 ^ (byte)(uVar3 >> 10)) == (byte)uVar3) {
    uVar2 = uVar2 | 1;
    uVar5 = uVar5 + uVar4;
  }
  lVar1 = CONCAT44(uVar5,uVar2) << 1;
  uVar5 = (uint)((ulonglong)lVar1 >> 0x20);
  uVar2 = (uint)lVar1;
  if ((char)(uVar3 >> 10) == '\x01') {
    uVar3 = uVar3 & 0xfffffffa | (uint)CARRY4(uVar5,param_2);
    uVar4 = param_2;
  }
  else {
    uVar3 = uVar3 & 0xfffffffa | (uint)(uVar5 < param_2);
    uVar4 = -param_2;
  }
  if ((byte)((byte)(uVar3 >> 9) & 1 ^ (byte)(uVar3 >> 10)) == (byte)uVar3) {
    uVar2 = uVar2 | 1;
    uVar5 = uVar5 + uVar4;
  }
  lVar1 = CONCAT44(uVar5,uVar2) << 1;
  uVar5 = (uint)((ulonglong)lVar1 >> 0x20);
  uVar2 = (uint)lVar1;
  if ((char)(uVar3 >> 10) == '\x01') {
    uVar3 = uVar3 & 0xfffffffa | (uint)CARRY4(uVar5,param_2);
    uVar4 = param_2;
  }
  else {
    uVar3 = uVar3 & 0xfffffffa | (uint)(uVar5 < param_2);
    uVar4 = -param_2;
  }
  if ((byte)((byte)(uVar3 >> 9) & 1 ^ (byte)(uVar3 >> 10)) == (byte)uVar3) {
    uVar2 = uVar2 | 1;
    uVar5 = uVar5 + uVar4;
  }
  lVar1 = CONCAT44(uVar5,uVar2) << 1;
  uVar5 = (uint)((ulonglong)lVar1 >> 0x20);
  uVar2 = (uint)lVar1;
  if ((char)(uVar3 >> 10) == '\x01') {
    uVar3 = uVar3 & 0xfffffffa | (uint)CARRY4(uVar5,param_2);
    uVar4 = param_2;
  }
  else {
    uVar3 = uVar3 & 0xfffffffa | (uint)(uVar5 < param_2);
    uVar4 = -param_2;
  }
  if ((byte)((byte)(uVar3 >> 9) & 1 ^ (byte)(uVar3 >> 10)) == (byte)uVar3) {
    uVar2 = uVar2 | 1;
    uVar5 = uVar5 + uVar4;
  }
  lVar1 = CONCAT44(uVar5,uVar2) << 1;
  uVar5 = (uint)((ulonglong)lVar1 >> 0x20);
  uVar2 = (uint)lVar1;
  if ((char)(uVar3 >> 10) == '\x01') {
    uVar3 = uVar3 & 0xfffffffa | (uint)CARRY4(uVar5,param_2);
    uVar4 = param_2;
  }
  else {
    uVar3 = uVar3 & 0xfffffffa | (uint)(uVar5 < param_2);
    uVar4 = -param_2;
  }
  if ((byte)((byte)(uVar3 >> 9) & 1 ^ (byte)(uVar3 >> 10)) == (byte)uVar3) {
    uVar2 = uVar2 | 1;
    uVar5 = uVar5 + uVar4;
  }
  lVar1 = CONCAT44(uVar5,uVar2) << 1;
  uVar5 = (uint)((ulonglong)lVar1 >> 0x20);
  uVar2 = (uint)lVar1;
  if ((char)(uVar3 >> 10) == '\x01') {
    uVar3 = uVar3 & 0xfffffffa | (uint)CARRY4(uVar5,param_2);
    uVar4 = param_2;
  }
  else {
    uVar3 = uVar3 & 0xfffffffa | (uint)(uVar5 < param_2);
    uVar4 = -param_2;
  }
  if ((byte)((byte)(uVar3 >> 9) & 1 ^ (byte)(uVar3 >> 10)) == (byte)uVar3) {
    uVar2 = uVar2 | 1;
    uVar5 = uVar5 + uVar4;
  }
  lVar1 = CONCAT44(uVar5,uVar2) << 1;
  uVar5 = (uint)((ulonglong)lVar1 >> 0x20);
  uVar2 = (uint)lVar1;
  if ((char)(uVar3 >> 10) == '\x01') {
    uVar3 = uVar3 & 0xfffffffa | (uint)CARRY4(uVar5,param_2);
    uVar4 = param_2;
  }
  else {
    uVar3 = uVar3 & 0xfffffffa | (uint)(uVar5 < param_2);
    uVar4 = -param_2;
  }
  if ((byte)((byte)(uVar3 >> 9) & 1 ^ (byte)(uVar3 >> 10)) == (byte)uVar3) {
    uVar2 = uVar2 | 1;
    uVar5 = uVar5 + uVar4;
  }
  lVar1 = CONCAT44(uVar5,uVar2) << 1;
  uVar5 = (uint)((ulonglong)lVar1 >> 0x20);
  uVar2 = (uint)lVar1;
  if ((char)(uVar3 >> 10) == '\x01') {
    uVar3 = uVar3 & 0xfffffffa | (uint)CARRY4(uVar5,param_2);
    uVar4 = param_2;
  }
  else {
    uVar3 = uVar3 & 0xfffffffa | (uint)(uVar5 < param_2);
    uVar4 = -param_2;
  }
  if ((byte)((byte)(uVar3 >> 9) & 1 ^ (byte)(uVar3 >> 10)) == (byte)uVar3) {
    uVar2 = uVar2 | 1;
    uVar5 = uVar5 + uVar4;
  }
  lVar1 = CONCAT44(uVar5,uVar2) << 1;
  uVar5 = (uint)((ulonglong)lVar1 >> 0x20);
  uVar2 = (uint)lVar1;
  if ((char)(uVar3 >> 10) == '\x01') {
    uVar3 = uVar3 & 0xfffffffa | (uint)CARRY4(uVar5,param_2);
    uVar4 = param_2;
  }
  else {
    uVar3 = uVar3 & 0xfffffffa | (uint)(uVar5 < param_2);
    uVar4 = -param_2;
  }
  if ((byte)((byte)(uVar3 >> 9) & 1 ^ (byte)(uVar3 >> 10)) == (byte)uVar3) {
    uVar2 = uVar2 | 1;
    uVar5 = uVar5 + uVar4;
  }
  lVar1 = CONCAT44(uVar5,uVar2) << 1;
  uVar5 = (uint)((ulonglong)lVar1 >> 0x20);
  uVar2 = (uint)lVar1;
  if ((char)(uVar3 >> 10) == '\x01') {
    uVar3 = uVar3 & 0xfffffffa | (uint)CARRY4(uVar5,param_2);
    uVar4 = param_2;
  }
  else {
    uVar3 = uVar3 & 0xfffffffa | (uint)(uVar5 < param_2);
    uVar4 = -param_2;
  }
  if ((byte)((byte)(uVar3 >> 9) & 1 ^ (byte)(uVar3 >> 10)) == (byte)uVar3) {
    uVar2 = uVar2 | 1;
    uVar5 = uVar5 + uVar4;
  }
  lVar1 = CONCAT44(uVar5,uVar2) << 1;
  uVar5 = (uint)((ulonglong)lVar1 >> 0x20);
  uVar2 = (uint)lVar1;
  if ((char)(uVar3 >> 10) == '\x01') {
    uVar3 = uVar3 & 0xfffffffa | (uint)CARRY4(uVar5,param_2);
    uVar4 = param_2;
  }
  else {
    uVar3 = uVar3 & 0xfffffffa | (uint)(uVar5 < param_2);
    uVar4 = -param_2;
  }
  if ((byte)((byte)(uVar3 >> 9) & 1 ^ (byte)(uVar3 >> 10)) == (byte)uVar3) {
    uVar2 = uVar2 | 1;
    uVar5 = uVar5 + uVar4;
  }
  if ((char)(uVar3 >> 10) != '\x01') {
    param_2 = -param_2;
  }
  if (uVar5 + param_2 == 0) {
    uVar2 = uVar2 + 1;
  }
  if ((char)(uVar3 >> 10) == '\x01') {
    uVar2 = -uVar2;
  }
  return uVar2;
}


