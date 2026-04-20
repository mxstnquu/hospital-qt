#include "RegistrationWindow.hpp"
#include "PatientWindow.hpp"
#include "DoctorWindow.hpp"
#include "RegistrarWindow.hpp"
#include "database.hpp"

#include <QApplication>
#include <QMessageBox>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    auto db = Database::connectToDB();
    if (!db.isOpen()) {
        QMessageBox::critical(nullptr,
                             "Фатальная ошибка",
                             "Не удалось подключиться к базе данных.");
        return 1;
    }

    RegistrationWindow regd;
    if (regd.exec() == QDialog::Accepted) {
        QString role = regd.getRole();
        int ref_id = regd.getRefId();

        if (role == "patient") {
            PatientWindow patientWin(ref_id);
            patientWin.show();
            return a.exec();
        } else if (role == "doctor") {
            DoctorWindow doctorWin(ref_id);
            doctorWin.show();
            return a.exec();
        } else if (role == "registrar") {
            RegistrarWindow registrarWin(ref_id);
            registrarWin.show();
            return a.exec();
        }
        else {
            QMessageBox::critical(nullptr, "Ошибка", "Неизвестная роль.");
            db.close();
            return 1;
        }
    } else {
        // Пользователь закрыл окно
        db.close();
        return 0;
    }

    db.close();
    return QCoreApplication::exec();
}
