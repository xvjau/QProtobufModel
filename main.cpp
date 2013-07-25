#include <QtGui/QApplication>
#include "ProtobufModel.h"


int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    ProtobufModel foo;
    foo.show();
    return app.exec();
}
