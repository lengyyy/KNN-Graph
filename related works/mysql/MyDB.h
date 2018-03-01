/*************************************************************************
    > File Name: MyDB.h
    > Author: SongLee
    > E-mail: lisong.shine@qq.com
    > Created Time: 2014年05月04日 星期日 23时25分50秒
    > Personal Blog: http://songlee24.github.io
 ************************************************************************/
#ifndef __MYBD_H__
#include "lib.h"
#include<mysql/mysql.h>

class MyDB
{
public:
	MyDB();
	~MyDB();
	bool initDB(string host, string user, string pwd, string db_name);
	void errorIntoMySQL();
	bool exeSQL(string sql);
	bool getData(string sql, vector<vector<string> >& data);
	bool addRecord(string table_name, unordered_map<string, string> result);

public:
	int errorNum;
	const char* errorInfo;

private:
	MYSQL *connection;
	MYSQL_RES *result;
	MYSQL_ROW row;
};

#endif