#ifndef DOCTORWINDOW_HPP
#define DOCTORWINDOW_HPP

#include <QMainWindow>
#include <QVBoxLayout>
#include <QWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QMessageBox>
#include <QInputDialog>
#include <QComboBox>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QCryptographicHash>
#include <QDebug>

class DoctorWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit DoctorWindow(qint32 doctorId,
                           QWidget *parent = nullptr);
    ~DoctorWindow() override;


private slots:
    void loadProfile();
    void loadAppointments();
    void editAppointment();
    // void onChangePassButtClicked();

private:
    qint32 m_doctorId;
    QTextEdit *m_infoEdit;

    QPushButton *m_profileButton;
    QPushButton *m_appointmentsButton;
    QPushButton *m_editAppointmentButton;
    // QPushButton *m_changePassButton;
};

#endif // DOCTORWINDOW_HPP
