#ifndef __REMAP_ANNON__
#define __REMAP_ANNON__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>

#include "remap_annon.h"

static void (*remap_annonymous)(uint64_t, uint64_t, uint64_t, uint64_t);

static char *extract_filename(const char *buf)
{
	char *filename = NULL;
	int spaces = 0;

	for (int i = 0; buf[i]; i++) {
		if (buf[i] == ' ') {
			spaces++;
			continue;
		}

		if (spaces >= 5) {
			filename = (char *)buf + i;
			break;
		}
	}

	return filename;
}

static void extract_ranges(unsigned long *start, unsigned long *end, const char *str)
{
	char *aux;
	*start = strtoul(str, &aux, 16);
	*end = strtol(aux + 1, NULL, 16);
}

static char *get_map_filename(void)
{
	char *name = NULL, *buf = NULL;
	size_t len = 0;
	ssize_t n;

	unsigned long start, end, addr;

	FILE *fh;

	fh = fopen("/proc/self/maps", "r");
	if (fh == NULL) {
		goto end;
	}

	addr = (unsigned long) get_map_filename;

	while ((n = getline(&buf, &len, fh)) != -1) {
		if (buf[n - 1] == '\n') {
			buf[n - 1] = 0x0;
		}

		extract_ranges(&start, &end, buf);

		if (addr >= start && addr < end) {
			if ((name = extract_filename(buf))) {
				name = strdup(name);
			}

			break;
		}
	}

	memset(buf, 0x0, len);
	free(buf);
	fclose(fh);

end:
	return name;
}

static void remap_annon(void)
{
	char *filename, *mapname, *aux, *buf = NULL;
	int perms;

	unsigned long start, end;

	size_t len = 0;
	ssize_t n;

	FILE *fh;

	filename = get_map_filename();
	if (!filename) {
		return;
	}

	fh = fopen("/proc/self/maps", "r");
	if (fh == NULL) {
		goto end;
	}

	while ((n = getline(&buf, &len, fh)) != -1) {
		if (buf[n - 1] == '\n') {
			buf[n - 1] = 0x0;
		}

		mapname = extract_filename(buf);
		if (!mapname || strcmp(mapname, filename)) {
			continue;
		}

		extract_ranges(&start, &end, buf);

		aux = strchr(buf, ' ');
		if (!aux) continue;

		perms = 0;

		if (aux[1] == 'r') perms |= PROT_READ;
		if (aux[2] == 'w') perms |= PROT_WRITE;
		if (aux[3] == 'x') perms |= PROT_EXEC;

		void *data = mmap(NULL, (end - start), PROT_READ | PROT_WRITE,
			MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

		if (data != MAP_FAILED) {
			memcpy(data, (char *)start, (end - start));

			remap_annonymous((uint64_t)start, (uint64_t)(end - start),
				(uint64_t)data, (uint64_t)perms);

			munmap(data, (end - start));
		}
	}

	free(buf);
	fclose(fh);

end:
	free(filename);
}

static void *load_shellcode(unsigned char *sc, int len)
{

	void *addr = mmap(NULL, (size_t)len, PROT_READ | PROT_EXEC | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	if (addr != MAP_FAILED) {
		memcpy(addr, sc, len);
	} else {
		addr = NULL;
	}

	return addr;
}

static __attribute__((constructor)) void remap_annon_init(void)
{
	remap_annonymous = load_shellcode(remap_annon_bin, remap_annon_bin_len);
	memset(remap_annon_bin, 0x0, remap_annon_bin_len);

	if (remap_annonymous) {
		remap_annon();
		munmap(remap_annonymous, remap_annon_bin_len);
	}
}

#endif
