
#include <QGuiApplication>
#include <QQuickWindow>
#include <QQmlEngine>
#include <QQmlComponent>

#include <test.pb.h>
#include <fstream>

#include "qprotobufmodel.h"

#include <iostream>

std::ostream& operator<<(std::ostream& str, const QString& _string)
{
    str << _string.toLatin1().data();
    return str;
}

int main(int argc, char** argv)
{
    QGuiApplication app(argc, argv);

    ::pb::DateTest dateTest;

    auto d = dateTest.add_date();
    d->set_day(1);
    d->set_month(11);
    d->set_year(2013);

    d = dateTest.add_date();
    d->set_day(12);
    d->set_month(1);
    d->set_year(2011);

    d = dateTest.add_date();
    d->set_day(5);
    d->set_month(7);
    d->set_year(2013);

    d = dateTest.add_date();
    d->set_day(19);
    d->set_month(2);
    d->set_year(2012);

    d = dateTest.add_date();
    d->set_day(12);
    d->set_month(8);
    d->set_year(2013);

    d = dateTest.add_date();
    d->set_day(20);
    d->set_month(10);
    d->set_year(2013);

    d = dateTest.add_date();
    d->set_day(1);
    d->set_month(11);
    d->set_year(2013);


    std::ofstream of("/tmp/test_data.pb");
    dateTest.SerializeToOstream(&of);
    of.close();

    try
    {
        QUrl mainWindowUrl("qrc:/qml/Test.qml");

        QQmlEngine engine;
        QObject::connect(&engine, SIGNAL(quit()), QCoreApplication::instance(), SLOT(quit()));

        qmlRegisterType<QProtobufModel>("br.com.bitforge", 1, 0, "ProtobufModel");

        QQmlComponent *component = new QQmlComponent(&engine);
        component->loadUrl(mainWindowUrl);

        QObject *topLevel = component->create();
        if (component->isError())
        {
            std::cerr << "QML loading error:\n";
            for(auto error : component->errors())
            {
                std::cerr << error.url().toString() << ':' << error.line() << ':' << error.column() << " - " << error.description() << "\n";
            }
            std::cerr << std::endl;

            return 2;
        }
        else
        {
            QQuickWindow *window = qobject_cast<QQuickWindow *>(topLevel);

            window->show();

            return app.exec();
        }
    }
    catch(const std::exception &e)
    {
        std::cerr << "Unknown error: " << e.what() << std::endl;
        return 1;
    }
}
