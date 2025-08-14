//
// Created by 1 on 11 8月 2025.
//

#ifndef QTCHATROOM_MESSAGE_H
#define QTCHATROOM_MESSAGE_H

#include <QDialog>
#include <QTcpSocket>

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
    //套接字用来向服务器请求聊天记录
    QTcpSocket* socket;
    //存储可能很长的单条信息
    QString incompleteData;
private slots:
    void readData();
    void on_queryButton_clicked();
    void on_lastButton_clicked();
    void on_nextButton_clicked();
};

#endif // QTCHATROOM_MESSAGE_H
