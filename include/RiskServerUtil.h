#pragma once

#include <string>
#include <sys/select.h>
#include <sys/time.h>

namespace utility {
static inline int ConvertTimeToHHMMSSss(const std::string &strTime) {
  int hour, minute, second, millisecond = 0;
  const char *p = strTime.data();
  size_t len = strTime.size();

  // HH:MM:SS (8 chars)
  if (len == 8 && p[2] == ':' && p[5] == ':') {
    if (!isdigit(p[0]) || !isdigit(p[1]) || !isdigit(p[3]) || !isdigit(p[4]) ||
        !isdigit(p[6]) || !isdigit(p[7])) {
      return 0;
    }

    hour = (p[0] - '0') * 10 + (p[1] - '0');
    minute = (p[3] - '0') * 10 + (p[4] - '0');
    second = (p[6] - '0') * 10 + (p[7] - '0');
  }
  // YYYYMMDD HH:MM:SS.sss (21 chars)
  else if (len == 21 && p[8] == ' ' && p[11] == ':' && p[14] == ':' &&
           p[17] == '.') {
    for (size_t i = 0; i < len; ++i) {
      if (i == 8 || i == 11 || i == 14 || i == 17)
        continue;
      if (!isdigit(p[i]))
        return 0;
    }

    hour = (p[9] - '0') * 10 + (p[10] - '0');
    minute = (p[12] - '0') * 10 + (p[13] - '0');
    second = (p[15] - '0') * 10 + (p[16] - '0');
    millisecond = (p[18] - '0') * 100 + (p[19] - '0') * 10 + (p[20] - '0');
  } else {
    return 0;
  }

  if (hour > 23 || minute > 59 || second > 59 || millisecond > 999) {
    return 0;
  }

  return hour * 1000000 + minute * 10000 + second * 1000 + millisecond;
}

static inline const int GetNowTimeHHMMSSss()
{
    struct timeval CurrentTime;
    gettimeofday(&CurrentTime, NULL); // 1970年1月1日到现在的秒数
    // 仅保留今天的部分 86400 = 24小时 以及 8小时时差
    CurrentTime.tv_sec = (CurrentTime.tv_sec + 3600 * 8) % 86400;
    CurrentTime.tv_sec %= 3600;
    int HH = CurrentTime.tv_sec / 60;
    CurrentTime.tv_sec %= 60;
    int MM = CurrentTime.tv_sec / 60;
    CurrentTime.tv_sec %= 60;
    int SS = CurrentTime.tv_sec;
    int sss = CurrentTime.tv_usec / 1000;
    return HH * 1000000 + MM * 10000 + SS * 1000 + sss;
}

} // namespace utility