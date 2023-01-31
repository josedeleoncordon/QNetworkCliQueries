#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <QSettings>
#include <QFile>

#include "qnetworkcliqueries_global.h"

class QNETWORKCLIQUERIES_EXPORT PropertiesEncoderDecoder
{
public:
    virtual QString encode(QString);
    virtual QString decode(QString);
};

PropertiesEncoderDecoder *newPropertiesEncoderDecoder();

class QNETWORKCLIQUERIES_EXPORT Properties
{
    public:
        static Properties *Instance();
        void saveSettings();
        void loadSettings();

        bool savelogs;
        QString user;
        QString password;
        QString queriespath;
        QString mnemonicspath;
        QString backupspath;
        QString logspath;
        QString linuxpromt;

        QString topologiaconfpath;
        QString topologiaiconpath;

        QString bduser;
        QString bdpassword;
        QString bd;
        QString bdhost;

        QMap<QString,QString> mapFuncionesArgumentos;
        QMap<QString,QVariant> mapGrupoRaizIP;

        void setNewEnconderDecoder(PropertiesEncoderDecoder*(*encoderdecoder)(void));

    private:
        static Properties *m_instance;

        explicit Properties();
        Properties(const Properties &) {}
        ~Properties();

        PropertiesEncoderDecoder*(*ed)(void) = nullptr;
};

#endif

