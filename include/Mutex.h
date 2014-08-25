// ***********************************************************************
// Filename         : Mutex.h
// Author           : LIZHENG
// Created          : 2014-05-16
// Description      : ��������Windows��Linuxƽ̨�µķ�װ
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-08-25
// 
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_MUTEX_H
#define ZL_MUTEX_H
#include "OsDefine.h"
#include <exception>
#ifdef OS_WINDOWS
#define OS_WINDOWS
#include <Windows.h>
#elif defined(OS_LINUX)
#define OS_LINUX
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#endif

namespace ZL
{
	#define DISALLOW_COPY_AND_ASSIGN(TypeName)         \
				 TypeName(const TypeName&);            \
                 TypeName& operator=(const TypeName&)  \

	class Mutex
	{
		DISALLOW_COPY_AND_ASSIGN(Mutex);
	public:
		Mutex() { Init_(); }
		~Mutex() { Close_(); }

	public:
		void Lock()
		{
		#ifdef	OS_WINDOWS
			EnterCriticalSection(&mutex_);
		#elif defined(OS_LINUX)
			if (pthread_mutex_lock(&mutex_) != 0)
			{
				throw std::exception();
			}
		#endif
		}

		bool TryLock() //const
		{
		#ifdef	OS_WINDOWS
			#if(_WIN32_WINNT >= 0x0400)
				return TryEnterCriticalSection(&mutex_) != 0;
			#else
				return false;
			#endif
		#elif defined(OS_LINUX)
			int ret = pthread_mutex_trylock(&mutex_);
			if (ret != 0)
			{
				printf("pthread_mutex_trylock fail,code = %d\n", ret);
				return false;
			}
			return true;
		#endif
		}

		void Unlock()
		{
		#ifdef	OS_WINDOWS
			LeaveCriticalSection(&mutex_);
		#elif defined(OS_LINUX)
			if (pthread_mutex_unlock(&mutex_) != 0)
			{
				printf("pthread_mutex_unlock\n");
			}
		#endif
		}

		#ifdef	OS_WINDOWS
		CRITICAL_SECTION*	GetMutex()
		#elif defined(OS_LINUX)
		pthread_mutex_t*	GetMutex()
		#endif
		{
			return &mutex_;
		}

	private:
		void Init_()
		{
		#ifdef	OS_WINDOWS
			#if (_WIN32_WINNT >= 0x0403)
				if (!InitializeCriticalSectionAndSpinCount(&mutex_, 0x80000400))
			#endif /* _WIN32_WINNT >= 0x0403 */
					InitializeCriticalSection(&mutex_);
		#elif defined(OS_LINUX)
			pthread_mutex_init(&mutex_, NULL);
		#endif
		}

		void Close_()
		{
		#ifdef	OS_WINDOWS
			DeleteCriticalSection(&mutex_);
		#elif defined(OS_LINUX)
			pthread_mutex_destroy(&mutex_);
		#endif
		}

	private:
	#ifdef	OS_WINDOWS
		mutable CRITICAL_SECTION mutex_;
	#elif defined(OS_LINUX)
		pthread_mutex_t mutex_;
	#endif
	};

	class MutexGuard
	{
		DISALLOW_COPY_AND_ASSIGN(MutexGuard);
	public:
		explicit MutexGuard(Mutex & mutex) :mutex_(mutex) { mutex_.Lock(); }
		~MutexGuard() { mutex_.Unlock(); }
	private:
		mutable Mutex& mutex_;
	};

	class RWMutex
	{
		DISALLOW_COPY_AND_ASSIGN(RWMutex);
	public:
		RWMutex() { Init_(); }
		~RWMutex() { Close_(); }
	public:
		bool ReadLock()
		{
		#ifdef	OS_WINDOWS
			::AcquireSRWLockShared(&rwlock_);
			return true;
		#elif defined(OS_LINUX)
			return pthread_rwlock_rdlock(&rwlock_) == 0;
		#endif
		}
		bool ReadUnLock()
		{
		#ifdef	OS_WINDOWS
			::ReleaseSRWLockShared(&rwlock_);
			return true;
		#elif defined(OS_LINUX)
			return pthread_rwlock_unlock(&rwlock_) == 0;
		#endif
		}
		bool WriteLock()
		{
		#ifdef	OS_WINDOWS
			::AcquireSRWLockExclusive(&rwlock_);
			return true;
		#elif defined(OS_LINUX)
			return pthread_rwlock_wrlock(&rwlock_) == 0;
		#endif
		}
		bool WriteUnLock()
		{
		#ifdef	OS_WINDOWS
			::ReleaseSRWLockExclusive(&rwlock_);
					return true;
		#elif defined(OS_LINUX)
			return pthread_rwlock_unlock(&rwlock_) == 0;
		#endif
		}
		bool TryReadLock()
		{
		#ifdef	OS_WINDOWS
			return ::TryAcquireSRWLockShared(&rwlock_) == TRUE;
		#elif defined(OS_LINUX)
			return pthread_rwlock_tryrdlock(&rwlock_) == 0;
		#endif
		}
		bool TryWriteLock()
		{
		#ifdef	OS_WINDOWS
			return ::TryAcquireSRWLockShared(&rwlock_) == TRUE;
		#elif defined(OS_LINUX)
			return pthread_rwlock_trywrlock(&rwlock_) == 0;
		#endif
		}

	private:
		void Init_()
		{
		#ifdef	OS_WINDOWS
			::InitializeSRWLock(&rwlock_);
		#elif defined(OS_LINUX)
			pthread_rwlock_init(&rwlock_, NULL);
		#endif
		}

		void Close_()
		{
		#ifdef	OS_WINDOWS
			//nothing
		#elif defined(OS_LINUX)
			pthread_rwlock_destroy(&rwlock_);
		#endif
		}

	private:
	#ifdef	OS_WINDOWS
		SRWLOCK          rwlock_;  //not support Windows XP
	#elif defined(OS_LINUX)
		pthread_rwlock_t rwlock_;
	#endif
	};

} /* namespace ZL */

#endif  /* ZL_MUTEX_H */