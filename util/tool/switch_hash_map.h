/**
 * @Copyright (c) 2010 Ganji Inc.
 * @file    ganji/util/tool/switch_hash_map.h
 * @namespace ganji::util::tool
 * @version 1.0
 * @author  yangfenqiang
 * @date    2011-06-25
 *
 */

#ifndef _GANJI_UTIL_TOOL_SWITCH_HASH_MAP_H_
#define _GANJI_UTIL_TOOL_SWITCH_HASH_MAP_H_

#include <backward/hashtable.h>
#include <bits/concept_check.h>
#include <list>
#include <vector>
#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <assert.h>
#include "util/thread/rwlock_guard.h"

using __gnu_cxx::hash;
using __gnu_cxx::hashtable;
using std::equal_to;
using std::allocator;
using std::pair;
using std::_Select1st;
using std::list;
using std::vector;
using ganji::util::thread::RWLockRead;
using ganji::util::thread::RWLockWrite;

namespace ganji { namespace util { namespace tool {
#ifdef SWITCH_HASH_MAP_INNERLOCK
#define LOCKWRITE(rwlock_) RWLockWrite wlock(rwlock_)
#define LOCKREAD(rwlock_) RWLockRead rlock(rwlock_)
#else
#define LOCKWRITE(rwlock_)
#define LOCKREAD(rwlock_)
#endif

template<class _Key, class _Tp, class _HashFcn = hash<_Key>,
  class _EqualKey = equal_to<_Key>, class _Alloc = allocator<_Tp> >
class SwitchHashMap;

template<class _Key, class _Tp, class _HashFcn = hash<_Key>,
  class _EqualKey = equal_to<_Key>, class _Alloc = allocator<_Tp> >
class SwitchHashMapAttr {
 public:
  typedef SwitchHashMap<_Key, _Tp, _HashFcn, _EqualKey, _Alloc> SHM_;
  typedef typename SHM_::SlotIter SlotIter;

  void SetOwner(SHM_* hashmap) {
    hashmap_ = hashmap;
    rwlock_ = hashmap->GetLock();
  }

  virtual bool NeedSwitch() {
    return false;
  }

  virtual void BeforeSwitch(SlotIter& current) {}

  virtual void AfterSwitch(SlotIter& prev) {}

  virtual ~SwitchHashMapAttr() {}

 protected:
  SHM_* hashmap_;
  pthread_rwlock_t* rwlock_;
};


template<class _Key, class _Tp, class _HashFcn = hash<_Key>,
  class _EqualKey = equal_to<_Key>, class _Alloc = allocator<_Tp> >
class SwitchHashMapByTime : public SwitchHashMapAttr<_Key, _Tp, _HashFcn, _EqualKey, _Alloc> {
 public:
  typedef SwitchHashMap<_Key, _Tp, _HashFcn, _EqualKey, _Alloc> SHM_;
  typedef typename SHM_::SlotIter SlotIter;

  explicit SwitchHashMapByTime(time_t duration) {
    duration_ = duration;
    updateTime_ = time(NULL);
  }

  bool NeedSwitch() {
    time_t now = time(NULL);
    if (now - updateTime_ > duration_) {
      return true;
    }
    return false;
  }

  void BeforeSwitch(SlotIter& current) {
    SlotIter& prev = this->hashmap_->PrevSlot(current);
    {
      RWLockWrite wlock(this->rwlock_);
      prev->state_ = SHM_::ONPROC;
    }
    prev->hash_map_->clear();
    prev->state_ = SHM_::SPARE;
  }

  void AfterSwitch(SlotIter& prev) {
    updateTime_ = time(NULL);
  }

 private:
  time_t duration_;
  time_t updateTime_;
};

template<class _Key, class _Tp, class _HashFcn = hash<_Key>,
  class _EqualKey = equal_to<_Key>, class _Alloc = allocator<_Tp> >
class SwitchHashMapBySize : public SwitchHashMapAttr<_Key, _Tp, _HashFcn, _EqualKey, _Alloc> {
  typedef SwitchHashMap<_Key, _Tp, _HashFcn, _EqualKey, _Alloc> SHM_;
  typedef hashtable<pair<const _Key, _Tp>, _Key, _HashFcn,
  _Select1st<pair<const _Key, _Tp> >,
  _EqualKey, _Alloc> HashMap;
  typedef typename HashMap::iterator iterator;
  typedef typename SHM_::SlotIter SlotIter;
 public:
  explicit SwitchHashMapBySize(size_t size) {
    size_ = size;
  }

  bool NeedSwitch() {
    if (this->hashmap_->Size() > size_) {
      return true;
    }
    return false;
  }

  void BeforeSwitch(SlotIter& current) {
  }

  void AfterSwitch(SlotIter& prev) {
    {
      RWLockWrite wlock(this->rwlock_);
      prev->state_ = SHM_::ONPROC;
    }

    // do something
    prev->hash_map_->clear();
    prev->state_ = SHM_::SPARE;
  }

 private:
  size_t size_;
};

class SwitchHashMapBase {
 public:
  virtual bool NeedSwitch() = 0;
  virtual void Switch() = 0;
  virtual ~SwitchHashMapBase(){}

  int looptime_;
  static pthread_t shareThread_;
  static pthread_rwlock_t shareVecLock_;
  static vector<SwitchHashMapBase*> shareVec_;
};

template <class _Key, class _Tp, class _HashFcn, class _EqualKey,
    class _Alloc>
class SwitchHashMap : public SwitchHashMapBase{
 public:
  struct Slot;

  typedef hashtable<pair<const _Key, _Tp>,_Key, _HashFcn,
  _Select1st<pair<const _Key, _Tp> >,
  _EqualKey, _Alloc> HashMap;

  typedef SwitchHashMapAttr<_Key, _Tp, _HashFcn, _EqualKey, _Alloc> HashMapAttr;

  typedef _Tp data_type;
  typedef typename list<Slot>::iterator SlotIter;
  typedef typename HashMap::key_type key_type;
  typedef typename HashMap::value_type value_type;
  typedef typename HashMap::hasher hasher;
  typedef typename HashMap::key_equal key_equal;
  typedef typename HashMap::size_type size_type;
  typedef typename HashMap::iterator iterator;
  typedef typename HashMap::const_iterator const_iterator;
  typedef typename HashMap::allocator_type allocator_type;

  friend class SwitchHashMapAttr<_Key, _Tp, _HashFcn, _EqualKey, _Alloc>;

  enum State {
    ONUSE,
    ONPROC,
    SPARE
  };

  struct Slot {
    HashMap* hash_map_;
    State state_;

    Slot(size_type n, const hasher& hf, const key_equal& eql,
        const allocator_type& alloc) {
      hash_map_ = new(std::nothrow) HashMap(n, hf, eql, alloc);
      state_ = SPARE;
    }

    Slot() {}

    ~Slot() {
//     if (hash_map_ != NULL) {
//        delete hash_map_;
//        hash_map_ = NULL;
//      }
    }
  };

 public:
  SwitchHashMap(size_type n = 128, const hasher& hf = hasher(),
      const key_equal& eql = key_equal(),
      const allocator_type& alloc = allocator_type())
      : buckets_(n), hf_(hf), eql_(eql), alloc_(alloc), containers_() {
  }

  ~SwitchHashMap() {
    pthread_rwlock_destroy(rwlock_);
    if (rwlock_ != NULL) {
      delete rwlock_;
    }
  }

  // 该数据结构产生的所有线程及内存，依赖操作系统回收
  int Init(HashMapAttr* attr, int looptime = 2, bool threadShareMode = false) {
    rwlock_ = new(std::nothrow) pthread_rwlock_t;
    if (rwlock_ == NULL) {
      return -1;
    }
    int ret = pthread_rwlock_init(rwlock_, NULL);
    if (ret != 0) {
      return -4;
    }

    for (int i = 0; i < 2; ++i) {
      Slot* slot = new(std::nothrow) Slot(buckets_, hf_, eql_, alloc_);
      if (slot == NULL || slot->hash_map_ == NULL) {
        return -2;
      }
      containers_.push_back(*slot);
    }
    Mount(containers_.begin());

    this->attr_ = attr;
    this->attr_->SetOwner(this);
    this->looptime_ = looptime;

    if (!threadShareMode) {
      ret = pthread_create(&thread_, NULL, SwitchHashMap::ThreadFunc, this);
      if (ret != 0) {
        return -3;
      }
    } else {
      if (shareVec_.size() == 0) {
        ret = pthread_rwlock_init(&shareVecLock_, NULL);
        if (ret != 0) {
          return -4;
        }
        ret = pthread_create(&shareThread_, NULL, SwitchHashMap::ShareThreadFunc, this);
        if (ret != 0) {
          return -3;
        }
      }
      RWLockWrite wlock(&shareVecLock_);
      shareVec_.push_back(this);
      assert(shareVec_.size() > 0);
    }
    return 0;
  }

  // faster, but unsafe method
  _Tp& Insert(const key_type& key) {
    return current_->hash_map_->find_or_insert(value_type(key, _Tp())).second;
  }

  void Add(const key_type& key, const data_type& value) {
    LOCKWRITE(rwlock_);
    current_->hash_map_->find_or_insert(value_type(key, value)).second = value;
  }

  size_t Remove(key_type key) {
    LOCKWRITE(rwlock_);
    return current_->hash_map_->erase(key);
  }

  void Remove(iterator iter) {
    LOCKWRITE(rwlock_);
    current_->hash_map_->erase(iter);
  }

  iterator
  Find(const key_type& key) {
    LOCKREAD(rwlock_);
    const iterator& iter = current_->hash_map_->find(key);
    if (iter == current_->hash_map_->end()){
      return End();
    }
    return iter;
  }

  const_iterator
  Find(const key_type& key) const {
    LOCKREAD(rwlock_);
    const const_iterator& iter = current_->hash_map_->find(key);
    if (iter == current_->hash_map_->end()){
      return End();
    }
    return iter;
  }

  iterator
  Search(const key_type& key) {
    SlotIter slot = current_;
    iterator iter;
    size_t size = containers_.size();
    LOCKREAD(rwlock_);
    while (slot->state_ == ONUSE && size-- > 0) {
      iter = slot->hash_map_->find(key);
      if (iter != slot->hash_map_->end()) {
        return iter;
      }
      PrevSlot(slot);
    }
    return End();
  }

  iterator
  End() {
    return containers_.begin()->hash_map_->end();
  }

  size_t Size() {
    return current_->hash_map_->size();
  }

  size_t TotalSize() {
    SlotIter iter = current_;
    size_t size = 0;
    for (size_t i = 0; i < containers_.size(); ++i) {
      size += iter->hash_map_->size();
      NextSlot(iter);
    }
    return size;
  }

 public:
  pthread_rwlock_t* GetLock(){
    return rwlock_;
  }

  const SlotIter& getCurrentSlot() {
    return current_;
  }

  SlotIter& NextSlot(SlotIter& iter) {
    if (++iter == containers_.end()) {
      iter = containers_.begin();
    }
    return iter;
  }

  SlotIter& PrevSlot(SlotIter& iter) {
    if (iter == containers_.begin()) {
      iter = containers_.end();
    }
    return --iter;
  }

 private:
  static void* ThreadFunc(void* param) {
    SwitchHashMap* obj = (SwitchHashMap*)param;
    while (1) {
      if (obj->attr_->NeedSwitch()) {
        obj->Switch();
      }
      sleep(obj->looptime_);
    }
    return NULL;
  }

  static void* ShareThreadFunc(void* param) {
    vector<SwitchHashMapBase*>::const_iterator iter = shareVec_.begin();
    int looptime = (*iter)->looptime_;
    while (1) {
      {
        RWLockRead rlock(&shareVecLock_);
        iter = shareVec_.begin();
        for (; iter != shareVec_.end(); ++iter) {
          if ((*iter)->NeedSwitch()) {
            (*iter)->Switch();
          }
        }
      }
      sleep(looptime);
    }
    return NULL;
  }

  bool NeedSwitch() {
    return attr_->NeedSwitch();
  }

  void Switch() {
    SlotIter iter = current_;
    attr_->BeforeSwitch(iter);
    SlotIter prev = DoSwitch();
    attr_->AfterSwitch(prev);
  }

  SlotIter DoSwitch() {
    SlotIter iter = current_;
    NextSlot(iter);

    SlotIter prev = current_;
    if (iter->state_ != SPARE) {
      Slot* slot = new(std::nothrow) Slot(buckets_, hf_, eql_, alloc_);
      slot->state_ = ONUSE;
      Mount(containers_.insert(iter, *slot));
    } else {
      Mount(iter);
    }
    return prev;
  }

  void Mount(const SlotIter& iter) {
    RWLockWrite wlock(rwlock_);
    current_ = iter;
    current_->state_ = ONUSE;
  }

 private:
  size_type buckets_;
  hasher hf_;
  key_equal eql_;
  allocator_type alloc_;

  list<Slot> containers_;
  SlotIter current_;

  pthread_t thread_;
  pthread_rwlock_t* rwlock_;

  HashMapAttr* attr_;
};
}}}
#endif  ///< _GANJI_UTIL_TOOL_SWITCH_HASH_MAP_H_
