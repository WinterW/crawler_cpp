#include <stdio.h>
#include <pthread.h>
#include "ganji/util/log/thread_fast_log.h"
#include "ganji/util/thread/thread_heartbeat.h"

using namespace ganji::util::log::ThreadFastLog;

void *thread_start(void *args) {
  ThreadHeartbeat tht("threads name", 2);
  int i = 0;
  while (i < 5) {
    tht.Heartbeat(); 
    sleep(1);
    i++;
  }
  return NULL;
}

int main(int argc, char * argv[]) {
  FastLogStat log_st = {kLogAll, kLogFatal, kLogSizeSplit};
  if (argc > 1 && strcmp(argv[1], "-lognotice") == 0) {
    log_st.events = (kLogNotice | kLogWarning | kLogFatal);
  }
  if (0 != OpenLog("log/", "test_thread_heartbeat", 2048, &log_st)) {
    printf("Failed to initialize log.\n");
    return -1;
  }
  pthread_t thr;
  pthread_create(&thr, NULL, thread_start, NULL);
  pthread_create(&thr, NULL, thread_start, NULL);

  sleep(10);
  CloseLog(0);
  return 0;
}
