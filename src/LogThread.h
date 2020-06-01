/*
  Copyright (c) 2020 hyt100

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/
#ifndef __MLOG_LOGTHREAD_H__
#define __MLOG_LOGTHREAD_H__
#include "LogCommon.h"
#include <functional>
#include <memory>
#include <atomic>
#include <pthread.h>


namespace mlog {


extern thread_local int t_cachedTid;
extern thread_local char t_tidString[32];
extern thread_local int t_tidStringLength;

pid_t gettid();
void cacheTid();
bool isMainThread();

inline const char* tidString() // for logging
{
  return t_tidString;
}

inline int tidStringLength() // for logging
{
  return t_tidStringLength;
}

class Thread : Noncopyable
{
 public:
  typedef std::function<void ()> ThreadFunc;
  explicit Thread(ThreadFunc func, const string &s = string());
  ~Thread();
  void start();
  int join();
  int numCreated() { return numCreated_.load(); }

 private:
  void setDefaultName();
  
  bool started_;
  bool joined_;
  ThreadFunc func_;
  string name_;
  CountDownLatch latch_;
  pthread_t pthreadID_;
  pid_t pid_;
  
  static std::atomic<int> numCreated_;
};



} //namespace mlog

#endif
