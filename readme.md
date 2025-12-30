# ks queue
金仕达的队列分为阻塞和非阻塞两种. 其核心区别是如下的代码: 
1. pop_impl的sem_wait注释, 启用sem_trywait
2. push_impl的sem_wait注释, 启用sem_trywait

底层基于原子变量实现, 涉及到了内存序的问题. 相关名词: CAS, ABA问题

