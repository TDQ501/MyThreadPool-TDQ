#pragma once
#include <string>
#include <queue>
#include <mutex>
#include <atomic>
#include <thread>
#include <condition_variable>
#include <memory>
#include <functional>
#include "Connection.h"
using namespace std;

/*
连接池功能模块
*/

class ConnectionPool
{
public:
	static ConnectionPool* getConnectionPool();
	shared_ptr<Connection> getConnection();

private:
	//单例# 把构造函数私有化（保证所有的实例拿到的是同一个单例对象）
	ConnectionPool();

	//从配置文件中加载配置
	bool loadConfigFile();

	//运行在独立的线程中，专门负责生产新连接
	void produceConnectionTask();

	//一个新的线程用于扫描超过maxIdleTime时间的空闲时间，对链接进行回收
	void scannerConnectionTask();

	string _ip; //mysql的IP地址
	unsigned short _port;  //unsigned short没有符号位 
	string _username;//mysql的用户名
	string _password;//mysql登录密码
	string _dbname;//数据库名称（user）
	int _initSize;//连接池的初始连接量
	int _maxSize;//连接池的最大连接量
	int _maxIdleTime;//连接池最大空闲时间
	int _connectionTimeout;//连接池获取连接的超时时间
	atomic_int _connectionCnt;//记录所创建的connection连接的总量

	queue<Connection*> _connectionQue; //存储MySQL链接的队列
	mutex _queueMutex;//维护线程安全的互斥锁
	condition_variable cv;//设置条件变量，用于连接生产线程和连接消费线程的通信
};
