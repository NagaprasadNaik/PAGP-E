#include <stdio.h>
#include <string.h>
#include <omp.h>

#define COUNT 10
#define FILE_NAME "words.txt"

/* Words we want to search for */
char search_words[COUNT][20] = {
    "The", "around", "graphics", "from", "by",
    "be", "any", "which", "various", "mount"
};

long counts[COUNT];

/* Check if character is A–Z or a–z */
inline int is_alpha(char c)
{
    return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}

/* Compare two words; if ignore_case != 0, compare case-insensitively */
int is_equal(char *a, const char *key, int ignore_case)
{
    char b[20];
    strcpy(b, key);

    int len_a = (int)strlen(a);
    int len_b = (int)strlen(b);

    if (len_a != len_b)
        return 0;

    if (ignore_case != 0)
    {
        int i;
        for (i = 0; i < len_a; i++)
            if (a[i] > 'Z') a[i] -= 32;   // to upper

        for (i = 0; i < len_b; i++)
            if (b[i] > 'Z') b[i] -= 32;   // to upper
    }

    return (strcmp(a, b) == 0);
}

/* Read next word (sequence of letters) from file */
void read_word(char *temp, FILE *fp)
{
    int i = 0;
    int ch;

    /* Skip non-alphabet characters */
    while ((ch = fgetc(fp)) != EOF && is_alpha((char)ch) == 0)
        ;

    /* Read the word */
    while (ch != EOF && is_alpha((char)ch) != 0)
    {
        temp[i++] = (char)ch;
        ch = fgetc(fp);
    }

    temp[i] = '\0';
}

/* Count occurrences of key in file_name */
long determine_count(const char *file_name, const char *key, int ignore_case)
{
    long word_count = 0;
    FILE *fp = fopen(file_name, "r");
    char temp[40];

    if (fp == NULL)
    {
        printf("Error opening file %s\n", file_name);
        return 0;
    }

    while (!feof(fp))
    {
        read_word(temp, fp);
        if (temp[0] == '\0')   // no more words
            break;

        if (is_equal(temp, key, ignore_case) != 0)
            word_count++;
    }

    fclose(fp);
    return word_count;
}

int main(void)
{
    int i;

    for (i = 0; i < COUNT; i++)
        counts[i] = 0;

    double t = omp_get_wtime();

    /* Parallelize the search for different words */
    #pragma omp parallel for
    for (i = 0; i < COUNT; i++)
    {
        counts[i] = determine_count(FILE_NAME, search_words[i], 1);
    }

    t = omp_get_wtime() - t;

    for (i = 0; i < COUNT; i++)
        printf("%s: %ld\n", search_words[i], counts[i]);

    printf("\nTime Taken: %lf\n", t);

    return 0;
}
