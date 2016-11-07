/**
 * @Copyright 2011 Ganji Inc.
 * @file    ganji/util/system/system.cc
 * @namespace ganji::util::system
 * @version 1.0
 * @author  lisizhong 
 * @date    2011-06-22
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#include "system.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#include "util/text/text.h"

using std::string;

namespace Text = ::ganji::util::text::Text;

namespace ganji { namespace util { namespace system { namespace System {
int GetMemKb(int *p_kb) {
  pid_t pid = getpid();
  const int kBufSize = 100;
  char buf[kBufSize];
  if (snprintf(buf, kBufSize, "/proc/%d/status", pid) >= kBufSize) {
    return -1;
  }
  FILE *fp = fopen(buf, "r");
  if (!fp) {
    return -1;
  }

  const char kRSS[] = "VmRSS:\t";
  while (fgets(buf, kBufSize, fp) != NULL) {
    string line = string(buf);
    size_t pos = string::npos;
    if (string::npos != (pos = line.find(kRSS))) {
      pos += strlen(kRSS);
      string value = line.substr(pos);
      Text::Trim(&value);
      // KB => MB
      *p_kb = atoi(value.c_str());
      fclose(fp);
      return 0;
    }
  }

  fclose(fp);
  return -1;
}

int GetMem(int *p_mb) {
  int kb = 0;
  int ret = GetMemKb(&kb);
  if (ret < 0)
    return -1;
  *p_mb = kb / 1024;

  return 0;
}
} } } }   ///< end of namespace ganji::util::system::System
