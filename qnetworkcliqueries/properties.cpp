#include "properties.h"

Properties * Properties::m_instance = nullptr;

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
    savelogs=false;
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

    settings.beginGroup("Connection");
    user = settings.value("ConnectionUser").toString();
    password = settings.value("ConnectionPassword").toString();
    settings.endGroup();

    settings.beginGroup("Queries");
    queriespath = settings.value("QueriesPath").toString();
    logspath = settings.value("LogsPath").toString();
    linuxpromt = settings.value("LinuxPropmt").toString();
#ifdef QT_DEBUG
    savelogs = settings.value("savelogs").toBool();
#endif
    settings.endGroup();

    settings.beginGroup("Topology");
    mnemonicspath = settings.value("MnemonicsPath").toString();
    topologiaconfpath = settings.value("topologiaconfpath").toString();
    topologiaiconpath = settings.value("topologiaiconpath").toString();
    settings.endGroup();
}

void Properties::saveSettings()
{
    QSettings settings;
    settings.setDefaultFormat(QSettings::NativeFormat);

    settings.beginGroup("Connection");
    settings.setValue("ConnectionUser",user);
    settings.setValue("ConnectionPassword",password);
    settings.endGroup();

    settings.beginGroup("Queries");
    settings.setValue("QueriesPath",queriespath);
    settings.setValue("LogsPath",logspath);
    settings.setValue("LinuxPropmt",linuxpromt);
    settings.setValue("savelogs",savelogs);
    settings.endGroup();

    settings.beginGroup("Topology");
    settings.setValue("MnemonicsPath",mnemonicspath);
    settings.setValue("topologiaconfpath",topologiaconfpath);
    settings.setValue("topologiaiconpath",topologiaiconpath);
    settings.endGroup();
}

