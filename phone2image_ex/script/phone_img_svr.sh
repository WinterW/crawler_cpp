#!/bin/bash

if [ $# -ne 1 ]
then
  echo "Usage:$0 {start|stop|restart}"
  exit 1
fi

SVR_PORT=20202 
PROC_COUNT=32
PROG=phone_img_svr

CONF_PATH=../conf/
LOG_PATH=../log/

# 设置环境变量
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/webserver/fcgi/lib:/usr/local/webserver/thrift/lib:/usr/local/webserver/gd-2.0.35/lib
export PATH=$PATH:/usr/local/webserver/spawn-fcgi/bin
ulimit -c unlimited

cd /server/www/ganji/ganji_online/cra/phone_img_svr/bin/

function start() {
  # 启动fcgi进程
  spawn-fcgi -p $SVR_PORT -F $PROC_COUNT $PROG $CONF_PATH $LOG_PATH
  if [ $? -ne 0 ]
  then
    echo "start $PROG failed:$?"
    exit 2
  else
    echo "start $PROG OK"
  fi
}

stop() {
  killall $PROG
}

case "$1" in
  start)
    start
    ;;

  stop)
    stop
    ;;

  restart)
    stop
    start
    echo "restart $PROG OK"
    ;;

  *)
    echo "Usage:$0 {start|stop|restart}"
    exit 4
    ;;
esac

exit 0

  

