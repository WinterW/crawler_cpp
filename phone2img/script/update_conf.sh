#!/bin/bash
# 更新img_url_svr/phone_img_svr的配置

IMG_URL_SVR_PID=$(ps -ef | grep img_url_svr | grep -v grep | awk '{print $2}' | xargs)
kill -USR1 $IMG_URL_SVR_PID

PHONE_IMG_SVR_PID=$(ps -ef | grep phone_img_svr | grep -v grep | awk '{print $2}' | xargs)
kill -USR1 $PHONE_IMG_SVR_PID
