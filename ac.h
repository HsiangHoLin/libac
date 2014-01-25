#ifndef _AC_H_
#define _AC_H_

#define DUPKEY_IGNORE	1
#define DUPKEY_ABORT	2

typedef struct AC_STRUCT_ ac_struct;

typedef struct AC_W_DATA_ ac_w_data;
struct AC_W_DATA_
{
    char* pattern;
	int id;
};

ac_struct* ac_new(void);
int ac_load_search_key(ac_w_data data, ac_struct* acs, int opt);
int ac_finalize(ac_struct *acs);
int ac_search(char *str, ac_struct *acs, void (*call_back_f)(ac_w_data data, int pos));
void ac_free(ac_struct *acs);

#endif
