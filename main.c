#include <stdio.h>
#include <string.h>
#include "ac.h"

#define BUFF_SIZE 1024

int main(int argc, char** argv)
{
    FILE *fin = fopen("key.in", "r");
    FILE *search_in = fopen("search.in", "r");
    char buf[BUFF_SIZE] = {0};

    ac_struct *acs = NULL;
    ac_init(&acs);
    while(fscanf(fin, "%s", buf) != EOF)
    {
        if(ac_load_search_key(buf, acs) != 0)
            goto exit;
    }
    ac_finalize(acs);
    while(fgets(buf, BUFF_SIZE, search_in) != NULL)
    {
        ac_search(buf, acs);
    }

exit:
    ac_free(acs);

    fclose(fin);
    fclose(search_in);
    return 0;
}
