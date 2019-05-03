include(../base.pri)

TEMPLATE = lib
TARGET = qssh
QT += network widgets
DEFINES += QSSH_LIBRARY

DESTDIR = $$APP_BUILD_TREE

CONFIG += shared dll

LIBS += -lbotan-2

#Enable debug log
#DEFINES += CREATOR_SSH_DEBUG

INCLUDEPATH += /usr/include/botan-2/

SOURCES = \
    $$PWD/sftpchannel.cpp \
    $$PWD/sftpdefs.cpp \
    $$PWD/sftpfilesystemmodel.cpp \
    $$PWD/sftpincomingpacket.cpp \
    $$PWD/sftpoperation.cpp \
    $$PWD/sftpoutgoingpacket.cpp \
    $$PWD/sftppacket.cpp \
    $$PWD/sshagent.cpp \
    $$PWD/sshcapabilities.cpp \
    $$PWD/sshchannel.cpp \
    $$PWD/sshchannelmanager.cpp \
    $$PWD/sshconnection.cpp \
    $$PWD/sshconnectionmanager.cpp \
    $$PWD/sshcryptofacility.cpp \
    $$PWD/sshdirecttcpiptunnel.cpp \
    $$PWD/sshforwardedtcpiptunnel.cpp \
    $$PWD/sshhostkeydatabase.cpp \
    $$PWD/sshincomingpacket.cpp \
    $$PWD/sshkeyexchange.cpp \
    $$PWD/sshkeygenerator.cpp \
    $$PWD/sshkeypasswordretriever.cpp \
    $$PWD/sshlogging.cpp \
    $$PWD/sshoutgoingpacket.cpp \
    $$PWD/sshpacket.cpp \
    $$PWD/sshpacketparser.cpp \
    $$PWD/sshremoteprocess.cpp \
    $$PWD/sshremoteprocessrunner.cpp \
    $$PWD/sshsendfacility.cpp \
    $$PWD/sshtcpipforwardserver.cpp \
    $$PWD/sshtcpiptunnel.cpp

HEADERS = \
    $$PWD/sftpchannel_p.h \
    $$PWD/sftpchannel.h \
    $$PWD/sftpdefs.h \
    $$PWD/sftpfilesystemmodel.h \
    $$PWD/sftpincomingpacket_p.h \
    $$PWD/sftpoperation_p.h \
    $$PWD/sftpoutgoingpacket_p.h \
    $$PWD/sftppacket_p.h \
    $$PWD/ssh_global.h \
    $$PWD/sshagent_p.h \
    $$PWD/sshbotanconversions_p.h \
    $$PWD/sshcapabilities_p.h \
    $$PWD/sshchannel_p.h \
    $$PWD/sshchannelmanager_p.h \
    $$PWD/sshconnection_p.h \
    $$PWD/sshconnection.h \
    $$PWD/sshconnectionmanager.h \
    $$PWD/sshcryptofacility_p.h \
    $$PWD/sshdirecttcpiptunnel_p.h \
    $$PWD/sshdirecttcpiptunnel.h \
    $$PWD/ssherrors.h \
    $$PWD/sshexception_p.h \
    $$PWD/sshforwardedtcpiptunnel_p.h \
    $$PWD/sshforwardedtcpiptunnel.h \
    $$PWD/sshhostkeydatabase.h \
    $$PWD/sshincomingpacket_p.h \
    $$PWD/sshkeyexchange_p.h \
    $$PWD/sshkeygenerator.h \
    $$PWD/sshkeypasswordretriever_p.h \
    $$PWD/sshlogging_p.h \
    $$PWD/sshoutgoingpacket_p.h \
    $$PWD/sshpacket_p.h \
    $$PWD/sshpacketparser_p.h \
    $$PWD/sshpseudoterminal.h \
    $$PWD/sshremoteprocess_p.h \
    $$PWD/sshremoteprocess.h \
    $$PWD/sshremoteprocessrunner.h \
    $$PWD/sshsendfacility_p.h \
    $$PWD/sshtcpipforwardserver_p.h \
    $$PWD/sshtcpipforwardserver.h \
    $$PWD/sshtcpiptunnel_p.h


