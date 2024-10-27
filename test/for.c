int sum(int x)
{
  int sum = 0;
  int i;
  //for (i = 1; i <= x; ++i) {
  for (i = 1; i <= x; i = i + 1) {
    sum = sum + i;
  }
  return sum;
}

