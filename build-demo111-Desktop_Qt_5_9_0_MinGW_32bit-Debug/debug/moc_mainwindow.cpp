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
    QByteArrayData data[22];
    char stringdata0[352];
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
QT_MOC_LITERAL(8, 118, 20), // "handleToggleListSlot"
QT_MOC_LITERAL(9, 139, 26), // "handleMusicListItemClicked"
QT_MOC_LITERAL(10, 166, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(11, 183, 4), // "item"
QT_MOC_LITERAL(12, 188, 14), // "updateProgress"
QT_MOC_LITERAL(13, 203, 8), // "position"
QT_MOC_LITERAL(14, 212, 14), // "updateDuration"
QT_MOC_LITERAL(15, 227, 8), // "duration"
QT_MOC_LITERAL(16, 236, 11), // "setPosition"
QT_MOC_LITERAL(17, 248, 27), // "handleProgressSliderPressed"
QT_MOC_LITERAL(18, 276, 28), // "handleProgressSliderReleased"
QT_MOC_LITERAL(19, 305, 26), // "onVolumeSliderValueChanged"
QT_MOC_LITERAL(20, 332, 1), // "k"
QT_MOC_LITERAL(21, 334, 17) // "updateLyricHeight"

    },
    "MainWindow\0handlePlaySlot\0\0handleModeSlot\0"
    "handleNextSlot\0handlePrevSlot\0"
    "handleAddMusicSlot\0handleChangeBackgroundSlot\0"
    "handleToggleListSlot\0handleMusicListItemClicked\0"
    "QListWidgetItem*\0item\0updateProgress\0"
    "position\0updateDuration\0duration\0"
    "setPosition\0handleProgressSliderPressed\0"
    "handleProgressSliderReleased\0"
    "onVolumeSliderValueChanged\0k\0"
    "updateLyricHeight"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   89,    2, 0x08 /* Private */,
       3,    0,   90,    2, 0x08 /* Private */,
       4,    0,   91,    2, 0x08 /* Private */,
       5,    0,   92,    2, 0x08 /* Private */,
       6,    0,   93,    2, 0x08 /* Private */,
       7,    0,   94,    2, 0x08 /* Private */,
       8,    0,   95,    2, 0x08 /* Private */,
       9,    1,   96,    2, 0x08 /* Private */,
      12,    1,   99,    2, 0x08 /* Private */,
      14,    1,  102,    2, 0x08 /* Private */,
      16,    1,  105,    2, 0x08 /* Private */,
      17,    0,  108,    2, 0x08 /* Private */,
      18,    0,  109,    2, 0x08 /* Private */,
      19,    1,  110,    2, 0x08 /* Private */,
      21,    0,  113,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 10,   11,
    QMetaType::Void, QMetaType::LongLong,   13,
    QMetaType::Void, QMetaType::LongLong,   15,
    QMetaType::Void, QMetaType::Int,   13,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   20,
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
        case 6: _t->handleToggleListSlot(); break;
        case 7: _t->handleMusicListItemClicked((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 8: _t->updateProgress((*reinterpret_cast< qint64(*)>(_a[1]))); break;
        case 9: _t->updateDuration((*reinterpret_cast< qint64(*)>(_a[1]))); break;
        case 10: _t->setPosition((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->handleProgressSliderPressed(); break;
        case 12: _t->handleProgressSliderReleased(); break;
        case 13: _t->onVolumeSliderValueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 14: _t->updateLyricHeight(); break;
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
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 15;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
