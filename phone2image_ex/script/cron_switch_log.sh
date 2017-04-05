#/bin/bash
# 每日切割nginx日志，并scp至远端服务器

YESTERDAY=`date +%Y%m%d -d "1 days ago"`
HOST_NAME=`hostname`
OLD_ACCESS_LOG=access.log.$HOST_NAME.$YESTERDAY
OLD_ERROR_LOG=error.log.$HOST_NAME.$YESTERDAY
REMOTE_SERVER=192.168.113.120
REMOTE_PATH=/home/ganji/phone_img_svr/access_log/
# 本地保存的日志天数
OLDEST_DAYS=`date +%Y%m%d -d "7 days ago"`

cd /home/ganji/nginx/logs

# 删除过期日志
rm -rf access.log.$HOST_NAME.$OLDEST_DAYS
rm -rf error.log.$HOST_NAME.$OLDEST_DAYS

if [ -f $OLD_ACCESS_LOG ]
then
  echo "old access log:$OLD_ACCESS_LOG exists"
  #exit 1
fi
if [ -f $OLD_ERROR_LOG ]
then
  echo "old error log:$OLD_ERROR_LOG exists"
  #exit 1
fi

date

# 切换日志
/bin/mv access.log $OLD_ACCESS_LOG
/bin/mv error.log $OLD_ERROR_LOG

# 重新load
/home/ganji/nginx/sbin/nginx -s reload

# scp至远程服务器
scp $OLD_ACCESS_LOG $REMOTE_SERVER:$REMOTE_PATH
success=1
if [ $? -ne 0 ]
then
  echo "scp $OLD_ACCESS_LOG failed"
  success=0
fi

scp $OLD_ERROR_LOG $REMOTE_SERVER:$REMOTE_PATH
if [ $? -ne 0 ]
then
  echo "scp $OLD_ERROR_LOG failed"
  success=0
fi

date
if [[ $success == 1 ]]
then
  echo "Success"
else
  echo "Failed"
fi
