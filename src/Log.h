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
#ifndef __MLOG_LOG_H__
#define __MLOG_LOG_H__
#include "LogTimestamp.h"
#include "LogStream.h"

namespace mlog {

class Logger
{
 public:
  enum LogLevel
  {
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL,
    NUM_LOG_LEVELS,
  };

  typedef void (*OutputFunc)(const char* msg, int len);
  typedef void (*FlushFunc)();

  // compile time calculation of basename of source file
  class SourceFile
  {
   public:
    template<int N>
    SourceFile(const char (&arr)[N])
      : data_(arr),
        size_(N-1)
    {
      const char* slash = strrchr(data_, '/'); // builtin function
      if (slash)
      {
        data_ = slash + 1;
        size_ -= static_cast<int>(data_ - arr);
      }
    }

    explicit SourceFile(const char* filename)
      : data_(filename)
    {
      const char* slash = strrchr(filename, '/');
      if (slash)
      {
        data_ = slash + 1;
      }
      size_ = static_cast<int>(strlen(data_));
    }

    const char* data_;
    int size_;
  };

  Logger(SourceFile file, int line, LogLevel level);
  Logger(SourceFile file, int line, LogLevel level, const char* func);
  Logger(SourceFile file, int line, bool toAbort);
  ~Logger();

  LogStream& stream() { return impl_.stream_; }

  static LogLevel logLevel() { return logLevel_; }
  static void setLogLevel(LogLevel level) { logLevel_ = level; }
  static void setOutput(OutputFunc func) { outputFunc_ = func; }
  static void setFlush(FlushFunc func) { flushFunc_ = func; }

 private:

  class Impl
  {
   public:
    typedef Logger::LogLevel LogLevel;
    Impl(LogLevel level, int savedErrno, const SourceFile& file, int line);
    void formatTime();
    void finish();

    Timestamp time_;
    LogStream stream_;
    LogLevel level_;
    int line_;
    SourceFile basename_;
  };

  Impl impl_;
  
  static OutputFunc outputFunc_;
  static LogLevel logLevel_;
  static FlushFunc flushFunc_;
};

//avoid dangling else: www.drdobbs.com/cpp/201804215
#define LOG_TRACE    if (mlog::Logger::logLevel() > mlog::Logger::TRACE)  ;\
    else mlog::Logger(__FILE__, __LINE__, mlog::Logger::TRACE, __func__).stream()
#define LOG_DEBUG    if (mlog::Logger::logLevel() <= mlog::Logger::DEBUG) ;\
    else mlog::Logger(__FILE__, __LINE__, mlog::Logger::DEBUG, __func__).stream()
#define LOG_INFO    if (mlog::Logger::logLevel() <= mlog::Logger::INFO)   ;\
    else mlog::Logger(__FILE__, __LINE__, mlog::Logger::INFO).stream()
#define LOG_WARN     mlog::Logger(__FILE__, __LINE__, mlog::Logger::WARN).stream()
#define LOG_ERROR    mlog::Logger(__FILE__, __LINE__, mlog::Logger::ERROR).stream()
#define LOG_FATAL    mlog::Logger(__FILE__, __LINE__, mlog::Logger::FATAL).stream()
#define LOG_SYSERR   mlog::Logger(__FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL mlog::Logger(__FILE__, __LINE__, true).stream()

const char* strerror_tl(int savedErrno);

// Taken from glog/logging.h
//
// Check that the input is non NULL.  This very useful in constructor
// initializer lists.

#define CHECK_NOTNULL(val) \
  ::mlog::CheckNotNull(__FILE__, __LINE__, "'" #val "' Must be non NULL", (val))

// A small helper for CHECK_NOTNULL().
template <typename T>
inline void CheckNotNull(Logger::SourceFile file, int line, const char *names, T* ptr)
{
  if (ptr == nullptr)
  {
    Logger(file, line, Logger::FATAL).stream() << names;
  }
}


} //namespace mlog

#endif
