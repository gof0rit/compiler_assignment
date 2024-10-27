#include <stdio.h>
  
int plus(int x);
int minus(int x);
int add(int x, int y);
int sub(int x, int y);
int mul(int x, int y);
int div(int x, int y);
int or(int x, int y);
int and(int x, int y);
int eq(int x, int y);
int ne(int x, int y);
int lt(int x, int y);
int gt(int x, int y);
int le(int x, int y);
int ge(int x, int y);
int if_cond(int x, int y, int z);
int while_sum(int x);
int for_sum(int x);
int increase_two(int x);
int gcd(int x, int y);
// 新加-按位与支持
int band(int x, int y);
// 新加-逻辑左移
int sll(int x, int y);

int main(void)
{
  int x = 2, y = 5;
  int res = 0;

  res = plus(x);
  printf("+%d = %d\n", x, res);
  res = minus(x);
  printf("-%d = %d\n", x, res);
  res = add(x, y);
  printf("%d + %d = %d\n", x, y, res);
  res = sub(x, y);
  printf("%d - %d = %d\n", x, y, res);
  res = mul(x, y);
  printf("%d * %d = %d\n", x, y, res);
  res = div(x, y);
  printf("%d / %d = %d\n", x, y, res);
  res = or(x, y);
  printf("%d || %d = %d\n", x, y, res);
  res = and(x, y);
  printf("%d && %d = %d\n", x, y, res);
  res = eq(x, y);
  printf("%d == %d = %d\n", x, y, res);
  res = ne(x, y);
  printf("%d != %d = %d\n", x, y, res);
  res = lt(x, y);
  printf("%d < %d = %d\n", x, y, res);
  res = gt(x, y);
  printf("%d > %d = %d\n", x, y, res);
  res = le(x, y);
  printf("%d <= %d = %d\n", x, y, res);
  res = ge(x, y);
  printf("%d >= %d = %d\n", x, y, res);

  res = if_cond(x < y, x, y);
  printf("%d < %d ? %d : %d = %d\n", x, y, x, y, res);
  res = if_cond(x > y, x, y);
  printf("%d > %d ? %d : %d = %d\n", x, y, x, y, res);

  res = while_sum(x);
  printf("1 + ... + %d = %d\n", x, res);
  res = for_sum(y);
  printf("1 + ... + %d = %d\n", y, res);

  res = increase_two(x);
  printf("%d + 1 + 1 = %d\n", x, res);

  res = gcd(x, y);
  printf("gcd(%d, %d) = %d\n", x, y, res);

  // 新加-按位与支持
  res = band(x, y);
  printf("%d & %d = %d\n", x, y, res); // 010 & 101 = 0

  // 新加-逻辑左移
  res = sll(x, y);
  printf("%d << %d = %d\n", x, y, res); // 2 << 5 = 64

  return 0;
}
