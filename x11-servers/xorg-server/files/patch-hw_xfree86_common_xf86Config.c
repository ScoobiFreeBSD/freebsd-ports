--- hw/xfree86/common/xf86Config.c.orig	2015-10-27 20:12:01 UTC
+++ hw/xfree86/common/xf86Config.c
@@ -1121,7 +1121,7 @@ checkCoreInputDevices(serverLayoutPtr se
     XF86ConfInputRec defPtr, defKbd;
     MessageType from = X_DEFAULT;

-    const char *mousedrivers[] = { "mouse", "synaptics", "evdev", "vmmouse",
+    const char *mousedrivers[] = { "synaptics", "evdev", "mouse", "vmmouse",
         "void", NULL
     };

@@ -1362,13 +1362,16 @@ checkCoreInputDevices(serverLayoutPtr se
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
