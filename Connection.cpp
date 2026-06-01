#include "pch.h"
#include "public.h"
#include "Connection.h"


/*
实现数据库的操作:既定操作熟悉用法
*/
Connection::Connection()
    {
        //初始化数据链接
        _conn = mysql_init(nullptr);
    }

    // 释放数据库连接资源
Connection::~Connection()
    {
        //释放数据库链接资源
        if (_conn != nullptr)
            mysql_close(_conn);
    }

    // 连接数据库 传入的参数分别为：IP地址 端口号 用户名 密码 数据库名称
bool Connection::connect(string ip, unsigned short port, string user, string password,string dbname)
    {
        MYSQL* p = mysql_real_connect(_conn, ip.c_str(), user.c_str(),
            password.c_str(), dbname.c_str(), port, nullptr, 0);
        return p != nullptr;
    }

    // 更新操作 insert、delete、update sql来接受
bool Connection::update(string sql)
    {
        if (mysql_query(_conn, sql.c_str()))
        {
            LOG("更新失败:" + sql);
            return false;
        }
        return true;
    }

    // 查询操作 select
MYSQL_RES* Connection::query(string sql)
    {
        if (mysql_query(_conn, sql.c_str()))
        {
            LOG("查询失败:" + sql);
            return nullptr;
        }
        return mysql_use_result(_conn);
    }
