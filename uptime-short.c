#include <stdio.h>
#include <time.h>
#include <sys/sysinfo.h>

struct time {
	int years;
	int days;
	int hours;
	int minutes;
	int secondes;
};

int main() {
	struct sysinfo c;
	sysinfo(&c);

	int tt = c.uptime;
	struct time t;
	t.secondes = tt % 60;
	tt = tt / 60;
	t.minutes = tt % 60;
	tt = tt / 60;
	t.hours = tt % 24;
	tt = tt / 24;
	t.days = tt % 365;
	tt = tt / 365;
	t.years = tt;

	printf("%02i:%03i:%02i:%02i:%02i\n", t.years, t.days, t.hours, t.minutes, t.secondes);
}

