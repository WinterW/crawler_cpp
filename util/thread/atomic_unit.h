/**
* @Copyright 2011 Ganji Inc.
* @file    ganji/util/thread/atomic_unit.h
* @namespace ganji::util::thread
* @version 1.0
* @author  lihaifeng
* @date    2011-07-18
*
* 改动程序后， 请使用tools/cpplint/cpplint.py 检查代码是否符合编码规范!
* 遵循的编码规范请参考: http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
* Change Log:
*/

#ifndef _GANJI_UTIL_ATOMIC_UTIL_H_
#define _GANJI_UTIL_ATOMIC_UTIL_H_

#define LOCK_PREFIX \
  ".section .smp_locks,\"a\"\n" \
  "  .align 8\n"  \
  "  .quad 661f\n" /* address */  \
  ".previous\n" \
  "661:\n\tlock; "

typedef struct { volatile int counter; } atomic_t;
#define ATOMIC_INIT(i)  { (i) }
#define atomic_read(v)  ((v)->counter)
#define atomic_set(v,i) (((v)->counter) = (i))

static __inline__ void atomic_sub(int i, atomic_t *v)
{       
  __asm__ __volatile__(
  LOCK_PREFIX "subl %1,%0"
  :"=m" (v->counter)
  :"ir" (i), "m" (v->counter));
}

static __inline__ int atomic_add_return(int i, atomic_t *v)
{
  int __i = i;
  __asm__ __volatile__(
  LOCK_PREFIX "xaddl %0, %1;"
  :"=r"(i)
  :"m"(v->counter), "0"(i));
  return i + __i;
}


#endif ///< _GANJI_UTIL_ATOMIC_UTIL_H_
