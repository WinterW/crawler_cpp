/**
 * @Copyright 2011 GanJi Inc.
 * @file    ganji/crawler/conf_crawler/link_base/struct_def.h
 * @namespace ganji::crawler::conf_crawler::link_base
 * @version 1.0
 * @author  lisizhong
 * @date    2011-07-24
 *
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 */

#ifndef _GANJI_CRAWLER_CONF_CRAWLER_LINK_BASE_STRUCT_DEF_H_
#define _GANJI_CRAWLER_CONF_CRAWLER_LINK_BASE_STRUCT_DEF_H_

#include <string.h>
#include <limits.h>
#include <hash_fun.h>

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <functional>
#include <algorithm>

#include "conf_crawler/link_base/conf_crawler_types.h"
#include "conf_crawler/common/struct_def.h"

namespace ganji { namespace crawler { namespace conf_crawler { namespace link_base {
/// url for link with next depth
const char kLinkUrl[] = "LINK_URL";
/// post url with next depth
const char kPostUrl[] = "POST_URL";
/// img url with next depth
const char kImgUrl[] = "IMG_URL";
/// phone img url with next depth
const char kPhoneImgUrl[] = "PHONE_IMG_URL";
/// next page url
const char kNextPage[] = "NEXT_PAGE_URL";

const char kIsBannedPage[] = "IS_BANNED_PAGE";

/// the seed table for link_base
const char kSeedTable[] = "seed_table";
/// the prefix for select sql
const char kSelectSqlPrefix[] = "select seed_url, is_valid, downloader_type, header_fields_type, template_type, url_template, max_depth, store_body_depth, store_extract_depth, down_interval, freq, init_freq, max_freq, min_freq,dynamic_page_turn,max_pages from ";
const char kStaticTable[] = "seed_table";

/// delimeter for type field
const char kTypeDelim = ',';

/// the collection name in mongodb
const char kMongoBodyCltn[] = "body";
const char kMongoExtractCltn[] = "extract";
const char kMongoImgCltn[] = "img";
/// static seed collection in mongodb
const char kMongoStaticSeedCltn[] = "seed";

const int kErrBufLen = 1024;

struct ExtractResultItem {
  ExtractResultItem(const ExtractItem &extract_item,
                    const MatchedResultItem &matched_result_item)
    : extract_item_(extract_item),
    matched_result_item_(matched_result_item) {
  }

  ExtractItem extract_item_;
  MatchedResultItem matched_result_item_;
};

/**
 * @struct ExpandLinkItem
 * @brief information for expand link
 */
const int kUninitialized = -1;
struct ExpandLinkItem {
  ExpandLinkItem()
    : is_img_(false),
    is_phone_img_(false),
    depth_(kUninitialized),
    add_time_(0) {
  }

  std::string url_;                        ///< link url
  std::string referer_;                    ///< referer
  std::string seed_url_;                   ///< seed url
  bool is_img_;                            ///< whether the url is for img
  bool is_phone_img_;                      ///< whether the url is for phone img
  int depth_;                              ///< depth
  time_t add_time_;                        ///< timestamp when add
};

/**
 * @struct BaseSeedItem
 * @brief base info for seed
 */
struct BaseSeedItem {
  BaseSeedItem()
    : max_depth_(-1),
    store_body_depth_(0),
    store_extract_depth_(0),
    is_friendly_(true),
    //modified by wangsj
    //down_interval_(0),
    //end
    is_valid_(false) {
  }

  std::string seed_url_;              ///< seed url
  std::vector<DownloaderType::type> downloader_type_list_;          ///< downloader type
  std::vector<HeaderFieldsType::type> header_fields_type_list_;     ///< extract type
  std::vector<TemplateType::type> template_type_list_;              ///< extract type
  std::string url_template_;          ///< template name
  int max_depth_;                     ///< max crawl/extract depth, start from 0, DO NOT process #depth>max_depth_
  int store_body_depth_;              ///< start depth to store body
  int store_extract_depth_;           ///< start depth to store extract
  bool is_friendly_;                  ///< whether download is friendly
  //modified by wangsj
  //int down_interval_;               ///< download interval if friendly, in secs
  std::vector<int> down_interval_;    ///< download interval if friendly, in millisecs
  //end
  bool is_valid_;                     ///< whether the seed is valid
};

const int kCrawlSegCount = 3;
/**
 * @struct SeedItem
 * @brief info for seed
 */
struct SeedItem: public BaseSeedItem {
  SeedItem()
    : id_(-1),
    freq_(-1),
    init_freq_(-1),
    max_freq_(-1),
    min_freq_(-1),
    prev_time_(0),
    next_time_(0),
    dynamic_page_turn_(0),
    max_pages_(0){
    pages = 0;
    memset(link_count_, 0, sizeof(link_count_));
  }

  int id_;                      ///< id in mysql table
  int freq_;                    ///< refresh frequency, in secs
  int init_freq_;               ///< initial frequency, in secs
  int max_freq_;                ///< upper limit for freq_, in secs
  int min_freq_;                ///< lower limit for freq_, in secs
  time_t prev_time_;            ///< previous processed time
  time_t next_time_;            ///< next time to process
  int link_count_[kCrawlSegCount];             ///< #new link crawled in a round
  int dynamic_page_turn_;   /// dynamic page turning
  int max_pages_;
  int pages;
  std::string lasturl;
};//__attribute__((packed));
/// seed url => seed item
typedef std::unordered_map<std::string, SeedItem, StringHash, StringHash> SeedItemMap;

/**
 * @struct StaticSeedItem
 * @brief info for static seed
 */
struct StaticSeedItem: public BaseSeedItem {
  int down_interval_;               ///< download interval if friendly, in secs
  StaticSeedItem()
    : task_id_(-1) {
  }

  int task_id_;           ///< task id
};
/// static seed url => seed item
typedef std::unordered_map<std::string, StaticSeedItem, StringHash, StringHash> StaticSeedItemMap;
/// url => expand link item
typedef std::unordered_map<std::string, ExpandLinkItem, StringHash, StringHash> ExpandLinkMap;
}}}};

#endif  ///< _GANJI_CRAWLER_CONF_CRAWLER_LINK_BASE_STRUCT_DEF_H_
