--- src/knemod/interfacestatistics.h.orig	2015-08-20 19:27:09 UTC
+++ src/knemod/interfacestatistics.h
@@ -120,7 +120,7 @@ private:
     QTimer* mWarnTimer;
     QTimer* mEntryTimer;
     bool mTrafficChanged;
-    int mWeekStartDay;
+    //int mWeekStartDay;
     StorageData mStorageData;
     QHash<int, StatisticsModel*> mModels;
     QList<StatsRule> mStatsRules;
