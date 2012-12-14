#ifndef AMBQT_GLOBAL_H
#define AMBQT_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(AMBQT_LIBRARY)
#  define AMBQTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define AMBQTSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // AMBQT_GLOBAL_H
