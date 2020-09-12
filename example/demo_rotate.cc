#include <iostream>
#include "../src/LogRotate.h"

int main(int argc, char *argv[])
{
  mlog::LogRotate r("xxx");
  r.rotate();
  return 0;
}