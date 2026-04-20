#include "database.hpp"


QSqlDatabase Database::connectToDB()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");

    db.setHostName("127.0.0.1");
    db.setPort(5432);
    db.setDatabaseName("hospital_db");
    db.setUserName("postgres");

    if (db.open()) {
        qDebug() << "Соединение с БД установлено.";
    } else {
        qDebug() << "Ошибка при установлении соединения с БД.";
    }

    return db;
}