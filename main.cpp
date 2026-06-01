#include "pch.h"
#include <iostream>
#include "Connection.h"
#include"CommonConnectionPool.h"
using namespace std;

int main()
{
	clock_t begin = clock();
	ConnectionPool* cp = ConnectionPool::getConnectionPool();
	//for (int i = 0;i < 10000;i++)
	//{
	//	// //不带连接池性能测试
	//	//Connection conn;
	//	//char sql[1204] = { 0 };
	//	//sprintf(sql, "insert into user (name,age,sex) values('%s',%d,'%s')", "tdq", 26, "male");
	//	//conn.connect("127.0.0.1", 3306, "root", "111111", "user");
	//	//conn.update(sql);

	//	//使用连接池压力测试
	//	/*shared_ptr<Connection> sp = cp->getConnection();
	//	char sql[1204] = { 0 };
	//	sprintf(sql, "insert into user (name,age,sex) values('%s',%d,'%s')", "tdq", 26, "male");
	//	sp->update(sql);*/
	//}

	//多线程测试
	thread t1([]() {
		ConnectionPool* cp = ConnectionPool::getConnectionPool();
		for (int i = 0;i < 2500;i++)
		{
			char sql[1204] = { 0 };
			sprintf(sql, "insert into user (name,age,sex) values('%s',%d,'%s')", "tdq", 26, "male");
			shared_ptr<Connection> sp = cp->getConnection();
			sp->update(sql);
		}
	});
	thread t2([]() {
		ConnectionPool* cp = ConnectionPool::getConnectionPool();
		for (int i = 0;i < 2500;i++)
		{
			char sql[1204] = { 0 };
			sprintf(sql, "insert into user (name,age,sex) values('%s',%d,'%s')", "tdq", 26, "male");
			shared_ptr<Connection> sp = cp->getConnection();
			sp->update(sql);
		}
		});
	thread t3([]() {
		ConnectionPool* cp = ConnectionPool::getConnectionPool();
		for (int i = 0;i < 2500;i++)
		{
			char sql[1204] = { 0 };
			sprintf(sql, "insert into user (name,age,sex) values('%s',%d,'%s')", "tdq", 26, "male");
			shared_ptr<Connection> sp = cp->getConnection();
			sp->update(sql);
		}
		});
	thread t4([]() {
		ConnectionPool* cp = ConnectionPool::getConnectionPool();
		for (int i = 0;i < 2500;i++)
		{
			char sql[1204] = { 0 };
			sprintf(sql, "insert into user (name,age,sex) values('%s',%d,'%s')", "tdq", 26, "male");
			shared_ptr<Connection> sp = cp->getConnection();
			sp->update(sql);
		}
		});

	t1.join();
	t2.join();
	t3.join();
	t4.join();
	clock_t end = clock();
	cout << (end - begin) << "ms" << endl;
	return 0;
} 