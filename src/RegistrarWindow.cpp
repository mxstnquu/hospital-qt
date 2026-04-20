#include "RegistrarWindow.hpp"

RegistrarWindow::RegistrarWindow(qint32 registrarId, QWidget *parent)
    : QMainWindow(parent), m_registrarId(registrarId)
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    QVBoxLayout *layout = new QVBoxLayout(central);

    m_infoEdit = new QTextEdit(this);
    m_infoEdit->setReadOnly(true);
    layout->addWidget(m_infoEdit);

    m_patientsButton = new QPushButton("Список пациентов", this);
    layout->addWidget(m_patientsButton);
    connect(m_patientsButton, &QPushButton::clicked, this, &RegistrarWindow::loadPatients);

    m_addPatientButton = new QPushButton("Добавить пациента", this);
    layout->addWidget(m_addPatientButton);
    connect(m_addPatientButton, &QPushButton::clicked, this, &RegistrarWindow::addPatient);

    m_editPatientButton = new QPushButton("Редактировать пациента", this);
    layout->addWidget(m_editPatientButton);
    connect(m_editPatientButton, &QPushButton::clicked, this, &RegistrarWindow::editPatient);

    m_doctorsButton = new QPushButton("Список врачей", this);
    layout->addWidget(m_doctorsButton);
    connect(m_doctorsButton, &QPushButton::clicked, this, &RegistrarWindow::loadDoctors);

    m_createAppButton = new QPushButton("Создать запись к врачу", this);
    layout->addWidget(m_createAppButton);
    connect(m_createAppButton, &QPushButton::clicked, this, &RegistrarWindow::createAppointment);

    m_viewAppsButton = new QPushButton("Просмотр всех записей", this);
    layout->addWidget(m_viewAppsButton);
    connect(m_viewAppsButton, &QPushButton::clicked, this, &RegistrarWindow::viewAppointments);

    m_editAppButton = new QPushButton("Редактировать запись", this);
    layout->addWidget(m_editAppButton);
    connect(m_editAppButton, &QPushButton::clicked, this, &RegistrarWindow::editAppointment);

    m_infoEdit->append("=== Панель регистратуры ===\nДобро пожаловать!");
}

RegistrarWindow::~RegistrarWindow() = default;

void RegistrarWindow::loadPatients()
{
    m_infoEdit->clear();

    QSqlQuery query;
    query.prepare("SELECT patient_id, med_card_number, full_name, "
                  "birth_date, phone, address, gender, discount "
                  "FROM patients ORDER BY patient_id");
    if (!query.exec()) {
        qDebug() << "Ошибка загрузки пациентов:" << query.lastError().text();
        m_infoEdit->append("Не удалось загрузить список пациентов.");
        return;
    }

    m_infoEdit->append("=== Список пациентов ===\n");
    bool hasRows = false;
    while (query.next()) {
        hasRows = true;
        int id = query.value(0).toInt();
        QString medCard = query.value(1).toString();
        QString fullName = query.value(2).toString();
        QString birthDate = query.value(3).toDate().toString("dd.MM.yyyy");
        QString phone = query.value(4).toString();
        QString address = query.value(5).toString();
        QString gender = query.value(6).toString();
        double discount = query.value(7).toDouble();

        QString entry = QString("ID: %1\nМедкарта: %2\nФИО: %3\nДата рождения: "
                                "%4\nТелефон: %5\nАдрес: %6\nПол: %7\nСкидка: %8%"
                                "\n----------------------------------------")
                            .arg(id).arg(medCard, fullName, birthDate,
                                 phone, address, gender).arg(discount);
        m_infoEdit->append(entry);
    }
    if (!hasRows)
        m_infoEdit->append("Нет пациентов.");
}

void RegistrarWindow::addPatient()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Добавление пациента");
    QFormLayout form(&dialog);

    QLineEdit *medCardEdit = new QLineEdit(&dialog);
    QLineEdit *fullNameEdit = new QLineEdit(&dialog);
    QDateEdit *birthDateEdit = new QDateEdit(QDate::currentDate(), &dialog);
    birthDateEdit->setCalendarPopup(true);
    birthDateEdit->setDisplayFormat("dd.MM.yyyy");
    QLineEdit *phoneEdit = new QLineEdit(&dialog);
    QLineEdit *addressEdit = new QLineEdit(&dialog);
    QComboBox *genderCombo = new QComboBox(&dialog);
    genderCombo->addItems({"М", "Ж"});
    QLineEdit *discountEdit = new QLineEdit("0", &dialog);

    form.addRow("Номер медкарты:", medCardEdit);
    form.addRow("ФИО:", fullNameEdit);
    form.addRow("Дата рождения:", birthDateEdit);
    form.addRow("Телефон:", phoneEdit);
    form.addRow("Адрес:", addressEdit);
    form.addRow("Пол:", genderCombo);
    form.addRow("Скидка (%):", discountEdit);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok |
                                                         QDialogButtonBox::Cancel, &dialog);
    form.addRow(buttons);
    connect(buttons, &QDialogButtonBox::accepted,
            &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected,
            &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted)
        return;

    QString medCard = medCardEdit->text().trimmed();
    QString fullName = fullNameEdit->text().trimmed();
    QDate birthDate = birthDateEdit->date();
    QString phone = phoneEdit->text().trimmed();
    QString address = addressEdit->text().trimmed();
    QString gender = genderCombo->currentText();
    double discount = discountEdit->text().toDouble();

    if (medCard.isEmpty() || fullName.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Медкарта и ФИО обязательны.");
        return;
    }

    QSqlQuery maxIdQuery;
    maxIdQuery.exec("SELECT COALESCE(MAX(patient_id), 0) + 1 FROM patients");
    maxIdQuery.next();
    int newId = maxIdQuery.value(0).toInt();

    QSqlQuery insertQuery;
    insertQuery.prepare("INSERT INTO patients (patient_id, med_card_number, "
                        "full_name, birth_date, phone, address, gender, discount) "
                        "VALUES (:id, :med_card, :full_name, :birth_date, "
                        ":phone, :address, :gender, :discount)");
    insertQuery.bindValue(":id", newId);
    insertQuery.bindValue(":med_card", medCard);
    insertQuery.bindValue(":full_name", fullName);
    insertQuery.bindValue(":birth_date", birthDate);
    insertQuery.bindValue(":phone", phone);
    insertQuery.bindValue(":address", address);
    insertQuery.bindValue(":gender", gender);
    insertQuery.bindValue(":discount", discount);

    if (!insertQuery.exec()) {
        qDebug() << "Ошибка добавления пациента:" << insertQuery.lastError().text();
        QMessageBox::critical(this, "Ошибка", "Не удалось добавить пациента.");
        return;
    }

    // Создаём пользователя для этого пациента
    QSqlQuery userQuery;
    userQuery.prepare("INSERT INTO users "
                      "(user_login, user_password, user_role, ref_id, full_name) "
                      "VALUES (:login, :password, 'patient', :ref_id, :full_name)");
    userQuery.bindValue(":login", medCard);

    QByteArray tempHash = QCryptographicHash::hash(medCard.toUtf8(),
                                                   QCryptographicHash::Sha256).toHex();
    userQuery.bindValue(":password", QString::fromUtf8(tempHash));
    userQuery.bindValue(":ref_id", newId);
    userQuery.bindValue(":full_name", fullName);

    if (!userQuery.exec()) {
        qDebug() << "Ошибка создания пользователя:" << userQuery.lastError().text();
        QMessageBox::warning(this, "Предупреждение",
                             "Пациент добавлен, но не создан пользователь.");
    } else {
        QMessageBox::information(this, "Успех",
                                 "Пациент добавлен. Логин: " + medCard + ", пароль: " + medCard);
    }

    loadPatients();
}

void RegistrarWindow::editPatient()
{
    // Сначала выбрать пациента
    QSqlQuery listQuery;
    listQuery.exec("SELECT patient_id, full_name, med_card_number "
                   "FROM patients ORDER BY full_name");
    QStringList items;
    QVector<int> ids;
    while (listQuery.next()) {
        int id = listQuery.value(0).toInt();
        QString name = listQuery.value(1).toString();
        QString medCard = listQuery.value(2).toString();
        items << QString("%1 - %2 (%3)").arg(id).arg(name, medCard);
        ids << id;
    }
    if (items.isEmpty()) {
        QMessageBox::information(this, "Нет пациентов",
                                 "Нет доступных пациентов для редактирования.");
        return;
    }

    bool ok;
    QString selected = QInputDialog::getItem(this, "Выбор пациента",
                                             "Выберите пациента:", items, 0, false, &ok);
    if (!ok || selected.isEmpty()) return;
    int idx = items.indexOf(selected);
    int patientId = ids[idx];

    // Загружаем текущие данные
    QSqlQuery getQuery;
    getQuery.prepare("SELECT phone, address "
                     "FROM patients "
                     "WHERE patient_id = :id");
    getQuery.bindValue(":id", patientId);
    if (!getQuery.exec() || !getQuery.next()) {
        QMessageBox::critical(this, "Ошибка", "Не удалось загрузить данные.");
        return;
    }
    QString currentPhone = getQuery.value(0).toString();
    QString currentAddress = getQuery.value(1).toString();

    QDialog dialog(this);
    dialog.setWindowTitle("Редактирование пациента");
    QFormLayout form(&dialog);
    QLineEdit *phoneEdit = new QLineEdit(currentPhone, &dialog);
    QLineEdit *addressEdit = new QLineEdit(currentAddress, &dialog);
    form.addRow("Телефон:", phoneEdit);
    form.addRow("Адрес:", addressEdit);
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok |
                                                         QDialogButtonBox::Cancel, &dialog);
    form.addRow(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted)
        return;

    QString newPhone = phoneEdit->text().trimmed();
    QString newAddress = addressEdit->text().trimmed();

    QSqlQuery updateQuery;
    updateQuery.prepare("UPDATE patients SET phone = :phone, "
                        "address = :address WHERE patient_id = :id");
    updateQuery.bindValue(":phone", newPhone);
    updateQuery.bindValue(":address", newAddress);
    updateQuery.bindValue(":id", patientId);
    if (!updateQuery.exec()) {
        qDebug() << "Ошибка обновления:" << updateQuery.lastError().text();
        QMessageBox::critical(this, "Ошибка",
                              "Не удалось обновить данные.");
        return;
    }

    QMessageBox::information(this, "Успех",
                             "Данные пациента обновлены.");
    loadPatients();
}

void RegistrarWindow::loadDoctors()
{
    m_infoEdit->clear();
    QSqlQuery query;
    query.prepare("SELECT doctor_id, full_name, speciality, category "
                  "FROM doctors ORDER BY full_name");
    if (!query.exec()) {
        qDebug() << "Ошибка загрузки врачей:" << query.lastError().text();
        m_infoEdit->append("Не удалось загрузить список врачей.");
        return;
    }

    m_infoEdit->append("=== Список врачей ===\n");
    while (query.next()) {
        int id = query.value(0).toInt();
        QString name = query.value(1).toString();
        QString spec = query.value(2).toString();
        QString cat = query.value(3).toString();
        QString entry = QString("ID: %1\n%2\nСпециальность: %3\nКатегория: %4"
                                "\n----------------------------------------")
                            .arg(id).arg(name, spec, cat.isEmpty() ? "не указана" : cat);
        m_infoEdit->append(entry);
    }
}

void RegistrarWindow::createAppointment()
{
    // Выбор пациента
    QSqlQuery patQuery;
    patQuery.exec("SELECT patient_id, full_name, med_card_number "
                  "FROM patients ORDER BY full_name");
    QStringList patItems;

    QVector<int> patIds;
    while (patQuery.next()) {
        int id = patQuery.value(0).toInt();
        QString name = patQuery.value(1).toString();
        QString medCard = patQuery.value(2).toString();
        patItems << QString("%1 - %2 (%3)").arg(id).arg(name, medCard);
        patIds << id;
    }
    if (patItems.isEmpty()) {
        QMessageBox::warning(this, "Нет пациентов",
                             "Сначала добавьте пациентов.");
        return;
    }

    bool ok;
    QString selectedPat = QInputDialog::getItem(this,
                                                "Выбор пациента", "Пациент:", patItems, 0, false, &ok);
    if (!ok) return;
    int patientId = patIds[patItems.indexOf(selectedPat)];

    // Выбор врача
    QSqlQuery docQuery;
    docQuery.exec("SELECT doctor_id, full_name, speciality "
                  "FROM doctors ORDER BY full_name");
    QStringList docItems;
    QVector<int> docIds;
    while (docQuery.next()) {
        int id = docQuery.value(0).toInt();
        QString name = docQuery.value(1).toString();
        QString spec = docQuery.value(2).toString();
        docItems << QString("%1 - %2 (%3)").arg(id).arg(name, spec);
        docIds << id;
    }
    if (docItems.isEmpty()) {
        QMessageBox::warning(this, "Нет врачей",
                             "Нет доступных врачей.");
        return;
    }
    QString selectedDoc = QInputDialog::getItem(this, "Выбор врача",
                                                "Врач:", docItems, 0, false, &ok);
    if (!ok) return;
    int doctorId = docIds[docItems.indexOf(selectedDoc)];

    // Дата и время
    QDialog dialog(this);
    dialog.setWindowTitle("Дата и время приёма");
    QFormLayout form(&dialog);
    QDateTimeEdit *dtEdit = new QDateTimeEdit(QDateTime::currentDateTime().addSecs(3600), &dialog);
    dtEdit->setMinimumDateTime(QDateTime::currentDateTime().addSecs(3600));
    dtEdit->setCalendarPopup(true);
    form.addRow("Дата и время:", dtEdit);
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok |
                                                         QDialogButtonBox::Cancel, &dialog);
    form.addRow(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) return;
    QDateTime dt = dtEdit->dateTime();

    // Проверка конфликта
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT COUNT(*) FROM patient_appointments "
                       "WHERE doctor_id = :doc_id AND appointment_date = :dt");
    checkQuery.bindValue(":doc_id", doctorId);
    checkQuery.bindValue(":dt", dt);
    checkQuery.exec();
    checkQuery.next();
    if (checkQuery.value(0).toInt() > 0) {
        QMessageBox::warning(this, "Конфликт",
                             "Это время уже занято у выбранного врача.");
        return;
    }

    QSqlQuery insertQuery;
    insertQuery.prepare("INSERT INTO patient_appointments "
                        "(patient_id, doctor_id, appointment_date, status) "
                        "VALUES (:pat_id, :doc_id, :dt, 'planned')");
    insertQuery.bindValue(":pat_id", patientId);
    insertQuery.bindValue(":doc_id", doctorId);
    insertQuery.bindValue(":dt", dt);
    if (!insertQuery.exec()) {
        qDebug() << "Ошибка создания записи:" << insertQuery.lastError().text();
        QMessageBox::critical(this, "Ошибка", "Не удалось создать запись.");
        return;
    }

    QMessageBox::information(this, "Успех", "Запись создана.");
}

void RegistrarWindow::viewAppointments()
{
    m_infoEdit->clear();
    QSqlQuery query;
    query.prepare(
        "SELECT pa.appointment_id, pa.appointment_date, p.full_name AS patient, "
        "       d.full_name AS doctor, pa.status, diag.diagnosis_name, pa.treatment_notes "
        "FROM patient_appointments pa "
        "JOIN patients p ON pa.patient_id = p.patient_id "
        "JOIN doctors d ON pa.doctor_id = d.doctor_id "
        "LEFT JOIN diagnoses diag ON pa.diagnosis_code = diag.diagnosis_code "
        "ORDER BY pa.appointment_date DESC"
        );

    if (!query.exec()) {
        qDebug() << "Ошибка загрузки записей:" << query.lastError().text();
        m_infoEdit->append("Не удалось загрузить записи.");
        return;
    }

    m_infoEdit->append("=== Все записи к врачам ===\n");
    bool hasRows = false;
    while (query.next()) {
        hasRows = true;
        int id = query.value(0).toInt();
        QDateTime dt = query.value(1).toDateTime();
        QString patient = query.value(2).toString();
        QString doctor = query.value(3).toString();
        QString status = query.value(4).toString();
        QString diagnosis = query.value(5).toString();
        QString notes = query.value(6).toString();

        QString statusText = (status == "planned") ? "Запланировано" : (status == "completed" ? "Завершено" : status);
        QString entry = QString("ID: %1\nДата: %2\nПациент: %3\nВрач: %4\nСтатус: %5")
                            .arg(id).arg(dt.toString("dd.MM.yyyy hh:mm"), patient, doctor, statusText);
        if (!diagnosis.isEmpty()) entry += QString("\nДиагноз: %1").arg(diagnosis);
        if (!notes.isEmpty()) entry += QString("\nЗаметки: %1").arg(notes);
        entry += "\n----------------------------------------\n";
        m_infoEdit->append(entry);
    }
    if (!hasRows) m_infoEdit->append("Нет записей.");
}

void RegistrarWindow::editAppointment()
{
    // Выбор записи
    QSqlQuery listQuery;
    listQuery.prepare(
        "SELECT appointment_id, appointment_date, p.full_name, d.full_name "
        "FROM patient_appointments pa "
        "JOIN patients p ON pa.patient_id = p.patient_id "
        "JOIN doctors d ON pa.doctor_id = d.doctor_id "
        "ORDER BY appointment_date"
        );
    listQuery.exec();
    QStringList items;
    QVector<int> ids;
    while (listQuery.next()) {
        int id = listQuery.value(0).toInt();
        QString dt = listQuery.value(1).toDateTime().toString("dd.MM.yyyy hh:mm");
        QString patient = listQuery.value(2).toString();
        QString doctor = listQuery.value(3).toString();
        items << QString("%1 - %2 - %3 (%4)").arg(id).arg(patient, doctor, dt);
        ids << id;
    }
    if (items.isEmpty()) {
        QMessageBox::information(this, "Нет записей", "Нет записей для редактирования.");
        return;
    }

    bool ok;
    QString selected = QInputDialog::getItem(this, "Выбор записи", "Запись:", items, 0, false, &ok);
    if (!ok) return;
    int appId = ids[items.indexOf(selected)];

    // Загружаем текущие данные
    QSqlQuery getQuery;
    getQuery.prepare("SELECT appointment_date, status, diagnosis_code, treatment_notes FROM patient_appointments WHERE appointment_id = :id");
    getQuery.bindValue(":id", appId);
    if (!getQuery.exec() || !getQuery.next()) {
        QMessageBox::critical(this, "Ошибка", "Не удалось загрузить данные записи.");
        return;
    }
    QDateTime currentDt = getQuery.value(0).toDateTime();
    QString currentStatus = getQuery.value(1).toString();
    QString currentDiagCode = getQuery.value(2).toString();
    QString currentNotes = getQuery.value(3).toString();

    QDialog dialog(this);
    dialog.setWindowTitle("Редактирование записи");
    QFormLayout form(&dialog);

    QDateTimeEdit *dtEdit = new QDateTimeEdit(currentDt, &dialog);
    dtEdit->setCalendarPopup(true);
    form.addRow("Дата и время:", dtEdit);

    QComboBox *statusCombo = new QComboBox(&dialog);
    statusCombo->addItems({"planned", "completed"});
    int idx = statusCombo->findText(currentStatus);
    if (idx >= 0) statusCombo->setCurrentIndex(idx);
    form.addRow("Статус:", statusCombo);

    // Диагнозы
    QComboBox *diagCombo = new QComboBox(&dialog);
    diagCombo->addItem("(не указан)", "");
    QSqlQuery diagQuery;
    diagQuery.exec("SELECT diagnosis_code, diagnosis_name "
                   "FROM diagnoses");
    while (diagQuery.next()) {
        QString code = diagQuery.value(0).toString();
        QString name = diagQuery.value(1).toString();
        diagCombo->addItem(QString("%1 - %2").arg(code, name), code);
    }
    idx = diagCombo->findData(currentDiagCode);
    if (idx >= 0) diagCombo->setCurrentIndex(idx);
    form.addRow("Диагноз:", diagCombo);

    QLineEdit *notesEdit = new QLineEdit(currentNotes, &dialog);
    form.addRow("Заметки:", notesEdit);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    form.addRow(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) return;

    QDateTime newDt = dtEdit->dateTime();
    QString newStatus = statusCombo->currentText();
    QString newDiagCode = diagCombo->currentData().toString();
    QString newNotes = notesEdit->text().trimmed();

    QSqlQuery updateQuery;
    updateQuery.prepare("UPDATE patient_appointments SET appointment_date = :dt, status = :status, "
                        "diagnosis_code = :diag, treatment_notes = :notes WHERE appointment_id = :id");
    updateQuery.bindValue(":dt", newDt);
    updateQuery.bindValue(":status", newStatus);
    updateQuery.bindValue(":diag", newDiagCode.isEmpty() ? QVariant(QVariant::String) : newDiagCode);
    updateQuery.bindValue(":notes", newNotes);
    updateQuery.bindValue(":id", appId);
    if (!updateQuery.exec()) {
        qDebug() << "Ошибка обновления записи:" << updateQuery.lastError().text();
        QMessageBox::critical(this, "Ошибка", "Не удалось обновить запись.");
        return;
    }

    QMessageBox::information(this, "Успех", "Запись обновлена.");
    viewAppointments();
}
