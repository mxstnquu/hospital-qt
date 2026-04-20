#include "PatientWindow.hpp"

PatientWindow::PatientWindow(qint32 patientId, QWidget *parent)
    : QMainWindow(parent), m_patientId(patientId)
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    QVBoxLayout *layout = new QVBoxLayout(central);

    // Окно с Выводом Информации
    m_infoEdit = new QTextEdit(this);
    m_infoEdit -> setReadOnly(true);
    layout -> addWidget(m_infoEdit);

    // Приветствие
    loadHello();

    // Кнопка Просмотра Истории Посещений
    m_histButton = new QPushButton("Просмотр истории посещений", this);
    layout -> addWidget(m_histButton);
    connect(m_histButton, &QPushButton::clicked,
            this, &PatientWindow::onHistButtClicked);

    // Кнопка Просмотра Личной Информации Пациента
    m_showBioButton = new QPushButton("Просмотр личной информации", this);
    layout -> addWidget(m_showBioButton);
    connect(m_showBioButton, &QPushButton::clicked,
            this, &PatientWindow::onBioButtClicked);

    // Кнопка Просмотра Расписания Врачей
    m_viewSchedulesButton = new QPushButton("Просмотр расписания врачей", this);
    layout -> addWidget(m_viewSchedulesButton);
    connect(m_viewSchedulesButton, &QPushButton::clicked,
            this, &PatientWindow::onViewSchedulesButtClicked);

    // Кнопка для Записи Ко Врачу
    m_bookButton = new QPushButton("Записаться ко врачу", this);
    layout -> addWidget(m_bookButton);
    connect(m_bookButton, &QPushButton::clicked,
            this, &PatientWindow::onBookButtClicked);

    // Кнопка Редактирования Личной Информации
    m_editBioButton = new QPushButton("Изменить личную информацию", this);
    layout -> addWidget(m_editBioButton);
    connect(m_editBioButton, &QPushButton::clicked,
            this, &PatientWindow::onEditBioButtClicked);

    // Кнопка Смены Пароля Пациента
    m_changePassButton = new QPushButton("Сменить пароль", this);
    layout -> addWidget(m_changePassButton);
    connect(m_changePassButton, &QPushButton::clicked,
            this, &PatientWindow::onChangePassButtClicked);
}

PatientWindow::~PatientWindow() = default;


void PatientWindow::loadHello()
{
    // Приветствие -
    // Загружается Сразу После Входа
    m_infoEdit->clear();

    QSqlQuery query;
    query.prepare("SELECT med_card_number, full_name "
                  "FROM patients "
                  "WHERE patient_id = :patient_id");
    query.bindValue(":patient_id", m_patientId);

    if (!query.exec()) {
        qDebug() << "Ошибка выполнения запроса" << query.lastError().text();
        m_infoEdit -> append("Ошибка загрузки данных.");
        return;
    }

    QString medCard = "";
    QString fullName = "";
    if (query.next()) {
        medCard   = query.value(0).toString();
        fullName  = query.value(1).toString();
    }

    QString profileText = QString("Здравствуйте, %1\n"
                                  "Номер вашей мед карты: %2.")
                              .arg(fullName, medCard);

    m_infoEdit->append(profileText);
}

void PatientWindow::onHistButtClicked()
{
    m_infoEdit->clear();

    QSqlQuery query;
    query.prepare(
        "SELECT pa.appointment_date, d.full_name AS doctor_name, d.speciality, "
        "       pa.status, diag.diagnosis_name, pa.treatment_notes "
        "FROM patient_appointments pa "
        "JOIN doctors d ON pa.doctor_id = d.doctor_id "
        "LEFT JOIN diagnoses diag ON pa.diagnosis_code = diag.diagnosis_code "
        "WHERE pa.patient_id = :patient_id "
        "ORDER BY pa.appointment_date DESC"
        );
    query.bindValue(":patient_id", m_patientId);

    if (!query.exec()) {
        qDebug() << "Ошибка запроса истории:" << query.lastError().text();
        m_infoEdit->append("Не удалось загрузить историю посещений.");
        return;
    }

    m_infoEdit->append("=== История посещений ===\n");

    bool hasRows = false;
    while (query.next()) {
        hasRows = true;
        QDateTime dt = query.value(0).toDateTime();
        QString dateStr = dt.toString("dd.MM.yyyy hh:mm");
        QString doctor = query.value(1).toString();
        QString speciality = query.value(2).toString();
        QString status = query.value(3).toString();
        QString diagnosis = query.value(4).toString();
        QString notes = query.value(5).toString();

        QString statusText;
        if (status == "planned") statusText = "Запланировано";
        else if (status == "completed") statusText = "Завершено";
        else statusText = status;

        QString entry = QString("Дата: %1\nВрач: %2 (%3)\nСтатус: %4")
                            .arg(dateStr, doctor, speciality, statusText);
        if (!diagnosis.isEmpty())
            entry += QString("\nДиагноз: %1").arg(diagnosis);
        if (!notes.isEmpty())
            entry += QString("\nЗаметки: %1").arg(notes);
        entry += "\n----------------------------------------\n";
        m_infoEdit->append(entry);
    }

    if (!hasRows)
        m_infoEdit->append("У вас пока нет посещений.");
}

void PatientWindow::onBioButtClicked()
{
    m_infoEdit->clear();

    QSqlQuery query;
    query.prepare("SELECT med_card_number, full_name, "
                  "birth_date, phone, address, gender, discount "
                  "FROM patients "
                  "WHERE patient_id = :patient_id");
    query.bindValue(":patient_id", m_patientId);

    if (!query.exec()) {
        qDebug() << "Ошибка выполнения запроса" << query.lastError().text();
        m_infoEdit -> append("Ошибка загрузки данных.");
        return;
    }

    if (query.next()) {
        QString medCard   = query.value(0).toString();
        QString fullName  = query.value(1).toString();
        QString birthDate = query.value(2).toDate().toString("dd.MM.yyyy");
        QString phone     = query.value(3).toString();
        QString address   = query.value(4).toString();
        QString gender    = query.value(5).toString();
        double discount   = query.value(6).toDouble();

        QString info = QString(
                           "=== Личная информация пациента ===\n"
                           "Номер медкарты: %1\n"
                           "ФИО: %2\n"
                           "Дата рождения: %3\n"
                           "Телефон: %4\n"
                           "Адрес: %5\n"
                           "Пол: %6\n"
                           "Скидка: %7%\n"
                           )
                           .arg(medCard, fullName, birthDate,
                                phone, address, gender)
                           .arg(discount);

        m_infoEdit -> append(info);
    } else {
        m_infoEdit -> append("Пациент не найден. ID - "
                           + QString::number(m_patientId));
    }
}

void PatientWindow::onViewSchedulesButtClicked()
{
    m_infoEdit->clear();

    // Получаем спиосок всех врачей
    QSqlQuery doctorsQuery;
    doctorsQuery.prepare("SELECT doctor_id, full_name, speciality, category "
                         "FROM doctors ORDER BY full_name");
    if (!doctorsQuery.exec()) {
        qDebug() << "Ошибка загрузки списка врачей:" << doctorsQuery.lastError().text();
        m_infoEdit->append("Не удалось загрузить расписание.");
        return;
    }

    m_infoEdit->append("=== Расписание врачей ===\n");

    while (doctorsQuery.next()) {
        int docId = doctorsQuery.value(0).toInt();
        QString name = doctorsQuery.value(1).toString();
        QString spec = doctorsQuery.value(2).toString();
        QString cat = doctorsQuery.value(3).toString();

        QString docInfo = QString("Врач: %1\nСпециальность: %2\nКатегория: %3")
                              .arg(name, spec, cat.isEmpty() ? "не указана" : cat);
        m_infoEdit->append(docInfo);

        // Получаем будущие записи этого врача
        QSqlQuery appointmentsQuery;
        appointmentsQuery.prepare(
            "SELECT appointment_date, pa.patient_id, p.full_name "
            "FROM patient_appointments pa "
            "LEFT JOIN patients p ON pa.patient_id = p.patient_id "
            "WHERE pa.doctor_id = :doctor_id AND pa.status = 'planned' "
            "  AND pa.appointment_date > NOW() "
            "ORDER BY pa.appointment_date"
            );
        appointmentsQuery.bindValue(":doctor_id", docId);
        if (appointmentsQuery.exec()) {
            bool hasAppointments = false;
            while (appointmentsQuery.next()) {
                hasAppointments = true;
                QDateTime dt = appointmentsQuery.value(0).toDateTime();
                QString patientName = appointmentsQuery.value(2).toString();
                QString dateStr = dt.toString("dd.MM.yyyy hh:mm");
                m_infoEdit->append(QString("  Занято: %1 - %2").arg(dateStr, patientName));
            }
            if (!hasAppointments)
                m_infoEdit->append(" - Свободных записей (нет занятых слотов).");
        } else {
            qDebug() << "Ошибка получения записей врача:" << appointmentsQuery.lastError().text();
        }
        m_infoEdit->append("----------------------------------------\n");
    }
}

void PatientWindow::onBookButtClicked()
{
    // диалог выбора врача и даты
    QDialog dialog(this);
    dialog.setWindowTitle("Запись ко врачу");

    QFormLayout form(&dialog);

    QComboBox *doctorCombo = new QComboBox(&dialog);
    QSqlQuery docQuery;
    docQuery.prepare("SELECT doctor_id, full_name, speciality FROM doctors ORDER BY full_name");
    if (!docQuery.exec()) {
        QMessageBox::critical(this, "Ошибка", "Не удалось загрузить список врачей.");
        return;
    }

    QVector<QPair<int, QString>> doctors;
    while (docQuery.next()) {
        int id = docQuery.value(0).toInt();
        QString name = docQuery.value(1).toString();
        QString spec = docQuery.value(2).toString();
        QString display = QString("%1 (%2)").arg(name, spec);
        doctorCombo->addItem(display, id);
        doctors.append({id, display});
    }
    form.addRow("Врач:", doctorCombo);

    QDateTimeEdit *dateTimeEdit = new QDateTimeEdit(QDateTime::currentDateTime(), &dialog);
    // не раньше чем через час
    dateTimeEdit->setMinimumDateTime(QDateTime::currentDateTime().addSecs(3600));
    dateTimeEdit->setCalendarPopup(true);
    form.addRow("Дата и время:", dateTimeEdit);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    form.addRow(buttons);

    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    int doctorId = doctorCombo->currentData().toInt();
    QDateTime selectedDateTime = dateTimeEdit->dateTime();

    // Проверка конфликта записей
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT COUNT(*) FROM patient_appointments "
                       "WHERE doctor_id = :doctor_id AND appointment_date = :app_date");
    checkQuery.bindValue(":doctor_id", doctorId);
    checkQuery.bindValue(":app_date", selectedDateTime);
    if (!checkQuery.exec()) {
        QMessageBox::critical(this, "Ошибка", "Не удалось проверить расписание.");
        return;
    }
    checkQuery.next();
    int count = checkQuery.value(0).toInt();
    if (count > 0) {
        QMessageBox::warning(this, "Конфликт", "Выбранное время уже занято у этого врача.");
        return;
    }

    // Вставка новой записи
    QSqlQuery insertQuery;
    insertQuery.prepare(
        "INSERT INTO patient_appointments (patient_id, doctor_id, appointment_date, status) "
        "VALUES (:patient_id, :doctor_id, :app_date, 'planned')"
        );
    insertQuery.bindValue(":patient_id", m_patientId);
    insertQuery.bindValue(":doctor_id", doctorId);
    insertQuery.bindValue(":app_date", selectedDateTime);

    if (!insertQuery.exec()) {
        qDebug() << "Ошибка записи:" << insertQuery.lastError().text();
        QMessageBox::critical(this, "Ошибка", "Не удалось записаться к врачу.");
        return;
    }

    QMessageBox::information(this, "Успешно", "Вы успешно записаны к врачу!");
    m_infoEdit->clear();
    m_infoEdit->append("=== Запись ко врачу ===\nВы успешно записаны на " +
                       selectedDateTime.toString("dd.MM.yyyy hh:mm"));
}

void PatientWindow::onEditBioButtClicked()
{
    QSqlQuery selectQuery;
    selectQuery.prepare("SELECT phone, address FROM patients WHERE patient_id = :patient_id");
    selectQuery.bindValue(":patient_id", m_patientId);
    if (!selectQuery.exec() || !selectQuery.next()) {
        QMessageBox::critical(this, "Ошибка", "Не удалось загрузить текущие данные.");
        return;
    }
    QString currentPhone = selectQuery.value(0).toString();
    QString currentAddress = selectQuery.value(1).toString();

    // ввода новых данных
    QDialog dialog(this);
    dialog.setWindowTitle("Изменение личной информации");

    QFormLayout form(&dialog);
    QLineEdit *phoneEdit = new QLineEdit(currentPhone, &dialog);
    QLineEdit *addressEdit = new QLineEdit(currentAddress, &dialog);
    form.addRow("Телефон:", phoneEdit);
    form.addRow("Адрес:", addressEdit);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    form.addRow(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted)
        return;

    QString newPhone = phoneEdit->text().trimmed();
    QString newAddress = addressEdit->text().trimmed();

    QSqlQuery updateQuery;
    updateQuery.prepare("UPDATE patients SET phone = :phone, address = :address WHERE patient_id = :patient_id");
    updateQuery.bindValue(":phone", newPhone);
    updateQuery.bindValue(":address", newAddress);
    updateQuery.bindValue(":patient_id", m_patientId);

    if (!updateQuery.exec()) {
        qDebug() << "Ошибка обновления данных:" << updateQuery.lastError().text();
        QMessageBox::critical(this, "Ошибка", "Не удалось сохранить изменения.");
        return;
    }

    QMessageBox::information(this, "Успех", "Личная информация обновлена.");
    // обновлённые данные
    onBioButtClicked();
}

void PatientWindow::onChangePassButtClicked()
{
    // Запрашиваем старый пароль,
    // новый пароль и подтверждение нового
    bool ok;
    QString oldPass = QInputDialog::getText(this, "Смена пароля", "Введите старый пароль:",
                                            QLineEdit::Password, "", &ok);
    if (!ok || oldPass.isEmpty())
        return;

    // Хеширование старого пароля
    QByteArray oldHash = QCryptographicHash::hash(oldPass.toUtf8(), QCryptographicHash::Sha256).toHex();

    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT user_password FROM users WHERE user_role = 'patient' AND ref_id = :ref_id");
    checkQuery.bindValue(":ref_id", m_patientId);

    if (!checkQuery.exec() || !checkQuery.next()) {
        QMessageBox::critical(this, "Ошибка", "Не удалось проверить пароль.");
        return;
    }

    QString storedHash = checkQuery.value(0).toString();
    if (storedHash != QString::fromUtf8(oldHash)) {
        QMessageBox::warning(this, "Ошибка", "Неверный старый пароль.");
        return;
    }

    QString newPass = QInputDialog::getText(this, "Смена пароля", "Введите новый пароль:",
                                            QLineEdit::Password, "", &ok);
    if (!ok || newPass.isEmpty())
        return;

    QString confirmPass = QInputDialog::getText(this, "Смена пароля", "Подтвердите новый пароль:",
                                                QLineEdit::Password, "", &ok);
    if (!ok || confirmPass != newPass) {
        QMessageBox::warning(this, "Ошибка", "Пароли не совпадают.");
        return;
    }

    // Хеш нового пароля
    QByteArray newHash = QCryptographicHash::hash(newPass.toUtf8(), QCryptographicHash::Sha256).toHex();

    QSqlQuery updateQuery;
    updateQuery.prepare("UPDATE users SET user_password = :new_pass WHERE user_role = 'patient' AND ref_id = :ref_id");
    updateQuery.bindValue(":new_pass", QString::fromUtf8(newHash));
    updateQuery.bindValue(":ref_id", m_patientId);

    if (!updateQuery.exec()) {
        qDebug() << "Ошибка смены пароля:" << updateQuery.lastError().text();
        QMessageBox::critical(this, "Ошибка", "Не удалось сменить пароль.");
        return;
    }

    QMessageBox::information(this, "Успех", "Пароль успешно изменён.");
    m_infoEdit->clear();
    m_infoEdit->append("=== Смена пароля ===\nПароль успешно изменён.");
}