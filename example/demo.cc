#include "../src/Log.h"
#include <string>
#include <stdio.h>

int main(int argc, char *argv[])
{
  mlog::LogServer* log = mlog::LogServer::Instance("xxx", mlog::Logger::TRACE);
  log->start();

  mlog::sleepUsec(1*1000*1000);
  
  for (int i = 0; i<10; ++i)
  {
    LOG_TRACE << "this is " << i;
    LOG_DEBUG << "this is " << i;
    LOG_WARN << "this is " << i;
    LOG_ERROR << "this is " << i;
  }
  return 0;
}

