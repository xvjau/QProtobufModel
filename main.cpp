
#include <QGuiApplication>
#include <QQuickWindow>
#include <QQmlEngine>
#include <QQmlComponent>

#include <test.pb.h>
#include <fstream>

#include <iostream>

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

        QQmlComponent *component = new QQmlComponent(&engine);
        component->loadUrl(mainWindowUrl);

        QObject *topLevel = component->create();
        QQuickWindow *window = qobject_cast<QQuickWindow *>(topLevel);

        window->show();

        return app.exec();
    }
    catch(const std::exception &e)
    {
        std::cerr << "Unknown error: " << e.what() << std::endl;
        return 1;
    }
}
