#include "DoctorWindow.hpp"

DoctorWindow::DoctorWindow(qint32 doctorId, QWidget *parent)
    : QMainWindow(parent), m_doctorId(doctorId)
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    QVBoxLayout *layout = new QVBoxLayout(central);

    // Окно вывода информации
    m_infoEdit = new QTextEdit(this);
    m_infoEdit->setReadOnly(true);
    layout->addWidget(m_infoEdit);

    // Кнопка просмотра профиля
    m_profileButton = new QPushButton("Мой профиль", this);
    layout->addWidget(m_profileButton);
    connect(m_profileButton, &QPushButton::clicked, this, &DoctorWindow::loadProfile);

    // Кнопка просмотра приёмов
    m_appointmentsButton = new QPushButton("Мои приёмы", this);
    layout->addWidget(m_appointmentsButton);
    connect(m_appointmentsButton, &QPushButton::clicked, this, &DoctorWindow::loadAppointments);

    // Кнопка редактирования диагноза/заметок
    m_editAppointmentButton = new QPushButton("Редактировать приём", this);
    layout->addWidget(m_editAppointmentButton);
    connect(m_editAppointmentButton, &QPushButton::clicked, this, &DoctorWindow::editAppointment);

    // Загружаем профиль по умолчанию
    loadProfile();
}

DoctorWindow::~DoctorWindow() = default;

void DoctorWindow::loadProfile()
{
    m_infoEdit->clear();

    QSqlQuery query;
    query.prepare("SELECT full_name, speciality, category FROM doctors WHERE doctor_id = :doctor_id");
    query.bindValue(":doctor_id", m_doctorId);

    if (!query.exec()) {
        qDebug() << "Ошибка загрузки профиля врача:" << query.lastError().text();
        m_infoEdit->append("Ошибка загрузки данных.");
        return;
    }

    if (query.next()) {
        QString fullName = query.value(0).toString();
        QString speciality = query.value(1).toString();
        QString category = query.value(2).toString();

        QString info = QString(
                           "=== Профиль врача ===\n"
                           "ID: %1\n"
                           "ФИО: %2\n"
                           "Специальность: %3\n"
                           "Категория: %4\n"
                           ).arg(m_doctorId).arg(fullName, speciality, category.isEmpty() ? "не указана" : category);

        m_infoEdit->append(info);
    } else {
        m_infoEdit->append("Врач не найден. ID - " + QString::number(m_doctorId));
    }
}

void DoctorWindow::loadAppointments()
{
    m_infoEdit->clear();

    QSqlQuery query;
    query.prepare(
        "SELECT pa.appointment_id, pa.appointment_date, p.full_name AS patient_name, "
        "       pa.status, dg.diagnosis_name, pa.treatment_notes "
        "FROM patient_appointments pa "
        "JOIN patients p ON pa.patient_id = p.patient_id "
        "LEFT JOIN diagnoses dg ON pa.diagnosis_code = dg.diagnosis_code "
        "WHERE pa.doctor_id = :doctor_id "
        "ORDER BY pa.appointment_date DESC"
        );
    query.bindValue(":doctor_id", m_doctorId);

    if (!query.exec()) {
        qDebug() << "Ошибка загрузки приёмов:" << query.lastError().text();
        m_infoEdit->append("Не удалось загрузить список приёмов.");
        return;
    }

    m_infoEdit->append("=== Мои приёмы ===\n");
    bool hasRows = false;
    while (query.next()) {
        hasRows = true;
        int appId = query.value(0).toInt();
        QDateTime dt = query.value(1).toDateTime();
        QString dateStr = dt.toString("dd.MM.yyyy hh:mm");
        QString patient = query.value(2).toString();
        QString status = query.value(3).toString();
        QString diagnosis = query.value(4).toString();
        QString notes = query.value(5).toString();

        QString statusText = (status == "planned") ? "Запланировано" : (status == "completed" ? "Завершено" : status);

        QString entry = QString("ID записи: %1\nДата: %2\nПациент: %3\nСтатус: %4")
                            .arg(appId).arg(dateStr, patient, statusText);
        if (!diagnosis.isEmpty())
            entry += QString("\nДиагноз: %1").arg(diagnosis);
        if (!notes.isEmpty())
            entry += QString("\nЗаметки: %1").arg(notes);
        entry += "\n----------------------------------------\n";
        m_infoEdit->append(entry);
    }

    if (!hasRows)
        m_infoEdit->append("У вас пока нет назначенных приёмов.");
}

void DoctorWindow::editAppointment()
{
    // список приёмов
    QSqlQuery listQuery;
    listQuery.prepare(
        "SELECT appointment_id, appointment_date, p.full_name "
        "FROM patient_appointments pa "
        "JOIN patients p ON pa.patient_id = p.patient_id "
        "WHERE pa.doctor_id = :doctor_id AND pa.status != 'completed' "
        "ORDER BY appointment_date"
        );
    listQuery.bindValue(":doctor_id", m_doctorId);
    if (!listQuery.exec()) {
        QMessageBox::critical(this, "Ошибка", "Не удалось загрузить список приёмов.");
        return;
    }

    QStringList items;
    QVector<int> ids;
    while (listQuery.next()) {
        int id = listQuery.value(0).toInt();
        QString date = listQuery.value(1).toDateTime().toString("dd.MM.yyyy hh:mm");
        QString patient = listQuery.value(2).toString();
        items << QString("%1 - %2 (%3)").arg(id).arg(patient, date);
        ids << id;
    }

    if (items.isEmpty()) {
        QMessageBox::information(this, "Нет приёмов",
                                 "Нет активных приёмов для редактирования.");
        return;
    }

    bool ok;
    QString selected = QInputDialog::getItem(this, "Выбор приёма",
                                             "Выберите запись:",
                                             items, 0, false, &ok);
    if (!ok || selected.isEmpty()) return;

    int idx = items.indexOf(selected);
    if (idx < 0) return;
    int appointmentId = ids[idx];

    // текущие данные
    QSqlQuery getQuery;
    getQuery.prepare("SELECT diagnosis_code, treatment_notes "
                     "FROM patient_appointments "
                     "WHERE appointment_id = :app_id");

    getQuery.bindValue(":app_id", appointmentId);
    if (!getQuery.exec() || !getQuery.next()) {
        QMessageBox::critical(this, "Ошибка",
                              "Не удалось загрузить данные приёма.");
        return;
    }
    QString currentDiagCode = getQuery.value(0).toString();
    QString currentNotes = getQuery.value(1).toString();


    // Диалог выбора диагноза
    QDialog dialog(this);
    dialog.setWindowTitle("Редактирование приёма");
    QFormLayout form(&dialog);

    // Список диагнозов
    QComboBox *diagCombo = new QComboBox(&dialog);
    QSqlQuery diagQuery;
    diagQuery.exec("SELECT diagnosis_code, diagnosis_name "
                   "FROM diagnoses "
                   "ORDER BY diagnosis_name");

    diagCombo->addItem("(не указан)", "");
    while (diagQuery.next()) {
        QString code = diagQuery.value(0).toString();
        QString name = diagQuery.value(1).toString();
        diagCombo->addItem(QString("%1 - %2").arg(code, name), code);
    }

    // Установить текущий диагноз
    int index = diagCombo->findData(currentDiagCode);
    if (index >= 0) diagCombo->setCurrentIndex(index);
    else diagCombo->setCurrentIndex(0);

    form.addRow("Диагноз:", diagCombo);

    QLineEdit *notesEdit = new QLineEdit(currentNotes, &dialog);
    form.addRow("Заметки:", notesEdit);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    form.addRow(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) return;

    QString newDiagCode = diagCombo->currentData().toString();
    QString newNotes = notesEdit->text().trimmed();

    // Обновление
    QSqlQuery updateQuery;
    updateQuery.prepare("UPDATE patient_appointments "
                        "SET diagnosis_code = :diag_code, "
                        "treatment_notes = :notes "
                        "WHERE appointment_id = :app_id");

    updateQuery.bindValue(":diag_code",
                          newDiagCode.isEmpty() ? QVariant(QVariant::String) : newDiagCode);
    updateQuery.bindValue(":notes", newNotes);
    updateQuery.bindValue(":app_id", appointmentId);

    if (!updateQuery.exec()) {
        qDebug() << "Ошибка обновления приёма:" << updateQuery.lastError().text();
        QMessageBox::critical(this, "Ошибка",
                              "Не удалось сохранить изменения.");
        return;
    }

    QMessageBox::information(this, "Успех",
                             "Данные приёма обновлены.");
    loadAppointments();
}
