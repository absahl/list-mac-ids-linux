#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    FILE* pp = NULL;
    char res[512] = {0};
    size_t res_len = 0;
    char* line = NULL;
    size_t len = 0;
    ssize_t read = -1;
    char* ptr = NULL;
    char iface[32] = {0};

    pp = popen("ifconfig", "r");
    if (pp == NULL)
    {
        printf("popen failed\n");
        return -1;
    }

    while ((read = getline(&line, &len, pp)) != -1)
    {
        // ignore empty lines or line breaks
        if (read <= 1)
        {
            continue;
        }

        // line starts with a non-space character
        // thus, contains an interface name
        if (!isspace(line[0]))
        {
            ptr = strtok(line, ":");
            strncpy(iface, ptr, sizeof(iface) - 1);
        }
        else
        {
            ptr = strtok(line, " \t");

            // only interested in lines starting with "ether"
            if (strcmp(ptr, "ether") != 0)
            {
                continue;
            }

            ptr = strtok(NULL, " \t");
            res_len += snprintf(res + res_len, sizeof(res) - res_len - 1,
                                "%s %s\n", iface, ptr);
        }
    }
    pclose(pp);

    if (line)
    {
        free(line);
        line = NULL;
    }

    if (argc > 1)
    {
        FILE* fp = fopen(argv[1], "w");
        if (fp == NULL)
        {
            printf("fopen failed\n");
            return -2;
        }

        fwrite((const void*) res, sizeof(char), res_len, fp);
        fclose(fp);
        fp = NULL;
    }
    else
    {
        printf("%s", res);
    }

    return 0;
}
