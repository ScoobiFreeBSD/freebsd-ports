--- src/knemod/storage/xmlstorage.h.orig	2015-08-20 19:27:09 UTC
+++ src/knemod/storage/xmlstorage.h
@@ -34,7 +34,7 @@ class XmlStorage
         bool loadStats( QString name, StorageData *gd, QHash<int, StatisticsModel*> *models );
     private:
         void loadGroup( StorageData *gd, const QDomElement& parentItem, StatisticsModel* statistics );
-        Interface *mInterface;
+        //Interface *mInterface;
 };

 #endif
