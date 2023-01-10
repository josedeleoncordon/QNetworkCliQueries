#ifndef QUERIESMESSAGEHANDLER_H
#define QUERIESMESSAGEHANDLER_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QMutex>
#include <QTime>
#include <QLoggingCategory>

class QueriesMessageHandler
{
public:
    static void initHandler(QString path, bool savelogs = true);
    static QueriesMessageHandler* instance();
    void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);    

private:
    QueriesMessageHandler(QString path,bool savelogs);
    ~QueriesMessageHandler();

    void close(); //para cerrar todos los archivos al finalizar
    void init(); //para limpiar todos los archivos de una sesion anterior

    static QueriesMessageHandler *m_instance;
    struct log
    {
        QString ip;
        QFile *file;
        QTextStream *ts;
    };

    QTime _time;

    bool _savelogs;
    QFile _dbgFie;
    QTextStream _dbgS;
    QFile _otherFie;
    QTextStream _otherS;

    QList<log*> _lstLogs;
    QRegExp _expIP;
    QString _path;
    QMap<QString,QString> _mapIPnumero;
    int _c;
    QMutex _mutex;   
};

void queriesMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

#endif // QUERIESMESSAGEHANDLER_H
