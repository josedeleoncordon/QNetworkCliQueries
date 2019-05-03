#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <QSettings>

#include "prtlfunc_global.h"

class QPRTLFUNCSHARED_EXPORT Properties
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

        void loadSettings();

    private:
        static Properties *m_instance;

        explicit Properties();
        Properties(const Properties &) {}
        ~Properties();
};

#endif

