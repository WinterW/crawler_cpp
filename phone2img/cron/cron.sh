#/bin/bash
# 每日切割nginx日志，清除过期数据
# XXX nginx切割日志需要sudo权限
# crontab:
# cd /server/www/ganji/ganji_online/cra/phone_img_svr/cron; sh -x cron.sh > log/cron.log.`date \%Y\%m\%d -d "1 days ago"` 2&>1

# 数据过期天数
CRON_LOG_EXPIRE_DAY=30
MONITOR_LOG_EXPIRE_DAY=30
CRON_LOG_DELETE_DATE=`date +%Y%m%d -d "$CRON_LOG_EXPIRE_DAY days ago"`
MONITOR_LOG_DELETE_DATE=`date +%Y%m%d -d "$MONITOR_LOG_EXPIRE_DAY days ago"`

# svr路径
SVR_PATH=/home/work/phone_img_svr
SVR_LOG_PATH=$SVR_PATH/log
# nginx日志路径
NGINX_LOG_PATH=$SVR_PATH/www/log
# crontab脚本的路径
CRON_PATH=$SVR_PATH/cron/
CRON_LOG_PATH=$CRON_PATH/log
# 监控日志路径
MONITOR_LOG_PATH=$SVR_PATH/monitor/log


# 删除crontab运行过期日志
cd $CRON_LOG_PATH
rm -rf cron.log.$CRON_LOG_DELETE_DATE

# 删除过期监控日志
cd $MONITOR_LOG_PATH
rm -rf log.$MONITOR_LOG_DELETE_DATE

# 删除phone_img_svr 过期日志
cd $SVR_LOG_PATH
find ./ -type f -mtime +30|xargs rm -f

exit 0

