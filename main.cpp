#include <QApplication>
#include "login.h"
#include <QIcon>
#include <QDir>
#include <QLibraryInfo>

bool isFirst=true;
int main(int argc, char* argv[]) {
    // 设置插件路径
    QApplication::addLibraryPath(QCoreApplication::applicationDirPath() + "/plugins");
    QApplication a(argc, argv);
    qputenv("QT_PLUGIN_PATH", QCoreApplication::applicationDirPath().toUtf8() + "/plugins");

    auto *login = new Login();
    login->show();
    return QApplication::exec();
}
