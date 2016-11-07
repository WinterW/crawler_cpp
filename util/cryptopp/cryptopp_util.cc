/**
 * @Copyright (c) 2011 Ganji Inc.
 * @file          ganji/util/cryptopp/cryptopp_util.cc
 * @namespace     ganji::util::cryptopp
 * @version       1.0
 * @author        liubijian
 * @date          2011-04-21
 *
 * Change Log:
 *
 */
#include "cryptopp_util.h"

namespace ganji { namespace util { namespace cryptopp { namespace CryptoPPUtil {
std::string HexToStr(byte value) {
  char sz_buf[3];
  byte h = (value >> 4) & 0x0f;
  byte l = (value & 0x0f);
  if (h >= 10) {
    sz_buf[0] = 'a'+h-10;
  } else {
    sz_buf[0] = '0' + h;
  }
  if (l >= 10) {
    sz_buf[1] = 'a'+l-10;
  } else {
    sz_buf[1] = '0' + l;
  }
  sz_buf[2] = 0;
  std::string str = sz_buf;
  return str;
}

uint64_t StrToHex(const std::string &value) {
  uint64_t ret = 0;
  for (std::size_t i = 0; i < value.length(); i++) {
    int x = 0;
    if (value[i] >= '0' && value[i] <= '9') {
      x = value[i] - '0';
    } else if (value[i] >= 'a' && value[i] <= 'f') {
      x = value[i] - 'a' + 10;
    }
    ret <<= 4;
    ret += x;
  }
  return ret;
}

std::string Md5Encode(const std::string &in_str) {
  std::string out_string = "";
  if (in_str.empty()) {
    return out_string;
  }

  CryptoPP::Weak::MD5 md5;

  byte out[CryptoPP::Weak::MD5::DIGESTSIZE];
  memset(out, 0, CryptoPP::Weak::MD5::DIGESTSIZE);
  int len = in_str.length();
  byte *message = new byte[len];
  memset(message, 0, len);
  memcpy(message, in_str.c_str(), len);
  md5.CalculateDigest(out, message, len);

  for (int i = 0; i < CryptoPP::Weak::MD5::DIGESTSIZE; ++i) {
    out_string += HexToStr(out[i]);
  }

  delete []message;
  return out_string;
}

std::string AesEncode(const std::string &in_str, const std::string &key) {
  std::string out_string = "";
  if (in_str.empty() || key.empty()) {
    return out_string;
  }

  CryptoPP::AESEncryption aes_encryptor;

  unsigned char aes_key[CryptoPP::AES::MAX_KEYLENGTH];
  unsigned char in_block[CryptoPP::AES::BLOCKSIZE];
  unsigned char out_block[CryptoPP::AES::BLOCKSIZE];
  unsigned char xor_block[CryptoPP::AES::BLOCKSIZE];

  memset(aes_key, 0, CryptoPP::AES::MAX_KEYLENGTH);
  memset(in_block, 0, CryptoPP::AES::BLOCKSIZE);
  memset(out_block, 0, CryptoPP::AES::BLOCKSIZE);
  memset(xor_block, 0, CryptoPP::AES::BLOCKSIZE);

  int key_len;
  int original_key_len = static_cast<int>(key.length());
  if (original_key_len > CryptoPP::AES::MAX_KEYLENGTH) {
    key_len = CryptoPP::AES::MAX_KEYLENGTH;
  } else {
    key_len = original_key_len;
  }
  for (int i = 0; i < key_len; ++i) {
    aes_key[i] = key[i];
  }

  for (std::size_t pos = 0; pos < in_str.length(); pos += CryptoPP::AES::BLOCKSIZE) {
    std::string tmp_str = "";
    if (pos + CryptoPP::AES::BLOCKSIZE > in_str.length()) {
      tmp_str = in_str.substr(pos);
    } else {
      tmp_str = in_str.substr(pos, CryptoPP::AES::BLOCKSIZE);
    }

    for (std::size_t i = 0; i < tmp_str.length(); ++i) {
      in_block[i] = tmp_str[i];
    }

    aes_encryptor.SetKey(aes_key, CryptoPP::AES::MAX_KEYLENGTH);
    aes_encryptor.ProcessAndXorBlock(in_block, xor_block, out_block);
    for (int i = 0; i < CryptoPP::AES::BLOCKSIZE; ++i) {
      out_string += HexToStr(out_block[i]);
    }
    memset(in_block, 0, CryptoPP::AES::BLOCKSIZE);
    memset(out_block, 0, CryptoPP::AES::BLOCKSIZE);
  }
  return out_string;
}

std::string AesDecode(const std::string &in_str, const std::string &key) {
  std::string out_string = "";
  if (in_str.empty() || key.empty()) {
    return out_string;
  }

  CryptoPP::AESDecryption aes_decryptor;

  unsigned char aes_key[CryptoPP::AES::MAX_KEYLENGTH];
  unsigned char in_block[CryptoPP::AES::BLOCKSIZE];
  unsigned char out_block[CryptoPP::AES::BLOCKSIZE];
  unsigned char xor_block[CryptoPP::AES::BLOCKSIZE];

  memset(aes_key, 0, CryptoPP::AES::MAX_KEYLENGTH);
  memset(in_block, 0, CryptoPP::AES::BLOCKSIZE);
  memset(out_block, 0, CryptoPP::AES::BLOCKSIZE);
  memset(xor_block, 0, CryptoPP::AES::BLOCKSIZE);

  int key_len;
  int original_key_len = static_cast<int>(key.length());
  if (original_key_len > CryptoPP::AES::MAX_KEYLENGTH) {
    key_len = CryptoPP::AES::MAX_KEYLENGTH;
  } else {
    key_len = original_key_len;
  }
  for (int i = 0; i < key_len; ++i) {
    aes_key[i] = key[i];
  }

  int step = CryptoPP::AES::BLOCKSIZE * 2;
  for (std::size_t pos = 0; pos < in_str.length(); pos += step) {
    std::string tmp_str = "";
    if (pos + step > in_str.length()) {
      tmp_str = in_str.substr(pos);
    } else {
      tmp_str = in_str.substr(pos, step);
    }
    for (std::size_t i = 0, n = 0; i < tmp_str.length(); ++n, i += 2) {
      std::string tmp_string = "";
      if (i + 2 > in_str.length()) {
        tmp_string = tmp_str.substr(i);
      } else {
        tmp_string = tmp_str.substr(i, 2);
      }
      if (n >= 16) {
        break;
      }
      in_block[n] = StrToHex(tmp_string);
    }
    aes_decryptor.SetKey(aes_key, CryptoPP::AES::MAX_KEYLENGTH);
    aes_decryptor.ProcessAndXorBlock(in_block, xor_block, out_block);
    for (int i = 0; i < CryptoPP::AES::BLOCKSIZE; ++i) {
      out_string += out_block[i];
    }
    memset(in_block, 0, CryptoPP::AES::BLOCKSIZE);
    memset(out_block, 0, CryptoPP::AES::BLOCKSIZE);
  }
  return out_string;
}
} } } }  // end of namespace ganji::util::cryptopp
