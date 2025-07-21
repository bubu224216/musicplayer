/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../demo111/mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[19];
    char stringdata0[305];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 14), // "handlePlaySlot"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 14), // "handleModeSlot"
QT_MOC_LITERAL(4, 42, 14), // "handleNextSlot"
QT_MOC_LITERAL(5, 57, 14), // "handlePrevSlot"
QT_MOC_LITERAL(6, 72, 18), // "handleAddMusicSlot"
QT_MOC_LITERAL(7, 91, 26), // "handleChangeBackgroundSlot"
QT_MOC_LITERAL(8, 118, 14), // "updateProgress"
QT_MOC_LITERAL(9, 133, 8), // "position"
QT_MOC_LITERAL(10, 142, 14), // "updateDuration"
QT_MOC_LITERAL(11, 157, 8), // "duration"
QT_MOC_LITERAL(12, 166, 11), // "setPosition"
QT_MOC_LITERAL(13, 178, 27), // "handleProgressSliderPressed"
QT_MOC_LITERAL(14, 206, 28), // "handleProgressSliderReleased"
QT_MOC_LITERAL(15, 235, 26), // "handleMusicListItemClicked"
QT_MOC_LITERAL(16, 262, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(17, 279, 4), // "item"
QT_MOC_LITERAL(18, 284, 20) // "handleToggleListSlot"

    },
    "MainWindow\0handlePlaySlot\0\0handleModeSlot\0"
    "handleNextSlot\0handlePrevSlot\0"
    "handleAddMusicSlot\0handleChangeBackgroundSlot\0"
    "updateProgress\0position\0updateDuration\0"
    "duration\0setPosition\0handleProgressSliderPressed\0"
    "handleProgressSliderReleased\0"
    "handleMusicListItemClicked\0QListWidgetItem*\0"
    "item\0handleToggleListSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   79,    2, 0x0a /* Public */,
       3,    0,   80,    2, 0x0a /* Public */,
       4,    0,   81,    2, 0x0a /* Public */,
       5,    0,   82,    2, 0x0a /* Public */,
       6,    0,   83,    2, 0x0a /* Public */,
       7,    0,   84,    2, 0x0a /* Public */,
       8,    1,   85,    2, 0x0a /* Public */,
      10,    1,   88,    2, 0x0a /* Public */,
      12,    1,   91,    2, 0x0a /* Public */,
      13,    0,   94,    2, 0x0a /* Public */,
      14,    0,   95,    2, 0x0a /* Public */,
      15,    1,   96,    2, 0x0a /* Public */,
      18,    0,   99,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::LongLong,    9,
    QMetaType::Void, QMetaType::LongLong,   11,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 16,   17,
    QMetaType::Void,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MainWindow *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->handlePlaySlot(); break;
        case 1: _t->handleModeSlot(); break;
        case 2: _t->handleNextSlot(); break;
        case 3: _t->handlePrevSlot(); break;
        case 4: _t->handleAddMusicSlot(); break;
        case 5: _t->handleChangeBackgroundSlot(); break;
        case 6: _t->updateProgress((*reinterpret_cast< qint64(*)>(_a[1]))); break;
        case 7: _t->updateDuration((*reinterpret_cast< qint64(*)>(_a[1]))); break;
        case 8: _t->setPosition((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->handleProgressSliderPressed(); break;
        case 10: _t->handleProgressSliderReleased(); break;
        case 11: _t->handleMusicListItemClicked((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 12: _t->handleToggleListSlot(); break;
        default: ;
        }
    }
}

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow.data,
      qt_meta_data_MainWindow,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 13;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
