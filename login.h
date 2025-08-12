
#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include <QMouseEvent>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QTcpSocket>
#include <QMessageBox>
#include <QAbstractSocket>
#include <QSettings>
#include "ui_Chat.h"
#include "chat.h"


QT_BEGIN_NAMESPACE
namespace Ui { class Login; }
QT_END_NAMESPACE

class Login : public QDialog {
Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login() override;

private:
    Ui::Login *ui;
    //拖动窗口设置
    bool m_dragging;          // 拖动状态标志
    QPoint m_dragStartPosition; // 拖动起始位置
    //存储本地账号数据的数据库
    QSqlDatabase userData;
    //连接用的套接字，处理注册和登陆逻辑
    QTcpSocket *tcpSocket=nullptr;
    //登陆成功后打开的聊天界面
    Chat* chat=nullptr;
    //判断账号密码是否合法
    bool userAndPasswordIsOk();
    //处理服务器响应后的断开连接
    void disconnectAfterResponse();
    //本地数据库更新操作
    void localDatabaseUpdates();
private slots:
    //检查本地数据库
    void checkLocalDatabase();

    //处理鼠标拖动窗口的槽函数
    void on_closeButton_clicked();
    void on_loginButton_clicked();
    void on_registerButton_clicked();

    //处理连接失败的槽函数
    void connectError(QAbstractSocket::SocketError error);
    //连接成功后发送码
    void connectLongin();
    void connectRegister();
    // 自动登录状态改变的槽函数
    void on_autoLoginCheck_stateChanged(int state);
    //接收服务器反馈的槽函数
    void readServer();
    //忘记密码
    void on_forgotButton_clicked();
    //改变账号
    void changeUser(int);
protected:
    //拖动窗口设置
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
};


#endif //LOGIN_H
