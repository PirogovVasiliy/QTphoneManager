#include "mainwindow.h"
#include "addcontactdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QInputDialog>
#include <QMessageBox>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>




MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    //настройка главного окна
    this->setWindowTitle("Телефонный Справочник");
    this->resize(800, 600);

    //центральный виджет, в котором размещаются остальные
    QWidget *centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);

    //вертикальный макет
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    //таблица
    tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(9);
    tableWidget->setHorizontalHeaderLabels({ "Фамилия", "Имя", "Отчество", "Адрес", "Дата рождения", "Email",
        "Рабочие номера", "Домашние номера", "Служебные номера" });
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive); //позволить пользователю менять ширину колонок
    //настраиваем шиирну
    tableWidget->setColumnWidth(8, 250);
    tableWidget->setColumnWidth(7, 250);
    tableWidget->setColumnWidth(6, 250);
    tableWidget->setColumnWidth(0, 150);
    tableWidget->setColumnWidth(1, 150);
    tableWidget->setColumnWidth(2, 150);
    tableWidget->setColumnWidth(5, 250);
    tableWidget->setColumnWidth(3, 150);
    tableWidget->setColumnWidth(4, 150);
    tableWidget->setSortingEnabled(true); //сортировка
    tableWidget->setSelectionBehavior(QTableWidget::SelectRows); //выбор строк
    tableWidget->setEditTriggers(QTableWidget::NoEditTriggers); //запрет на редактирование данных в таблице
    tableWidget->setSelectionMode(QAbstractItemView::SingleSelection); //запрет на выделение сразу нескольких контактов


    mainLayout->addWidget(tableWidget); //добавляем таблицу

    //кнопки управления
    QHBoxLayout *buttonLayout = new QHBoxLayout();//горизонтальное расположение для кнопок

    QPushButton *addButton = new QPushButton("Добавить", this);
    QPushButton *deleteButton = new QPushButton("Удалить", this);
    QPushButton *editButton = new QPushButton("Редактировать", this);
    QPushButton *searchButton = new QPushButton("Поиск", this);
    QPushButton *resetFilterButton = new QPushButton("Сбросить фильтр", this);

    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addWidget(editButton);
    buttonLayout->addWidget(searchButton);
    buttonLayout->addWidget(resetFilterButton);

    mainLayout->addLayout(buttonLayout); //добавляем кнопки

    //связываем сигналы кнопок со слотами
    connect(addButton, &QPushButton::clicked, this, &MainWindow::onAddContact);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::onDeleteContact);
    connect(editButton, &QPushButton::clicked, this, &MainWindow::onEditContact);
    connect(searchButton, &QPushButton::clicked, this, &MainWindow::onSearchContact);
    connect(resetFilterButton, &QPushButton::clicked, this, &MainWindow::onResetFilter);


    addAllContacts();
    tableWidget->setCurrentItem(nullptr);//очистка выделения контакта
}


void MainWindow::onAddContact()
{
    AddContactDialog dialog(AddContactDialog::AddMode, this);

    if (dialog.exec() == QDialog::Accepted) { //запуск диалогового окна в модальном режиме
        //получение данных
        QString surName = dialog.getSurName();
        QString firstName = dialog.getFirstName();
        QString lastName = dialog.getLastName();
        QString address = dialog.getAddress();
        QDate birthDate = dialog.getBirthDate();
        QString email = dialog.getEmail();
        QString workNumbers = dialog.getWorkNumbers().join(", ");
        QString homeNumbers = dialog.getHomeNumbers().join(", ");
        QString officialNumbers = dialog.getOfficialNumbers().join(", ");

        //проверяем уникальность email
        if (isEmailUnique(email) == false) {
            QMessageBox::warning(this, "Ошибка", "Контакт с указанным email уже существует!");
            return;
        }
        //добавление данных в таблицу
        int newRow = tableWidget->rowCount(); //получаем кол во строк в таблице
        tableWidget->insertRow(newRow); //вставляем новую строку в конец
        tableWidget->setItem(newRow, 0, new QTableWidgetItem(surName));
        tableWidget->setItem(newRow, 1, new QTableWidgetItem(firstName));
        tableWidget->setItem(newRow, 2, new QTableWidgetItem(lastName));
        tableWidget->setItem(newRow, 3, new QTableWidgetItem(address));
        tableWidget->setItem(newRow, 4, new QTableWidgetItem(birthDate.toString("dd.MM.yyyy")));
        tableWidget->setItem(newRow, 5, new QTableWidgetItem(email));
        tableWidget->setItem(newRow, 6, new QTableWidgetItem(workNumbers));
        tableWidget->setItem(newRow, 7, new QTableWidgetItem(homeNumbers));
        tableWidget->setItem(newRow, 8, new QTableWidgetItem(officialNumbers));

        //сохранение в файл
        saveContactToFile(surName, firstName, lastName, address, birthDate, email, dialog.getWorkNumbers(), dialog.getHomeNumbers(), dialog.getOfficialNumbers());

        //сохранение в бд
        addContactToDatabase(surName, firstName, lastName, address, birthDate, email, workNumbers, homeNumbers, officialNumbers);
    }
    tableWidget->setCurrentItem(nullptr);//очистка выделения контакта

}

void MainWindow::onDeleteContact()
{
    int row = tableWidget->currentRow();

    //проверяем, выбрал ли пользователь строку
    if (row == -1) {
        QMessageBox::warning(this, "Ошибка", "Выберите контакт для удаления.");
        return;
    }

    //подтверждение удаления
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Подтверждение удаления",
                                  "Вы действительно хотите удалить этот контакт?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        tableWidget->removeRow(row);//удаляем строку

        rewriteContactsFile();
        rewriteDatabase();
    }
    tableWidget->setCurrentItem(nullptr);//очистка выделения контакта
}


void MainWindow::onEditContact()
{
    if (tableWidget->currentRow() == -1) { //если не выбрана строка для редактирования
        QMessageBox::warning(this, "Ошибка", "Выберите контакт для редактирования.");
        return;
    }

    //получение данных из выделенной строки
    int row = tableWidget->currentRow();
    QString surName = tableWidget->item(row, 0)->text();
    QString firstName = tableWidget->item(row, 1)->text();
    QString lastName = tableWidget->item(row, 2)->text();
    QString address = tableWidget->item(row, 3)->text();
    QDate birthDate = QDate::fromString(tableWidget->item(row, 4)->text(), "dd.MM.yyyy");
    QString currentEmail = tableWidget->item(row, 5)->text();
    QStringList workNumbers = tableWidget->item(row, 6)->text().split(", ");
    QStringList homeNumbers = tableWidget->item(row, 7)->text().split(", ");
    QStringList officialNumbers = tableWidget->item(row, 8)->text().split(", ");

    AddContactDialog dialog(AddContactDialog::EditMode, this); //открываем форму в режиме редактирования
    //передаем, полученные из таблицы данные
    dialog.setContactData(surName, firstName, lastName, address, birthDate, currentEmail, workNumbers, homeNumbers, officialNumbers);

    if (dialog.exec() == QDialog::Accepted) {
            //получаем обновленные данные из формы
            QString updatedSurName = dialog.getSurName();
            QString updatedFirstName = dialog.getFirstName();
            QString updatedLastName = dialog.getLastName();
            QString updatedAddress = dialog.getAddress();
            QDate updatedBirthDate = dialog.getBirthDate();
            QString updatedEmail = dialog.getEmail();
            QString updatedWorkNumbers = dialog.getWorkNumbers().join(", ");
            QString updatedHomeNumbers = dialog.getHomeNumbers().join(", ");
            QString updatedOfficialNumbers = dialog.getOfficialNumbers().join(", ");

            //если email изменился, проверяем его уникальность
            if (updatedEmail != currentEmail && isEmailUnique(updatedEmail) == false) {
                QMessageBox::warning(this, "Ошибка", "Контакт с указанным email уже существует!");
                return;
            }

            //обновляем данные в таблице
            tableWidget->setItem(row, 0, new QTableWidgetItem(updatedSurName));
            tableWidget->setItem(row, 1, new QTableWidgetItem(updatedFirstName));
            tableWidget->setItem(row, 2, new QTableWidgetItem(updatedLastName));
            tableWidget->setItem(row, 3, new QTableWidgetItem(updatedAddress));
            tableWidget->setItem(row, 4, new QTableWidgetItem(updatedBirthDate.toString("dd.MM.yyyy")));
            tableWidget->setItem(row, 5, new QTableWidgetItem(updatedEmail));
            tableWidget->setItem(row, 6, new QTableWidgetItem(updatedWorkNumbers));
            tableWidget->setItem(row, 7, new QTableWidgetItem(updatedHomeNumbers));
            tableWidget->setItem(row, 8, new QTableWidgetItem(updatedOfficialNumbers));

            rewriteContactsFile();
            rewriteDatabase();
        }

    tableWidget->setCurrentItem(nullptr);//очистка выделения контакта
}


void MainWindow::onSearchContact()
{
    //открываем диалоговое окно
    AddContactDialog dialog(AddContactDialog::SearchMode, this);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    //получаем данные
    QString searchSurName = dialog.getSurName();
    QString searchFirstName = dialog.getFirstName();
    QString searchLastName = dialog.getLastName();
    QString searchAddress = dialog.getAddress();
    QString searchBirthDate = dialog.getBirthDate().toString("dd.MM.yyyy");
    QString searchEmail = dialog.getEmail();
    QString searchWorkNumber = normalizePhone(dialog.getWorkNumber());
    QString searchHomeNumber = normalizePhone(dialog.getHomeNumber());
    QString searchOfficialNumber = normalizePhone(dialog.getOfficialNumber());
    QString todayDate = QDate::currentDate().toString("dd.MM.yyyy"); //сегодняшняя дата

    //перебираем строки таблицы и скрываем неподходящие
    for (int row = 0; row < tableWidget->rowCount(); row++) {
        bool match = true;

        if (!searchSurName.isEmpty() && tableWidget->item(row, 0)->text() != searchSurName)
            match = false;

        if (!searchFirstName.isEmpty() && tableWidget->item(row, 1)->text() != searchFirstName)
            match = false;

        if (!searchLastName.isEmpty() && tableWidget->item(row, 2)->text() != searchLastName)
            match = false;

        if (!searchAddress.isEmpty() && tableWidget->item(row, 3)->text() != searchAddress)
            match = false;

        if (!searchBirthDate.isEmpty() && searchBirthDate != todayDate && tableWidget->item(row, 4)->text() != searchBirthDate)
            match = false;

        if (!searchEmail.isEmpty() && tableWidget->item(row, 5)->text() != searchEmail)
            match = false;

        if (!searchWorkNumber.isEmpty()) {
            QStringList workNumbers = tableWidget->item(row, 6)->text().split(", ");
            bool found = false;
            for (const QString &number : workNumbers) {
                if (normalizePhone(number) == searchWorkNumber) {
                    found = true;
                    break;
                }
            }
            if (found == false)
                match = false;
        }

        if (!searchHomeNumber.isEmpty()) {
            QStringList homeNumbers = tableWidget->item(row, 7)->text().split(", ");
            bool found = false;
            for (const QString &number : homeNumbers) {
                if (normalizePhone(number) == searchHomeNumber) {
                    found = true;
                    break;
                }
            }
            if (found == false)
                match = false;     
        }

        if (!searchOfficialNumber.isEmpty()) {
            QStringList officialNumbers = tableWidget->item(row, 8)->text().split(", ");
            bool found = false;
            for (const QString &number : officialNumbers) {
                if (normalizePhone(number) == searchOfficialNumber) {
                    found = true;
                    break;
                }
            }
            if (found == false)
                match = false;
        }

        tableWidget->setRowHidden(row, !match);
    }
    tableWidget->setCurrentItem(nullptr);//очистка выделения контакта
}



void MainWindow::onResetFilter()
{
    //показываем все строки таблицы
    for (int row = 0; row < tableWidget->rowCount(); row++)
        tableWidget->setRowHidden(row, false);

    tableWidget->setCurrentItem(nullptr);//очистка выделения контакта
}


QString MainWindow::normalizePhone(const QString &phone) const
{
    QString normalizedPhone;

    for (QChar ch : phone) {
        if (ch.isDigit())
            normalizedPhone += ch;
    }

    if (normalizedPhone.startsWith("7"))
        normalizedPhone[0] = '8';

    return normalizedPhone;
}


bool MainWindow::isEmailUnique(const QString &email) const {
    for (int row = 0; row < tableWidget->rowCount(); row++) {
        if (tableWidget->item(row, 5)->text().compare(email, Qt::CaseInsensitive) == 0) {
            return false;
        }
    }
    return true;
}


void MainWindow::saveContactToFile(const QString &surName, const QString &firstName, const QString &lastName,
                                   const QString &address, const QDate &birthDate, const QString &email,
                                   const QStringList &workNumbers, const QStringList &homeNumbers,
                                   const QStringList &officialNumbers) {

    QFile file("C:/qtprojects/PhoneManager/contactsData.txt");

    //qDebug() << "Попытка открыть файл для записи.";
    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        //qDebug() << "Ошибка открытия файла: " << file.errorString();
    } else {
        //qDebug() << "Файл успешно открыт.";
    }

    QTextStream out(&file);

    out << "Surname: " << surName << "\n";
    out << "First Name: " << firstName << "\n";
    out << "Last Name: " << lastName << "\n";
    out << "Address: " << address << "\n";
    out << "Birth Date: " << birthDate.toString("dd.MM.yyyy") << "\n";
    out << "Email: " << email << "\n";

    out << "Work Numbers:\n";
    for (const QString &number : workNumbers) {
        out << "  - " << number << "\n";
    }

    out << "Home Numbers:\n";
    for (const QString &number : homeNumbers) {
        out << "  - " << number << "\n";
    }

    out << "Official Numbers:\n";
    for (const QString &number : officialNumbers) {
        out << "  - " << number << "\n";
    }

    out << "\n";
    file.close();
}


void MainWindow::rewriteContactsFile() {
    QFile file("C:/qtprojects/PhoneManager/contactsData.txt");

    //открываем файл в режиме "только запись" с удалением предыдущего содержимого
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл для записи!");
        return;
    }

    QTextStream out(&file);

    for (int row = 0; row < tableWidget->rowCount(); row++) {
        QString surName = tableWidget->item(row, 0)->text();
        QString firstName = tableWidget->item(row, 1)->text();
        QString lastName = tableWidget->item(row, 2)->text();
        QString address = tableWidget->item(row, 3)->text();
        QString birthDate = tableWidget->item(row, 4)->text();
        QString email = tableWidget->item(row, 5)->text();

        QStringList workNumbers = tableWidget->item(row, 6)->text().split(", ");
        QStringList homeNumbers = tableWidget->item(row, 7)->text().split(", ");
        QStringList officialNumbers = tableWidget->item(row, 8)->text().split(", ");

        out << "Surname: " << surName << "\n";
        out << "First Name: " << firstName << "\n";
        out << "Last Name: " << lastName << "\n";
        out << "Address: " << address << "\n";
        out << "Birth Date: " << birthDate << "\n";
        out << "Email: " << email << "\n";

        out << "Work Numbers:\n";
        for (const QString &number : workNumbers) {
            out << "  - " << number << "\n";
        }

        out << "Home Numbers:\n";
        for (const QString &number : homeNumbers) {
            out << "  - " << number << "\n";
        }

        out << "Official Numbers:\n";
        for (const QString &number : officialNumbers) {
            out << "  - " << number << "\n";
        }

        out << "\n";
    }

    file.close();
}




void MainWindow::addContactToTable(const QString &surName, const QString &firstName, const QString &lastName,
                                   const QString &address, const QString &birthDate, const QString &email,
                                   const QStringList &workNumbers, const QStringList &homeNumbers,
                                   const QStringList &officialNumbers)
{
    int newRow = tableWidget->rowCount();
    tableWidget->insertRow(newRow);

    tableWidget->setItem(newRow, 0, new QTableWidgetItem(surName));
    tableWidget->setItem(newRow, 1, new QTableWidgetItem(firstName));
    tableWidget->setItem(newRow, 2, new QTableWidgetItem(lastName));
    tableWidget->setItem(newRow, 3, new QTableWidgetItem(address));
    tableWidget->setItem(newRow, 4, new QTableWidgetItem(birthDate));
    tableWidget->setItem(newRow, 5, new QTableWidgetItem(email));
    tableWidget->setItem(newRow, 6, new QTableWidgetItem(workNumbers.join(", ")));
    tableWidget->setItem(newRow, 7, new QTableWidgetItem(homeNumbers.join(", ")));
    tableWidget->setItem(newRow, 8, new QTableWidgetItem(officialNumbers.join(", ")));
}

void MainWindow::addContactToDatabase(const QString& surname, const QString& firstName, const QString& lastName, const QString& address,
                                      const QDate& birthDate, const QString& email, const QString& workNumbers,
                                      const QString& homeNumbers, const QString& officialNumbers)
{
    QSqlQuery query;

    query.prepare("INSERT INTO Contacts (surname, first_name, last_name, address, birth_date, email, work_numbers, home_numbers, official_numbers) "
                  "VALUES (:surname, :firstName, :lastName, :address, :birthDate, :email, :workNumbers, :homeNumbers, :officialNumbers)");

    query.bindValue(":surname", surname);
    query.bindValue(":firstName", firstName);
    query.bindValue(":lastName", lastName);
    query.bindValue(":address", address);
    query.bindValue(":birthDate", birthDate);
    query.bindValue(":email", email);
    query.bindValue(":workNumbers", workNumbers);
    query.bindValue(":homeNumbers", homeNumbers);
    query.bindValue(":officialNumbers", officialNumbers);

    if (!query.exec()) {
        qDebug() << "Ошибка добавления контакта в базу данных:" << query.lastError().text();
    } else {
        qDebug() << "Контакт успешно добавлен в базу данных.";
    }
}


void MainWindow::rewriteDatabase() {
    QSqlQuery query;

    query.exec("DELETE FROM contacts");

    for (int row = 0; row < tableWidget->rowCount(); row++) {
        QString surName = tableWidget->item(row, 0)->text();
        QString firstName = tableWidget->item(row, 1)->text();
        QString lastName = tableWidget->item(row, 2)->text();
        QString address = tableWidget->item(row, 3)->text();
        QDate birthDate = QDate::fromString(tableWidget->item(row, 4)->text(), "dd.MM.yyyy");
        QString email = tableWidget->item(row, 5)->text();
        QString workNumbers = tableWidget->item(row, 6)->text();
        QString homeNumbers = tableWidget->item(row, 7)->text();
        QString officialNumbers = tableWidget->item(row, 8)->text();

        QString queryStr = R"(
            INSERT INTO contacts (surname, first_name, last_name, address, birth_date, email, work_numbers, home_numbers, official_numbers)
            VALUES (:surname, :first_name, :last_name, :address, :birth_date, :email, :work_numbers, :home_numbers, :official_numbers))";

        query.prepare(queryStr);
        query.bindValue(":surname", surName);
        query.bindValue(":first_name", firstName);
        query.bindValue(":last_name", lastName);
        query.bindValue(":address", address);
        query.bindValue(":birth_date", birthDate);
        query.bindValue(":email", email);
        query.bindValue(":work_numbers", workNumbers);
        query.bindValue(":home_numbers", homeNumbers);
        query.bindValue(":official_numbers", officialNumbers);

        query.exec();
    }
}

void MainWindow::addAllContacts(){
    addContactToTable("Иванов", "Алексей", "Петрович", "ул. Центральная, 25", "15.07.1990", "ivanov.a@example.com",
                      { "+79161234567", "+79035554433", "88127751231", "+75164421112" }, { "8(812)345-67-89" }, { "+78121237777" });

    addContactToTable("Смирнова", "Екатерина", "Олеговна", "пр. Московский, 10", "03.02.1985", "smirnova.k@example.com",
                      {}, { "8(495)678-45-12", "8(812)999-88-77" }, {});

    addContactToTable("Павлов", "Дмитрий", "Сергеевич", "ул. Гоголя, 14", "25.12.1993", "pavlov.d@example.com",
                      { "+79101112233" }, {}, { "+78124445566", "+79031234567" });

    addContactToTable("Воронцов", "Игорь", "Анатольевич", "ул. Советская, 5", "10.10.2000", "vorontsov.i@example.com",
                      { "+79998887766" }, { "8(495)333-22-11" }, {});

    addContactToTable("Кузнецова", "Марина", "Владимировна", "пр. Ленина, 8", "19.06.1988", "kuznetsova.m@example.com",
                      { "+79263334455", "+79152229988" }, {}, {});

    addContactToTable("Михайлов", "Виктор", "Александрович", "ул. Новая, 17", "12.04.1978", "mikhailov.v@example.com",
                      { "+79265556677" }, { "8(812)456-78-90" }, { "+78125559988" });

    addContactToTable("Соколова", "Анна", "Владимировна", "пр. Пушкинский, 32", "08.09.1995", "sokolova.a@example.com",
                      { "+79114443322" }, {}, { "+78121112233" });

    addContactToTable("Григорьев", "Олег", "Павлович", "ул. Строителей, 9", "22.11.1983", "grigoriev.o@example.com",
                      { "+79996668855" }, { "8(495)222-11-33" }, {});

    addContactToTable("Фёдорова", "Елена", "Сергеевна", "пр. Октябрьский, 5", "22.05.2003", "fedorova.e@example.com",
                      {}, { "8(812)777-99-55" }, { "+78127776655" });

    addContactToTable("Пирогов", "Артём", "Игоревич", "ул. Солнечная, 21", "18.03.2002", "borisov.a@example.com",
                      { "+79035557788", "+79112233445" }, {}, {});

    addContactToTable("Пирогов", "Василий", "Романович", "ул. Александра Матросова, 20к2", "22.05.2003", "kamvasilii@gmail.com",
                      {}, {"8(914)997-00-01", "89149960001", "+79149970001"}, {});

    saveAllContactsToFile();

    addAllContactsToDatabase();

}


void MainWindow::saveAllContactsToFile() {

    saveContactToFile("Иванов", "Алексей", "Петрович", "ул. Центральная, 25", QDate::fromString("15.07.1990", "dd.MM.yyyy"), "ivanov.a@example.com",
                      { "+79161234567", "+79035554433", "88127751231", "+75164421112" }, { "8(812)345-67-89" }, { "+78121237777" });

    saveContactToFile("Смирнова", "Екатерина", "Олеговна", "пр. Московский, 10",
                      QDate::fromString("03.02.1985", "dd.MM.yyyy"), "smirnova.k@example.com",
                      {}, { "8(495)678-45-12", "8(812)999-88-77" }, {});

    saveContactToFile("Павлов", "Дмитрий", "Сергеевич", "ул. Гоголя, 14",
                      QDate::fromString("25.12.1993", "dd.MM.yyyy"), "pavlov.d@example.com",
                      { "+79101112233" }, {}, { "+78124445566", "+79031234567" });

    saveContactToFile("Воронцов", "Игорь", "Анатольевич", "ул. Советская, 5",
                      QDate::fromString("10.10.2000", "dd.MM.yyyy"), "vorontsov.i@example.com",
                      { "+79998887766" }, { "8(495)333-22-11" }, {});

    saveContactToFile("Кузнецова", "Марина", "Владимировна", "пр. Ленина, 8",
                      QDate::fromString("19.06.1988", "dd.MM.yyyy"), "kuznetsova.m@example.com",
                      { "+79263334455", "+79152229988" }, {}, {});

    saveContactToFile("Михайлов", "Виктор", "Александрович", "ул. Новая, 17",
                      QDate::fromString("12.04.1978", "dd.MM.yyyy"), "mikhailov.v@example.com",
                      { "+79265556677" }, { "8(812)456-78-90" }, { "+78125559988" });

    saveContactToFile("Соколова", "Анна", "Владимировна", "пр. Пушкинский, 32",
                      QDate::fromString("08.09.1995", "dd.MM.yyyy"), "sokolova.a@example.com",
                      { "+79114443322" }, {}, { "+78121112233" });

    saveContactToFile("Григорьев", "Олег", "Павлович", "ул. Строителей, 9",
                      QDate::fromString("22.11.1983", "dd.MM.yyyy"), "grigoriev.o@example.com",
                      { "+79996668855" }, { "8(495)222-11-33" }, {});

    saveContactToFile("Фёдорова", "Елена", "Сергеевна", "пр. Октябрьский, 5",
                      QDate::fromString("22.05.2003", "dd.MM.yyyy"), "fedorova.e@example.com",
                      {}, { "8(812)777-99-55" }, { "+78127776655" });

    saveContactToFile("Пирогов", "Артём", "Игоревич", "ул. Солнечная, 21",
                      QDate::fromString("18.03.2002", "dd.MM.yyyy"), "borisov.a@example.com",
                      { "+79035557788", "+79112233445" }, {}, {});

    saveContactToFile("Пирогов", "Василий", "Романович", "ул. Александра Матросова, 20к2",
                      QDate::fromString("22.05.2003", "dd.MM.yyyy"), "kamvasilii@gmail.com",
                      {}, { "8(914)997-00-01", "89149960001", "+79149970001" }, {});
}



void MainWindow::addAllContactsToDatabase() {
    addContactToDatabase("Иванов", "Алексей", "Петрович", "ул. Центральная, 25", QDate::fromString("15.07.1990", "dd.MM.yyyy"), "ivanov.a@example.com",
                         "+79161234567, +79035554433, 88127751231, +75164421112", "8(812)345-67-89", "+78121237777");

    addContactToDatabase("Смирнова", "Екатерина", "Олеговна", "пр. Московский, 10", QDate::fromString("03.02.1985", "dd.MM.yyyy"), "smirnova.k@example.com",
                         "", "8(495)678-45-12, 8(812)999-88-77", "");

    addContactToDatabase("Павлов", "Дмитрий", "Сергеевич", "ул. Гоголя, 14", QDate::fromString("25.12.1993", "dd.MM.yyyy"), "pavlov.d@example.com",
                         "+79101112233", "", "+78124445566, +79031234567");

    addContactToDatabase("Воронцов", "Игорь", "Анатольевич", "ул. Советская, 5", QDate::fromString("10.10.2000", "dd.MM.yyyy"), "vorontsov.i@example.com",
                         "+79998887766", "8(495)333-22-11", "");

    addContactToDatabase("Кузнецова", "Марина", "Владимировна", "пр. Ленина, 8", QDate::fromString("19.06.1988", "dd.MM.yyyy"), "kuznetsova.m@example.com",
                         "+79263334455, +79152229988", "", "");

    addContactToDatabase("Михайлов", "Виктор", "Александрович", "ул. Новая, 17", QDate::fromString("12.04.1978", "dd.MM.yyyy"), "mikhailov.v@example.com",
                         "+79265556677", "8(812)456-78-90", "+78125559988");

    addContactToDatabase("Соколова", "Анна", "Владимировна", "пр. Пушкинский, 32", QDate::fromString("08.09.1995", "dd.MM.yyyy"), "sokolova.a@example.com",
                         "+79114443322", "", "+78121112233");

    addContactToDatabase("Григорьев", "Олег", "Павлович", "ул. Строителей, 9", QDate::fromString("22.11.1983", "dd.MM.yyyy"), "grigoriev.o@example.com",
                         "+79996668855", "8(495)222-11-33", "");

    addContactToDatabase("Фёдорова", "Елена", "Сергеевна", "пр. Октябрьский, 5", QDate::fromString("22.05.2003", "dd.MM.yyyy"), "fedorova.e@example.com",
                         "", "8(812)777-99-55", "+78127776655");

    addContactToDatabase("Пирогов", "Артём", "Игоревич", "ул. Солнечная, 21", QDate::fromString("18.03.2002", "dd.MM.yyyy"), "borisov.a@example.com",
                         "+79035557788, +79112233445", "", "");

    addContactToDatabase("Пирогов", "Василий", "Романович", "ул. Александра Матросова, 20к2", QDate::fromString("22.05.2003", "dd.MM.yyyy"), "kamvasilii@gmail.com",
                         "", "8(914)997-00-01, 89149960001, +79149970001", "");
}





