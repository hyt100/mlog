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
#ifndef __MLOG_LOGROTATE_H__
#define __MLOG_LOGROTATE_H__
#include "LogCommon.h"
#include <vector>
#include <string>

namespace mlog {

class LogRotate : Noncopyable
{
 public:
  enum RotateType
  {
      ROTATE_TYPE_NONE,
      ROTATE_TYPE_DATE,
      ROTATE_TYPE_NUM
  };

  LogRotate(string basename, RotateType type = ROTATE_TYPE_DATE, unsigned int total = 2)
    : type_(type), basename_(basename), filenamearg_(basename), total_(total), max_num_(16)
  {
    scanlist_.reserve(total_);
  }
  ~LogRotate() = default;

  int rotate();

 private:
  int scan();
  int remove(const char *name);

  RotateType type_;
  const string basename_;
  FilenameArg filenamearg_;
  unsigned int total_; //rotate total files
  unsigned int max_num_; 
  std::vector<string> scanlist_;
};


} //mlog

#endif