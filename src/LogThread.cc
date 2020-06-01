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
#include "LogThread.h"
#include <assert.h>
#include <type_traits>
#include <sys/prctl.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <linux/unistd.h>

using namespace mlog;

namespace mlog
{

thread_local int t_cachedTid = 0;
thread_local char t_tidString[32];
thread_local int t_tidStringLength = 6;

static_assert(std::is_same<int, pid_t>::value, "pid_t should be int");

pid_t gettid()
{
  return static_cast<pid_t>(::syscall(SYS_gettid));
}

void cacheTid()
{
  if (mlog_likely(t_cachedTid == 0))
  {
    t_cachedTid = mlog::gettid();
    t_tidStringLength = snprintf(t_tidString, sizeof t_tidString, "%5d ", t_cachedTid);
  }
}

bool isMainThread()
{
  return gettid() == ::getpid();
}


namespace detail 
{
struct ThreadData
{
  typedef Thread::ThreadFunc ThreadFunc;
  ThreadFunc func_;
  string name_;
  CountDownLatch *latch_;
  pid_t *pid_;
  
  ThreadData(ThreadFunc func, const string &s, CountDownLatch *latch, pid_t *pid)
    : func_(func), name_(s), latch_(latch), pid_(pid)
  {}
};

void* thread_routine(void *arg)
{
  ThreadData* data = static_cast<ThreadData*>(arg);
  *data->pid_ = mlog::gettid();
  data->pid_ = NULL;
  data->latch_->countDown();
  data->latch_ = NULL;
  ::prctl(PR_SET_NAME, data->name_.c_str());
  
  data->func_();

  delete data;
  return NULL;
}

}//namespace detail

Thread::Thread(ThreadFunc func, const string &s)
  : func_(func),
    name_(s),
    latch_(1),
    started_(false),
    joined_(false),
    pthreadID_(0),
    pid_(0)
{
  setDefaultName();
}

Thread::~Thread()
{
  if (started_ && !joined_)
  {
    pthread_detach(pthreadID_);
  }
}

std::atomic<int> Thread::numCreated_{0};

void Thread::setDefaultName()
{
  int num = numCreated_.fetch_add(1);
  if (name_.empty())
  {
    char buf[32];
    snprintf(buf, sizeof(buf)-1, "Thread%d", num);
    name_ = buf;
  }
}

void Thread::start()
{
  assert(!started_);
  detail::ThreadData *data = new detail::ThreadData(func_, name_, &latch_, &pid_);
  started_ = true;
  if (pthread_create(&pthreadID_, NULL, &detail::thread_routine, static_cast<void*>(data)) != 0)
  {
    started_ = false;
    delete data;
  }
  else
  {
    latch_.wait();
  }
}

int Thread::join()
{
  assert(started_);
  assert(!joined_);
  joined_ = true;
  return pthread_join(pthreadID_, NULL);
}

}//namespace mlog
