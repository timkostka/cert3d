#include "gsl_includes.h"

// An GSL_CODELOCK object provides the ability to atomically access a flag
// to lock access to an object or given portion of code.

// timeout delay while waiting for a lock
// If a forced lock cannot be obtained within this time, execution halts with an
// error message.  Set to 0 to disable.
//const uint16_t gsl_codelock_timeout_ms = 2000;

struct GSL_CODELOCK {

  // 0 if unlocked, 1 if locked
  volatile uint8_t locked;

  // number of times this has been locked
  //uint32_t lock_count;

  // number of failed locks
  //uint32_t failed_lock_count;

  // initializer
  GSL_CODELOCK(void) : locked(0) {
  }

  // attempt to lock, return true if successful
  /*
  bool Lock(void) {
    // try to obtain a lock as long as it's not locked
    // if it's already locked, just return
    if (__LDREXB(&locked) != 0) {
      //++failed_lock_count;
      return false;
    }
    // try to lock it
    // Note: if an interrupt accesses locked between __LDREXB and __STREXB,
    // this will fail
    if (__STREXB(1, &locked) != 0) {
      //++failed_lock_count;
      return false;
    }
    //ASSERT_EQ(locked, 1);
    //++lock_count;
    //__DMB();
    return true;
  }*/

  // attempt to lock, return true if successful
  bool Lock(void) {
    // keep trying to get a lock as long as it's unlocked
    while (__LDREXB(&locked) == 0) {
      // try to lock it
      // Note: If an interrupt accesses locked between __LDREXB and __STREXB,
      // this will fail.  Because of this, these function can act as a mutex.
      if (__STREXB(1, &locked) == 0) {
        return true;
      }
      //++failed_lock_count;
    }
    return false;
  }

  // try to lock continuously and return when successful
  // warning: calling this within an interrupt is generally unsafe as
  // the program can lock if the main thread currently has the code lock
  void WaitAndLock(void) {
    //uint32_t start = GSL_DEL_Ticks();
    while (!Lock()) {
      //if (gsl_codelock_timeout_ms &&
      //    GSL_DEL_ElapsedMS(start) > gsl_codelock_timeout_ms) {
      //  HALT("Code lock timeout.");
      //}
    }
  }

  // unlock
  void Unlock(void) {
    ASSERT(locked);
    //__DMB();
    locked = 0;
  }
};

// try to lock
struct GSL_TRYCODELOCK {
  // pointer to code block
  GSL_CODELOCK * const lock;
  // true if lock was successful
  bool locked;
  // initialize
  GSL_TRYCODELOCK(GSL_CODELOCK & lock) : lock(&lock) {
    locked = lock.Lock();
  }
  // de-initialize
  ~GSL_TRYCODELOCK(void) {
    if (locked) {
      lock->Unlock();
    }
  }
  // conversion to bool
  operator bool(void) const {
    return locked;
  }
};

// force a lock
struct GSL_FORCECODELOCK {
  // pointer to code block
  GSL_CODELOCK * const lock;
  // initialize
  GSL_FORCECODELOCK(GSL_CODELOCK & lock) : lock(&lock) {
    lock.WaitAndLock();
  }
  // de-initialize
  ~GSL_FORCECODELOCK(void) {
    lock->Unlock();
  }
};

// the following code is simply a check that this works as expected

/*

// code lock
GSL_CODELOCK lock;

uint32_t value;

void call1(void) {
  if (!lock.Lock()) {
    return;
  }
  value += 10;
  GSL_DEL_US(10);
  value -= 10;
  lock.Unlock();
}


void call2(void) {
  if (!lock.Lock()) {
    return;
  }
  value += 100;
  GSL_DEL_MS(1);
  value -= 100;
  lock.Unlock();
}


void call3(void) {
  if (!lock.Lock()) {
    return;
  }
  value += 1000;
  GSL_DEL_MS(5);
  value -= 1000;
  lock.Unlock();
}

void CheckLock(void) {
  // set up automatic calls
  GSL_TIM_SetFrequency(TIM1, 50000, 0.1f);
  GSL_TIM_SetUpdateCallback(TIM1, call1, 3);
  GSL_TIM_SetFrequency(TIM3, 200.0f + PI, 0.1f);
  GSL_TIM_SetUpdateCallback(TIM3, call1, 2);
  GSL_TIM_SetFrequency(TIM4, 51.2f + 1.0f / 3.0f, 0.1f);
  GSL_TIM_SetUpdateCallback(TIM4, call1, 1);
  GSL_TIM_Start(TIM1);
  GSL_TIM_Start(TIM3);
  GSL_TIM_Start(TIM4);

  while (1) {
    GSL_DEL_MS(1000);
    //LOG("\nlock_count=", lock.lock_count);
    //LOG(", failed_lock_count=", lock.failed_lock_count);
    lock.WaitAndLock();
    ASSERT_EQ(value, 0);
    lock.Unlock();
  }
}

*/
