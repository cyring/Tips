#include <stdio.h>
#include <stropts.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/watchdog.h>

int main(int argc, char *argv[]) {
	int fd = open("/dev/watchdog", O_RDWR), nmi_watchdog = 0;
	if (fd >= 0) {
		ioctl(fd, WDIOC_GETSTATUS, &nmi_watchdog);
		printf("The nmi_watchdog status is %d\n", nmi_watchdog);

		ioctl(fd, WDIOC_SETOPTIONS, WDIOS_DISABLECARD, 0);

		ioctl(fd, WDIOC_GETSTATUS, &nmi_watchdog);
		printf("The nmi_watchdog status is now %d\n", nmi_watchdog);
		close(fd);
	} else {
		printf("cannot open /dev/watchdog\n");
	}
	return(fd);
}
