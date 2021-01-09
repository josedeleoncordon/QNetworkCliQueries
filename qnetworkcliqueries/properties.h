#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <QSettings>

#include "qnetworkcliqueries_global.h"

class QNETWORKCLIQUERIES_EXPORT Properties
{
    public:
        static Properties *Instance();
        void saveSettings();

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

    private:
        static Properties *m_instance;

        explicit Properties();
        Properties(const Properties &) {}
        ~Properties();

        void loadSettings();
};

#endif

