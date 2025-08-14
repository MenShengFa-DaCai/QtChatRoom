
#include "message.h"
#include "ui_Message.h"
extern QString mainIp;
Message::Message(QWidget* parent) : QDialog(parent), ui(new Ui::Message) {
    ui->setupUi(this);
    //套接字链接服务器
    socket = new QTcpSocket();
    socket->connectToHost(mainIp, 11451);
    // 发消息给服务器，请求聊天记录
    QTextStream out(socket);
    //链接收到消息的槽函数
    connect(socket, SIGNAL(readyRead()), this, SLOT(readData()));
    out<<"HISTORY";
}

Message::~Message() { delete ui; }

//收到消息做如下处理
void Message::readData() {
    // 读取所有可用数据
    QByteArray data = socket->readAll();
    incompleteData.append(data);
    // 处理所有完整行
    int pos;
    while ((pos = incompleteData.indexOf('\n')) != -1) {
        QString line = incompleteData.left(pos).trimmed();
        incompleteData = incompleteData.mid(pos + 1);
        if (!line.isEmpty()) {
            ui->textBrowser->append(line);
        }
    }

}
//查询聊天记录
void Message::on_queryButton_clicked() {

}
//上一个
void Message::on_lastButton_clicked() {

}
//下一个
void Message::on_nextButton_clicked() {

}
