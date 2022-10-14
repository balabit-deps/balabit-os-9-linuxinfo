/*
        linuxinfo_intel.c

        Copyright (C) 1998-2000
        All Rights Reserved.

        Alex Buell <alex.buell@munted.eu>

	Copyright (C) 2003 
	Brian Almeida <bma@debian.org>

	Copyright (C) 2004
	Martin Michlmayr <tbm@cyrius.com>

	Copyright (C) 2004-2007,2009,2010,2013,2014,2016-2018
	Helge Kreutzmann <debian@helgefjell.de>


        Version Author  Date            Comments
        ----------------------------------------------------------------------
        1.0.0   AIB     199803??        Initial development
	1.0.1	AIB	20000405	Updated and reworked code
	1.0.2	AIB	20000527	Added AMD K7
	1.0.3	AIB	20000815	Added AMD K6-3 500MHz
	1.0.4	AIB	20010909	Added getphysicalmemory() call
	1.0.5   OPAL    20030417        Added Athlon XP and MP.
	1.0.6   KRE     20040711        Added Opteron
	see git history for later changes

        This is the x86 port of linuxinfo
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linuxinfo.h"

#ifdef system_intel

static char *vendors[] =
{
	"GenuineIntel", "Intel",
	"AuthenticAMD", "AMD",
	"CyrixInstead", "Cyrix",
	"UMC UMC UMC ", "UMC",
	"CentaurHauls", "Centaur",
	"NexGenDriven", "NexGen",
	NULL, NULL
};

static char *models[] =
{
#ifdef use_regexp
        "K6 \\(166 - 266\\)", "K6",
        "AMD-K6tm w/ mul", "K6 (MMX)",
        "K6-2 \\(PR233 - PR333\\)", "K6-2",
        "AMD-K6\\(tm\\) 3D", "K6-3 (3DNow)",
        "AMD-K6\\(tm\\) 3D+", "K6-3 (3DNow+)",
	"AMD-K6\\(tm\\)-III", "K6-3 (3DNowExt)",
	"AMD-K7\\(tm\\)", "K7",
	"Athlon", "Athlon",
	"AMD Athlon\\(tm\\) processor", "Athlon",
        "AMD Athlon\\(tm\\) Processor", "Athlon",
        "AMD Athlon\\(TM\\) XP", "Athlon XP",
        "AMD Athlon\\(tm\\) XP [0-9]{4}+", "Athlon XP",
        "AMD Athlon\\(TM\\) MP", "Athlon MP",
	"AMD Opteron\\(tm\\) Processor [0-9]{3}","Opteron",
	"AMD Opteron\\(tm\\) X3421 APU", "Opteron APU",
	"AMD Opteron 23xx \\(Gen 3 Class Opteron\\)", "Opteron Gen 3",
	"AMD Sempron\\(tm\\)   [0-9]{4}+", "Sempron",
	"AMD Athlon\\(tm\\) 64 Processor [0-9]{4}+", "x86_64",
	"AMD Athlon\\(tm\\) 64 X2 Dual Core Processor [0-9]{4}+", "x86_64",
	"AMD Athlon\\(tm\\) II X4 [0-9]{3} Processor", "Athlon II",
	"AMD Phenom\\(tm\\) II X4 965 Processor", "Phenom II",
	"AMD Phenom\\(tm\\) II X6 1055T Processor", "Phenom II",
	"AMD A6-6310 APU with AMD Radeon R4 Graphics", "A6-6310 with Radeon R4",
	"AMD FX\\(tm\\)-4300 Quad-Core Processor", "x86_64 FX",
        "Am5x86", "5x86",
        "C6", "C6",
	"C6-2", "C6-2",
        "6x86", "6x86",
        "6x86MX", "6x86MX",
        "Geode\\(TM\\) Integrated Processor by National Semi", "Geode by Nat. Sem.",
	"Geode\\(TM\\) Integrated Processor by AMD PCS", "Geode by AMD PCS",
        "Pentium 75+", "Pentium",
        "Pentium 75 - 200", "Pentium",
        "Pentium MMX", "Pentium MMX",
        "Mobile Pentium MMX", "Mobile Pentium MMX",
        "Pentium Pro", "Pentium Pro",
        "Pentium II \\(Klamath\\)", "Pentium II (Klamath)",
        "Pentium II \\(Deschutes\\)", "Pentium II (Deschutes)",
        "Pentium III \\(Katmai\\)", "Pentium III (Katmai)",
	"Pentium III \\(Coppermine\\)", "Pentium III (Coppermine)",
	"Intel\\(R\\) Pentium\\(R\\) CPU G2020T @ [0-3]\\.[0-9]{2}GHz", "Pentium",
	"Intel\\(R\\) Pentium\\(R\\) 4 CPU [0-3]\\.[0-9]{2}GHz", "Pentium 4",
	"Intel\\(R\\) Pentium\\(R\\) M processor", "Pentium M",
	"Intel\\(R\\) Pentium\\(R\\) M processor [0-9]{4}MHz", "Pentium M",
	"Intel\\(R\\) Pentium\\(R\\) 4 Mobile CPU [0-3]\\.[0-9]{2}GHz","Pentium 4 Mobile",
	"Intel\\(R\\) XEON\\(TM\\) CPU [0-3]\\.[0-9]{2}GHz", "Xeon",
	"Intel\\(R\\) Xeon\\(TM\\) CPU [0-3]\\.[0-9]{2}GHz", "Xeon",
	"Intel\\(R\\) Xeon\\(R\\) CPU           E5345  @ [0-3]\\.[0-9]{2}GHz","Xeon",
	"Intel\\(R\\) Xeon\\(R\\) CPU           L5420  @ [0-3]\\.[0-9]{2}GHz","Xeon",
	"Intel\\(R\\) Xeon\\(R\\) CPU E5-2650 v3 @ [0-3]\\.[0-9]{2}GHz", "Xeon",
        "Celeron \\(Coppermine\\)", "Celeron (Coppermine)",
        "Celeron \\(Mendocino\\)", "Celeron (Mendocino)",
        "Celeron \\(Covington\\)", "Celeron (Covington)",
	"Intel\\(R\\) Celeron\\(R\\) CPU [0-3]\\.[0-9]{2}GHz", "Celeron",
	"Intel\\(R\\) Celeron\\(R\\) M processor         [0-3]\\.[0-9]{2}GHz", "Celeron M",
	"Mobile Intel\\(R\\) Celeron\\(TM\\) CPU          [0-9]{3}MHz", "Mobile Celeron",
	"Mobile Intel\\(R\\) Pentium\\(R\\) 4     CPU [0-3]\\.[0-9]{2}GHz", "Mobile Pentium 4",
	"Intel\\(R\\) Core\\(TM\\) Solo CPU     U1400  @ [0-3]\\.[0-9]{2}GHz", "Core Solo",
	"Intel\\(R\\) Core\\(TM\\)2 CPU          6600  @ [0-3]\\.[0-9]{2}GHz", "Core 2",
	"Intel\\(R\\) Core\\(TM\\)2 Duo CPU     E[0-9]{3}0  @ [0-3]\\.[0-9]{2}GHz", "Core 2 Duo",
	"Intel\\(R\\) Core\\(TM\\)2 Duo CPU     P8600  @ [0-3]\\.[0-9]{2}GHz", "Core 2 Duo",
	"Intel\\(R\\) Core\\(TM\\)2 Quad CPU    Q6600  @ [0-3]\\.[0-9]{2}GHz", "Core 2 Quad",
	"Intel\\(R\\) Core\\(TM\\)2 Quad CPU    Q9550  @ [0-3]\\.[0-9]{2}GHz", "Core 2 Quad",
	"Intel\\(R\\) Core\\(TM\\) i5-6600 CPU @ [0-3]\\.[0-9]{2}GHz", "Core i5",
	"Intel\\(R\\) Core\\(TM\\) i5-[0-9]{3}0U CPU @ [0-2]\\.[0-9]{2}GHz", "Core i5",
	"Intel\\(R\\) Core\\(TM\\) i7-6600U CPU @ [0-3]\\.[0-9]{2}GHz", "Core i7",
	"Intel\\(R\\) Core\\(TM\\) i7-6700T CPU @ [0-3]\\.[0-9]{2}GHz", "Core i7",
	"Intel\\(R\\) Core\\(TM\\) i7-4600M CPU @ [0-3]\\.[0-9]{2}GHz", "Core i7",
	"Intel\\(R\\) Core\\(TM\\) i7-8550U CPU @ [0-3]\\.[0-9]{2]GHz", "Core i7",
	"Genuine Intel\\(R\\) CPU            2140  @ [0-3]\\.[0-9]{2}GHz", "Pentium 4",
	"Intel\\(R\\) Atom\\(TM\\) CPU N[0-9]{2}0   @ [0-3]\\.[0-9]{2}GHz", "Atom",
#else
	"K6 (166 - 266)", "K6",
        "AMD-K6tm w/ mul", "K6 (MMX)",
        "K6-2 (PR233 - PR333)", "K6-2",
        "AMD-K6(tm) 3D", "K6-3 (3DNow)",
        "AMD-K6(tm) 3D+", "K6-3 (3DNow+)",
	"AMD-K6(tm)-III", "K6-3 (3DNowExt)",
	"AMD-K7(tm)", "K7",
	"Athlon", "Athlon",
	"AMD Athlon(tm) processor", "Athlon",
        "AMD Athlon(tm) Processor", "Athlon",
        "AMD Athlon(TM) XP", "Athlon XP",
        "AMD Athlon(tm) XP 1600+", "Athlon XP",
        "AMD Athlon(TM) MP", "Athlon MP",
	"AMD Opteron(tm) Processor 240", "Opteron",
	"AMD Opteron(tm) Processor 242", "Opteron",
	"AMD Opteron(tm) Processor 244", "Opteron",
	"AMD Opteron(tm) Processor 246", "Opteron",
	"AMD Opteron(tm) Processor 248", "Opteron",
	"AMD Opteron(tm) X3421 APU", "Opteron APU",
	"AMD Opteron 23xx (Gen 3 Class Opteron)", "Opteron Gen 3",
	"AMD Sempron(tm)   2300+", "Sempron",
	"AMD Sempron(tm)   2600+", "Sempron",
	"AMD Athlon(tm) 64 Processor 2800+", "x86_64",
	"AMD Athlon(tm) 64 Processor 3000+", "x86_64",
	"AMD Athlon(tm) 64 Processor 3200+", "x86_64",
	"AMD Athlon(tm) 64 Processor 3400+", "x86_64",
	"AMD Athlon(tm) 64 Processor 3500+", "x86_64",
	"AMD Athlon(tm) 64 Processor 3700+", "x86_64",
	"AMD Athlon(tm) 64 X2 Dual Core Processor 5400+", "x86_64",
	"AMD Athlon(tm) II X4 620 Processor", "Athlon II",
	"AMD Phenom(tm) II X4 965 Processor", "Phenom II",
	"AMD Phenom(tm) II X6 1055T Processor", "Phenom II",
	"AMD A6-6310 APU with AMD Radeon R4 Graphics", "A6-6310 with Radeon R4",
	"AMD FX(tm)-4300 Quad-Core Processor", "x86_64 FX",
        "Am5x86", "5x86",
        "C6", "C6",
	"C6-2", "C6-2",
        "6x86", "6x86",
        "6x86MX", "6x86MX",
        "Geode(TM) Integrated Processor by National Semi", "Geode by Nat. Sem.",
	"Geode(TM) Integrated Processor by AMD PCS", "Geode by AMD PCS",
        "Pentium 75+", "Pentium",
        "Pentium 75 - 200", "Pentium",
        "Pentium MMX", "Pentium MMX",
        "Mobile Pentium MMX", "Mobile Pentium MMX",
        "Pentium Pro", "Pentium Pro",
        "Pentium II (Klamath)", "Pentium II (Klamath)",
        "Pentium II (Deschutes)", "Pentium II (Deschutes)",
        "Pentium III (Katmai)", "Pentium III (Katmai)",
	"Pentium III (Coppermine)", "Pentium III (Coppermine)",
	"Intel(R) Pentium(R) CPU G2020T @ 2.50GHz", "Pentium",
	"Intel(R) Pentium(R) 4 CPU 1.80GHz", "Pentium 4",
	"Intel(R) Pentium(R) 4 CPU 2.00GHz", "Pentium 4",
	"Intel(R) Pentium(R) 4 CPU 2.80GHz", "Pentium 4",
	"Intel(R) Pentium(R) 4 CPU 3.00GHz", "Pentium 4",
	"Intel(R) Pentium(R) 4 CPU 3.20GHz", "Pentium 4",
	"Intel(R) Pentium(R) M processor", "Pentium M",
	"Intel(R) Pentium(R) M processor 1600MHz", "Pentium M",
	"Intel(R) Pentium(R) 4 Mobile CPU 1.80GHz","Pentium 4 Mobile",
	"Intel(R) XEON(TM) CPU 2.20GHz", "Xeon",
	"Intel(R) XEON(TM) CPU 2.40GHz", "Xeon",
	"Intel(R) Xeon(TM) CPU 3.00GHz", "Xeon",
	"Intel(R) Xeon(TM) CPU 3.20GHz", "Xeon",
	"Intel(R) Xeon(TM) CPU 3.40GHz", "Xeon",
	"Intel(R) Xeon(TM) CPU 3.60GHz", "Xeon",
	"Intel(R) Xeon(R) CPU           E5345  @ 2.33GHz","Xeon",
	"Intel(R) Xeon(R) CPU           L5420  @ 2.50GHz","Xeon",
	"Intel(R) Xeon(R) CPU E5-2650 v3 @ 2.30GHz", "Xeon",
        "Celeron (Coppermine)", "Celeron (Coppermine)",
        "Celeron (Mendocino)", "Celeron (Mendocino)",
        "Celeron (Covington)", "Celeron (Covington)",
	"Intel(R) Celeron(R) CPU 1.70GHz", "Celeron",
	"Intel(R) Celeron(R) CPU 2.20GHz", "Celeron",
	"Intel(R) Celeron(R) CPU 2.40GHz", "Celeron",
	"Intel(R) Celeron(R) M processor         1.40GHz", "Celeron M",
	"Mobile Intel(R) Celeron(TM) CPU          733MHz", "Mobile Celeron",
	"Mobile Intel(R) Pentium(R) 4     CPU 3.06GHz", "Mobile Pentium 4",
	"Intel(R) Core(TM) Solo CPU     U1400  @ 1.20GHz", "Core Solo",
	"Intel(R) Core(TM)2 CPU          6600  @ 2.40GHz", "Core 2",
	"Intel(R) Core(TM)2 Duo CPU     E6750  @ 2.66GHz", "Core 2 Duo",
	"Intel(R) Core(TM)2 Duo CPU     E7400  @ 2.80GHz", "Core 2 Duo",
	"Intel(R) Core(TM)2 Duo CPU     E8400  @ 3.00GHz", "Core 2 Duo",
	"Intel(R) Core(TM)2 Duo CPU     P8600  @ 2.40GHz", "Core 2 Duo",
	"Intel(R) Core(TM)2 Quad CPU    Q6600  @ 2.40GHz", "Core 2 Quad",
	"Intel(R) Core(TM)2 Quad CPU    Q9550  @ 2.83GHz", "Core 2 Quad",
	"Intel(R) Core(TM) i5-4210U CPU @ 1.70GHz", "Core i5",
	"Intel(R) Core(TM) i5-5300U CPU @ 2.30GHz", "Core i5",
	"Intel(R) Core(TM) i5-6200U CPU @ 2.30GHz", "Core i5",
	"Intel(R) Core(TM) i5-7200U CPU @ 2.50GHz", "Core i5",
	"Intel(R) Core(TM) i5-8250U CPU @ 1.60GHz", "Core i5",
	"Intel(R) Core(TM) i5-6600 CPU @ 3.30GHz", "Core i5",
	"Intel(R) Core(TM) i7-4600M CPU @ 2.90GHz", "Core i7",
	"Intel(R) Core(TM) i7-6600U CPU @ 2.60GHz", "Core i7",
	"Intel(R) Core(TM) i7-6700T CPU @ 2.80GHz", "Core i7",
	"Intel(R) Core(TM) i7-8550U CPU @ 1.80GHz", "Core i7",
	"Genuine Intel(R) CPU            2140  @ 1.60GHz", "Pentium 4",
	"Intel(R) Atom(TM) CPU N450   @ 1.66GHz", "Atom",
	"Intel(R) Atom(TM) CPU N270   @ 1.60GHz", "Atom",
#endif
        NULL, NULL
};

void GetHardwareInfo(int fd, struct hw_stat *hw)
{
	char chip[BUFSIZ], vendor[BUFSIZ], model[BUFSIZ], Mhz[BUFSIZ], family[BUFSIZ];
	char temp_string[BUFSIZ], temp_string2[BUFSIZ];

	int processors = 0; 
	float bogomips = 0.0;
	float tempMHz = 0.0;

	char *p;

	strcpy(chip, "Unknown");
	strcpy(vendor, "Unknown");
	strcpy(model, "Unknown");
	strcpy(family, "Unknown");

	while (read_line(fd, temp_string, BUFSIZ) != 0) 
	{
		if (splitstring(temp_string, temp_string2))
		{
			if (strncmp(temp_string, "processor", strlen("processor")) == 0)
				processors++;

			if (strncmp(temp_string, "vendor_id", strlen("vendor_id")) == 0)
			{
				int i = 0;

				while (vendors[i] != NULL)
				{
					if (strncmp(temp_string2, vendors[i], strlen(vendors[i])) == 0)
						strcpy(vendor, vendors[++i]);

					i += 2;
				}
			}
#ifdef use_regexp
			if (strncmp(temp_string, "model name", strlen("model name")) == 0)
#else
			if (strncmp(temp_string, "model", strlen("model")) == 0)
#endif
			{
				int i = 0;
				int found = 0;
		
				while ((models[i] != NULL)&&(!found))
				{
#ifdef use_regexp
					if (regstrcmp(temp_string2, models[i]) == 0)
#else
					if (strncmp(temp_string2, models[i], strlen(models[i])) == 0)
#endif
					{
						strcpy(model, models[++i]);
						found=1;
					}
					i += 2;
				}
			}

			if (strncmp(temp_string, "cpu family", strlen("cpu family")) == 0)
				strcpy(family, temp_string2);

			if (strncmp(temp_string, "cpu MHz", strlen("cpu MHz")) == 0)
			{
				if (atol(temp_string2) > tempMHz)
					tempMHz = atol(temp_string2);
			}

			if (strncmp(temp_string, "bogomips", strlen("bogomips")) == 0)
				bogomips = bogomips + atof(temp_string2);
		}

		
	}

	sprintf(hw->hw_memory, LONGSPEC, getphysicalmemory());
	hw->hw_processors = processors;
	sprintf(hw->hw_cpuinfo, "%s %s", vendor, model);

	if ((strcmp(vendor, "Unknown") == 0) && (strcmp(model, "Unknown") == 0))
	{
		switch (family[0])
		{
			case '4': sprintf(hw->hw_cpuinfo, "Unknown 486");
				break;
		}
	}

	if (bogomips == 0.0)
		sprintf(hw->hw_bogomips, "?");
	else
		sprintf(hw->hw_bogomips, "%0.2f", bogomips);

	if (tempMHz == 0.0)
		sprintf(hw->hw_megahertz, "?");
	else
		sprintf(hw->hw_megahertz, "%d", (int)tempMHz);
}

#endif /* system_intel */
