#include "queriesdao.h"
#include "funciones.h"
#include "factory.h"
#include "properties.h"
#include "qnetworkquerieslogging.h"

QueriesDAO::QueriesDAO(Queries *qry, QObject *parent) : QObject(parent)
{
    _iniciar();
    m_qry = qry;
    m_user = Properties::Instance()->user;
    m_pwd = Properties::Instance()->password;
    m_linuxprompt = Properties::Instance()->linuxpromt;
}

QueriesDAO::QueriesDAO(Queries *qry, QString user, QString pwd, QString linuxprompt, QObject *parent) : QObject(parent)
{
    _iniciar();
    m_qry=qry;
    m_user=user;
    m_pwd=pwd;
    m_linuxprompt=linuxprompt;
}

QueriesDAO::~QueriesDAO()
{}

void QueriesDAO::_iniciar()
{
    term = nullptr;
    m_connectionprotol = QRemoteShell::SSHTelnet;
}
