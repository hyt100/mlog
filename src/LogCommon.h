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
#ifndef __MLOG_LOGCOMMON_H__
#define __MLOG_LOGCOMMON_H__
#include <pthread.h>
#include <string.h>
#include <string>
#include <algorithm>

namespace mlog {

using std::string;


#define mlog_likely(x)    __builtin_expect(!!(x), 1)
#define mlog_unlikely(x)  __builtin_expect(!!(x), 0)


class Noncopyable
{
 public:
  Noncopyable(const Noncopyable&) = delete;
  void operator=(const Noncopyable&) = delete;

 protected:
  Noncopyable() = default;
  ~Noncopyable() = default;
};


class Mutex : Noncopyable
{
 public:
  Mutex()
  {
    pthread_mutex_init(&lock_, NULL);
  }
  
  ~Mutex()
  {
    pthread_mutex_destroy(&lock_);
  }
  
  void lock()
  {
    pthread_mutex_lock(&lock_);
  }
  
  void unlock()
  {
    pthread_mutex_unlock(&lock_);
  }
  
  pthread_mutex_t* get() /* non-const */
  {
    return &lock_;
  }
  
 private:
  pthread_mutex_t lock_;
};

class MutexGuard : Noncopyable
{
 public:
  explicit MutexGuard(Mutex& mutex)
    : mutex_(mutex)
  {
    mutex_.lock();
  }
  
  ~MutexGuard()
  {
    mutex_.unlock();
  }
  
 private:
  Mutex& mutex_;
};

// Prevent misuse like:
// MutexGuard(mutex_);
// A tempory object doesn't hold the lock for long!
#define MutexGuard(x) error "Missing guard object name"


class Condition : Noncopyable
{
 public:
  explicit Condition(Mutex& mutex)
    : mutex_(mutex)
  {
    pthread_cond_init(&cond_, NULL);
  }

  ~Condition()
  {
    pthread_cond_destroy(&cond_);
  }

  void notify()
  {
    pthread_cond_signal(&cond_);
  }

  void notifyAll()
  {
    pthread_cond_broadcast(&cond_);
  }

  void wait()
  {
    pthread_cond_wait(&cond_, mutex_.get());
  }

  // returns true if time out, false otherwise.
  bool waitTimeout(double seconds);

 private:
  Mutex& mutex_;
  pthread_cond_t cond_;
};

class CountDownLatch : Noncopyable
{
 public:
  explicit CountDownLatch(int count)
    : mutex_(), condition_(mutex_), count_(count)
  {}

  void wait();

  void countDown();

  int getCount() const;

 private:
  mutable Mutex mutex_;
  Condition condition_ ;
  int count_ ;
};

// For passing C-style string argument to a function.
class StringArg
{
 public:
  StringArg(const char* str)
    : str_(str)
  { }

  StringArg(const string& str)
    : str_(str.c_str())
  { }

  const char* c_str() const { return str_; }

 private:
  const char* str_;
};

class FilenameArg
{
 public:
  FilenameArg(StringArg arg)
  {
    const char *slash = strrchr(arg.c_str(), '/');
    if (NULL == slash)
    {
      strcpy(path_, ".");
      strncpy(name_, arg.c_str(), sizeof name_ - 1);
    }
    else
    {
      int path_len = std::min(static_cast<size_t>(slash - arg.c_str() + 1), sizeof path_ - 1);
      int name_len = std::min(strlen(slash), sizeof name_ - 1);
      strncpy(path_, arg.c_str(), path_len);
      strncpy(name_, slash, name_len);
    }
  }

  const char* get_path() const { return path_; }
  const char* get_name() const { return name_; }

 private:
  char path_[512];
  char name_[512];
};

void sleepUsec(int64_t usec); 


//Taken from  boost/operators.hpp
namespace detail
{
template <typename T> class empty_base {};
} //namespace detail

template <class T, class B = detail::empty_base<T>>
class less_than_comparable : B
{
  friend bool operator>(const T& x, const T& y)  { return y < x; }
  friend bool operator<=(const T& x, const T& y) { return !static_cast<bool>(y < x); }
  friend bool operator>=(const T& x, const T& y) { return !static_cast<bool>(x < y); }
};

template <class T, class B = detail::empty_base<T> >
class equality_comparable : B
{
  friend bool operator!=(const T& x, const T& y) { return !static_cast<bool>(x == y); }
};


} //namespace mlog

#endif
