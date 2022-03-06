# ThreadPoolDesign

Difference thread pool design implement in C++

实现 1
比较奇怪的一个设计，线程池中存在多个线程本地队列和一个全局队列，全局任务会被放入全局队列，而进程本地队列会采用轮转的方式来进行任务的分派。
