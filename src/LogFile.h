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
#ifndef __MLOG_LOGFILE_H__
#define __MLOG_LOGFILE_H__
#include "LogCommon.h"
#include <memory>
#include <stdio.h>

namespace mlog {

class File : Noncopyable
{
 public:
  explicit File(StringArg filename);
  ~File();

  size_t write(const char *data, size_t len);

  void flush();

  size_t writtenBytes() { return writtenBytes_; }

 private:
  FILE *fp_;
  char buffer_[64*1024];
  size_t writtenBytes_;
};

class LogFile : Noncopyable
{
 public:
  LogFile(const string &basename,
        size_t rollSize,
        bool threadSafe = true,
        int flushInterval = 3,
        int checkEveryN = 100);
  ~LogFile() = default;

  void append(const char *message, int len);
  void flush();
  bool rollFile();

 private:
  void append_unlocked(const char* message, int len);

  static string getLogFileName(const string& basename, time_t* now);
  
  const string basename_;
  const size_t rollSize_;
  const int flushInterval_;
  const int checkEveryN_;
  std::unique_ptr<Mutex> mutexPtr_;
  std::unique_ptr<File> filePtr_;
  time_t lastRoll_;
  time_t lastFlush_;
  time_t startOfPeriod_;
  int count_;

  const static int kRollPeriodSeconds_ = 60*60*24;
};


} //namespace mlog

#endif
