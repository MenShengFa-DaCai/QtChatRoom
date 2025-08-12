
#include "message.h"
#include "ui_Message.h"

Message::Message(QWidget* parent) : QDialog(parent), ui(new Ui::Message) {
    ui->setupUi(this);
    // 发消息给服务器，请求聊天记录
}

Message::~Message() { delete ui; }
