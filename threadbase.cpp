// Thread.cpp: implementation of the CThread class.
//
//////////////////////////////////////////////////////////////////////
#include "threadbase.h"
#include <string.h>
#include <time.h>
#ifndef WIN32
	#include <unistd.h>
    #include <sys/time.h>
    #include <pthread.h>
    #include <errno.h>
#endif


//////////////////////////////////////////////////////////////////////
//线程基类

#ifdef WIN32
DWORD WINAPI _ThreadExecuteEntry(LPVOID pParam)
#else
void * _ThreadExecuteEntry(void *pParam)
#endif
{
	CThreadBase *pThread = (CThreadBase *)pParam;
	if(pThread->InitInstance())
		pThread->Run();
	pThread->ExitInstance();
	pThread->m_Finished = true;

	//zhb,2019.07.11
	pThread->m_Done = false;

	
#ifdef WIN32
	return 0;
#else
	return (void*)0;
#endif
}

CThreadBase::CThreadBase(const char * name)
{
	m_hThread = 0;

	//2019.8.1, zhb
	m_Finished = true;
	m_Terminated = false;
	m_Done = false;
	m_thread_id = 0;
    strncpy(this->m_name, name, sizeof(this->m_name));
    this->m_name[sizeof(this->m_name) - 1] = 0;
}


void CThreadBase::Stop(void)
{
	m_Terminated = true;
	if((THREAD_HANDLE)0 != GetHandle() && !m_Finished)
	{
		Join();
	}

	//zhb, 2019.07.11
	m_Terminated = false;
}

bool CThreadBase::IsStopped(void)
{
	return m_Finished;
}

THREAD_ID CThreadBase::GetThreadId(void)
{
	return m_thread_id;
}

bool CThreadBase::Launch()
{
	if (m_Done) return false;

#ifdef WIN32
	DWORD ThreadId;
	m_hThread = ::CreateThread(NULL,0,_ThreadExecuteEntry,this,0,&ThreadId);
    m_thread_id = ThreadId;
	if (m_hThread != NULL) {
		m_Done = true;

		//zhb, 2019.07.11
		m_Finished = false;
	}
#else
	pthread_attr_t attr;
	pthread_attr_init(&attr);
#ifdef HP_UX
	pthread_attr_setstacksize(&attr,1024000);
#endif
	m_Done = (::pthread_create(&m_hThread,&attr,&_ThreadExecuteEntry,this)==0);
	m_thread_id = m_hThread;

	//zhb, 2019.07.11
	if (m_Done == true)
		m_Finished = false;
#endif

	//if (!m_Done) throw "Create a thread failure!";
	return m_Done;
}

CThreadBase::~CThreadBase()
{
	Stop();
#ifdef WIN32
	if (m_hThread!=0) ::CloseHandle(m_hThread);
#endif
}

bool CThreadBase::WaitFor(int seconds)
{
	if (!m_Done) return true;

	if (seconds > 0) {
		time_t t0,t1;
		time(&t0);
		while (!m_Finished) {
			OSSleep(100);
			time(&t1);
			if (t1-t0 >= seconds) break;
		}
	}
	else if (seconds < 0) {
		while (!m_Finished) OSSleep(100);
	}
	return m_Finished;
}

void CThreadBase::Terminate()
{
	m_Terminated = true;
}

void CThreadBase::Delay(int seconds)
{
	time_t t0,t1;
	time(&t0);
	while (!m_Terminated) {
		OSSleep(100);
		time(&t1);
		if (t1-t0 >= seconds) break;
	}
}

bool CThreadBase::Join()
{
#ifdef WIN32
    return (WaitForSingleObject(GetHandle(),INFINITE) != 0);
#else
    return (pthread_join(GetHandle(), NULL) == 0);
#endif
}

//////////////////////////////////////////////////////////////////////
//临界锁

CSynchro::CSynchro()
{
#ifdef WIN32
	InitializeCriticalSection(&m_Section);
#else
	pthread_mutex_init(&m_lock,NULL);
#endif
}

CSynchro::~CSynchro()
{
#ifdef WIN32
	DeleteCriticalSection(&m_Section);
#else
	pthread_mutex_destroy(&m_lock);
#endif
}

void CSynchro::Lock()
{
#ifdef WIN32
	EnterCriticalSection(&m_Section);
#else
	pthread_mutex_lock(&m_lock);
#endif
}

void CSynchro::UnLock()
{
#ifdef WIN32
	LeaveCriticalSection(&m_Section);
#else
	pthread_mutex_unlock(&m_lock);
#endif
}

//////////////////////////////////////////////////////////////////////
//公共函数

void OSSleep(int milliseconds)
{
#ifdef WIN32
    ::Sleep(milliseconds);
#else
    timespec req, rem;
    req.tv_sec = milliseconds / 1000;
    req.tv_nsec = (milliseconds % 1000) * 1000000;
    
    int iResult = nanosleep(&req, &rem);
    // In Linux 2.4, if nanosleep() is stopped by a signal (e.g., SIGTSTP), 
    // then the call fails with the error EINTR after the process is resumed by  a  SIGCONT
    // signal. If the system call is subsequently restarted, 
    // then the time that the process spent in the stopped state is not counted against the sleep inter-val.
    while( iResult == -1 && errno == EINTR )
    {
        req.tv_sec = rem.tv_sec;
        req.tv_nsec = rem.tv_nsec;
        nanosleep(&req, &rem);
    }
#endif
}

THREAD_HANDLE GetCurrentHandle()
{
#ifdef WIN32
	return ::GetCurrentThread();
#else
	return ::pthread_self();
#endif
}
