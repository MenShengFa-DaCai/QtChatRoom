
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
    // 初始化按钮状态
    ui->lastButton->setEnabled(false);
    ui->nextButton->setEnabled(false);
}

Message::~Message() { delete ui; }

void Message::highlightCurrentResult() {
    //清空上次查询
    clearHighlights();

    if (currentResultIndex < 0 || currentResultIndex >= searchResults.size())
        return;

    // 设置当前高亮格式
    QTextCharFormat currentFormat;
    currentFormat.setBackground(QColor(255, 165, 0)); // 橙色
    currentFormat.setForeground(Qt::black);

    // 获取当前匹配项并高亮
    QTextCursor cursor = searchResults[currentResultIndex];
    cursor.mergeCharFormat(currentFormat);

    // 滚动到可见位置
    ui->textBrowser->setTextCursor(cursor);
    ui->textBrowser->ensureCursorVisible();
}

void Message::clearHighlights() {
    if (currentKeyword.isEmpty()) return;

    QTextDocument* doc = ui->textBrowser->document();
    QTextCursor cursor(doc);

    // 清除所有高亮
    QTextCharFormat normalFormat;
    normalFormat.setBackground(Qt::transparent);

    cursor.beginEditBlock();
    while (!cursor.isNull() && !cursor.atEnd()) {
        cursor = doc->find(currentKeyword, cursor, QTextDocument::FindWholeWords | QTextDocument::FindCaseSensitively);
        if (!cursor.isNull()) {
            cursor.mergeCharFormat(normalFormat);
            cursor.movePosition(QTextCursor::WordRight, QTextCursor::KeepAnchor);
        }
    }
    cursor.endEditBlock();
}
// 收到消息做如下处理
void Message::readData() {
    // 读取所有可用数据
    QByteArray data = socket->readAll();
    incompleteData.append(data);
    // 处理所有完整行
    int pos;
    while ((pos = static_cast<int>(incompleteData.indexOf('\n'))) != -1) {
        QString line = incompleteData.left(pos).trimmed();
        incompleteData = incompleteData.mid(pos + 1);
        if (!line.isEmpty()) {
            ui->textBrowser->append(line);
        }
    }

}
//查询聊天记录
void Message::on_queryButton_clicked() {
    QString keyword = ui->keywordEdit->text().trimmed();
    if (keyword.isEmpty()) return;

    // 清除之前的结果
    clearHighlights();
    searchResults.clear();
    currentResultIndex = -1;
    currentKeyword = keyword;

    // 获取文档
    QTextDocument* doc = ui->textBrowser->document();
    QTextCursor cursor(doc);

    // 设置高亮格式
    QTextCharFormat highlightFormat;
    highlightFormat.setBackground(Qt::yellow);
    highlightFormat.setForeground(Qt::black);

    QTextDocument::FindFlags flags = QTextDocument::FindCaseSensitively;

    // 查找所有匹配项
    while (!cursor.isNull() && !cursor.atEnd()) {
        cursor = doc->find(keyword, cursor, flags);

        if (!cursor.isNull()) {
            searchResults.append(cursor);
            // cursor.movePosition(QTextCursor::WordRight, QTextCursor::KeepAnchor);
        }
    }

    // 更新按钮状态并显示结果
    if (!searchResults.isEmpty()) {
        currentResultIndex = 0;
        ui->lastButton->setEnabled(true);
        ui->nextButton->setEnabled(true);
        highlightCurrentResult();
    } else {
        ui->lastButton->setEnabled(false);
        ui->nextButton->setEnabled(false);
    }
}
//上一个
void Message::on_lastButton_clicked() {
    if (searchResults.isEmpty()) return;

    currentResultIndex--;
    if (currentResultIndex < 0) {
        currentResultIndex = searchResults.size() - 1; // 循环到最后一个
    }

    highlightCurrentResult();
}
//下一个
void Message::on_nextButton_clicked() {
    if (searchResults.isEmpty()) return;
    currentResultIndex++;
    if (currentResultIndex >= searchResults.size()) {
        currentResultIndex = 0; // 循环到第一个
    }
    highlightCurrentResult();
}
