/****************************************************************************
** GlobalKeyHandler meta object code from reading C++ file 'globalkeyhandler.h'
**
** Created: Tue Dec 5 17:33:41 2006
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.6   edited Mar 8 17:43 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "globalkeyhandler.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *GlobalKeyHandler::className() const
{
    return "GlobalKeyHandler";
}

QMetaObject *GlobalKeyHandler::metaObj = 0;
static QMetaObjectCleanUp cleanUp_GlobalKeyHandler( "GlobalKeyHandler", &GlobalKeyHandler::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString GlobalKeyHandler::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "GlobalKeyHandler", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString GlobalKeyHandler::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "GlobalKeyHandler", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* GlobalKeyHandler::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QObject::staticMetaObject();
    metaObj = QMetaObject::new_metaobject(
	"GlobalKeyHandler", parentObject,
	0, 0,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_GlobalKeyHandler.setMetaObject( metaObj );
    return metaObj;
}

void* GlobalKeyHandler::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "GlobalKeyHandler" ) )
	return this;
    return QObject::qt_cast( clname );
}

bool GlobalKeyHandler::qt_invoke( int _id, QUObject* _o )
{
    return QObject::qt_invoke(_id,_o);
}

bool GlobalKeyHandler::qt_emit( int _id, QUObject* _o )
{
    return QObject::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool GlobalKeyHandler::qt_property( int id, int f, QVariant* v)
{
    return QObject::qt_property( id, f, v);
}

bool GlobalKeyHandler::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
