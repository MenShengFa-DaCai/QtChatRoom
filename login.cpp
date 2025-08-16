#include "login.h"
#include "ui_Login.h"
extern bool isFirst;
QString mainIp;
QString mainUser;

Login::Login(QWidget *parent) :
    QDialog(parent), ui(new Ui::Login),m_dragging(false) {
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    //寻找数据库中所有记录的账号，在userComboBox的表中列出
    //创建数据库
    QString dbPath = QCoreApplication::applicationDirPath() + "/LocalUsers.db";
    userData=QSqlDatabase::addDatabase("QSQLITE");
    userData.setDatabaseName(dbPath);
    // 读取上次保存的服务器IP
    QSettings settings("QtChatRoom.ini", QSettings::IniFormat);
    QString lastServerIP = settings.value("Server/IP", "127.0.0.1").toString();
    ui->serverIP->setText(lastServerIP);
    if(userData.open()) {
        //qDebug记得注释掉
        // qDebug()<<"open";
        //创建表
        // 检查USERS表是否已存在
        //数据库操作函数
        checkLocalDatabase();
        connect(ui->userComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(changeUser(int)));
    }else {
        QMessageBox::critical(this,"警告","本地数据库异常，请检查。");
    }
}

Login::~Login() {
    delete ui;
}


//判断账户密码是否合理的函数
bool Login::userAndPasswordIsOk() {
    if (ui->passwordEdit->text().isEmpty()||ui->userComboBox->currentText().isEmpty()) {
        QMessageBox::critical(this,"注意！","账号或密码不得为空！");
        return false;
    }
    //检查账号密码中的空格，如果有不允许注册登陆
    static QRegularExpression re("\\s");
    if (ui->passwordEdit->text().contains(re)||ui->userComboBox->currentText().contains(re)) {
        QMessageBox::critical(this,"注意！","账号与密码中不允许包含空格类字符！");
        return false;
    }
    //排除过长的用户名
    if (ui->userComboBox->currentText().length()>20||ui->passwordEdit->text().length()>20) {
        QMessageBox::critical(this,"注意！","账号和密码均不得超过20个字符！");
        return false;
    }
    return true;
}

void Login::on_closeButton_clicked() {
    close();
}
//登陆按钮按下，处理登陆逻辑
void Login::on_loginButton_clicked() {
    //建立连接
    if (!userAndPasswordIsOk()) {
        return;
    }
    tcpSocket = new QTcpSocket(this);
    //连接信号槽，判断成功还是失败
    connect(tcpSocket,SIGNAL(errorOccurred(QAbstractSocket::SocketError)),
        this,SLOT(connectError(QAbstractSocket::SocketError)));
    connect(tcpSocket,SIGNAL(connected()),this,SLOT(connectLongin()));
    tcpSocket->connectToHost(ui->serverIP->text(), 11451);
}
//注册按钮按下，处理注册逻辑
void Login::on_registerButton_clicked() {
    //和登陆一样
    if (!userAndPasswordIsOk()) {
        return;
    }
    //创建套接字
    tcpSocket = new QTcpSocket(this);
    //连接信号槽，判断成功还是失败
    connect(tcpSocket,SIGNAL(errorOccurred(QAbstractSocket::SocketError)),
        this,SLOT(connectError(QAbstractSocket::SocketError)));
    connect(tcpSocket,SIGNAL(connected()),this,SLOT(connectRegister()));
    tcpSocket->connectToHost(ui->serverIP->text(), 11451);
}

//连接失败（包括注册和登陆）
void Login::connectError(QAbstractSocket::SocketError error) {
    QMessageBox::critical(this,"服务器掉线",tcpSocket->errorString());
    delete tcpSocket;
}

//连接成功（此次登陆和注册都会连接，分开处理）
//登陆
void Login::connectLongin() {
    //先连接收服务器信息的槽函数
    QTextStream out(tcpSocket);
    //发送完账号和密码，收到服务端发来的成功或者失败的信息后发送请求断开链接
    connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(readServer()));
    //发送操作码
    //登录逻辑
    out << "LOGIN ";
    //向服务器发送账号密码
    out << ui->userComboBox->currentText();
    out << " ";
    out << ui->passwordEdit->text();
    out.flush();
}
//注册
void Login::connectRegister() {
    //同样先连接槽函数
    QTextStream out(tcpSocket);
    //发送完账号和密码，收到服务端发来的成功或者失败的信息后发送请求断开链接
    connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(readServer()));
    //发送操作码
    //注册逻辑
    out << "REGISTER ";
    out << ui->userComboBox->currentText();
    out << " ";
    out << ui->passwordEdit->text();
    out.flush();
}

// 实现自动登录勾选则自动勾选记住密码的槽函数
void Login::on_autoLoginCheck_stateChanged(int state) {
    // 当自动登录被勾选时，自动勾选记住密码
    if (state == Qt::Checked) {
        ui->rememberCheck->setChecked(true);
    }
}

void Login::readServer() {
    //服务器反馈发出后，处理服务器信息
    //这边收到服务器信息后应该先断开连接
    //服务器反馈发出后，处理服务器信息
    QTextStream in(tcpSocket);
    QString response = in.readAll();

    // 根据响应类型处理
    if (response == "LOGIN SUCCESS") {
        QMessageBox::information(this, "登录成功", "登录成功！");
        // 保存服务器IP
        QSettings settings(QSettings::IniFormat, QSettings::UserScope, "QtChatRoom", "QtChatRoom");
        settings.setValue("Server/IP", ui->serverIP->text());
        //数据库更新
        localDatabaseUpdates();
        mainIp=ui->serverIP->text();
        mainUser=ui->userComboBox->currentText();

        // 使用同一个socket连接进行聊天
        chat = new Chat(tcpSocket);  // 将socket传递给聊天窗口
        chat->show();
        // qDebug()<<"打开聊天";
        //发送完账号和密码，收到服务端发来的成功或者失败的信息后发送请求断开链接
        disconnect(tcpSocket,SIGNAL(readyRead()),this,SLOT(readServer()));
        close();
    } else if (response == "LOGIN FAILED") {
        QMessageBox::critical(this, "登录失败", "该用户已在线，或者用户名或密码错误！");
        // 断开连接
        disconnectAfterResponse();
    } else if (response == "REGISTER SUCCESS") {
        QMessageBox::information(this, "注册成功", "注册成功！");
        // 断开连接
        disconnectAfterResponse();
    } else if (response == "REGISTER FAILED") {
        QMessageBox::critical(this, "注册失败", "该用户名已被注册！");
        // 断开连接
        disconnectAfterResponse();
    } else {
        QMessageBox::critical(this, "未知响应", "收到未知服务器响应: " + response);
        // 断开连接
        disconnectAfterResponse();
    }
}

void Login::on_forgotButton_clicked() {
    QMessageBox::critical(this,"提示","请联系服务器提供者获取您的密码信息！");
}

//设置下拉复选框后，密码重新填充，自动登陆和记住密码复原
void Login::changeUser(int) {
    QSqlQuery query;
    query.prepare("SELECT * FROM USERS WHERE id=?;");
    query.bindValue(0, ui->userComboBox->currentText());
    if (query.exec()) {
        // qDebug()<<"查询指令完成";
        if (query.next()) {
            ui->rememberCheck->setChecked(query.value(3).toBool());
            ui->autoLoginCheck->setChecked(query.value(2).toBool());
            if (ui->rememberCheck->isChecked()) {
                ui->passwordEdit->setText(query.value(1).toString());
            }else {
                ui->passwordEdit->setText("");
            }
        }else {
            // qDebug()<< "数据库里没有？";
        }
    }
}

//处理服务器响应后断开连接
void Login::disconnectAfterResponse() {
    if (tcpSocket && tcpSocket->state() == QAbstractSocket::ConnectedState) {
        tcpSocket->disconnectFromHost();
        tcpSocket->deleteLater();
        tcpSocket = nullptr;
    }
}



// 检查数据库
//在重新选择下拉菜单的账号后密码并不会更新
void Login::checkLocalDatabase() {
    QSqlQuery checkQuery;
    if (checkQuery.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='USERS';")) {
        if (checkQuery.next()) {
            // 表已存在，直接打开
            // qDebug()<<"check";
            //打开表的情况下，寻找数据库中所有记录的账号，在userComboBox的表中列出
            //另外，如果有账号勾选了记住密码，数据库中记录的密码应填入passwordEdit
            //如果上次登陆勾选了自动登陆，则判断是否为主动退出登陆，如果不是则自动登陆账号
            // 查询所有账号并添加到下拉框
            QSqlQuery userQuery("SELECT id FROM USERS;");
            while (userQuery.next()) {
                QString account = userQuery.value(0).toString();
                ui->userComboBox->addItem(account);
            }
            // 检查是否有自动登陆的账号
            QSqlQuery rememberQuery("SELECT id, passport FROM USERS WHERE automatic = 1;");
            if (rememberQuery.next()) {
                // 找到一个自动登陆的账号
                QString account = rememberQuery.value(0).toString();
                QString password = rememberQuery.value(1).toString();
                // 设置下拉框选中该账号
                int index = ui->userComboBox->findText(account);
                if (index != -1) {
                    ui->userComboBox->setCurrentIndex(index);
                }
                // 填充密码
                ui->passwordEdit->setText(password);
                // 勾选记住密码
                ui->rememberCheck->setChecked(true);
                //执行自动登录逻辑
                //如果不是点击退出账号调起的窗口，自动登录
                if (isFirst) {
                    QSqlQuery clearAuto;
                    clearAuto.prepare("UPDATE USERS SET automatic = 0;");
                    if (clearAuto.exec()) {
                        // qDebug()<<"clearAuto";
                    }
                    ui->autoLoginCheck->setChecked(true);
                    ui->loginButton->click();
                }else {
                    QSqlQuery clearAuto;
                    clearAuto.prepare("UPDATE USERS SET automatic = 0;");
                    if (clearAuto.exec()) {
                        // qDebug()<<"clearAuto";
                    }
                }
            }else {
                //没有自动登录账号走这里
                //几乎完全一样，但不执行自动登陆
                QSqlQuery rememberNot("SELECT * FROM USERS;");
                if (rememberNot.next()) {
                    QString account = rememberNot.value(0).toString();
                    QString password = rememberNot.value(3).toBool()?rememberNot.value(1).toString():"";
                    bool r=rememberNot.value(3).toBool();
                    int index = ui->userComboBox->findText(account);
                    if (index != -1) {
                        ui->userComboBox->setCurrentIndex(index);
                    }
                    ui->passwordEdit->setText(password);
                    ui->rememberCheck->setChecked(r);
                }
            }
        } else {
            // 表不存在，创建新表
            //最后一列储存是否记住密码，倒数第二列储存是否自动登录
            QString createSql = "CREATE TABLE USERS(id TEXT PRIMARY KEY, passport TEXT,automatic INTEGER,remember INTEGER);";
            QSqlQuery createQuery;
            if (createQuery.exec(createSql)) {
                //建表完成
                // qDebug()<<"create";
            } else {
                //建表失败
                // qDebug()<<"failed";
            }
        }
    } else {
        //检查语句执行失败
        // qDebug()<<"数据库检查失败";
    }
}

//更新数据库
void Login::localDatabaseUpdates() {
    //登陆成功后关闭登陆界面，打开聊天室主界面，记录一下账号放在数据库中
    QSqlQuery query;
    //预处理
    query.prepare("INSERT INTO USERS VALUES(?,?,?,?);");
    query.bindValue(0,ui->userComboBox->currentText());
    query.bindValue(1,ui->passwordEdit->text());
    query.bindValue(2,ui->rememberCheck->isChecked());
    query.bindValue(3,ui->autoLoginCheck->isChecked());
    //执行命令
    if (query.exec()) {
        // qDebug()<<"已将账号信息加入数据库";
    }else {
        // qDebug()<<"添加命令执行失败";
        //到这里大概就是数据库中已经存在，所以更新数据
        query.clear();
        query.prepare("UPDATE USERS SET passport=?, automatic=?, remember=? WHERE id=?");
        query.bindValue(0,ui->passwordEdit->text());
        query.bindValue(1,ui->autoLoginCheck->isChecked());
        query.bindValue(2,ui->rememberCheck->isChecked());
        query.bindValue(3,ui->userComboBox->currentText());
        //执行命令
        if (query.exec()) {
            //更新成功
            // qDebug()<<"更新成功";
        }else {
            // qDebug()<<"更新失败";
        }
    }
}


//下面是拖动窗口的逻辑
void Login::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        // 获取鼠标点击位置下的控件
        QWidget* child = childAt(event->pos());

        // 检查点击位置是否在关键组件内
        bool isCriticalComponent = false;

        // 列出所有关键组件（需要正常交互的组件）
        QList<QWidget*> criticalWidgets = {
            ui->closeButton,
            ui->loginButton,
            ui->registerButton,
            ui->forgotButton,
            ui->userComboBox,
            ui->passwordEdit,
            ui->rememberCheck,
            ui->autoLoginCheck
        };

        // 检查点击的组件或其父组件是否在关键组件列表中
        QWidget* widget = child;
        while (widget && widget != this) {
            if (criticalWidgets.contains(widget)) {
                isCriticalComponent = true;
                break;
            }
            widget = widget->parentWidget();
        }

        // 如果不是关键组件，则开始拖动
        if (!isCriticalComponent) {
            m_dragging = true;
            m_dragStartPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
            event->accept();
            return;
        }
    }
    QDialog::mousePressEvent(event);
}

void Login::mouseMoveEvent(QMouseEvent* event) {
    if (m_dragging && event->buttons() & Qt::LeftButton) {
        QPoint newPosition = event->globalPosition().toPoint() - m_dragStartPosition;
        move(newPosition);
        event->accept();
        return;
    }
    QDialog::mouseMoveEvent(event);
}

void Login::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
        event->accept();
    }
    QDialog::mouseReleaseEvent(event);
}

