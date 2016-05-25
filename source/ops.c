/*
 * File         : ops.c
 * This file is part of POSIX-RTOS
 * COPYRIGHT (C) 2015 - 2016, DongHeng
 *
 * Change Logs:
 * DATA             Author          Note
 * 2015-05-28       DongHeng        create, just fir DXI-II frame
 */

int eth_memcmp(const void *src, const void *dest, int size)
{
	int l = size;
	char *s = (char *)src;
	char *d = (char *)dest;

	while(l-- && *s++ == *d++);

	return l + 1;
}

void* eth_memcpy(const void *src, const void *dest, int size)
{
	int l = size;
	char *s = (char *)src;
	char *d = (char *)dest;

	while(l--)
		*s++ = *d++;

	return src;
}
