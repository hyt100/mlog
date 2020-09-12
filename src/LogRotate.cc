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
#include "LogCommon.h"
#include "LogRotate.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h> //for readdir
#include <iostream>

using namespace mlog;

int LogRotate::remove(const char *name)
{
  unlink(name);
  std::cout<< "delete " << name << std::endl;
  return 0;
}

int LogRotate::rotate()
{
  if (type_ == ROTATE_TYPE_NONE)
    return 0;
  if (scan() != 0)
    return -1;

  if (type_ == ROTATE_TYPE_DATE)
  {
    if (scanlist_.size() > total_)
    {
      int delete_num = scanlist_.size() - total_;
      for (int i=0; i<delete_num; ++i)
      {
        remove(scanlist_[i].c_str());
      }
    }
    return 0;
  }


  return 0;
}

int LogRotate::scan()
{
  // std::cout << "path:" << filenamearg_.get_path() << " name:" << filenamearg_.get_name() << std::endl;
  DIR *pDir = opendir(filenamearg_.get_path());
  if (!pDir)
  {
    perror("opendir");
    return -1;
  }
  
  scanlist_.clear();
  while (1)
  {
    struct dirent *pDent = readdir(pDir);
    if (!pDent)
      break;
    if (pDent->d_type != DT_REG)
      continue;
    if (strncmp(pDent->d_name, filenamearg_.get_name(), strlen(filenamearg_.get_name())) != 0)
      continue;
    
    scanlist_.push_back(pDent->d_name);
    std::sort(scanlist_.begin(), scanlist_.end());
  }
  closedir(pDir);

  for (auto &x : scanlist_)
    std::cout << x << std::endl;
  return 0;
}