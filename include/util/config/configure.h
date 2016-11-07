/** 
 * @Copyright (c) 2010 Ganji Inc.
 * @file    ganji/util/config/configure.h
 * @namespace ganji::util::config
 * @version 1.0
 * @author  yangfenqiang
 * @date    2011-06-21
 * 
 */

#ifndef _GANJI_UTIL_CONFIG_CONFIGURE_H_
#define _GANJI_UTIL_CONFIG_CONFIGURE_H_

#include <stdio.h>
#include <string>
#include "ganji/util/xml/tinyxml.h"

using std::string;

namespace ganji { namespace util { namespace config {
class Configure {
 public:
  struct AttrIter {
    AttrIter() {}

    AttrIter(TiXmlAttribute* attr) : attr_(attr) {}

    AttrIter& operator= (const AttrIter& attr) {
      attr_ = attr.attr_;
      return *this;
    }

    AttrIter& operator++ () {
      attr_ = attr_->Next();
      return *this;
    }

    bool HasNext() {
      if (attr_ != NULL) {
        return true;
      }
      return false;
    }

    string Key() {
      return attr_->Name();
    }

    int IntValue() {
      return attr_->IntValue();
    }

    double DoubleValue() {
      return attr_->DoubleValue();
    }

    string Value() {
      return attr_->Value();
    }

   private:
    TiXmlAttribute* attr_;
  };

  struct TagIter {
    TagIter() {}

    TagIter(TiXmlElement* tag) : tag_(tag->FirstChildElement()) {}

    TagIter& operator= (const TagIter& tag) {
      tag_ = tag.tag_;
      return *this;
    }

    TagIter& operator++ () {
      tag_ = tag_->NextSiblingElement();
      return *this;
    }

    bool HasNext() {
      if (tag_ != NULL) {
        return true;
      }
      return false;
    }

    string Name() {
      return tag_->Value();
    }

    AttrIter Attri() {
      return tag_->FirstAttribute();
    }

    int IntAttri(const char* name){
      const char* value = tag_->Attribute(name);
      return atoi(value);
    }

    const char* Attri(const char* name){
      return tag_->Attribute(name);
    }

    double DoubleAttri(const char* name){
      const char* value = tag_->Attribute(name);
      return atof(value);
    }

   private:
    TiXmlElement* tag_;
  };

  Configure(const char* fname) : conf_(fname) {
    if (!conf_.LoadFile()) {
      fprintf(stderr, "Load file [%s] failed", fname);
      abort();
    }
    rootElem_ = conf_.RootElement();
  }

  int getValue(const char* tagPath, int& value) {
    TiXmlAttribute* attr = singleValue(tagPath);
    if (attr == NULL) {
      return -1;
    }
    value = attr->IntValue();
    return 0;
  }

  int getValue(const char* tagPath, string& value) {
    TiXmlAttribute* attr = singleValue(tagPath);
    if (attr == NULL) {
      return -1;
    }
    value = attr->Value();
    return 0;
  }

  int getValue(const char* tagPath, double& value) {
    TiXmlAttribute* attr = singleValue(tagPath);
    if (attr == NULL) {
      return -1;
    }
    value = attr->DoubleValue();
    return 0;
  }

  int getValue(const char* tagPath, AttrIter& value) {
    TiXmlAttribute* attr = singleValue(tagPath);
    if (attr == NULL) {
      return -1;
    }
    value = AttrIter(attr);
    return 0;
  }

  int getValue(const char* tagPath, TagIter& value) {
    snprintf(buf_, kMaxBufSize_-1, "%s", tagPath);
    TiXmlElement* tag = getTag(buf_);
    if (tag == NULL) {
      return -1;
    }
    value = TagIter(tag);
    return 0;
  }

  static char* getToken(char*& str_begin, const char delimiter) {
    if (str_begin == 0) {
      return 0;
    }
    char *word_begin = str_begin;
    char *word_end = str_begin;
    while (*word_end) {
      if (*word_end == delimiter) {
        break;
      }
      ++word_end;
    }
    if (*word_end != 0) {
      *word_end = 0;
      str_begin = word_end + 1;
    } else {
      str_begin = 0;
    }
    return word_begin;
  }

 private:
  TiXmlAttribute* singleValue(const char* tagPath) {
    snprintf(buf_, kMaxBufSize_-1, "%s", tagPath);
    char* str = buf_;
    char* tags = getToken(str, ':');
    char* attri = getToken(str, ':');

    TiXmlElement* elem = getTag(tags);
    if (elem == NULL) {
      return NULL;
    }
    TiXmlAttribute* attr = elem->FirstAttribute();
    if (attri != NULL) {
      while (attr != NULL && strcmp(attr->Name(), attri) != 0) {
        attr = attr->Next();
      }
    }
    return attr;
  }

  TiXmlElement* getTag(char* tagPath) {
    char* tag = 0;
    TiXmlElement* temp = rootElem_;
    while ((tag = getToken(tagPath, '.')) != NULL) {
      TiXmlElement* elem = temp->FirstChildElement(tag);
      if (elem == NULL) {
        return NULL;
      } else {
        temp = elem;
      }
    }
    return temp;
  }

  static const int kMaxBufSize_ =512;
  char buf_[kMaxBufSize_];
  TiXmlElement* rootElem_;
  TiXmlDocument conf_;
};
}}}
#endif  ///< _GANJI_UTIL_CONFIG_CONFIGURE_H_
