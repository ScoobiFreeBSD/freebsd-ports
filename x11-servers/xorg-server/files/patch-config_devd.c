--- config/devd.c.orig	2016-02-22 14:03:46 UTC
+++ config/devd.c
@@ -0,0 +1,613 @@
+/*
+ * Copyright (c) 2012 Baptiste Daroussin
+ * Copyright (c) 2013, 2014 Alex Kozlov
+ * Copyright (c) 2014 Robert Millan
+ * Copyright (c) 2014 Jean-Sebastien Pedron
+ *
+ * Permission is hereby granted, free of charge, to any person obtaining a
+ * copy of this software and associated documentation files (the "Software"),
+ * to deal in the Software without restriction, including without limitation
+ * the rights to use, copy, modify, merge, publish, distribute, sublicense,
+ * and/or sell copies of the Software, and to permit persons to whom the
+ * Software is furnished to do so, subject to the following conditions:
+ *
+ * The above copyright notice and this permission notice (including the next
+ * paragraph) shall be included in all copies or substantial portions of the
+ * Software.
+ *
+ * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
+ * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
+ * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
+ * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
+ * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
+ * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
+ * DEALINGS IN THE SOFTWARE.
+ *
+ * Author: Baptiste Daroussin <bapt@FreeBSD.org>
+ */
+
+#ifdef HAVE_DIX_CONFIG_H
+#include <dix-config.h>
+#endif
+
+#include <sys/types.h>
+#include <sys/kbio.h>
+#include <sys/socket.h>
+#include <sys/stat.h>
+#include <sys/sysctl.h>
+#include <sys/un.h>
+
+#include <ctype.h>
+#include <errno.h>
+#include <fcntl.h>
+#include <stdlib.h>
+#include <stdio.h>
+#include <stdbool.h>
+#include <unistd.h>
+
+#include "input.h"
+#include "inputstr.h"
+#include "hotplug.h"
+#include "config-backends.h"
+#include "os.h"
+
+#define DEVD_SOCK_PATH "/var/run/devd.pipe"
+
+#define DEVD_EVENT_ADD		'+'
+#define DEVD_EVENT_REMOVE	'-'
+
+#define RECONNECT_DELAY		5 * 1000
+
+static int sock_devd;
+static bool is_console_kbd = false;
+static bool is_kbdmux = false;
+OsTimerPtr rtimer;
+
+struct hw_type {
+        const char *driver;
+        int (*config_device)(const char *driver, const char *devd_line, InputOption *options, InputAttributes *attrs);
+        int ignore_path;
+};
+
+static int get_default_device(const char *driver, const char *devd_line, InputOption *options, InputAttributes *attrs);
+static int get_usb_device(const char *driver, const char *devd_line, InputOption *options, InputAttributes *attrs);
+static int get_psm_device(const char *driver, const char *devd_line, InputOption *options, InputAttributes *attrs);
+
+static struct hw_type hw_types[] = {
+	{ "ukbd", get_usb_device, 0 },
+	{ "atkbd", get_default_device, 1 },
+	{ "kbdmux", get_default_device, 0 },
+	{ "vboxguest", get_default_device, 0 },
+	{ "sysmouse", get_default_device, 0 },
+	{ "ums", get_usb_device, 0 },
+	{ "psm", get_psm_device, 0 },
+	{ "joy", get_default_device, 0 },
+	{ "atp", get_usb_device, 0 },
+	{ "uep", get_usb_device, 0 },
+	{ NULL, NULL, 0 }
+};
+
+static bool
+sysctl_exists(const struct hw_type *device, int unit,
+	char *devname, size_t devname_len)
+{
+	char sysctlname[PATH_MAX];
+	size_t len;
+	int ret;
+
+	if (device == NULL || device->driver == NULL)
+		return false;
+
+	/* Check if a sysctl exists. */
+	snprintf(sysctlname, sizeof(sysctlname), "dev.%s.%i.%%desc",
+	    device->driver, unit);
+	ret = sysctlbyname(sysctlname, NULL, &len, NULL, 0);
+
+	if (ret == 0 && len > 0) {
+		snprintf(devname, devname_len, "%s%i", device->driver, unit);
+		return true;
+	}
+
+	return false;
+}
+
+static bool
+devpath_exists(const struct hw_type *device,
+	char *devname, size_t devname_len)
+{
+	char *devpath;
+	struct stat st;
+	int ret;
+
+	if (device == NULL || device->driver == NULL)
+		return false;
+
+	/* Check if /dev/$driver exists. */
+	asprintf(&devpath, "/dev/%s", device->driver);
+	if (devpath == NULL)
+		return false;
+
+	ret = stat(devpath, &st);
+	free(devpath);
+
+	if (ret == 0) {
+		strncpy(devname, device->driver, devname_len);
+		return true;
+	}
+
+	return false;
+}
+
+static int
+is_path_openable(const char *path)
+{
+	int fd;
+
+	if ((fd = open(path, O_RDONLY)) >= 0) {
+		close(fd);
+	}
+	return fd >= 0;
+}
+
+static char *
+rtrim(char *str)
+{
+    if (str && *str) {
+        char *cp;
+
+        for (cp = str + strlen(str) - 1; isspace(*cp); --cp) {
+            *cp = '\0';
+        }
+    }
+    return str;
+}
+
+static char *
+sysctl_get_str(const char *sysctlname)
+{
+	char *dest = NULL;
+	size_t len;
+
+	if (sysctlname == NULL)
+		return NULL;
+
+	if (sysctlbyname(sysctlname, NULL, &len, NULL, 0) == 0) {
+		dest = malloc(len + 1);
+		if (dest) {
+			if (sysctlbyname(sysctlname, dest, &len, NULL, 0) == 0)
+				dest[len] = '\0';
+			else {
+				free(dest);
+				dest = NULL;
+			}
+		}
+	}
+
+	return dest;
+}
+
+static void
+device_added(const char *devname)
+{
+	char path[PATH_MAX] = "";
+	char sysctlname[PATH_MAX];
+	char *product = NULL;
+	char *config_info = NULL;
+	InputOption *options = NULL;
+	InputAttributes attrs = { };
+	DeviceIntPtr dev = NULL;
+	int i;
+
+	for (i = 0; hw_types[i].driver != NULL; i++) {
+		size_t len;
+
+		len = strlen(hw_types[i].driver);
+		if (strcmp(devname, hw_types[i].driver) == 0 ||
+			(strncmp(devname, hw_types[i].driver, len) == 0 &&
+				isnumber(*(devname + len)))) {
+			break;
+		}
+	}
+
+	if (hw_types[i].driver == NULL) {
+		LogMessage(X_INFO, "config/devd: device %s unsupported.\n",
+				devname);
+		return;
+	}
+
+	if (!hw_types[i].ignore_path) {
+		snprintf(path, sizeof(path), "/dev/%s", devname);
+	}
+
+	options = input_option_new(NULL, "_source", "server/devd");
+	if (!options)
+		return;
+
+	/* Generic device configuration */
+	snprintf(sysctlname, sizeof(sysctlname), "dev.%s.%s.%%desc",
+	    hw_types[i].driver, devname + strlen(hw_types[i].driver));
+	product = sysctl_get_str(sysctlname);
+	attrs.product = strdup(product != NULL ? product : "(unnamed)");
+	attrs.vendor = strdup("(unnamed)");
+	/* XXX implement usb_id */
+	attrs.usb_id = NULL;
+	attrs.device = strdup(path);
+	options = input_option_new(options, "name", product != NULL ? product : "(unnamed)");
+	options = input_option_new(options, "path", path);
+	options = input_option_new(options, "device", path);
+
+	/* Device-specific configuration */
+	if (hw_types[i].config_device) {
+		if (hw_types[i].config_device(hw_types[i].driver, devname, options, &attrs)) {
+			goto unwind;
+		}
+	}
+
+	if (asprintf(&config_info, "devd:%s", devname) == -1) {
+		config_info = NULL;
+		goto unwind;
+	}
+
+	if (device_is_duplicate(config_info)) {
+		LogMessage(X_WARNING, "config/devd: device %s (%s) already added. "
+				"ignoring\n", attrs.product, path);
+		goto unwind;
+	}
+
+	options = input_option_new(options, "config_info", config_info);
+	LogMessage(X_INFO, "config/devd: adding input device %s (%s)\n",
+			attrs.product, path);
+
+	NewInputDeviceRequest(options, &attrs, &dev);
+
+unwind:
+	if (config_info)
+		free(config_info);
+	input_option_free_list(&options);
+	free(attrs.usb_id);
+	free(attrs.product);
+	free(attrs.device);
+	free(attrs.vendor);
+}
+
+static void
+device_removed(char *devname)
+{
+	char *config_info;
+
+	rtrim(devname);
+	if (asprintf(&config_info, "devd:%s", devname) == -1)
+		return;
+
+	remove_devices("devd", config_info);
+
+	free(config_info);
+}
+
+static bool is_kbdmux_enabled(void)
+{
+	/* Xorg uses /dev/ttyv0 as a console device */
+	/* const char device[]="/dev/console"; */
+	const char device[]="/dev/ttyv0";
+	keyboard_info_t info;
+	int fd;
+
+	fd = open(device, O_RDONLY);
+
+	if (fd < 0)
+		return false;
+
+	if (ioctl(fd, KDGKBINFO, &info) == -1) {
+		close(fd);
+		return false;
+	}
+
+	close(fd);
+
+	if (!strncmp(info.kb_name, "kbdmux", 6))
+		return true;
+
+	return false;
+}
+
+static void
+disconnect_devd(int sock)
+{
+	if (sock >= 0) {
+		RemoveGeneralSocket(sock);
+		close(sock);
+	}
+}
+
+static int
+connect_devd(void)
+{
+	struct sockaddr_un devd;
+	int sock;
+
+	sock = socket(AF_UNIX, SOCK_STREAM, 0);
+	if (sock < 0) {
+		LogMessage(X_ERROR, "config/devd: fail opening stream socket\n");
+		return -1;
+	}
+
+	devd.sun_family = AF_UNIX;
+	strlcpy(devd.sun_path, DEVD_SOCK_PATH, sizeof(devd.sun_path));
+
+	if (connect(sock, (struct sockaddr *) &devd, sizeof(devd)) < 0) {
+		close(sock);
+		LogMessage(X_ERROR, "config/devd: fail to connect to devd\n");
+		return -1;
+	}
+
+	AddGeneralSocket(sock);
+
+	return	sock;
+}
+
+static CARD32
+reconnect_handler(OsTimerPtr timer, CARD32 time, void *arg)
+{
+	int newsock;
+
+	if ((newsock = connect_devd()) > 0) {
+		sock_devd = newsock;
+		TimerFree(rtimer);
+		rtimer = NULL;
+		LogMessage(X_INFO, "config/devd: reopening devd socket\n");
+		return 0;
+	}
+
+	/* Try again after RECONNECT_DELAY */
+	return RECONNECT_DELAY;
+}
+
+static ssize_t
+socket_getline(int fd, char **out)
+{
+	char *buf, *newbuf;
+	ssize_t ret, cap, sz = 0;
+	char c;
+
+	cap = 1024;
+	buf = malloc(cap * sizeof(char));
+	if (!buf)
+		return -1;
+
+	for (;;) {
+		ret = read(sock_devd, &c, 1);
+		if (ret < 0) {
+			if (errno == EINTR)
+				continue;
+			free(buf);
+			return -1;
+		/* EOF - devd socket is lost */
+		} else if (ret == 0) {
+			disconnect_devd(sock_devd);
+			rtimer = TimerSet(NULL, 0, 1, reconnect_handler, NULL);
+			LogMessage(X_WARNING, "config/devd: devd socket is lost\n");
+			return -1;
+		}
+		if (c == '\n')
+			break;
+
+		if (sz + 1 >= cap) {
+			cap *= 2;
+			newbuf = realloc(buf, cap * sizeof(char));
+			if (!newbuf) {
+				free(buf);
+				return -1;
+			}
+			buf = newbuf;
+		}
+		buf[sz] = c;
+		sz++;
+	}
+
+	buf[sz] = '\0';
+	if (sz >= 0)
+		*out = buf;
+	else
+		free(buf);
+
+	/* Number of bytes in the line, not counting the line break */
+	return sz;
+}
+
+static void
+wakeup_handler(void *data, int err, void *read_mask)
+{
+	char *line = NULL;
+	char *walk;
+
+	if (err < 0)
+		return;
+
+	if (FD_ISSET(sock_devd, (fd_set *) read_mask)) {
+		if (socket_getline(sock_devd, &line) < 0)
+			return;
+
+		walk = strchr(line + 1, ' ');
+		if (walk != NULL)
+			walk[0] = '\0';
+
+		switch (*line) {
+		case DEVD_EVENT_ADD:
+			device_added(line + 1);
+			break;
+		case DEVD_EVENT_REMOVE:
+			device_removed(line + 1);
+			break;
+		default:
+			break;
+		}
+		free(line);
+	}
+}
+
+static void
+block_handler(void *data, struct timeval **tv, void *read_mask)
+{
+}
+
+static int get_default_device(const char *driver, const char *devd_line, InputOption *options, InputAttributes *attrs)
+{
+	int retval = 0;
+
+	if (strcmp(driver, "atkbd") == 0) {
+		attrs->flags |= ATTR_KEYBOARD;
+		options = input_option_new(options, "driver", "kbd");
+
+		/* Skip keyboard devices if kbdmux is enabled */
+		if (is_kbdmux && is_console_kbd) {
+			LogMessage(X_INFO, "config/devd: kbdmux is enabled, ignoring device %s\n",
+					devd_line);
+			retval = -1;
+		}
+		else if (attrs->device[0] && !is_path_openable(attrs->device)) {
+			LogMessage(X_INFO, "config/devd: device %s already opened\n",
+					attrs->device);
+			/*
+			 * Fail if cannot open device, it breaks
+			 * AllowMouseOpenFail, but it should not matter when
+			 * config/devd enabled
+			 */
+			retval = -1;
+		}
+		else if (is_console_kbd) {
+			/*
+			 * There can be only one keyboard attached to console and
+			 * it is already added.
+			 */
+			LogMessage(X_WARNING, "config/devd: console keyboard is "
+					"already added, ignoring %s (%s)\n",
+					attrs->product, attrs->device);
+			retval = -1;
+		}
+		else {
+			/*
+			 * Don't pass "device" option if the keyboard
+			 * is already attached to the console (ie.
+			 * open() fails).  This would activate a
+			 * special logic in xf86-input-keyboard.
+			 * Prevent any other attached to console
+			 * keyboards being processed. There can be only
+			 * one such device.
+			 */
+			is_console_kbd = true;
+		}
+
+	} else if (strcmp(driver, "joy") == 0) {
+		attrs->flags |= ATTR_JOYSTICK;
+		options = input_option_new(options, "driver", "joystick");
+	} else {
+		retval = -1;
+	}
+	return retval;
+}
+
+static int get_usb_device(const char *driver, const char *devd_line, InputOption *options, InputAttributes *attrs)
+{
+	int retval = 0;
+
+	if (strcmp(driver, "ukbd") == 0) {
+		attrs->flags |= ATTR_KEYBOARD;
+		options = input_option_new(options, "driver", "kbd");
+	}
+	else if (strcmp(driver, "ums") == 0 || strcmp(driver, "uhid") == 0) {
+		/* Change "path" and "device" to use sysmouse(4) */
+		InputOption *option;
+
+		attrs->flags |= ATTR_POINTER;
+		options = input_option_new(options, "driver", "mouse");
+		option = input_option_find(options, "path");
+		if (option) {
+			input_option_set_value(option, "/dev/sysmouse");
+		}
+		option = input_option_find(options, "device");
+		if (option) {
+			input_option_set_value(option, "/dev/sysmouse");
+		}
+	} else if (strcmp(driver, "uep") == 0) {
+		attrs->flags |= ATTR_TOUCHSCREEN;
+		options = input_option_new(options, "driver", "egalax");
+	} else {
+		retval = -1;
+	}
+	return retval;
+}
+
+static int get_psm_device(const char *driver, const char *devd_line, InputOption *options, InputAttributes *attrs)
+{
+	int retval = 0;
+
+	if (strcmp(driver, "psm") == 0) {
+		char *str;
+
+		if ((str = sysctl_get_str("hw.psm.synaptics.margin_top")) ||
+			(str = sysctl_get_str("dev.psm.synaptics.margin_top")))
+		{
+			free(str);
+			attrs->flags |= ATTR_TOUCHPAD;
+			options = input_option_new(options, "driver", "synaptics");
+		} else {
+			attrs->flags |= ATTR_POINTER;
+			options = input_option_new(options, "driver", "mouse");
+		}
+	} else {
+		retval = -1;
+	}
+	return retval;
+}
+
+int
+config_devd_init(void)
+{
+	char devicename[1024];
+	int i, j;
+
+	LogMessage(X_INFO, "config/devd: probing input devices...\n");
+
+	/*
+	 * Add fake keyboard and give up on keyboards management
+	 * if kbdmux is enabled
+	 */
+	if ((is_kbdmux = is_kbdmux_enabled()) == true)
+		device_added("kbdmux");
+
+	for (i = 0; hw_types[i].driver != NULL; i++) {
+		/* First scan the sysctl to determine the hardware */
+		for (j = 0; j < 16; j++) {
+			if (sysctl_exists(&hw_types[i], j,
+					devicename, sizeof(devicename)) != 0)
+				device_added(devicename);
+		}
+
+		if (devpath_exists(&hw_types[i], devicename, sizeof(devicename)) != 0)
+			device_added(devicename);
+	}
+
+	if ((sock_devd = connect_devd()) < 0)
+		return 0;
+
+	RegisterBlockAndWakeupHandlers(block_handler, wakeup_handler, NULL);
+
+	return 1;
+}
+
+void
+config_devd_fini(void)
+{
+	LogMessage(X_INFO, "config/devd: terminating backend...\n");
+
+	if (rtimer) {
+		TimerFree(rtimer);
+		rtimer = NULL;
+	}
+
+	disconnect_devd(sock_devd);
+
+	RemoveBlockAndWakeupHandlers(block_handler, wakeup_handler, NULL);
+
+	is_console_kbd = false;
+}
