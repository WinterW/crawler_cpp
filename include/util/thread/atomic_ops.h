/**
 * @Copyright 2011 Ganji Inc.
 * @file    src/ganji/util/tool/atomic_ops.h
 * @version 1.0
 * @author  lixinjun
 * @date    2011-02-12
 *
 * @brief   atomic operations support
 * Change Log:
 *
 */
#ifndef _GANJI_UTIL_TOOL_ATOMIC_OPS_H_
#define _GANJI_UTIL_TOOL_ATOMIC_OPS_H_

typedef volatile int atomic_t;

static int atomic_inc(atomic_t *v) {
  return __sync_add_and_fetch((int *)v, 1);
}

static int atomic_dec(atomic_t *v) {
  return __sync_sub_and_fetch((int *)v, 1);
}

static int atomic_add(atomic_t *v, int value) {
  return __sync_add_and_fetch((int *)v, value);
}

static int atomic_sub(atomic_t *v, int value) {
  return __sync_sub_and_fetch((int *)v, value);
}
#endif  // _GANJI_UTIL_TOOL_ATOMIC_OPS_H_

