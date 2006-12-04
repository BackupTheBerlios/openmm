/****************************************************************************
** Tv meta object code from reading C++ file 'tv.h'
**
** Created: Tue Dec 5 17:33:46 2006
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.6   edited Mar 8 17:43 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "tv.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *Tv::className() const
{
    return "Tv";
}

QMetaObject *Tv::metaObj = 0;
static QMetaObjectCleanUp cleanUp_Tv( "Tv", &Tv::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString Tv::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "Tv", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString Tv::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "Tv", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* Tv::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = Module::staticMetaObject();
    metaObj = QMetaObject::new_metaobject(
	"Tv", parentObject,
	0, 0,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_Tv.setMetaObject( metaObj );
    return metaObj;
}

void* Tv::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "Tv" ) )
	return this;
    return Module::qt_cast( clname );
}

bool Tv::qt_invoke( int _id, QUObject* _o )
{
    return Module::qt_invoke(_id,_o);
}

bool Tv::qt_emit( int _id, QUObject* _o )
{
    return Module::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool Tv::qt_property( int id, int f, QVariant* v)
{
    return Module::qt_property( id, f, v);
}

bool Tv::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
