#ifndef QUERIESMESSAGEHANDLER_H
#define QUERIESMESSAGEHANDLER_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QMutex>

class QueriesMessageHandler
{
public:
    static void initHandler(QString path);
    static QueriesMessageHandler* instance();
    void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

private:
    QueriesMessageHandler(QString path);
    ~QueriesMessageHandler();
    static QueriesMessageHandler *m_instance;
    struct log
    {
        QString ip;
        QFile *file;
        QTextStream *ts;
    };

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
