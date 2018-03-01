//
// Created by leng yue on 02/03/2018.
//
#include <string>
#include <unordered_map>
#include "mysql/MyDB.h"
int main()
{
    MyDB db;
    unordered_map<string, string> myInfo;
    db.initDB("120.24.163.35", "lengyue", "123456", "experiment");
    db.addRecord("KNNG",myInfo);
}
