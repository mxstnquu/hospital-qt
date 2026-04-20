# hospital-qt
Hospital Information System (C++/Qt/PostgreSQL)

Учебный проект в рамках дисциплины в вузе. Демонстрирует навыки разработки приложений на C++/Qt и СУБД PostgreSQL.
Кроссплатформенная информационная система для управления медицинским учреждением.


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
psql -U postgres -f docs/struct_bd.psql
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
