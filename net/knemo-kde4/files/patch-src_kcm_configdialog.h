--- src/kcm/configdialog.h.orig	2015-08-20 19:27:09 UTC
+++ src/kcm/configdialog.h
@@ -132,7 +132,7 @@ private:
     bool mLock;
     Ui::ConfigDlg* mDlg;
     const KCalendarSystem* mCalendar;
-    int mMaxDay;
+    //int mMaxDay;
     StatsRuleModel *statsModel;
     WarnModel *warnModel;

