/*
 * A program to set maximum cpu frequency on linux systems
 *
 * Copyright (c) 2019 Gaurav Kumar Yadav <gaurav712@protonmail.com>
 * for license and copyright information, see the LICENSE file distributed with this source
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>

/* Default assumptions */
#define CPUCORESDIR         "/sys/devices/system/cpu/"  /* Default directory to find cores' frequency files */
#define MAXFREQFILE         "/cpufreq/cpuinfo_max_freq" /* To append after "/sys/dev...../cpuN" */
#define MINFREQFILE         "/cpufreq/cpuinfo_min_freq" /* To append after "/sys/dev...../cpuN" */
#define SCALEMAXFREQFILE    "/cpufreq/scaling_max_freq" /* To append after "/sys/dev...../cpuN" */

/* Declarations */
static void showHelp(void);
static void getCores(float freq);
static short checkName(char *name);             /* Check if the name is of the form "cpuN" */
static void setFreq(float freq, char *core);    /* Set-up core's frequency */

int main(int argc, char *argv[]) {

    /* Check validity of command-line arguments */
    if(argc != 2) {
        showHelp();
        exit(1);
    }

    /* Get list of cores */
    getCores(atof(argv[1]));

    return 0;
}

void
showHelp(void) {

    /* Show that moron the manual */
    fprintf(stderr, "\nUSAGE:\ncpufreq [n]\t:\t'n' is a floating point number that represents frequency in GigaHertz\n\n");
}

void
getCores(float freq) {

    DIR *dirp;
    struct dirent *dir;

    if((dirp = opendir(CPUCORESDIR)) == NULL && errno) {
        fprintf(stderr, "opendir() failed! : %s\n", strerror(errno));
        exit(1);
    }

    while((dir = readdir(dirp)) != NULL) {

        if(checkName(dir->d_name)) {

            /* Change frequency */
            setFreq(freq, dir->d_name);
        }
    }
}

short
checkName(char *name) {

    short count, len;

    if(!(strncmp(name, "cpu", 3))) {

        count = 3;
        len = strlen(name);

        while(count < len) {
            if(name[count] < '0' || name[count] > '9')
                return 0;
            count++;
        }
    } else {
        return 0;
    }

    return 1;
}

void
setFreq(float freq, char *core) {

    char path[PATH_MAX], temp[PATH_MAX];
    FILE *fp;
    unsigned maxFreq, minFreq, freqToSet = (freq * 1000000);

    strcpy(path, CPUCORESDIR);
    strcat(path, core);

    /* Get maximum frequency allowed by the chip */

    strcpy(temp, path);
    strcat(temp, MAXFREQFILE);

    if((fp = fopen(temp, "r")) == NULL) {
        fprintf(stderr, "couldn't get max freq!\n");
        exit(1);
    }

    fscanf(fp, "%u", &maxFreq);
    fclose(fp);

    /* Get minimum frequency allowed by the chip */

    strcpy(temp, path);
    strcat(temp, MINFREQFILE);

    if((fp = fopen(temp, "r")) == NULL) {
        fprintf(stderr, "couldn't get min freq!\n");
        exit(1);
    }

    fscanf(fp, "%u", &minFreq);
    fclose(fp);

    /* Open file for writing */
    strcpy(temp, path);
    strcat(temp, SCALEMAXFREQFILE);

    if((fp = fopen(temp, "w")) == NULL) {
        fprintf(stderr, "couldn't get min freq! : %s\n", strerror(errno));
        exit(1);
    }

    /* Check for boundations */
    if(freqToSet > maxFreq) {
        fprintf(stderr, "\e[31;1mMaximum frequency allowed by your chip is %.2fGHz! Setting %s to this frequency!\e[0;0m\n", (float)maxFreq/1000000.0, core);
        fprintf(fp, "%u", maxFreq);
    } else if(freqToSet < minFreq) {
        fprintf(stderr, "\e[31;1mMinimum frequency allowed by your chip is %.2fGHz! Setting %s to this frequency!\e[0;0m\n", (float)minFreq/1000000.0, core);
        fprintf(fp, "%u", minFreq);
    } else {
        printf("Setting up %s\n", core);
        fprintf(fp, "%u", freqToSet);
    }

    fclose(fp);
}

