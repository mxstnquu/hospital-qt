#include "RegistrationWindow.hpp"


RegistrationWindow::RegistrationWindow(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Авторизация");
    setMinimumWidth(400);
    setFixedHeight(600);

    // Логин
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout -> addWidget(new QLabel("Логин:"));
    m_loginEdit = new QLineEdit(this);
    layout -> addWidget(m_loginEdit);

    // Пароль
    layout -> addWidget(new QLabel("Пароль:"));
    m_passwEdit = new QLineEdit(this);
    m_passwEdit -> setEchoMode(QLineEdit::Password);
    layout -> addWidget(m_passwEdit);

    // Кнопка Входа
    m_loginButton = new QPushButton("Вход", this);
    layout -> addWidget(m_loginButton);
    connect(m_loginButton, &QPushButton::clicked,
            this, &RegistrationWindow::onLoginClicked);
}

RegistrationWindow::~RegistrationWindow() = default;


void RegistrationWindow::onLoginClicked()
{
    QString login = m_loginEdit -> text().trimmed();
    QString pass = m_passwEdit -> text();

    if (login.isEmpty() || pass.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите логин и пароль.");
        return;
    }

    // Хешируем пароль SHA-256
    QByteArray hashedPass = QCryptographicHash::hash(pass.toUtf8(),
                                                     QCryptographicHash::Sha256).toHex();

    // Создание запроса для подтверждения входа
    QSqlQuery query;
    query.prepare("SELECT user_password, user_role, ref_id, full_name "
                  "FROM users "
                  "WHERE user_login = :login AND is_active = true");
    query.bindValue(":login", login);

    if (!query.exec()) {
        qDebug() << "Ошибка выполнения запроса:" << query.lastError().text();
        QMessageBox::critical(this, "Ошибка БД", "Не удалось выполнить запрос.");
        return;
    }

    if (query.next()) {
        QString storedHash = query.value(0).toString();
        // Сравниваем хеши
        if (storedHash != QString::fromUtf8(hashedPass)) {
            QMessageBox::warning(this, "Ошибка", "Неверный логин или пароль.");
            m_passwEdit->clear();
            return;
        }

        m_role = query.value(1).toString();
        m_ref_id = query.value(2).toInt();
        QString fullName = query.value(3).toString();

        // Корректировка для сотрудников
        if (m_ref_id == 0 && m_role != "patient" && m_role != "doctor") {
            m_ref_id = -1;
        }

        accept();
    } else {
        QMessageBox::warning(this, "Ошибка", "Пользователь не найден или неактивен.");
        m_passwEdit->clear();
    }
}