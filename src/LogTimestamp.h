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
#ifndef __MLOG_LOGTIMESTAMP_H__
#define __MLOG_LOGTIMESTAMP_H__
#include "LogCommon.h"
#include <stdint.h>

namespace mlog {

class Timestamp : public mlog::less_than_comparable<Timestamp>,
                        public mlog::equality_comparable<Timestamp>
{
 public:
  static const int kMicroSecondsPerSecond = 1000 * 1000;

  Timestamp()
    : microSecondsSinceEpoch_(0)
  {}

  Timestamp(int64_t microSecondsSinceEpoch)
    : microSecondsSinceEpoch_(microSecondsSinceEpoch)
  {}

  // default copy/assignment/dtor are Okay

  void swap(Timestamp &that)
  {
    std::swap(this->microSecondsSinceEpoch_, that.microSecondsSinceEpoch_);
  }

  void add(double seconds)
  {
    int64_t delta = static_cast<int64_t>(seconds * kMicroSecondsPerSecond);
    microSecondsSinceEpoch_ += delta;
  }

  int64_t microSecondsSinceEpoch() const { return microSecondsSinceEpoch_; }
  int64_t secondsSinceEpoch() const { return microSecondsSinceEpoch_ / kMicroSecondsPerSecond; }

  string toString() const;
  string toFormattedString(bool showMicroseconds = true) const;

  // Gets time difference of two timestamps, result in seconds.
  static double diffrence(Timestamp &hight, Timestamp &low)
  {
    int64_t diff = hight.microSecondsSinceEpoch_ - low.microSecondsSinceEpoch_;
    return static_cast<double>(diff) / kMicroSecondsPerSecond;
  }

  static Timestamp now();

 private:
  int64_t microSecondsSinceEpoch_;
};

inline bool operator<(Timestamp &left, Timestamp &right)
{
  return left.microSecondsSinceEpoch() < right.microSecondsSinceEpoch();
}

inline bool operator==(Timestamp &left, Timestamp &right)
{
  return left.microSecondsSinceEpoch() == right.microSecondsSinceEpoch();
}



} //namespace mlog

#endif
