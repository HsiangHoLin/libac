#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ac.h"

typedef struct AC_W_NODE_ ac_w_node;
struct AC_W_NODE_
{
    ac_w_node *next_list;
    ac_w_node *next_one;
    ac_w_node *failure;
    int is_final;
    char* pattern;
    char key;
};

struct AC_STRUCT_
{
    char is_finalized;
    ac_w_node *root;
    ac_w_node *current;
    int counter;
};

int ac_init(ac_struct** acs)
{
    *acs = (ac_struct*) malloc(sizeof(ac_struct));
    if(!*acs)
        return -1;
    ac_struct* ac = *acs;
    ac->is_finalized = 0;
    ac->counter = 0;
    ac->root = (ac_w_node*) malloc(sizeof(ac_w_node));
    if(!ac->root)
    {
        free(*acs);
        return -1;
    }
    ac->root->next_list = NULL;
    ac->root->next_one = NULL;
    ac->root->failure = ac->root;
    ac->root->key = 0;
    ac->root->is_final = 0;
    ac->current = ac->root;
    
    return 0;
}

ac_w_node* get_next_node(ac_w_node* n, char ch)
{
    ac_w_node *next = n->next_list;
    while(next)
    {
        if(next->key == ch)
            break;
        next = next->next_one;
    }
    return next;
}

void set_next_node(ac_w_node* p_n, ac_w_node* c_n)
{
    ac_w_node *next = p_n->next_list;
    if(next == NULL)
        p_n->next_list = c_n;
    while(next)
    {
        if(next->next_one == NULL)
        {
            next->next_one = c_n;
            break;
        }
        next = next->next_one;
    }
}

ac_w_node* new_node(char key)
{
    ac_w_node* n = (ac_w_node*) malloc(sizeof(ac_w_node));
    if(!n)
        return NULL;
    n->next_list = NULL;
    n->next_one = NULL;
    n->pattern = NULL;
    n->key = key;
    n->is_final = 0;
    n->failure = NULL;
    return n;
}

int build_trie(char *str, ac_struct *ac)
{
    if(!ac || !ac->root) 
        return -1;

    int i;
    ac_w_node *n = ac->root;
    ac_w_node *prev = NULL;
    int len = strlen(str);
    for(i = 0; i < len; i++)
    {
        prev = n;
        n = get_next_node(n, str[i]);
        if(!n)
        {
            n = new_node(str[i]);
            if(!n)
                return -1;
            set_next_node(prev,n);
        }
        if( i == len -1)
        {
            if(n->is_final == 0)
            {
                n->is_final = 1;
                n->pattern = (char*) malloc(len+1);
                memcpy(n->pattern, str, len);
                n->pattern[len] = '\0';
            }
            else
            {
                printf("Duplicate keyword \"%s\"\n", n->pattern);
                //return -1;
            }
        }
    }
    return 0;
}

int ac_load_search_key(char *str, ac_struct *acs)
{
    int ret = 0;
    if(!acs || acs->is_finalized)
        return -1;
    ret = build_trie(str,acs);
    return ret;
}

ac_w_node* get_suffix_node(char *str, ac_struct *acs)
{
    ac_w_node* n = acs->root;
    int len = strlen(str);
    int i;
    for(i = 0; i < len; i++)
    {
        n = get_next_node(n, str[i]);
        if(!n)
            break;
    }
    return n;
}

ac_w_node* get_failure_node(char *str, ac_struct *acs)
{
    ac_w_node* failure = NULL;
    int len = strlen(str);
    int i;

    for(i = 1; i < len; i++)
    {
        failure = get_suffix_node(str + i, acs);
        if(failure)
            break;
    }

    if(!failure)
        failure = acs->root;
    return failure;
}

void dfs_traverse_node(ac_w_node *node, char *str, ac_struct *acs)
{
    ac_w_node *e_n = node->next_list;
    int len = strlen(str);
    char *s = (char*) malloc(sizeof(char) * len + 2);
    memcpy(s, str, len);
    s[len] = node->key;
    s[len+1] = '\0'; 
    while(e_n)
    {
        dfs_traverse_node(e_n,s,acs);
        e_n = e_n->next_one;
    }
    node->failure = get_failure_node(s, acs);
    free(s);
}

void ac_build_failure_path(ac_struct *acs)
{
    ac_w_node *n = acs->root;
    dfs_traverse_node(n,"",acs);
}

int ac_finalize(ac_struct *acs)
{
    ac_build_failure_path(acs);
    acs->is_finalized = 1;
    return 0;
}

void dfs_free_node(ac_w_node *node)
{
    ac_w_node *e_n = node->next_list;
    ac_w_node *tmp = NULL;
    while(e_n)
    {
        dfs_free_node(e_n);
        tmp = e_n->next_one;
        free(e_n->pattern);
        free(e_n);
        e_n = tmp;
    }
}

void ac_free(ac_struct *acs)
{
    dfs_free_node(acs->root);
    free(acs->root);
    free(acs);
}

int ac_search(char *str, ac_struct *acs, void (*call_back_f)(char* pattern, int pos))
{
    if(!acs || !acs->is_finalized)
        return -1;
    int len = strlen(str);
    int i;
    ac_w_node *n = NULL;

    for(i = 0; i < len; i++)
    {
        if(str[i] == '\n')
            continue;

        acs->counter++;
        n = get_next_node(acs->current, str[i]);
        if(n)
        {
            acs->current = n; 

            if(n->failure->is_final) // hit, do something cool here
            {
				if(call_back_f)
					(*call_back_f)(n->failure->pattern, acs->counter);
            }
        
            if(n->is_final) // hit, do something cool here
            {
				if(call_back_f)
					(*call_back_f)(n->pattern, acs->counter);
            }
        }
        else
        {
            if(acs->current == acs->root)
                continue;

            acs->current = acs->current->failure; 
            acs->counter--;
            i--;
        }
    }    
    return 0;
}
