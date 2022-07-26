#include "timers.h"
#include <time.h>

static unsigned long long timespec2ms(const struct timespec *spec)
{
	return (unsigned long long)spec->tv_sec * 1000 + spec->tv_sec / 1000000;
}

static unsigned long long now(void)
{
	struct timespec spec;

	clock_gettime(CLOCK_MONOTONIC, &spec);
	return timespec2ms(&spec);
}

void delay(int ms)
{
	const unsigned long long start = now();
	while (now() - start < ms);
}
