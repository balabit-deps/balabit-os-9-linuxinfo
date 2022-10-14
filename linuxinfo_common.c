/*
	linuxinfo_common.c
	
        Copyright (C) 1998-2000
        All Rights Reserved.

        Alex Buell <alex.buell@munted.eu>

        Copyright (C) 2004-2007,2009,2010,2013
        Helge Kreutzmann <debian@helgefjell.de>

        Version Author  Date            Comments
        ----------------------------------------------------------------------
        1.0.0   AIB     199803??        Initial development
	1.0.1	AIB	20000405	Added read_line function
	1.0.2	AIB	20000405	Moved strstr() from linuxinfo.h to here
	1.0.3	AIB	20010809	Added getphysicalmemory()
	1.0.4   KRE     20110104        Fix resource leak found by DACA/cppcheck

	Common functions module
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <libintl.h>
#define _(String) gettext (String)

#include "linuxinfo.h"

#ifdef use_regexp
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>
#endif

void GetOperatingSystemInfo(struct os_stat *os)
{
#ifndef system_unknown
	struct utsname buf;
	uname(&buf);

	strcpy(os->os_hostname, buf.nodename);
	strcpy(os->os_name, buf.sysname);
	strcpy(os->os_version, buf.release);
	strcpy(os->os_revision, buf.version);
#else
	strcpy(os->os_hostname, "Unknown");
	strcpy(os->os_name, "Unknown");
	strcpy(os->os_version, "Unknown");
	strcpy(os->os_revision, "Unknown");
#endif /* system_unknown */
}

/* Really neat hack to detect all libraries known on Linux */
#ifndef system_unknown
asm (".weak gnu_get_libc_version");
asm (".weak __libc_version");
asm (".weak __linux_C_lib_version");

extern char *gnu_get_libc_version (void);
extern char *__linux_C_lib_version;
extern char __libc_version [];
#endif /* only define if on a Linux system :o) */

void GetSystemLibcInfo(struct lib_stat *lib)
{
	int libc_major = 0, libc_minor = 0, libc_teeny = 0;
	char *ptr;

	/* initialise to unknown */
	strcpy(lib->lib_version, "Unknown");

#ifndef system_unknown
	if (gnu_get_libc_version != 0)
        {
		ptr = gnu_get_libc_version();
        }
        else if (&__libc_version != 0)
        {
		ptr = __libc_version;
        }
        else
        	ptr = __linux_C_lib_version;

        while (!isdigit (*ptr))
	        ptr++;

        sscanf (ptr, "%d.%d.%d", &libc_major, &libc_minor, &libc_teeny);
	sprintf(lib->lib_version, "%d.%d.%d", libc_major, libc_minor, libc_teeny);
#endif /* system_unknown */
}

int read_line(int fd, char *buffer, size_t length)
{
        off_t curpos = lseek(fd, 0, SEEK_CUR);
        int len = read(fd, buffer, length);
        char *p = strchr(buffer, 0x0a);

	if (fd < 0)
		return 0;

        if (len < 0)
                return len;

        if (p != NULL)
                len = (p - buffer) + 1;

        lseek(fd, curpos + len, SEEK_SET);

        if (p != NULL)
                *p = 0;

        return len;
}

int splitstring(char *first_string, char *second_string)
{
	char *p;

	p = strchr(first_string, ':');
	if (!p)
		return 0;

	*(p-1) = '\0', p++; 
	while (isspace(*p)) p++;
	strcpy(second_string, p);
}

LONGLONG getphysicalmemory(void)
{
	LONGLONG memory;
        struct stat st_buf;
	int meminfo_fd;
	char temp_string[BUFSIZ], temp_string2[BUFSIZ];
	int found;

	memory = 0;
        meminfo_fd = open(MEMINFO_FILE, O_RDONLY);
        found=0;
        if (meminfo_fd < 0)
	{
	    printf(_("Could not stat /proc/meminfo, result can be inaccurate\n"));
	    // printf(gettext("Could not stat /proc/meminfo, result can be inaccurate\n"));
        }
        else
        { while (read_line(meminfo_fd, temp_string, BUFSIZ) != 0)
	    {
		if (splitstring(temp_string, temp_string2))
		{
		    if ((strncmp(temp_string, "MemTota", strlen("MemTota")) == 0)&&!found)
		    {
		        found=1;
		        memory = (LONGLONG)atol(temp_string2);
		        memory /= 1024; 
		    }
		}
	    }
	    close(meminfo_fd);
	}
	return memory;
}


#if !(HAVE_STRSTR)
char *strstr (const char *haystack, const char *needle)
{
        c, sc;
        size_t len;

        if ((c = *find++) != 0)
        {
                len = strlen(find);
                do
                {
                        do
                        {
                                if ((sc = *s++) == 0)
                                        return (NULL);
                        }
                        while (sc != c);
                }
                while (strncmp(s, find, len) != 0);

                s--;
        }

        return ((char *)s);
}
#endif

#ifdef use_regexp
int regstrcmp(const char *subjstring, const char *pstring)
{
    pcre2_code *re;
    PCRE2_SPTR pattern;     /* PCRE2_SPTR is a pointer to unsigned code units of */
    PCRE2_SPTR subject;     /* the appropriate width (in this case, 8 bits). */

    int errornumber;
    int rc;

    PCRE2_SIZE erroroffset;

    size_t subject_length;
    pcre2_match_data *match_data;

/* As pattern and subject are char arguments, they can be straightforwardly
cast to PCRE2_SPTR as we are working in 8-bit code units. */

    pattern = (PCRE2_SPTR)pstring;
    subject = (PCRE2_SPTR)subjstring;
    subject_length = strlen((char *)subject);

/*************************************************************************
* Now we are going to compile the regular expression pattern, and handle *
* any errors that are detected.                                          *
*************************************************************************/

    re = pcre2_compile(
      pattern,               /* the pattern */
      PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
      0,                     /* default options */
      &errornumber,          /* for error number */
      &erroroffset,          /* for error offset */
      NULL);                 /* use default compile context */

/* Compilation failed: print the error message and exit. */ /* This should not happen */

    if (re == NULL)
    {
       PCRE2_UCHAR buffer[256];
       pcre2_get_error_message(errornumber, buffer, sizeof(buffer));
       printf(_("PCRE2 compilation failed at offset %d: %s\n"), (int)erroroffset,buffer);
       return 1;
    }

/*************************************************************************
* If the compilation succeeded, we call PCRE again, in order to do a     *
* pattern match against the subject string. This does just ONE match.    #
* Before running the match we must set up a match_data block for holding *
* the result.                                                            #
*************************************************************************/

/* Using this function ensures that the block is exactly the right size for
the number of capturing parentheses in the pattern. */

    match_data = pcre2_match_data_create_from_pattern(re, NULL);

    rc = pcre2_match(
      re,                   /* the compiled pattern */
      subject,              /* the subject string */
      subject_length,       /* the length of the subject */
      0,                    /* start at offset 0 in the subject */
      0,                    /* default options */
      match_data,           /* block for storing the result */
      NULL);                /* use default match context */

/* Matching failed: handle error cases */

    if (rc < 0)
    {
       switch(rc)
       {
           // case PCRE2_ERROR_NOMATCH: printf("No match\n"); break;
	    case PCRE2_ERROR_NOMATCH: break;
       /*
       Handle other special cases if you like
       */
           default: printf(_("Matching error %d\n"), rc); break;
       }
       pcre2_match_data_free(match_data);   /* Release memory used for the match */
       pcre2_code_free(re);                 /* data and the compiled pattern. */
       return 1;
    }

    pcre2_match_data_free(match_data);  /* Release the memory that was used */
    pcre2_code_free(re);                /* for the match data and the pattern. */
    return 0;                           /* Exit the function. */
}
#endif
