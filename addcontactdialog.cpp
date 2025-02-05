#include "addcontactdialog.h"
#include <QMessageBox>


AddContactDialog::AddContactDialog(DialogMode mode, QWidget *parent): QDialog(parent), mode(mode)
{
    this->setWindowTitle("Контакт");
    this->resize(500, 600);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    //поля для ввода данных
    mainLayout->addWidget(new QLabel("Фамилия:"));
    surNameEdit = new QLineEdit(this);
    mainLayout->addWidget(surNameEdit);

    mainLayout->addWidget(new QLabel("Имя:"));
    firstNameEdit = new QLineEdit(this);
    mainLayout->addWidget(firstNameEdit);

    mainLayout->addWidget(new QLabel("Отчество:"));
    lastNameEdit = new QLineEdit(this);
    mainLayout->addWidget(lastNameEdit);

    mainLayout->addWidget(new QLabel("Адрес:"));
    addressEdit = new QLineEdit(this);
    mainLayout->addWidget(addressEdit);

    mainLayout->addWidget(new QLabel("Дата рождения:"));
    birthDateEdit = new QDateEdit(QDate::currentDate(), this);
    mainLayout->addWidget(birthDateEdit);

    mainLayout->addWidget(new QLabel("Email:"));
    emailEdit = new QLineEdit(this);
    mainLayout->addWidget(emailEdit);

    //для рабочих номеров
    mainLayout->addWidget(new QLabel("Рабочие номера:"));
    workNumbersLayout = new QVBoxLayout();
    mainLayout->addLayout(workNumbersLayout);
    addWorkNumberField(); //добавляем первое поле
    addWorkNumberButton = new QPushButton("Добавить рабочий номер", this);//кнопка для добавления полей номеров
    //connect(addWorkNumberButton, &QPushButton::clicked, this, &AddContactDialog::addWorkNumberField);
    mainLayout->addWidget(addWorkNumberButton);

    //для домашних номеров
    mainLayout->addWidget(new QLabel("Домашние номера:"));
    homeNumbersLayout = new QVBoxLayout();
    mainLayout->addLayout(homeNumbersLayout);
    addHomeNumberField();
    addHomeNumberButton = new QPushButton("Добавить домашний номер", this);
    //connect(addHomeNumberButton, &QPushButton::clicked, this, &AddContactDialog::addHomeNumberField);
    mainLayout->addWidget(addHomeNumberButton);

    //для служебных номеров
    mainLayout->addWidget(new QLabel("Служебные номера:"));
    officialNumbersLayout = new QVBoxLayout();
    mainLayout->addLayout(officialNumbersLayout);
    addOfficialNumberField();
    addOfficialNumberButton = new QPushButton("Добавить служебный номер", this);
    //connect(addOfficialNumberButton, &QPushButton::clicked, this, &AddContactDialog::addOfficialNumberField);
    mainLayout->addWidget(addOfficialNumberButton);

    //кнопки действия
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    actionButton = new QPushButton(this);
    cancelButton = new QPushButton("Отмена", this);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    buttonLayout->addWidget(actionButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);

    //установка мода добавление/редактирования/поиск
    setMode(mode);
}

void AddContactDialog::addWorkNumberField()
{
    QLineEdit *newField = new QLineEdit(this);
    newField->setPlaceholderText("Введите рабочий номер...");
    workNumbersLayout->addWidget(newField);
    workNumberFields.append(newField);
}

void AddContactDialog::addHomeNumberField()
{
    QLineEdit *newField = new QLineEdit(this);
    newField->setPlaceholderText("Введите домашний номер...");
    homeNumbersLayout->addWidget(newField);
    homeNumberFields.append(newField);
}

void AddContactDialog::addOfficialNumberField()
{
    QLineEdit *newField = new QLineEdit(this);
    newField->setPlaceholderText("Введите служебный номер...");
    officialNumbersLayout->addWidget(newField);
    officialNumberFields.append(newField);
}

void AddContactDialog::setMode(DialogMode newMode)
{
    mode = newMode;
    switch (mode)
    {
    case AddMode:
        this->setWindowTitle("Добавить контакт");
        actionButton->setText("Добавить");
        connect(actionButton, &QPushButton::clicked, this, &QDialog::accept);
        connect(addWorkNumberButton, &QPushButton::clicked, this, &AddContactDialog::addWorkNumberField);
        connect(addHomeNumberButton, &QPushButton::clicked, this, &AddContactDialog::addHomeNumberField);
        connect(addOfficialNumberButton, &QPushButton::clicked, this, &AddContactDialog::addOfficialNumberField);
        break;
    case EditMode:
        this->setWindowTitle("Редактировать контакт");
        actionButton->setText("Сохранить");
        connect(actionButton, &QPushButton::clicked, this, &QDialog::accept);
        connect(addWorkNumberButton, &QPushButton::clicked, this, &AddContactDialog::addWorkNumberField);
        connect(addHomeNumberButton, &QPushButton::clicked, this, &AddContactDialog::addHomeNumberField);
        connect(addOfficialNumberButton, &QPushButton::clicked, this, &AddContactDialog::addOfficialNumberField);
        break;
    case SearchMode:
        this->setWindowTitle("Поиск контактов");
        actionButton->setText("Найти");
        connect(actionButton, &QPushButton::clicked, this, &QDialog::accept);
        //отключаем возможность добавлять номера
        disconnect(addWorkNumberButton, &QPushButton::clicked, this, &AddContactDialog::addWorkNumberField);
        disconnect(addHomeNumberButton, &QPushButton::clicked, this, &AddContactDialog::addHomeNumberField);
        disconnect(addOfficialNumberButton, &QPushButton::clicked, this, &AddContactDialog::addOfficialNumberField);
        break;
    }
}

QString AddContactDialog::getSurName() const {
    return surNameEdit->text().trimmed();
}

QString AddContactDialog::getFirstName() const {
    return firstNameEdit->text().trimmed();
}

QString AddContactDialog::getLastName() const {
    return lastNameEdit->text().trimmed();
}

QString AddContactDialog::getAddress() const {
    return addressEdit->text().trimmed();
}

QDate AddContactDialog::getBirthDate() const {
    return birthDateEdit->date();
}

QString AddContactDialog::getEmail() const {
    return emailEdit->text().trimmed();
}

QStringList AddContactDialog::getWorkNumbers() const {
    QStringList numbers;
    for (QLineEdit *field : workNumberFields) {
        if (!field->text().isEmpty()) {
            numbers.append(field->text().trimmed());
        }
    }
    return numbers;
}

QStringList AddContactDialog::getHomeNumbers() const {
    QStringList numbers;
    for (QLineEdit *field : homeNumberFields) {
        if (!field->text().isEmpty()) {
            numbers.append(field->text().trimmed());
        }
    }
    return numbers;
}

QStringList AddContactDialog::getOfficialNumbers() const {
    QStringList numbers;
    for (QLineEdit *field : officialNumberFields) {
        if (!field->text().isEmpty()) {
            numbers.append(field->text().trimmed());
        }
    }
    return numbers;
}

QString AddContactDialog::getWorkNumber() const {
    if (workNumberFields.isEmpty()) {
        return ""; //если список пуст, возвращаем пустую строку
    } else {
        return workNumberFields.first()->text().trimmed(); //если не пуст, возвращаем текст первого поля
    }
}

QString AddContactDialog::getHomeNumber() const {
    if (homeNumberFields.isEmpty()) {
        return "";
    } else {
        return homeNumberFields.first()->text().trimmed();
    }
}

QString AddContactDialog::getOfficialNumber() const {
    if (officialNumberFields.isEmpty()) {
        return "";
    } else {
        return officialNumberFields.first()->text().trimmed();
    }
}


void AddContactDialog::setContactData(const QString &surName, const QString &firstName,
                                      const QString &lastName, const QString &address,
                                      const QDate &birthDate, const QString &email,
                                      const QStringList &workNumbers,
                                      const QStringList &homeNumbers,
                                      const QStringList &officialNumbers)
{
    surNameEdit->setText(surName);
    firstNameEdit->setText(firstName);
    lastNameEdit->setText(lastName);
    addressEdit->setText(address);
    birthDateEdit->setDate(birthDate);
    emailEdit->setText(email);

    qDeleteAll(workNumberFields);
    workNumberFields.clear();
    for (const QString &number : workNumbers) {
        QLineEdit *field = new QLineEdit(this);
        field->setText(number);
        workNumbersLayout->addWidget(field);
        workNumberFields.append(field);
    }

    qDeleteAll(homeNumberFields);
    homeNumberFields.clear();
    for (const QString &number : homeNumbers) {
        QLineEdit *field = new QLineEdit(this);
        field->setText(number);
        homeNumbersLayout->addWidget(field);
        homeNumberFields.append(field);
    }

    qDeleteAll(officialNumberFields);
    officialNumberFields.clear();
    for (const QString &number : officialNumbers) {
        QLineEdit *field = new QLineEdit(this);
        field->setText(number);
        officialNumbersLayout->addWidget(field);
        officialNumberFields.append(field);
    }
}


void AddContactDialog::accept()
{
    if (!validateFields()) {
        //QMessageBox::warning(this, "Ошибка ввода", "Некоторые данные введены некорректно.");

        return; //если данные некорректны, не закрываем форму
    }

    QDialog::accept();
}


bool AddContactDialog::validateFields()
{
    if (mode == AddMode || mode == EditMode) {
        if (surNameEdit->text().trimmed().isEmpty() || firstNameEdit->text().trimmed().isEmpty() || birthDateEdit->text().trimmed().isEmpty() ||
            emailEdit->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Все обязательные поля должны быть заполнены.");
            return false;
        }


        if (validateBirthDate(birthDateEdit->date()) == false) {
            QMessageBox::warning(this, "Ошибка", "Дата рождения должна быть меньше текущей даты.");
            return false;
        }
    }

    if (validateName(surNameEdit->text()) == false) {
        QMessageBox::warning(this, "Ошибка", "Фамилия введена некорректно.");
        return false;
    }

    if (validateName(firstNameEdit->text())== false) {
        QMessageBox::warning(this, "Ошибка", "Имя введено некорректо.");
        return false;
    }

    if (validateName(lastNameEdit->text()) == false) {
        QMessageBox::warning(this, "Ошибка", "Отчество введено некорректно.");
        return false;
    }

    if (!workNumberFields.isEmpty() && !validatePhoneNumber(workNumberFields.first()->text()) && !workNumberFields.first()->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Некорректный формат рабочего номера.");
        return false;
    }

    if (!homeNumberFields.isEmpty() && !validatePhoneNumber(homeNumberFields.first()->text()) && !homeNumberFields.first()->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Некорректный формат домашнего номера.");
        return false;
    }

    if (!officialNumberFields.isEmpty() && !validatePhoneNumber(officialNumberFields.first()->text()) && !officialNumberFields.first()->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Некорректный формат служебного номера.");
        return false;
    }

    if (validateEmail(emailEdit->text()) == false) {
        QMessageBox::warning(this, "Ошибка", "Некорректный email.");
        return false;
    }

    return true;
}

bool AddContactDialog::validateName(const QString &name)
{
    if (name == "")
        return true;
    QRegularExpression regex("^[A-Za-zА-Яа-яЁё][A-Za-zА-Яа-яЁё\\d\\s-]*[A-Za-zА-Яа-яЁё\\d]$");
    return regex.match(name.trimmed()).hasMatch();
}

bool AddContactDialog::validatePhoneNumber(const QString &phoneNumber)
{
    QRegularExpression regex(R"(^(\+7|8)\(?\d{3}\)?\d{3}[-]?\d{2}[-]?\d{2}$)");
    return regex.match(phoneNumber.trimmed()).hasMatch();
}

bool AddContactDialog::validateBirthDate(const QDate &birthDate)
{
    return birthDate < QDate::currentDate();
}

bool AddContactDialog::validateEmail(const QString &email)
{
    if (email == "")
        return true;
    QRegularExpression regex(R"(^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$)");
    return regex.match(email.trimmed()).hasMatch();
}







