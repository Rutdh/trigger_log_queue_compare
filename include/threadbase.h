// thread.h: interface for the CThread class .
//
//////////////////////////////////////////////////////////////////////
#ifndef __THREAD_H__
#define __THREAD_H__

#ifdef WIN32
    #define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	typedef HANDLE THREAD_HANDLE;
	typedef DWORD  THREAD_ID;
    #pragma warning(disable: 4786)
#else
	#include <pthread.h>
	typedef pthread_t THREAD_HANDLE;
	typedef pthread_t THREAD_ID;
#endif

//线程对象基类
class CThreadBase
{
public:
	CThreadBase(const char * name = "ThreadBase");
	virtual ~CThreadBase();

	void Terminate();
	bool WaitFor(int seconds=-1);
	THREAD_HANDLE GetHandle() { return m_hThread; }
    const char * GetName(){return m_name;}
protected:
	volatile bool m_Terminated;
	void Delay(int seconds);

private:
	THREAD_HANDLE m_hThread;
	THREAD_ID     m_thread_id;
	
    char m_name[32];

public:
	bool m_Finished;

	//zhb,2019.07.11
	bool m_Done;
	bool Launch();
	virtual bool InitInstance() { return true; }
	virtual void Run()=0;
	virtual void ExitInstance() {}
    bool Join();
	THREAD_ID GetThreadId(void);
	//停止任务（退出线程）
	void Stop(void);
	//是否已经停止
	bool IsStopped(void);
};


//临界锁
class CSynchro
{
public:
	CSynchro();
	~CSynchro();
	void Lock();
	void UnLock();
private:
#ifdef WIN32
	CRITICAL_SECTION m_Section;
#else
	pthread_mutex_t m_lock;
#endif
};

//互斥锁
class   CSyncMutex {
public:
	CSyncMutex()
	{
#ifdef  WIN32
		m_lock=CreateMutex(NULL, FALSE, NULL);
#else
		pthread_mutexattr_t mattr;
		pthread_mutexattr_init(&mattr);
		pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_RECURSIVE );
		pthread_mutex_init(&m_lock, &mattr);
		pthread_mutexattr_destroy(&mattr);
#endif
	}

	~CSyncMutex()
	{
#ifdef  WIN32
		CloseHandle(m_lock);
#else
		pthread_mutex_destroy(&m_lock);
#endif
	}

	void Lock()
	{
#ifdef  WIN32
		WaitForSingleObject(m_lock, INFINITE);
#else
		pthread_mutex_lock(&m_lock);
#endif
	}

	bool TryLock()
	{
#ifdef  WIN32
		return (WaitForSingleObject(m_lock, 0) == WAIT_OBJECT_0);
#else
		return (pthread_mutex_trylock(&m_lock) == 0);
#endif
	}

	void UnLock()
	{
#ifdef  WIN32
		ReleaseMutex(m_lock);
#else
		pthread_mutex_unlock(&m_lock);
#endif
	}	
private:
#ifdef WIN32
	HANDLE  m_lock;
#else
	pthread_mutex_t m_lock;
#endif
};

template<class T>
class CSyncGuard
{
public:
    CSyncGuard(T &mtx) : m_mtx(mtx) {
        m_mtx.Lock();
    };
    
    ~CSyncGuard() {
        m_mtx.UnLock();
    };
private:
    T &m_mtx;
};

//////////////////////////////////////////////////////////////////////
//公共函数

//取当前线程的句柄
THREAD_HANDLE GetCurrentHandle();

//暂停
void OSSleep(int ms);

#endif
