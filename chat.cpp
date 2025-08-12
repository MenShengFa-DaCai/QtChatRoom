#include "chat.h"
#include "login.h"
#include "ui_Chat.h"
extern bool isFirst;
extern QString mainUser;
extern QString mainIp;

Chat::Chat(QWidget *parent) :
    QWidget(parent), ui(new Ui::Chat) {
    ui->setupUi(this);
    //链接服务器
    socket = new QTcpSocket(this);
    socket->connectToHost(mainIp, 11451);
    ui->serverIpLabel->setText(mainIp);
    ui->userLabel->setText("用户："+mainUser);
    //链接槽函数获取服务器发回的信息
    connect(socket, SIGNAL(readyRead()), this, SLOT(readServer()));
}

Chat::~Chat() {
    delete ui;
}
void Chat::on_exitButton_clicked() {
    isFirst = false;
    auto *login = new Login();
    login->show();
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
