/**
 * Print RPM package name, version, revision, and other meta-data.
 * Depends on RPM 5.x
 * 
 * Written by Lucas C. Villa Real <lucasvr@gobolinux.org>
 * Released under the GNU GPL version 2 or above.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>

#define _RPMEVR_INTERNAL
#include <rpm/rpmio.h>
#include <rpm/rpmcli.h>
#include <rpm/pkgio.h>
#include <rpm/rpmds.h>

struct optinfo {
	const char *name;
	const char *desc;
	void (*fn)(Header *);
};

void generic_print_msg(Header *hdr, const char *fmt)
{
	char *value = headerSprintf(*hdr, fmt, NULL, NULL, NULL);
	if (value) {
		printf("%s\n", value);
		free(value);
	}
}

void print_arch(Header *hdr)
{
	generic_print_msg(hdr, "%{arch}");
}

void print_compressor(Header *hdr)
{
	generic_print_msg(hdr, "%{payloadcompressor}");
}

void print_dependencies(Header *hdr)
{
	rpmds depinfo = rpmdsNew(*hdr, RPMTAG_REQUIRENAME, 0);

	/* init iterator */
	depinfo = rpmdsInit(depinfo);
	while (rpmdsNext(depinfo) >= 0) {
		const char *name = rpmdsN(depinfo);
		const char *version= rpmdsEVR(depinfo);
		evrFlags flags = rpmdsFlags(depinfo);
		if (flags & RPMSENSE_RPMLIB) {
			/* ignore dependencies on rpmlib features */
			continue;
		}
		printf("%s", name);
		if (version && strlen(version)) {
			if (flags)
				printf(" ");
			if (flags & RPMSENSE_LESS)
				printf("<");
			if (flags & RPMSENSE_GREATER)
				printf(">");
			if (RPMSENSE_EQUAL)
				printf("=");
			printf(" %s", version);
		}
		printf("\n");
	}

	rpmdsFree(depinfo);
}

void print_description(Header *hdr)
{
	generic_print_msg(hdr, "%{description}");
}

void print_distribution(Header *hdr)
{
	generic_print_msg(hdr, "%{distribution}");
}

void print_filenames(Header *hdr)
{
	rpmfi fileinfo = rpmfiNew(NULL, *hdr, RPMTAG_BASENAMES, 0);
	while (rpmfiNext(fileinfo) >= 0) {
		const char *name = rpmfiFN(fileinfo);
		printf("%s\n", name);
	}
	rpmfiFree(fileinfo);
}

void print_license(Header *hdr)
{
	generic_print_msg(hdr, "%{license}");
}

void print_name(Header *hdr)
{
	generic_print_msg(hdr, "%{name}");
}

void print_release(Header *hdr)
{
	generic_print_msg(hdr, "%{release}");
}

void print_summary(Header *hdr)
{
	generic_print_msg(hdr, "%{summary}");
}

void print_url(Header *hdr)
{
	generic_print_msg(hdr, "%{url}");
}

void print_version(Header *hdr)
{
	generic_print_msg(hdr, "%{version}");
}

static struct optinfo optinfo[] = {
	{ "--arch",         "architecture",         print_arch },
	{ "--compressor",   "payload compressor",   print_compressor },
	{ "--dependencies", "package dependencies", print_dependencies },
	{ "--description",  "package description",  print_description },
	{ "--distribution", "distribution name",    print_distribution },
	{ "--filenames",    "list of files",        print_filenames },
	{ "--license",      "package license",      print_license },
	{ "--name",         "package name",         print_name },
	{ "--release",      "release number",       print_release },
	{ "--summary",      "summary information",  print_summary },
	{ "--url",          "project url",          print_url },
	{ "--version",      "package version",      print_version },
	{ NULL,             NULL,                   NULL }
};

void usage(char *appname, int retval)
{
	int i;
	fprintf(stderr, "Usage: %s OPTION <file.rpm>\n\n"
		"Available (mutually-exclusive) options are:\n", appname);
	for (i=0; optinfo[i].name; ++i)
		fprintf(stderr, "  %-17s  %s\n", optinfo[i].name, optinfo[i].desc);
	exit(retval);
}

void parse_args(int argc, char **argv, char **rpmfile, int *opt)
{
	const int argc_option = 1;
	const int argc_rpmfile = 2;
	int i;

	if (argc != 3)
		usage(argv[0], 1);

	*rpmfile = NULL;
	*opt = -1;

	for (i=0; optinfo[i].name; ++i) {
		if (!strcmp(argv[argc_option], optinfo[i].name)) {
			*opt = i;
			break;
		}
	}

	if (*opt == -1)
		usage(argv[0], 1);

	*rpmfile = argv[argc_rpmfile];
}

int main(int argc, char **argv)
{
	int option = -1;
	char *rpmfile = NULL;
	parse_args(argc, argv, &rpmfile, &option);

	FD_t fd = Fopen(rpmfile, "r.ufdio");
	if (! fd || Ferror(fd)) {
		fprintf(stderr, "Failed to open %s: %s\n", rpmfile, Fstrerror(fd));
		return 1;
	}

	rpmVSFlags vsflags = 0;
	vsflags |=  RPMVSF_NOHDRCHK;
	vsflags |= _RPMVSF_NODIGESTS;
	vsflags |= _RPMVSF_NOSIGNATURES;

	rpmts ts = rpmtsCreate();
	rpmtsSetVSFlags(ts, vsflags);

	Header hdr;
	rpmRC rc = rpmReadPackageFile(ts, fd, rpmfile, &hdr);
	if (rc != RPMRC_OK) {
		fprintf(stderr, "Failed to read package file %s\n", rpmfile);
		return 1;
	}

	/* handle option */
	optinfo[option].fn(&hdr);

	headerFree(hdr);
	rpmtsFree(ts);
	Fclose(fd);

	return 0;
}
