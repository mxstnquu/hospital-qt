# hospital-qt
Hospital Information System (C++/Qt/PostgreSQL)

Учебный проект в рамках дисциплины в вузе. Демонстрирует навыки разработки приложений на C++/Qt и СУБД PostgreSQL.
Кроссплатформенная информационная система для управления медицинским учреждением.

# Screenshots
<img width="2560" height="1407" alt="изображение" src="https://github.com/user-attachments/assets/0bd1d90c-7f97-47b9-bfda-0be1bca8efb2" />
<img width="1277" height="699" alt="изображение" src="https://github.com/user-attachments/assets/4e7e5245-9aaa-40da-b4f1-aefd3b250ddc" />
<img width="1270" height="694" alt="изображение" src="https://github.com/user-attachments/assets/db9f9ecc-f937-4a7c-990f-33cdb3ff887e" />



## Возможности

Система предоставляет три интерфейса в зависимости от роли пользователя (выбор роли в зависимости от введенного логина).

Регистратура:
- Управление картотекой пациентов
- Ведение справочника врачей
- Создание и управление записями на приём

Врач:
- Просмотр своего расписания и активных приёмов
- Заполнение медицинской документации: установка диагноза из справочника, добавление лечебных заметок

Пациент:
- Просмотр личной информации
- Просмотр истории посещений с диагнозами и заметками
- Запись к выбранному врачу на свободное время
- Самостоятельное изменение личных данных
- Смена пароля с проверкой старого и хешированием нового


## Технологический стек

- Язык программирования: C++17
- Фреймворк для GUI и работы с БД: Qt 6 (Core, Widgets, SQL)
- Система сборки: CMake
- База данных: PostgreSQL
- Безопасность: хеширование паролей SHA-256, подготовленные SQL-запросы


## Быстрый старт

Предварительные требования:
- C++17 (GCC)
- Qt 6 - core, widgets, sql
- CMake 
- PostgreSQL

Настройка базы данных:
```
sudo -i -u postgres
createdb hospital_db
psql -d hospital_db -f docs/struct_bd.psql
```

# Build 

```
git clone https://github.com/mxstnquu/hospital-qt
cd ./hospital-qt
cmake -S . -B build
cmake --build build
./build/hospital-qt
```

# Структура проекта

```
hospital-qt/
├── CMakeLists.txt
├── LICENSE
├── README.md
├── .gitignore
├── docs/
│   └── struct_bd.psql
├── include/
│   ├── database.hpp
│   ├── DoctorWindow.hpp
│   ├── PatientWindow.hpp
│   ├── RegistrarWindow.hpp
│   └── RegistrationWindow.hpp
└── src/
    ├── main.cpp
    ├── database.cpp
    ├── DoctorWindow.cpp
    ├── PatientWindow.cpp
    ├── RegistrarWindow.cpp
    └── RegistrationWindow.cpp
```
