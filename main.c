/*
 ============================================================================
 Name        : main.c
 Author      : Hsiang-Ho Lin
 Version     : 1.0.0
 Copyright   : GPLv2
 Description : 
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ac.h"

#define BUFF_SIZE 1024

typedef struct KEY_ENTRY_ key_statis_entry;
struct KEY_ENTRY_
{
    char *key_str;
	int id;
    int hit_cnt;
    key_statis_entry *next;
};

typedef struct USR_RESULT_ usr_result;
struct USR_RESULT_
{
    key_statis_entry *key_list_head;
};

usr_result ur;

void user_init()
{
    ur.key_list_head = NULL;
}

void user_print_user_result()
{
    printf("---------\n");
    key_statis_entry* key_st = ur.key_list_head;

    while(key_st)
    {
        printf("[hit]:(%d) %s (%d)\n", key_st->id, key_st->key_str, key_st->hit_cnt);
        key_st = key_st->next;
    }
}

void user_free_user_result()
{
	key_statis_entry* tmp;
	key_statis_entry* k = ur.key_list_head;

	while(k)
	{
		free(k->key_str);
		tmp = k->next;
		free(k);
		k = tmp;
	}
}

void user_update_mru_hit_list(ac_w_data data, int pos)
{
	key_statis_entry* prev = NULL;
	key_statis_entry* k = ur.key_list_head;
	char* str = data.pattern;
	int id = data.id;

	while(k)
	{
		if(strcmp(k->key_str, str) == 0)
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

	k = malloc(sizeof(key_statis_entry));
	k->key_str = malloc(sizeof(char) * strlen(str) + 1);
	strcpy(k->key_str, str);
	k->id = id;
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
	int id = 0;
	ac_w_data data;

    ac_struct* acs = ac_new();
    while(fscanf(fin, "%s", buf) != EOF)
    {
		data.pattern = buf;
		data.id = id;
        if(ac_load_search_key(data, acs, DUPKEY_IGNORE) != 0)
            goto exit;
		id++;
    }
    ac_finalize(acs);
    while(fgets(buf, BUFF_SIZE, search_in) != NULL)
    {
        ac_search(buf, acs, &user_update_mru_hit_list);
    }
	user_print_user_result();
exit:
	user_free_user_result();
    ac_free(acs);
    fclose(fin);
    fclose(search_in);
    return 0;
}
