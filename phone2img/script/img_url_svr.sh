#!/bin/bash

if [ $# -ne 1 ]
then
  echo "Usage:$0 {start|stop|restart}"
  exit 1
fi

SVR_PORT=20203
PROC_COUNT=32
PROG=img_url_svr

CONF_PATH=../conf/
LOG_PATH=../log/
THIRD_LIB_PATH=../3rd/
BIN_PATH=/home/work/phone_img_svr/bin

# 设置环境变量
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$THIRD_LIB_PATH/fcgi/lib:$THIRD_LIB_PATH/thrift-0.5.0/lib:$THIRD_LIB_PATH/gd-2.0.35/lib
export PATH=$PATH:$THIRD_LIB_PATH/spawn-fcgi/bin
ulimit -c unlimited

cd $BIN_PATH

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

  

