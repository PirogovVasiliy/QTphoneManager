#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QtSql>
#include <QtCore>
#include <QSqlDatabase>
#include <QSqlError>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void onAddContact();
    void onDeleteContact();
    void onEditContact();
    void onSearchContact();
    void onResetFilter();

private:
    QTableWidget *tableWidget;

    QString normalizePhone(const QString &phone) const; //приводим номер к одному виду для сравнения

    //автоматическое заполнение таблицы
    void addContactToTable(const QString &surName, const QString &firstName, const QString &lastName,
                                       const QString &address, const QString &birthDate, const QString &email,
                                       const QStringList &workNumbers, const QStringList &homeNumbers,
                                       const QStringList &officialNumbers);

    void addAllContacts(); //добавление контактов

    bool isEmailUnique(const QString &email) const; // уникальность почты

    void saveContactToFile(const QString &surName, const QString &firstName, const QString &lastName,
                                       const QString &address, const QDate &birthDate, const QString &email,
                                       const QStringList &workNumbers, const QStringList &homeNumbers,
                                       const QStringList &officialNumbers);

    void rewriteContactsFile();

    void saveAllContactsToFile();

    void addContactToDatabase(const QString& surname, const QString& firstName, const QString& lastName, const QString& address,
                                          const QDate& birthDate, const QString& email, const QString& workNumbers,
                                          const QString& homeNumbers, const QString& officialNumbers);

    void addAllContactsToDatabase();

    void rewriteDatabase();

};

#endif // MAINWINDOW_H
