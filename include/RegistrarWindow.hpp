#ifndef REGISTRARWINDOW_HPP
#define REGISTRARWINDOW_HPP

#include <QMainWindow>
#include <QVBoxLayout>
#include <QWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QInputDialog>
#include <QComboBox>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QDateTimeEdit>
#include <QCryptographicHash>
#include <QDebug>

class RegistrarWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit RegistrarWindow(qint32 registrarId, QWidget *parent = nullptr);
    ~RegistrarWindow() override;

private slots:
    void loadPatients();
    void addPatient();
    void editPatient();
    void loadDoctors();
    void createAppointment();
    void viewAppointments();
    void editAppointment();
    // void onChangePassButtClicked();

private:
    qint32 m_registrarId;
    QTextEdit *m_infoEdit;

    QPushButton *m_patientsButton;
    QPushButton *m_addPatientButton;
    QPushButton *m_editPatientButton;
    QPushButton *m_doctorsButton;
    QPushButton *m_createAppButton;
    QPushButton *m_viewAppsButton;
    QPushButton *m_editAppButton;
    // QPushButton *m_changePassButton;
};

#endif // REGISTRARWINDOW_HPP
