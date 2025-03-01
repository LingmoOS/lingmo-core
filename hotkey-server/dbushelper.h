 #pragma once

 #include <QDBusArgument>
 #include <QMap>
 #include <QString>
 
 /// a{sa{sv}}
 using VariantMapMap = QMap<QString, QMap<QString, QVariant>>;
 
 /// sa{sv}
 /// identifier, properties{"keys": "QKeySequence in String", "description": "QString"}
 using Shortcut = QPair<QString, QVariantMap>;
 
 /// a(sa{sv})
 using Shortcuts = QList<Shortcut>;
 
 Q_DECLARE_METATYPE(VariantMapMap)
 Q_DECLARE_METATYPE(Shortcuts)
 