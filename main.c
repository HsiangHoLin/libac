#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ac.h"

#define BUFF_SIZE 1024

typedef struct KEY_ENTRY_ key_entry;
struct KEY_ENTRY_
{
	char *key;
	int hit_cnt;
	key_entry *next;
};

typedef struct USR_RESULT_ usr_res;
struct USR_RESULT_
{
	key_entry *key_list_head;
};

usr_res ur;

void print_ur()
{
    printf("---------\n");
	key_entry* k = ur.key_list_head;
	while(k)
	{
		printf("%s hit %d times\n", k->key, k->hit_cnt);
		k = k->next;
	}
}

void ur_free()
{
	key_entry* tmp;
	key_entry* k = ur.key_list_head;
	while(k)
	{
		free(k->key);
		tmp = k->next;
		free(k);
		k = tmp;
	}
}

void call_back(char* str, int pos)
{
	key_entry* prev = NULL;
	key_entry* k = ur.key_list_head;
	while(k)
	{
		if(strcmp(k->key, str) == 0)
		{
			k->hit_cnt++;
            if(prev)
            {
			    prev->next = k->next;
			    k->next = ur.key_list_head;
			    ur.key_list_head = k;
            }
			goto ret;
		}
		prev = k;
		k = k->next;
	}	
	k = malloc(sizeof(key_entry));
	k->key = malloc(sizeof(char) * strlen(str) + 1);
	strcpy(k->key, str);
	k->hit_cnt = 1;
	k->next = ur.key_list_head;
	ur.key_list_head = k;
	
ret:
	return;
}

int main(int argc, char** argv)
{
    FILE *fin = fopen("key.in", "r");
    FILE *search_in = fopen("search.in", "r");
    char buf[BUFF_SIZE] = {0};

	ur.key_list_head = NULL;
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
        ac_search(buf, acs, &call_back);
    }

	print_ur();

exit:
	ur_free();
    ac_free(acs);

    fclose(fin);
    fclose(search_in);
    return 0;
}
