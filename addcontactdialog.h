#ifndef ADDCONTACTDIALOG_H
#define ADDCONTACTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QDateEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

class AddContactDialog : public QDialog
{
    Q_OBJECT

public:
    //перечисление для режимов работы формы
    enum DialogMode {
        AddMode,   //добавления контакта
        EditMode,  //редактирования контакта
        SearchMode //поиска контактов
    };

    AddContactDialog(DialogMode mode, QWidget *parent = nullptr);

    //ф-ия для заполнения формы данными из таблицы
    void setContactData(const QString &surName, const QString &firstName, const QString &lastName,
                        const QString &address, const QDate &birthDate, const QString &email,
                        const QStringList &workNumbers, const QStringList &homeNumbers,
                        const QStringList &officialNumbers);

    //сеттер и геттер режима работы
    void setMode(DialogMode mode);
    DialogMode getMode() const { return mode; }

    //геттеры для получения данных из формы
    QString getSurName() const;
    QString getFirstName() const;
    QString getLastName() const;
    QString getAddress() const;
    QDate getBirthDate() const;
    QString getEmail() const;
    QStringList getWorkNumbers() const;
    QStringList getHomeNumbers() const;
    QStringList getOfficialNumbers() const;
    QString getWorkNumber() const;
    QString getHomeNumber() const;
    QString getOfficialNumber() const;

private slots:
    void accept() override;  //перегружаем для валидации перед закрытием

private:
    //текущий режим работы формы
    DialogMode mode;

    //поля для ввода данных
    QLineEdit *surNameEdit;
    QLineEdit *firstNameEdit;
    QLineEdit *lastNameEdit;
    QLineEdit *addressEdit;
    QDateEdit *birthDateEdit;
    QLineEdit *emailEdit;
    //списки полей для ввода номеров
    QList<QLineEdit *> workNumberFields;
    QList<QLineEdit *> homeNumberFields;
    QList<QLineEdit *> officialNumberFields;

    //кнопки для добавления полей ввода номеров
    QPushButton *addWorkNumberButton;
    QPushButton *addHomeNumberButton;
    QPushButton *addOfficialNumberButton;

    //макеты для полей ввода номеров
    QVBoxLayout *workNumbersLayout;
    QVBoxLayout *homeNumbersLayout;
    QVBoxLayout *officialNumbersLayout;

    //кнопки действия
    QPushButton *actionButton; //кнопка "Добавить", "Сохранить" или "Найти"
    QPushButton *cancelButton; //кнопка "Отмена"

    //методы для динамического добавления полей номеров
    void addWorkNumberField();
    void addHomeNumberField();
    void addOfficialNumberField();

    //методы валидации
    bool validateFields();
    bool validateName(const QString &name);
    bool validatePhoneNumber(const QString &phoneNumber);
    bool validateBirthDate(const QDate &birthDate);
    bool validateEmail(const QString &email);
};

#endif // ADDCONTACTDIALOG_H
