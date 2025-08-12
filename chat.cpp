#include "chat.h"
#include "login.h"
#include "ui_Chat.h"
extern bool isFirst;
extern QString mainUser;
extern QString mainIp;

// 修改构造函数以接收socket
Chat::Chat(QTcpSocket *socket, QWidget *parent) :
    QWidget(parent), ui(new Ui::Chat), socket(socket) {
    ui->setupUi(this);

    // 使用传递的socket
    ui->serverIpLabel->setText(mainIp);
    ui->userLabel->setText("用户："+mainUser);

    // 链接槽函数获取服务器发回的信息
    connect(socket, SIGNAL(readyRead()), this, SLOT(readServer()));
}

Chat::~Chat() {
    delete ui;
}
//处理服务器响应后断开连接
void Chat::disconnectAfterResponse() {
    if (socket && socket->state() == QAbstractSocket::ConnectedState) {
        socket->disconnectFromHost();
        socket->deleteLater();
        socket = nullptr;
    }
}

void Chat::on_exitButton_clicked() {
    isFirst = false;
    auto *login = new Login();
    login->show();
    disconnectAfterResponse();
    close();
}
//一旦收到服务器消息，调用该函数
void Chat::readServer() {
    QTextStream in(socket);
    QString response = in.readAll();
    ui->textBrowser->append(response);
}
//点击发送按钮调用该函数
void Chat::on_sendButton_clicked() {
    QTextStream out(socket);
    QString message = ui->lineEdit->text();
    // 将消息内容用引号包裹，确保包含空格的消息能被正确解析
    out << "MSG \"" << message.replace("\"", "\\\"") << "\" " << mainUser;
    out.flush();
    ui->lineEdit->clear();
}
//查看历史记录
void Chat::on_historyButton_clicked() {
    msg = new Message();
    msg->show();
}
