#include <iostream>
#include <string>
#include "ganji/util/cryptopp/cryptopp_util.h"
#include "ganji/util/config/commonconfig.h"

namespace CryptoPPUtil = ganji::util::cryptopp::CryptoPPUtil;
int main( int, char** ) {
  //ganji::util::config::CommonConfig conf("/server/www/cc_dev/src/ganji/sandbox/test.conf");
  //std::string md5_key = conf.str_configs_["acf_md5_key"];
  //std::string aes_key = conf.str_configs_["acf_aes_key"];
  std::string aes_key = "ganjiaeskey";
  //std::string base_url = "r=20110422142152_333_010&u=31154114-4e16-11e0-9cb6-842b2b047f42&i=211.151.69.44&c=7&mc=1&ci=0&d=173&s=7126&x=0&so=1&a=124:29:7:1:0:173:7126:199:1:2.3:2.2:1,121:233:7:1:0:173:7126:199:2:2.1:1.5:1,234:923:7:1:0:173:7126:199:3:1.9:1.4:3,99:101:7:1:0:173:7126:199:4:1.5:1.2:1,299:423:7:1:0:173:7126:199:5:1.2:1.0:3&t=1";
  //std::string base_url = "r=20110422142152_333_010&u=31154114-4e16-11e0-9cb6-842b2b047f42&i=211.151.69.44&c=7&mc=1&ci=0&d=173&s=7126&x=0&so=1&a=299:423:7:1:0:173:7126:199:5:1.2:1.0:3&t=2";
  //std::string base_url = "r=20110513085255_406_40684&u=234&i=192.168.113.171&c=7&mc=1&ci=12&d=0&s=0&x=0&so=2&a=22013:4034440:0:1:12:182:442:1009:3:100.00:100.00:1&t=2&si=0183adac9c99a2909ae431db80e249d9";
  std::string base_url = "1172904256 r=20110513102555_363_83214&u=2343ekf&i=121.18.126.163&c=7&mc=1&ci=12&d=0&s=0&x=0&so=2&a=22767:4028374:0:1:12:173:794:2020:4:96.00:94.05:1&t=2&si=f9bd1f0f7466849aa585bbdd13d5d135";
  //std::string base_url = "r=20110513021054_779_162&u=2345&i=192.168.113.171&c=7&mc=1&ci=12&d=0&s=0&x=0&so=2&a=28811:4034217:0:1:12:174:234:6063:5:100.00:0.10:1&t=2";
  // MD5编码
  //std::string md5_url = md5_key + base_url;
  //while (true) {
  //std::string encode_md5_url = CryptoPPUtil::Md5Encode(md5_url);
  //std::cout << "1";

  // AES编码
  //std::string aes_url = base_url +"&si=" + encode_md5_url;
  std::string aes_url = base_url;
  std::string encode_aes_url = CryptoPPUtil::AesEncode(aes_url, aes_key);

  // AES解码
  std::string decode_url = CryptoPPUtil::AesDecode(encode_aes_url, aes_key);

  std::cout << "url: " << aes_url << std::endl << std::endl;
  //std::cout << "encode md5 url: " << encode_md5_url << std::endl << std::endl;
  std::cout << "encode aes url: " << encode_aes_url << " length: " << encode_aes_url.size() << std::endl << std::endl;
  std::cout << "decode ase url: " << decode_url << " length: " << decode_url.size() << std::endl << std::endl;
  return 0;
}



