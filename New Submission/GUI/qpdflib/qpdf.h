#ifndef QPDF_H
#define QPDF_H

#ifdef QPDFLIB_BUILD
#   define QPDF_EXPORT  Q_DECL_EXPORT
#else
#   define QPDF_EXPORT  Q_DECL_IMPORT
#endif

#endif // QPDF_H
