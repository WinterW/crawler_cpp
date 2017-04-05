/**
 * @Copyright 2010 GanJi Inc.
 * @file    ganji/crawler/phone2image/test_des.cc
 * @namespace main func, no namespace
 * @version 1.0
 * @author  lisizhong
 * @date    2011-10-13
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <string>

#include "util/text/text.h"
#include "phone2image_ex/enc_dec.h"
#include "phone2image_ex/img_config.h"
#include "phone2image_ex/img_builder.h"

using std::string;
using ganji::crawler::phone2image::EncDec;
using ganji::crawler::phone2image::ImgConfigMgr;

namespace Text = ganji::util::text::Text;

void test() {
  ImgConfigMgr img_config_mgr;

  int ret = img_config_mgr.Init("conf");
  if (ret < 0) {
    printf("global data mgr Init failed");
    return;
  }

  EncDec enc_dec;
  ret = enc_dec.Init(&img_config_mgr);
  if (ret < 0) {
    printf("enc_dec Init failed");
    return;
  }

  for (int i = 0; i < 10000; i++) {
    int64_t phone_num = 13401000000 + i;
    string phone = Text::Int64ToStr(phone_num);
    string cat = "0";
    int level = 1;
    time_t cur_time = 12345;
    string img_id;
    ret = enc_dec.EncodeParams(phone, cat, "", "", level, cur_time, &img_id);
    if (ret < 0) {
      printf("enc_dec EncodeParams failed");
      return;
    }
    printf("%s\n", img_id.c_str());

    map<string, string> param_map;
    ret = enc_dec.ParseParams(img_id, param_map);
    if (ret < 0) {
      printf("enc_dec ParseParams failed");
      return;
    }
  }
}

int main(int argc, char *argv[]) {
  test();

  return 0;
}

