int sum(int x)
{
  int sum = 0;
  int i = 1;
  while (i <= x) {
    sum = sum + i;
    i = i + 1;
  }
  return sum;
}
