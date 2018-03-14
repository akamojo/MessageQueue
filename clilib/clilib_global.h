#ifndef CLILIB_GLOBAL_H
#define CLILIB_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(CLILIB_LIBRARY)
#  define CLILIBSHARED_EXPORT Q_DECL_EXPORT
#else
#  define CLILIBSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // CLILIB_GLOBAL_H
