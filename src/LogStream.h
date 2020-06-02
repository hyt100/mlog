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
#ifndef __MLOG_LOGSTREAM_H__
#define __MLOG_LOGSTREAM_H__
#include "LogCommon.h"
#include <string.h> // memcpy memset

namespace mlog {

namespace detail {

const int kSmallBuffer = 4000;
const int kLargeBuffer = 4000*1000;

template <int SIZE>
class FixedBuffer : Noncopyable
{
 public:
  FixedBuffer()
    : cur_(buf_)
  {}
  ~FixedBuffer() = default;

  const char *data() const { return buf_; }
  int length() const {   return static_cast<int>(cur_ - buf_); }

  char *current() {  return cur_; }
  int available() { return static_cast<int>(end() - cur_); }
  void add(size_t len) { cur_ += len; }

  void reset() { cur_ = buf_; }
  void bzero() { memset(buf_, 0, sizeof buf_); }

  void append(const char *buf, size_t len)
  {
    if (available() > static_cast<int>(len))
    {
      memcpy(cur_, buf, len);
      cur_ += len;
    }
  }

 private:
  const char* end() { return buf_ + sizeof buf_; }
  char buf_[SIZE];
  char *cur_;
};

} //namespace detail

class LogStream : Noncopyable
{
  typedef LogStream self;
 public:
  typedef detail::FixedBuffer<detail::kSmallBuffer> Buffer;

  void append(const char *buf, int len) { buffer_.append(buf, len); }
  const Buffer& buffer() const { return buffer_; }
  void reset() { buffer_.reset(); }

  self& operator<<(bool v)
  {
    buffer_.append(v ? "1" : "0", 1);
    return *this;
  }

  self& operator<<(char v)
  {
    buffer_.append(&v, 1);
    return *this;
  }
  // self& operator<<(signed char);
  // self& operator<<(unsigned char);
  
  self& operator<<(short);
  self& operator<<(unsigned short);
  self& operator<<(int);
  self& operator<<(unsigned int);
  self& operator<<(long);
  self& operator<<(unsigned long);
  self& operator<<(long long);
  self& operator<<(unsigned long long);

  // self& operator<<(long double);
  self& operator<<(double);
  self& operator<<(float v)
  {
    *this << static_cast<double>(v);
    return *this;
  }

  self& operator<<(const void*);

  self& operator<<(const char *str)
  {
    if (str)
      buffer_.append(str, strlen(str));
    else
      buffer_.append("(null)", 6);
    return *this;
  }
  self& operator<<(const unsigned char* str)
  {
    return operator<<(reinterpret_cast<const char*>(str));
  }

  self& operator<<(const string &v)
  {
    buffer_.append(v.c_str(), v.size());
    return *this;
  }

 private:
  template <typename T> 
  void formatInteger(T v);
  
  Buffer buffer_;
  
  static const int kMaxNumericSize = 32;
};

class Fmt
{
 public:
  template<typename T>
  Fmt(const char* fmt, T val);

  const char* data() const { return buf_; }
  int length() const { return length_; }

 private:
  char buf_[32];
  int length_;
};

inline LogStream& operator<<(LogStream& s, const Fmt& fmt)
{
  s.append(fmt.data(), fmt.length());
  return s;
}



} //namespace mlog

#endif
