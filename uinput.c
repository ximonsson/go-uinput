#include "uinput.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>

#define die(str, args...) { perror(str); return -1; }

static int fd;

int setup_uinput ()
{
	struct uinput_user_dev uidev;
	// open /dev/uinput for writing
	fd = open ("/dev/uinput", O_WRONLY | O_NONBLOCK);
	if (fd < 0)
		die ("error: open fd");

	// set bits for sending key and sync events
	if (ioctl (fd, UI_SET_EVBIT, EV_KEY) < 0)
		die ("error: ioctl");
	if (ioctl (fd, UI_SET_EVBIT, EV_SYN) < 0)
		die ("error: ioctl");

	// enable all keys - why not?
	int k = 0;
	for (k = 0; k < KEY_MICMUTE; k ++)
	{
		if (ioctl (fd, UI_SET_KEYBIT, k) < 0)
			perror ("error: ioctl");
	}

	// create device
	memset   (&uidev, 0, sizeof (uidev));
	snprintf (uidev.name, UINPUT_MAX_NAME_SIZE, "network-keyboard");

	uidev.id.bustype = BUS_USB;
	uidev.id.vendor  = 0x1;
	uidev.id.product = 0x1;
	uidev.id.version = 1;

	if (write (fd, &uidev, sizeof (uidev)) < 0)
		die ("error: write");
	if (ioctl (fd, UI_DEV_CREATE) < 0)
		die ("error: UI_DEV_CREATE");

	return 0;
}


int key_event (int key, int event)
{
	// write press event
	struct input_event ev;
	memset (&ev, 0, sizeof (ev));
	ev.type  = EV_KEY;
	ev.value = event;
	ev.code  = key;

	if (write (fd, &ev, sizeof (ev)) < 0)
		die ("error: write key up event");

	// write sync event
	ev.type  = EV_SYN;
	ev.code  = 0;
	ev.value = 0;

	if (write (fd, &ev, sizeof (ev)) < 0)
		die ("error: write sync");

	return 0;
}


int close_uinput ()
{
	if (ioctl (fd, UI_DEV_DESTROY))
		die ("error: destroy ioctl");
	close (fd);
	printf ("uinput out!\n");
	return 0;
}
