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
#include "LogFile.h"
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

using namespace mlog;

File::File(StringArg filename)
  : writtenBytes_(0)
{
  fp_ = ::fopen(filename.c_str(), "ae");  // 'e' for O_CLOEXEC
  if (fp_)
    ::setbuffer(fp_, buffer_, sizeof buffer_);
}

File::~File()
{
  if (fp_)
    ::fclose(fp_);
}

size_t File::write(const char * data, size_t len)
{
  if (fp_ == nullptr)
    return 0;
  size_t ret = ::fwrite(data, len, 1, fp_);
  if (ret > 0)
    writtenBytes_ += ret;
  return ret;
}

void File::flush()
{
  ::fflush(fp_);
}

LogFile::LogFile(const string &basename,
                  size_t rollSize,
                  bool threadSafe,
                  int flushInterval,
                  int checkEveryN)
  : basename_(basename),
    rollSize_(rollSize),
    flushInterval_(flushInterval),
    checkEveryN_(checkEveryN),
    mutexPtr_(threadSafe ? new Mutex : nullptr),
    lastRoll_(0),
    lastFlush_(0),
    startOfPeriod_(0),
    count_(0)
{
  rollFile();
}

void LogFile::append(const char* message, int len)
{
  if (mutexPtr_)
  {
    MutexGuard guard(*mutexPtr_);
    append_unlocked(message, len);
  }
  else
  {
    append_unlocked(message, len);
  }
}

void LogFile::flush()
{
  if (mutexPtr_)
  {
    MutexGuard guard(*mutexPtr_);
    filePtr_->flush();
  }
  else
  {
    filePtr_->flush();
  }
}

void LogFile::append_unlocked(const char* message, int len)
{
  filePtr_->write(message, len);

  if (filePtr_->writtenBytes() > rollSize_)
  {
    rollFile();
  }
  else
  {
    ++count_;
    if (count_ >= checkEveryN_)
    {
      count_ = 0;
      time_t now = ::time(NULL);
      time_t thisPeriod_ = now / kRollPeriodSeconds_ * kRollPeriodSeconds_;
      if (thisPeriod_ != startOfPeriod_)
      {
        rollFile();
      }
      else if (now - lastFlush_ > flushInterval_)
      {
        lastFlush_ = now;
        filePtr_->flush();
      }
    }
  }
}

bool LogFile::rollFile()
{
  time_t now = 0;
  string filename = getLogFileName(basename_, &now);
  time_t start = now / kRollPeriodSeconds_ * kRollPeriodSeconds_;

  if (now > lastRoll_)
  {
    lastRoll_ = now;
    lastFlush_ = now;
    startOfPeriod_ = start;
    filePtr_.reset(new mlog::File(filename));
    return true;
  }
  return false;
}

string LogFile::getLogFileName(const string& basename, time_t* now)
{
  string filename;
  filename.reserve(basename.size() + 64);
  filename = basename;

  char timebuf[32];
  struct tm tm;
  *now = time(NULL);
  gmtime_r(now, &tm); // FIXME: localtime_r ?
  strftime(timebuf, sizeof timebuf, ".%Y%m%d-%H%M%S.", &tm);
  filename += timebuf;
  filename += "log";

  return filename;
}

