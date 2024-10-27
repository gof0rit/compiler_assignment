int increase_one(int x)
{
  return x + 1;
}

int increase_two(int x)
{
  return increase_one(increase_one(x));
}
