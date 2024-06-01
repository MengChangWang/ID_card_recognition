#include "ID_card_recognition.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ID_card_recognition w;
    w.show();
    return a.exec();
}
