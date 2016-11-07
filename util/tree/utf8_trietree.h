/** 
 * @Copyright (c) 2010 Ganji Inc.
 * @file          ganji/util/tree/utf8_trietree.h
 * @namespace     ganji::util::tree
 * @version       1.0
 * @author        haohuang
 * @date          2010-07-23
 * 
 * trie tree that support utf8 coding.
 * TO DO: 当前Trie树使用map作为多分枝存储结构， 比较浪费内存。 以后可以考虑更好的数据结构来实现
 * 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
 * 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 * Change Log:
 *
 */

#ifndef _GANJI_UTIL_TREE_UTF8_TRIETREE_H_
#define _GANJI_UTIL_TREE_UTF8_TRIETREE_H_

#include <string>
#include <map>
#include <vector>

#include "util/encoding/utf8conv.h"

using std::vector;

namespace ganji { namespace util { namespace tree {

template<typename INFO>
class Utf8TrieTree{
 public:
  typedef std::map<std::string,Utf8TrieTree> Tree;

 public:
  Utf8TrieTree() {
    m_isLeaf=false;
  }

  virtual ~Utf8TrieTree() {
  }

  /// 从trie树中检索状态。
  /// @param[in] strch, 一个gbk字，用来判断该字是否是Utf8TrieTree的节点。
  /// @param[in] info, 对应的字的信息
  /// @param[out] ppkws, 如果该字是Utf8TrieTree的节点, 则指向该文字的info信息。 否则指向空
  /// @return Return NULL if failed to create instance of Thread obj, else return the point of the thread object
  int Find(const std::string &strch, vector<INFO> &info, Utf8TrieTree **ppkws);
  
  /// 向trie树种添加一个词
  /// @param[in] str, 添加的词
  /// @param[in] info, 该词条所对应的数据结构
  void AddWord(const std::string &str, const INFO &info);

  ///清除Tree中存储的内容
  void Clear() {
    m_info.clear();
    m_tree.clear();
  }
 protected:
  vector<INFO> m_info;
  bool m_isLeaf;
  Tree m_tree;
};

template<typename INFO>
class Utf8TextScanner {
 public:
  struct KWInfo{
    std::string keyword;
    vector<INFO> info;
    size_t pos;
  };
 
 public:
  /// 初始化trie树， 一次性将vecIn中的所有单词插入到trie树种
  /// @param [vecIn], 构造trie树的词表
  void Initialize(const std::vector<std::pair<std::string, INFO> > &vecIn) {
    for (typename std::vector<std::pair<std::string,INFO> >::const_iterator itr = vecIn.begin(); itr!= vecIn.end(); ++itr) {
      m_kws.AddWord(itr->first,itr->second);    
    }
  }

  /// 在一段文本中查询是否出现了词表中的某个词， 并记录出现的位置
  /// @param[in] txt, 被匹配的文本
  /// @param[out] kwinfos, 匹配情况以及匹配发生的位置
  void Find (std::string &txt, std::vector<KWInfo> *pkwinfos);
  /// 清除Tree中的内容，释放空间
  void ClearTree() {
    m_kws.Clear();
  }
 protected:
  Utf8TrieTree<INFO> m_kws;
};

#define __Utf8TrieTree_Func template<class INFO>

__Utf8TrieTree_Func int Utf8TrieTree<INFO>::Find(const std::string &strch, vector<INFO> &info, Utf8TrieTree **ppkws) {
  typename Tree::iterator itr = m_tree.find(strch);
  int ret = -1;
  if (itr == m_tree.end()) {
    ret = -1;    
  } else {
    if (itr->second.m_isLeaf == true) {
      info = itr->second.m_info;
      ret = 1;
    } else {
      ret = 0;
    }    
    *ppkws = &itr->second;   
  }
  return ret;
}

__Utf8TrieTree_Func void Utf8TrieTree<INFO>::AddWord(const std::string &str ,const INFO &info) {
  std::string cur;
  size_t i =0; 
  for ( ; i < str.length(); ++i) {
    if ((i!=0) && ((str[i] & 0xc0) != 0x80)) {
      break;
    }
    cur+=str[i];
  }
  if (m_tree.find(cur) == m_tree.end()) {
    Utf8TrieTree tkws;
    m_tree[cur]= tkws;
  }

  if (i < str.length()) {
    std::string sub = str.substr(i);
    m_tree[cur].AddWord(sub,info);
  } else {
    m_tree[cur].m_isLeaf = true;
    m_tree[cur].m_info.push_back(info);
  }
  return;
}

#define __Utf8TextScanner_Func template<class INFO>

__Utf8TrieTree_Func void Utf8TextScanner<INFO>::Find (std::string &txt, std::vector<KWInfo> *pkwinfos) {
  std::vector<KWInfo> &kwinfos = *pkwinfos;
  std::string str_found;
  vector<INFO> info_found;
  vector<INFO> info_tmp;
  std::string str_temp;
  size_t pos_found;
  std::vector<std::pair<std::string,size_t> > vecch;

  ganji::util::encoding::Utf8Conv::Utf8ToVector(txt, &vecch);

  Utf8TrieTree<INFO>  *kws = &m_kws;
  Utf8TrieTree<INFO>  *pkws = &m_kws;
  size_t vsize = vecch.size();
  for (size_t i = 0; i < vsize; ++i) {
    str_found="";
    str_temp="";
    for (size_t j = i; j < vsize; ++j) {
      int t = kws->Find(vecch[j].first, info_tmp,&pkws);
      kws=pkws;
      if (t < 0) {
        if (str_found.length() > 0) {
          KWInfo kwi;
          kwi.keyword = str_found;
          kwi.pos = pos_found;
          kwi.info = info_found;
          kwinfos.push_back(kwi); 
        }
        kws = &m_kws;
        break ;   
      } else {
        str_temp += vecch[j].first;
        if (t > 0) {
          pos_found = vecch[i].second;
          str_found = str_temp;
          info_found = info_tmp;
        }
        if (j == (vsize-1)) {
          if (str_found.length() > 0) {
            KWInfo kwi;
            kwi.keyword = str_found;
            kwi.pos = pos_found;
            kwi.info = info_found;
            kwinfos.push_back(kwi); 
          }
        }
      }
    }
  }
}
} } }   ///< end of namespace ganji::util::tree
#endif  ///< _GANJI_UTIL_TREE_UTF8_TRIETREE_H_
