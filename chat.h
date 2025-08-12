#ifndef CHAT_H
#define CHAT_H

#include <QWidget>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QTcpSocket>
#include "message.h"


QT_BEGIN_NAMESPACE
namespace Ui { class Chat; }
QT_END_NAMESPACE

class Chat : public QWidget {
Q_OBJECT

public:
    explicit Chat(QWidget *parent = nullptr);
    ~Chat() override;
private:
    Ui::Chat *ui;
    QTcpSocket *socket;
    Message *msg=nullptr;
private slots:
    void on_exitButton_clicked();
    void readServer();
    void on_sendButton_clicked();
    void on_historyButton_clicked();
};


#endif //CHAT_H
