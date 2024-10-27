int plus(int x)
{
  return +x;
} 
 
int minus(int x)
{
  return -x;
}

int add(int x, int y)
{
  return x + y;
}

int sub(int x, int y)
{
  return x - y;
}

int mul(int x, int y)
{
  return x * y;
}

int div(int x, int y)
{
  return x / y;
}

int or(int x, int y)
{
  return x || y;
}

int and(int x, int y)
{
  return x && y;
}

int eq(int x, int y)
{
  return x == y;
}

int ne(int x, int y)
{
  return x != y;
}

int lt(int x, int y)
{
  return x < y;
}

int gt(int x, int y)
{
  return x > y;
}

int le(int x, int y)
{
  return x <= y;
}

int ge(int x, int y)
{
  return x >= y;
}

int if_cond(int x, int y, int z)
{
  if (x)
    return y;
  else
    return z; 
}

int while_sum(int x)
{
  int sum = 0;
  int i = 1;
  while (i <= x) {
    sum = sum + i;
    i = i + 1;
  }
  return sum;
}

int for_sum(int x)
{
  int sum = 0;
  int i;
  for (i = 1; i <= x; i = i + 1) {
    sum = sum + i;
  }
  return sum;
}

int increase_one(int x)
{
  return x + 1;
}

int increase_two(int x)
{
  return increase_one(increase_one(x));
}

/* Greatest Common Divisor */
int gcd(int x, int y)
{
  int i, d;
  for (i = 1; i <= x && i <= y; i = i + 1) {
    if (x % i == 0 && y % i == 0)
      d = i;
  }
  return d;
}

// 新加-按位与支持
int band(int x, int y)
{
  return x & y;
}

// 新加-逻辑左移
int sll(int x, int y)
{
  return x << y;
}