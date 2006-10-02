/****************************************************************************
** JAMTV meta object code from reading C++ file 'jamtv.h'
**
** Created: Sun Oct 1 22:06:48 2006
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.6   edited Mar 8 17:43 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "jamtv.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *JAMTV::className() const
{
    return "JAMTV";
}

QMetaObject *JAMTV::metaObj = 0;
static QMetaObjectCleanUp cleanUp_JAMTV( "JAMTV", &JAMTV::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString JAMTV::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "JAMTV", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString JAMTV::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "JAMTV", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* JAMTV::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = Menu::staticMetaObject();
    metaObj = QMetaObject::new_metaobject(
	"JAMTV", parentObject,
	0, 0,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_JAMTV.setMetaObject( metaObj );
    return metaObj;
}

void* JAMTV::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "JAMTV" ) )
	return this;
    return Menu::qt_cast( clname );
}

bool JAMTV::qt_invoke( int _id, QUObject* _o )
{
    return Menu::qt_invoke(_id,_o);
}

bool JAMTV::qt_emit( int _id, QUObject* _o )
{
    return Menu::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool JAMTV::qt_property( int id, int f, QVariant* v)
{
    return Menu::qt_property( id, f, v);
}

bool JAMTV::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
