/*************************************************************************
    > File Name: MyDB.h
    > Author: SongLee
    > E-mail: lisong.shine@qq.com
    > Created Time: 2014年05月04日 星期日 23时25分50秒
    > Personal Blog: http://songlee24.github.io
 ************************************************************************/
#ifndef __MYBD_H__
#define __MYBD_H__
#include<mysql/mysql.h>
#include <vector>
#include <unordered_map>
using namespace std;

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

MyDB::MyDB()
{
	connection = mysql_init(NULL); // 初始化数据库连接变量
	if(connection == NULL)
	{
		cout << "Error:" << mysql_error(connection);
		exit(1);
	}
}

MyDB::~MyDB()
{
	if(connection != NULL)  // 关闭数据库连接
	{
		mysql_close(connection);
	}
}

bool MyDB::initDB(string host, string user, string pwd, string db_name)
{
	// 函数mysql_real_connect建立一个数据库连接
	// 成功返回MYSQL*连接句柄，失败返回NULL
	connection = mysql_real_connect(connection, host.c_str(),
			user.c_str(), pwd.c_str(), db_name.c_str(), 0, NULL, 0);
	if(connection == NULL)
	{
		cout << "Error:" << mysql_error(connection);
		exit(1);
	}
	return true;
}

void MyDB::errorIntoMySQL()
{
	errorNum=mysql_errno(connection);
	errorInfo=mysql_error(connection);
}

bool MyDB::exeSQL(string sql)
{
	// mysql_query()执行成功返回0，失败返回非0值。与PHP中不一样
	if(mysql_query(connection, sql.c_str()))
	{
		cout << "Query Error:" << mysql_error(connection) <<endl;
		exit(1);
	}
	else
	{
		result = mysql_use_result(connection); // 获取结果集
		// mysql_field_count()返回connection查询的列数
		for(int i=0; i < mysql_field_count(connection); ++i)
		{
			// 获取下一行
			row = mysql_fetch_row(result);
			if(row <= 0)
			{
				break;
			}
			// mysql_num_fields()返回结果集中的字段数
			for(int j=0; j < mysql_num_fields(result); ++j)
			{
				cout << row[j] << " ";
			}
			cout << endl;
		}
		// 释放结果集的内存
		mysql_free_result(result);
	}
	return true;
}

bool MyDB::getData(string sql, vector<vector<string> >& data){
	if(0!=mysql_query(connection, sql.c_str()))
	{
		errorIntoMySQL();
		return false;
	}

	result=mysql_store_result(connection);

	int row=mysql_num_rows(result);
	int field=mysql_num_fields(result);

	MYSQL_ROW line=NULL;
	line=mysql_fetch_row(result);

	int j=0;
	std::string temp;
	while(NULL!=line)
	{
		std::vector<std::string> linedata;
		for(int i=0; i<field;i++)
		{
			if(line[i])
			{
				temp = line[i];
				linedata.push_back(temp);
			}
			else
			{
				temp = "";
				linedata.push_back(temp);
			}
		}
		line=mysql_fetch_row(result);
		data.push_back(linedata);
	}
	return true;
}

bool MyDB::addRecord(string table_name, unordered_map<string, string> result){
	int id;
	vector<vector<string>> tem;
	if(!getData("select max(id) from "+table_name,tem))
	{
		cout<<"get id fail"<<endl;
		exit(false);
	}
	if(tem[0][0] == "")
		id = 0;
	else
		id = stoi(tem[0][0])+1;
	for(auto key : result){
		exeSQL("insert into "+ table_name +" values ("+to_string(id)+",\""+key.first+"\",\""+key.second+"\")");
	}
	return true;
}

#endif