TEMPLATE  = subdirs
CONFIG   += ordered

SUBDIRS = qremoteshell qnetworkcliqueries
exists(uftapps/uftapps.pro): SUBDIRS += uftapps
