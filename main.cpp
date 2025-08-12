#include <QApplication>
#include "login.h"
bool isFirst=true;
int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    auto *login = new Login();
    login->show();
    return QApplication::exec();
}
