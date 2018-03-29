--- hw/xfree86/common/xf86Config.c.orig	2016-07-19 17:14:30 UTC
+++ hw/xfree86/common/xf86Config.c
@@ -1169,7 +1169,7 @@ checkCoreInputDevices(serverLayoutPtr servlayoutp, Boo
     XF86ConfInputRec defPtr, defKbd;
     MessageType from = X_DEFAULT;
 
-    const char *mousedrivers[] = { "mouse", "synaptics", "evdev", "vmmouse",
+    const char *mousedrivers[] = { "synaptics", "mouse", "evdev", "vmmouse",
         "void", NULL
     };
 
@@ -1410,13 +1410,16 @@ checkCoreInputDevices(serverLayoutPtr servlayoutp, Boo
     }
 
     if (!xf86Info.forceInputDevices && !(foundPointer && foundKeyboard)) {
-#if defined(CONFIG_HAL) || defined(CONFIG_UDEV) || defined(CONFIG_WSCONS)
+#if defined(CONFIG_HAL) || defined(CONFIG_UDEV) || defined(CONFIG_WSCONS) || \
+		defined(CONFIG_DEVD)
         const char *config_backend;
 
 #if defined(CONFIG_HAL)
         config_backend = "HAL";
 #elif defined(CONFIG_UDEV)
         config_backend = "udev";
+#elif defined(CONFIG_DEVD)
+        config_backend = "devd";
 #else
         config_backend = "wscons";
 #endif
