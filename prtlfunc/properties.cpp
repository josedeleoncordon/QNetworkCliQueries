#include "properties.h"

Properties * Properties::m_instance = 0;

Properties * Properties::Instance()
{
    if (!m_instance)
    {
        m_instance = new Properties();
        m_instance->loadSettings();
    }
    return m_instance;
}

Properties::Properties()
{
}

Properties::~Properties()
{
    saveSettings();
    delete m_instance;
    m_instance = 0;
}

void Properties::loadSettings()
{
    QSettings settings;
    settings.setDefaultFormat(QSettings::NativeFormat);

    user = settings.value("ConnectionUser").toString();
    password = settings.value("ConnectionPassword").toString();
    queriespath = settings.value("QueriesPath").toString();
    mnemonicspath = settings.value("MnemonicsPath").toString();
    backupspath = settings.value("BackupsPath").toString();
    logspath = settings.value("LogsPath").toString();
}

void Properties::saveSettings()
{
    QSettings settings;
    settings.setDefaultFormat(QSettings::NativeFormat);

//    settings.beginGroup("Connection");
    settings.setValue("ConnectionUser",user);
    settings.setValue("ConnectionPassword",password);
//    settings.endGroup();


    settings.setValue("QueriesPath",queriespath);
    settings.setValue("MnemonicsPath",mnemonicspath);
    settings.setValue("BackupsPath",backupspath);
    settings.setValue("LogsPath",logspath);
}

