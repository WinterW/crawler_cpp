#!/bin/bash
# 开机启动号码图片相关服务和脚本

source ~/.bash_profile

# 启动nginx
/home/ganji/nginx/sbin/nginx

# 启动fcgi进程
cd /server/www/ganji/ganji_online/com_service/phone_img_svr
spawn-fcgi -p 20202 ./phone_img_svr -F32
spawn-fcgi -p 20203 ./img_url_svr -F32

# 启动监控
cd /server/www/ganji/ganji_online/com_service/phone_img_svr/monitor
nohup python monitor.py &
