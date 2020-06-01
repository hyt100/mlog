#include "../src/Log.h"
#include "../src/LogAsync.h"
#include <string>
#include <stdio.h>

mlog::AsyncLogging *g_asyncLog = nullptr;

void asyncOutput(const char* msg, int len)
{
  g_asyncLog->append(msg, len);
}

int main(int argc, char *argv[])
{
  mlog::AsyncLogging asyncLog(std::string("xxx"), 1024);
  mlog::Logger::setLogLevel(mlog::Logger::TRACE);
  mlog::Logger::setOutput(asyncOutput);
  g_asyncLog = &asyncLog;
  g_asyncLog->start();
  //mlog::sleepUsec(2*1000*1000);
  
  for (int i = 0; i<100; ++i)
  {
    LOG_WARN << "this is " << i;
  }
  
  g_asyncLog->stop();
  return 0;
}

