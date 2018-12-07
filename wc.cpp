//Donald MacPhail
//-wc word count, run from command line

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <getopt.h>

//TODO: error checking, refinement, etc

int countLines(FILE *fp) {
    char ch;
    int count = 0;

    while(!feof(fp)) {
        ch = fgetc(fp);
        if(ch == '\n') {
            count++;
        }
    }
    fseek(fp, 0, SEEK_SET);
    return count;
}

int countWords(FILE *fp) {
    char ch;
    int count = 0;
    while(!feof(fp)) {
        ch = fgetc(fp);
        if(isspace(ch) || ch == '\0') {
            count++;
        }
    }
    fseek(fp, 0, SEEK_SET);
    return count;
}

int countChars(FILE *fp) {
    char ch;
    int count = 0;

    while(ch = fgetc(fp) != EOF) {
        ++count;
    }
    fseek(fp, 0, SEEK_SET);
    return count;
}

void processFlags(FILE *fp, char *fName, int optind, int lFlag, int wFlag, int cFlag, int err) {
    int lineCount, wordCount, charCount = 0;

    lineCount = countLines(fp);
    wordCount = countWords(fp);
    charCount = countChars(fp);
    if(optind == 1) {
        //wc with no flags
        printf("%d\t%d\t%d\t%s\n", lineCount, wordCount, charCount, fName);
    } else if(optind == 2) {
        //just 1 option.  line, char, or word count
        if(lFlag == 1) {
            printf("%d\t%s\n", lineCount, fName);
        } else if(wFlag == 1) {
            printf("%d\t%s\n", wordCount, fName);
        } else if(cFlag == 1) {
            printf("%d\t%s\n", charCount, fName);
        }
    } else if(optind == 3) {
        //2 flags, printing 2 of L/W/C
        if(lFlag == 1) {
            if(lFlag && cFlag == 1) {
                printf("%d\t%d\t%s\n", lineCount, charCount, fName);
            } else if(lFlag && wFlag == 1) {
                printf("%d\t%d\t%s\n", lineCount, wordCount, fName);
            }
        } else if(wFlag && cFlag == 1) {
            printf("%d\t%d\t%s\n", wordCount, charCount, fName);
        }
    } else if(optind == 4) {
        printf("%d\t%d\t%d\t%s\n", lineCount, wordCount, charCount, fName);
    } else if(err) {
        printf("error\n");
    }

}

int main(int argc, char **argv) {
    extern int optind; //used for getopt(), option index
    FILE *fp;
    char *fName;
    int lFlag, wFlag, cFlag, err = 0;
    int c = 0;
    int index;

    while ((c = getopt(argc, argv, "lcw")) != -1 ) {
        switch(c) {
            case 'l':
                lFlag = 1;
                break;
            case 'w':
                wFlag = 1;
                break;
            case 'c':
                cFlag = 1;
                break;
            case '?':
                err = 1;
                break;
        }
    }

    //get rest of arguments (non-option) eg: filename
    if(optind < argc) {
        for (index = optind; index < argc; index++) {
            //printf("File name: %s\n", argv[index]);
            fName = argv[index];
            fp = fopen(fName, "r");
            if(fp == NULL) {
                printf("Error opening %s\n", fName);
                exit(1);
            }
            processFlags(fp, fName, optind, lFlag, wFlag, cFlag, err);
        }
    } else {
        //no file name arguments, then use stdin
        fp = stdin;
        processFlags(fp, fName, optind, lFlag, wFlag, cFlag, err);
        if(fp == NULL) {
            printf("Unable to process stdin!\n");
            exit(1);
        }
    }

    fclose(fp);
}
