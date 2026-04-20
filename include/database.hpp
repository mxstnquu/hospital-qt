#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>

class Database
{
public:
    static QSqlDatabase connectToDB();


};



#endif // DATABASE_HPP
