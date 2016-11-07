#include "ganji/util/log/scribe_client_ex.h"

int main(int argc, char * argv[])
{
  if (argc < 5) {
    printf(".exe host port cache_folder msg\n");
    return -1;
  }
  ScribeClientEx sc(argv[1], atoi(argv[2]), argv[3]);
  if (! sc.Init()) {
    printf("reconnect scribe server failed.\n");
    return -1;
  }
  LogEntry le;
  le.category = "nocat";
  le.message = argv[4];
  vector<LogEntry> logs;
  for (size_t ui = 0; ui < 1000000; ++ui) {
    logs.push_back(le);
  }
  if (! sc.Send(logs)) {
    printf("sending failed.\n");
  }
  else {
    printf("sending succ.\n");
  }
  return 0;
}
