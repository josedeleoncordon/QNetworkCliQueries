#ifndef RECORDFACTORY_H
#define RECORDFACTORY_H

#include "recordbase.h"
#include "recordfactorybase.h"
#include "qant3_global.h"
#include <QString>

using namespace QAnt;
using namespace Dao;

class QANT3MVSHARED_EXPORT RecordFactory: public RecordFactoryBase
{
public:
	RecordBase *newRecord(QString table);
};

#endif
