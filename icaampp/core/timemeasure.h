#ifndef TIME_MEASURE_H_
#define TIME_MEASURE_H_

#include <sys/time.h>
#include <sys/resource.h>

// measure real process time, not global time

class TimeMeasure
{
 private:
  timeval tv_start, tv_stop;
  long counter;
  long call_counter;

 public:
  TimeMeasure() {
    counter = 0;
    call_counter = 0;
  }
  
  long get_counter() { return counter; }

  void start() {
    rusage ru;
    getrusage(RUSAGE_SELF, &ru);
    tv_start = ru.ru_utime;

    // ru_utime = user time
    // ru_stime = system time

    tv_start.tv_usec += ru.ru_stime.tv_usec;
    tv_start.tv_sec += ru.ru_stime.tv_sec;
  }

  void stop() {
    rusage ru;
    getrusage(RUSAGE_SELF, &ru);
    tv_stop = ru.ru_utime;
    tv_stop.tv_usec += ru.ru_stime.tv_usec;
    tv_stop.tv_sec += ru.ru_stime.tv_sec;

    long diff_sec = tv_stop.tv_sec - tv_start.tv_sec;
    long diff_usec = tv_stop.tv_usec - tv_start.tv_usec;
    if (diff_sec > 0) {
      diff_usec += diff_sec * 1000000;
    }
    counter += diff_usec;
    call_counter++;
  }


  double mean() {
    return counter / 1000000.0 / call_counter;
  }

};

#endif
