#ifndef PATIENTWINDOW_HPP
#define PATIENTWINDOW_HPP

#include <QMainWindow>
#include <QVBoxLayout>
#include <QWidget>
#include <QMessageBox>
#include <QFormLayout>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QInputDialog>
#include <QLineEdit>

#include <QPushButton>
#include <QTextEdit>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDate>
#include <QCryptographicHash>

class PatientWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit PatientWindow(qint32 patientId,
                           QWidget *parent = nullptr);
    ~PatientWindow() override;

private slots:
    void loadHello();

    void onHistButtClicked();
    void onBioButtClicked();

    void onViewSchedulesButtClicked();
    void onBookButtClicked(); // запись ко врачу
    void onEditBioButtClicked();
    void onChangePassButtClicked();

private:
    qint32 m_patientId;
    QTextEdit *m_infoEdit;

    QPushButton *m_histButton;
    QPushButton *m_showBioButton;
    QPushButton *m_viewSchedulesButton;
    QPushButton *m_bookButton;
    QPushButton *m_editBioButton;
    QPushButton *m_changePassButton;
};

#endif // PATIENTWINDOW_HPP
