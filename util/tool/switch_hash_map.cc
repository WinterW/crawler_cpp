/**
 * @Copyright (c) 2010 Ganji Inc.
 * @file    ganji/util/tool/switch_hash_map.h
 * @namespace ganji::util::tool
 * @version 1.0
 * @author  yangfenqiang
 * @date    2011-06-25
 *
 */

#include "switch_hash_map.h"
using ganji::util::tool::SwitchHashMapBase;

pthread_t SwitchHashMapBase::shareThread_;
vector<SwitchHashMapBase*> SwitchHashMapBase::shareVec_;
pthread_rwlock_t SwitchHashMapBase::shareVecLock_;


