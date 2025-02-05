#include "mainwindow.h"
#include <QFile>
#include <QApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>
#include <QtSql>
#include <QtCore>

void connectToDatabase() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("localhost");
    db.setDatabaseName("PhoneManager");
    db.setUserName("postgres");
    db.setPassword("Vas9149970001");
    db.setPort(5433);

    if (db.open()) {
        qDebug() << "Подключение к базе данных успешно!";
    } else {
        qDebug() << "Ошибка подключения к базе данных:" << db.lastError().text();
    }
}

void clearDatabase() {
    QSqlQuery query;

    query.exec("TRUNCATE TABLE Contacts");
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    connectToDatabase();
    clearDatabase();

    QFile file("C:/qtprojects/PhoneManager/contactsData.txt");
    if (file.exists())
        file.remove();

    MainWindow w;
    w.showMaximized();

    return a.exec();
}
