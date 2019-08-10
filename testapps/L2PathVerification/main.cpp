#include "mainwindow.h"
#include <QApplication>
#include <QDir>

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString txt;
    QString filename;
    switch (type) {
    case QtInfoMsg:
        txt = QString("%1").arg(msg);
        filename = QDir::toNativeSeparators(QDir::homePath()+"/info_.txt");
        break;

    case QtDebugMsg:
    case QtWarningMsg:
        txt = QString("%1").arg(msg);
        filename = QDir::toNativeSeparators(QDir::homePath()+"/debug_.txt");
        break;
    }

    if (txt.isEmpty())
        return;

    QFile outFile(filename);
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << txt << endl;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFile::remove(QDir::toNativeSeparators(QDir::homePath()+"/info_.txt"));
    QFile::remove(QDir::toNativeSeparators(QDir::homePath()+"/debug_.txt"));

//    qInstallMessageHandler(messageHandler);
    MainWindow w;
    w.show();

    return a.exec();
}
