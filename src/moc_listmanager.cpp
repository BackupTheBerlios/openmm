/****************************************************************************
** ListManager meta object code from reading C++ file 'listmanager.h'
**
** Created: Tue Dec 5 17:33:51 2006
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.6   edited Mar 8 17:43 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "listmanager.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *ListManager::className() const
{
    return "ListManager";
}

QMetaObject *ListManager::metaObj = 0;
static QMetaObjectCleanUp cleanUp_ListManager( "ListManager", &ListManager::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString ListManager::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "ListManager", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString ListManager::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "ListManager", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* ListManager::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QObject::staticMetaObject();
    static const QUParameter param_signal_0[] = {
	{ "title", &static_QUType_ptr, "Title", QUParameter::In }
    };
    static const QUMethod signal_0 = {"pushTitle", 1, param_signal_0 };
    static const QUParameter param_signal_1[] = {
	{ "title", &static_QUType_ptr, "Title", QUParameter::In }
    };
    static const QUMethod signal_1 = {"popTitle", 1, param_signal_1 };
    static const QMetaData signal_tbl[] = {
	{ "pushTitle(Title*)", &signal_0, QMetaData::Public },
	{ "popTitle(Title*)", &signal_1, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"ListManager", parentObject,
	0, 0,
	signal_tbl, 2,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_ListManager.setMetaObject( metaObj );
    return metaObj;
}

void* ListManager::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "ListManager" ) )
	return this;
    return QObject::qt_cast( clname );
}

#include <qobjectdefs.h>
#include <qsignalslotimp.h>

// SIGNAL pushTitle
void ListManager::pushTitle( Title* t0 )
{
    if ( signalsBlocked() )
	return;
    QConnectionList *clist = receivers( staticMetaObject()->signalOffset() + 0 );
    if ( !clist )
	return;
    QUObject o[2];
    static_QUType_ptr.set(o+1,t0);
    activate_signal( clist, o );
}

// SIGNAL popTitle
void ListManager::popTitle( Title* t0 )
{
    if ( signalsBlocked() )
	return;
    QConnectionList *clist = receivers( staticMetaObject()->signalOffset() + 1 );
    if ( !clist )
	return;
    QUObject o[2];
    static_QUType_ptr.set(o+1,t0);
    activate_signal( clist, o );
}

bool ListManager::qt_invoke( int _id, QUObject* _o )
{
    return QObject::qt_invoke(_id,_o);
}

bool ListManager::qt_emit( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->signalOffset() ) {
    case 0: pushTitle((Title*)static_QUType_ptr.get(_o+1)); break;
    case 1: popTitle((Title*)static_QUType_ptr.get(_o+1)); break;
    default:
	return QObject::qt_emit(_id,_o);
    }
    return TRUE;
}
#ifndef QT_NO_PROPERTIES

bool ListManager::qt_property( int id, int f, QVariant* v)
{
    return QObject::qt_property( id, f, v);
}

bool ListManager::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
