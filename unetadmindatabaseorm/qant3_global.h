#ifndef QANT3_GLOBAL_H
#define QANT3_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QANT3_LIBRARY)
#  define QANT3MVSHARED_EXPORT Q_DECL_EXPORT
#else
#  define QANT3MVSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // QANT3_GLOBAL_H