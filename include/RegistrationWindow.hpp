#ifndef REGISTRATIONWINDOW_HPP
#define REGISTRATIONWINDOW_HPP

#include <QDialog>
#include <QVBoxLayout>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QString>
#include <QCryptographicHash>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>


class RegistrationWindow : public QDialog
{
    Q_OBJECT
public:
    explicit RegistrationWindow(QWidget *parent = nullptr);
    ~RegistrationWindow() override;

    // qint32 getPatientId();
    QString getRole() { return m_role; }
    qint32 getRefId() { return m_ref_id; }


private slots:
    void onLoginClicked();

private:
    QLineEdit *m_loginEdit;
    QLineEdit *m_passwEdit;
    QPushButton *m_loginButton;

    // qint32 m_patientId;
    QString m_role;
    qint32 m_ref_id;

};

#endif // REGISTRATIONWINDOW_HPP
