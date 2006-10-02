/****************************************************************************
** SVDRP meta object code from reading C++ file 'svdrp.h'
**
** Created: Sun Oct 1 20:47:59 2006
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.6   edited Mar 8 17:43 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "svdrp.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *SVDRP::className() const
{
    return "SVDRP";
}

QMetaObject *SVDRP::metaObj = 0;
static QMetaObjectCleanUp cleanUp_SVDRP( "SVDRP", &SVDRP::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString SVDRP::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "SVDRP", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString SVDRP::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "SVDRP", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* SVDRP::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QObject::staticMetaObject();
    metaObj = QMetaObject::new_metaobject(
	"SVDRP", parentObject,
	0, 0,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_SVDRP.setMetaObject( metaObj );
    return metaObj;
}

void* SVDRP::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "SVDRP" ) )
	return this;
    return QObject::qt_cast( clname );
}

bool SVDRP::qt_invoke( int _id, QUObject* _o )
{
    return QObject::qt_invoke(_id,_o);
}

bool SVDRP::qt_emit( int _id, QUObject* _o )
{
    return QObject::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool SVDRP::qt_property( int id, int f, QVariant* v)
{
    return QObject::qt_property( id, f, v);
}

bool SVDRP::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES


const char *SVDRPRequest::className() const
{
    return "SVDRPRequest";
}

QMetaObject *SVDRPRequest::metaObj = 0;
static QMetaObjectCleanUp cleanUp_SVDRPRequest( "SVDRPRequest", &SVDRPRequest::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString SVDRPRequest::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "SVDRPRequest", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString SVDRPRequest::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "SVDRPRequest", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* SVDRPRequest::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QObject::staticMetaObject();
    static const QUMethod slot_0 = {"startRequest", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "startRequest()", &slot_0, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"SVDRPRequest", parentObject,
	slot_tbl, 1,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_SVDRPRequest.setMetaObject( metaObj );
    return metaObj;
}

void* SVDRPRequest::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "SVDRPRequest" ) )
	return this;
    if ( !qstrcmp( clname, "QThread" ) )
	return (QThread*)this;
    return QObject::qt_cast( clname );
}

bool SVDRPRequest::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: startRequest(); break;
    default:
	return QObject::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool SVDRPRequest::qt_emit( int _id, QUObject* _o )
{
    return QObject::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool SVDRPRequest::qt_property( int id, int f, QVariant* v)
{
    return QObject::qt_property( id, f, v);
}

bool SVDRPRequest::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
