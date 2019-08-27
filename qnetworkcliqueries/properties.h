#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <QSettings>

#include "qnetworkcliqueries_global.h"

class QNETWORKCLIQUERIES_EXPORT Properties
{
    public:
        static Properties *Instance();
        void saveSettings();

        QString user;
        QString password;
        QString queriespath;
        QString mnemonicspath;
        QString backupspath;
        QString logspath;
        QString linuxpromt;

    private:
        static Properties *m_instance;

        explicit Properties();
        Properties(const Properties &) {}
        ~Properties();

        void loadSettings();
};

#endif

