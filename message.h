//
// Created by 1 on 11 8月 2025.
//

#ifndef QTCHATROOM_MESSAGE_H
#define QTCHATROOM_MESSAGE_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
    class Message;
}
QT_END_NAMESPACE

class Message : public QDialog {
    Q_OBJECT

public:
    explicit Message(QWidget* parent = nullptr);
    ~Message() override;

private:
    Ui::Message* ui;
};

#endif // QTCHATROOM_MESSAGE_H
