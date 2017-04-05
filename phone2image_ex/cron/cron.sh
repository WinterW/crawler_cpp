#/bin/bash
# 每日切割nginx日志，清除过期数据
# XXX nginx切割日志需要sudo权限
# crontab:
# cd /server/www/ganji/ganji_online/cra/phone_img_svr/cron; sh -x cron.sh > log/cron.log.`date \%Y\%m\%d -d "1 days ago"` 2&>1

# 数据过期天数
NGINX_LOG_EXPIRE_DAY=30
CRON_LOG_EXPIRE_DAY=30
MONITOR_LOG_EXPIRE_DAY=30
NGINX_LOG_DELETE_DATE=`date +%Y%m%d -d "$NGINX_LOG_EXPIRE_DAY days ago"`
CRON_LOG_DELETE_DATE=`date +%Y%m%d -d "$CRON_LOG_EXPIRE_DAY days ago"`
MONITOR_LOG_DELETE_DATE=`date +%Y%m%d -d "$MONITOR_LOG_EXPIRE_DAY days ago"`

HOST_NAME=`hostname`
YESTERDAY=`date +%Y%m%d -d "1 days ago"`
YESTERDAY_ACCESS_LOG=access.log.$HOST_NAME.$YESTERDAY
YESTERDAY_ERROR_LOG=error.log.$HOST_NAME.$YESTERDAY
# nginx可执行程序
NGINX_BIN=/usr/local/webserver/nginx/sbin/nginx

# svr路径
SVR_PATH=/server/www/ganji/ganji_online/cra/phone_img_svr/
# nginx日志路径
NGINX_LOG_PATH=$SVR_PATH/www/log
# crontab脚本的路径
CRON_PATH=$SVR_PATH/cron/
CRON_LOG_PATH=$CRON_PATH/log
# 监控日志路径
MONITOR_LOG_PATH=$SVR_PATH/monitor/log

REMOTE_SERVER=192.168.113.120
REMOTE_PATH=/home/ganji/phone_img_svr/access_log/

# 创建crontab运行日志目录
if [ ! -d $CRON_LOG_PATH ]
then
  mkdir $CRON_LOG_PATH
  if [ $? -ne 0 ]
  then
    echo "mkdir $CRON_LOG_PATH failed"
    exit 1
  fi
fi

# 删除过期nginx日志
cd $NGINX_LOG_PATH
rm -rf access.log.$HOST_NAME.$NGINX_LOG_DELETE_DATE
rm -rf error.log.$HOST_NAME.$NGINX_LOG_DELETE_DATE

if [ -f $YESTERDAY_ACCESS_LOG ]
then
  echo "old access log:$YESTERDAY_ACCESS_LOG exists"
  #exit 1
fi
if [ -f $YESTERDAY_ERROR_LOG ]
then
  echo "old error log:$YESTERDAY_ERROR_LOG exists"
  #exit 1
fi

date

# 切换日志
/bin/mv -f access.log $YESTERDAY_ACCESS_LOG
/bin/mv -f error.log $YESTERDAY_ERROR_LOG

# 重新load nginx
sudo $NGINX_BIN -s reload

# 删除crontab运行过期日志
cd $CRON_LOG_PATH
rm -rf cron.log.$CRON_LOG_DELETE_DATE

# 删除过期监控日志
cd $MONITOR_LOG_PATH
rm -rf log.$MONITOR_LOG_DELETE_DATE

exit 0

# scp至远程服务器
scp $YESTERDAY_ACCESS_LOG $REMOTE_SERVER:$REMOTE_PATH
success=1
if [ $? -ne 0 ]
then
  echo "scp $YESTERDAY_ACCESS_LOG failed"
  success=0
fi

scp $YESTERDAY_ERROR_LOG $REMOTE_SERVER:$REMOTE_PATH
if [ $? -ne 0 ]
then
  echo "scp $YESTERDAY_ERROR_LOG failed"
  success=0
fi

date
if [[ $success == 1 ]]
then
  echo "Success"
else
  echo "Failed"
fi
