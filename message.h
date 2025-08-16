//
// Created by 1 on 11 8月 2025.
//

#ifndef QTCHATROOM_MESSAGE_H
#define QTCHATROOM_MESSAGE_H

#include <QDialog>
#include <QTcpSocket>
#include <QTextCursor>
#include <QTextCharFormat>

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

    //查找聊天记录的相关变量与函数
    QList<QTextCursor> searchResults; // 存储所有匹配的光标位置
    int currentResultIndex{};            // 当前高亮结果的索引
    QString currentKeyword;            // 当前搜索关键字
    void highlightCurrentResult();     // 高亮显示当前匹配项
    void clearHighlights();            // 清除所有高亮

private slots:
    void readData();
    void on_queryButton_clicked();
    void on_lastButton_clicked();
    void on_nextButton_clicked();
};

#endif // QTCHATROOM_MESSAGE_H
