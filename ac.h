typedef struct AC_STRUCT_ ac_struct;

int ac_init(ac_struct** acs);
int ac_load_search_key(char *str, ac_struct* acs);
int ac_finalize(ac_struct *acs);
void ac_free(ac_struct *acs);
int ac_search(char *str, ac_struct *acs);
