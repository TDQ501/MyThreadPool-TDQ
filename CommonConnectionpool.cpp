// MySQLConnectionpool.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。

#include <iostream>
#include <fstream>
#include "pch.h"
#include "public.h"
#include "CommonConnectionPool.h"


ConnectionPool* ConnectionPool::getConnectionPool()
{
	static ConnectionPool pool;
	return &pool;
}
shared_ptr<Connection>ConnectionPool::getConnection()
{
	unique_lock<mutex> lock(_queueMutex);
	while (_connectionQue.empty())
	{
		if (cv_status::timeout == cv.wait_for(lock, chrono::milliseconds(_connectionTimeout)))
		{
			if (_connectionQue.empty())
			{
				LOG("获取连接超时了...获取连接失败");
				return nullptr;
			}
		}
	}

	shared_ptr<Connection> sp(_connectionQue.front(),[&](Connection* pcon)
		 {
			unique_lock<mutex> lock(_queueMutex);
			pcon->refreshAliveTime();
			_connectionQue.push(pcon);
		});
	_connectionQue.pop();
	cv.notify_all();
	return sp;
}



ConnectionPool::ConnectionPool()
{
	if (!loadConfigFile())
	{
		return;
	}

	for (int i = 0;i < _initSize;i++)
	{
		Connection* conn = new Connection();
		conn->connect(_ip, _port, _username, _password, _dbname);
		conn->refreshAliveTime();
		_connectionQue.push(conn);
		_connectionCnt++;
	}

	
	thread produce(std::bind(&ConnectionPool::produceConnectionTask, this));
	produce.detach();

	
	thread scanner(std::bind(&ConnectionPool::scannerConnectionTask, this));
	scanner.detach();
}


void  ConnectionPool::produceConnectionTask()
{
	for (;;)
	{
		unique_lock<mutex> lock(_queueMutex);
		while (!_connectionQue.empty())
		{
			cv.wait(lock);
		}

		
		if (_connectionCnt < _maxSize)
		{
			Connection* conn = new Connection();
			conn->connect(_ip, _port, _username, _password, _dbname);
			conn->refreshAliveTime();
			_connectionQue.push(conn);
			_connectionCnt++;
		}

		
		cv.notify_all();
	}
}

void ConnectionPool::scannerConnectionTask()
{
	for (;;)
	{
		this_thread::sleep_for(chrono::seconds(_maxIdleTime));

		unique_lock<mutex> lock(_queueMutex);
		while (_connectionCnt >= _initSize)
		{
			Connection* p = _connectionQue.front();
			if (p->getAliveTime() >= _maxIdleTime * 1000)
			{
				_connectionQue.pop();
				_connectionCnt--;
				delete p;
			}
			else
			{
				break;
			}
		}
	}
}


bool ConnectionPool::loadConfigFile()
{
	std::ifstream ifs("mysql.ini");
	if (!ifs.is_open())
	{
		LOG("mysql.ini file does not exist or cannot be opened");
		return false;
	}

	string line;
	while (getline(ifs,line))
	{
		string str = line;
		int index = str.find("=", 0);
		if (index == -1)
		{
			continue;
		}
		string key = str.substr(0, index);
		string value = str.substr(index + 1, str.size() - index - 1);

		if (key == "ip")
		{
			_ip = value;
		}
		else if (key == "port")
		{
			_port=static_cast<unsigned short>(std::stoi(value));
		}
		else if (key == "username")
		{
			_username = value;
		}
		else if (key == "password")
		{
			_password = value;
		}
		else if (key == "dbname")
		{
			_dbname = value;
		}
		else if (key == "initSize")
		{
			_initSize = std::stoi(value);
		}
		else if (key == "maxSize")
		{
			_maxSize =std::stoi(value);
		}
		else if (key == "maxIdleTime")
		{
			_maxIdleTime = std::stoi(value);
		}
		else if (key == "connectionTimeOut")
		{
			_connectionTimeout = std::stoi(value);
		}
	}
	return true;
}
