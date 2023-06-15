#ifndef PROTOCOL_GLOBAL_H
#define PROTOCOL_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(PROTOCOL_LIBRARY)
#  define PROTOCOLSHARED_EXPORT Q_DECL_EXPORT
#else
#  define PROTOCOLSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // PROTOCOL_GLOBAL_H
