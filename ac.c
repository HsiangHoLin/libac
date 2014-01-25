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
	int id;
};

struct AC_STRUCT_
{
    char is_finalized;
    ac_w_node *root;
    ac_w_node *current;
    int counter;
};

static inline ac_w_node* trie_node_get_children_head(ac_w_node* n)
{
	return n->next_list;
}

static inline ac_w_node* trie_node_get_next_sibling(ac_w_node* n)
{
	return n->next_one;
}

void trie_node_set_next_sibling(ac_w_node* node, ac_w_node* next)
{
	node->next_one = next;
}

void trie_node_set_next_head(ac_w_node* node, ac_w_node* next)
{
	node->next_list = next;
}

ac_w_node* trie_node_lookup_children(ac_w_node* n, char ch)
{
    ac_w_node *next = trie_node_get_children_head(n);
    while(next)
    {
        if(next->key == ch)
            break;
        next = trie_node_get_next_sibling(next);
    }
    return next;
}

void trie_node_set_children(ac_w_node* p_n, ac_w_node* c_n)
{
    ac_w_node *next = trie_node_get_children_head(p_n);
    if(next == NULL)
		trie_node_set_next_head(p_n, c_n);
    while(next)
    {
        if(trie_node_get_next_sibling(next) == NULL)
        {
			trie_node_set_next_sibling(next, c_n);
            break;
        }
		next = trie_node_get_next_sibling(next);
    }
}

ac_w_node* trie_node_new(char key)
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
	n->id = -1;
    return n;
}

static inline int trie_node_get_key(ac_w_node* n)
{
	return n->key;
}

static inline int trie_node_is_final(ac_w_node* n)
{
	return n->is_final;
}

static inline ac_w_node* trie_node_get_failure(ac_w_node* n)
{
	return n->failure;
}

static inline char* trie_node_get_pattern(ac_w_node* n)
{
	return n->pattern;
}

static inline ac_w_data trie_node_get_data(ac_w_node* n)
{
	ac_w_data data;
	data.pattern = n->pattern;
	data.id = n->id;
	return data;
}

void trie_node_free_children(ac_w_node *node)
{
    ac_w_node *e_n = trie_node_get_children_head(node);
    ac_w_node *tmp = NULL;
    while(e_n)
    {
        trie_node_free_children(e_n);
        tmp = trie_node_get_next_sibling(e_n);
        free(trie_node_get_pattern(e_n));
        free(e_n);
        e_n = tmp;
    }
}

ac_w_node* trie_get_suffix_node(char *str, ac_struct *acs)
{
    ac_w_node* n = acs->root;
    int len = strlen(str);
    int i;
    for(i = 0; i < len; i++)
    {
        n = trie_node_lookup_children(n, str[i]);
        if(!n)
            break;
    }
    return n;
}

ac_w_node* trie_get_longest_suffix(char *str, ac_struct *acs)
{
    ac_w_node* failure = NULL;
    int len = strlen(str);
    int i;

    for(i = 1; i < len; i++)
    {
        failure = trie_get_suffix_node(str + i, acs);
        if(failure)
            break;
    }

    if(!failure)
        failure = acs->root;
    return failure;
}

void trie_set_failure(ac_w_node *node, char *str, ac_struct *acs)
{
    ac_w_node *e_n = trie_node_get_children_head(node);
    int len = strlen(str);
    char *s = (char*) malloc(sizeof(char) * len + 2);
    memcpy(s, str, len);
    s[len] = trie_node_get_key(node);
    s[len+1] = '\0'; 
    while(e_n)
    {
        trie_set_failure(e_n,s,acs);
        e_n = trie_node_get_next_sibling(e_n);
    }
    node->failure = trie_get_longest_suffix(s, acs);
    free(s);
}

int trie_build(ac_w_data data, ac_struct *ac, int opt)
{
    if(!ac || !ac->root) 
        return -1;

    int i;
    ac_w_node *n = ac->root;
    ac_w_node *prev = NULL;
    int len = strlen(data.pattern);
	char* str = data.pattern;

    for(i = 0; i < len; i++)
    {
        prev = n;
        n = trie_node_lookup_children(n, str[i]);
        if(!n)
        {
            n = trie_node_new(str[i]);
            if(!n)
                return -1;
            trie_node_set_children(prev,n);
        }
        if( i == len -1)
        {
            if(n->is_final == 0)
            {
                n->is_final = 1;
                n->pattern = (char*) malloc(len+1);
                memcpy(n->pattern, data.pattern, len);
                n->pattern[len] = '\0';
				n->id = data.id;
            }
            else
            {
				if(opt == DUPKEY_ABORT)
				{
                	printf("Duplicate keyword \"%s\"\n", n->pattern);
                	return -1;
				}
            }
        }
    }
    return 0;
}

ac_struct* ac_new(void)
{
    ac_struct* ac = (ac_struct*) malloc(sizeof(ac_struct));
    if(!ac)
        return NULL;
    ac->is_finalized = 0;
    ac->counter = 0;
    ac->root = (ac_w_node*) malloc(sizeof(ac_w_node));
    if(!ac->root)
    {
        free(ac);
        return NULL;
    }
    ac->root->next_list = NULL;
    ac->root->next_one = NULL;
    ac->root->failure = ac->root;
    ac->root->key = 0;
    ac->root->is_final = 0;
    ac->current = ac->root;
    
    return ac;
}

int ac_load_search_key(ac_w_data data, ac_struct *acs, int opt)
{
    int ret = 0;
    if(!acs || acs->is_finalized)
        return -1;
    ret = trie_build(data,acs,opt);
    return ret;
}

void ac_build_failure_path(ac_struct *acs)
{
    ac_w_node *n = acs->root;
    trie_set_failure(n,"",acs);
}

int ac_finalize(ac_struct *acs)
{
    ac_build_failure_path(acs);
    acs->is_finalized = 1;
    return 0;
}

void ac_free(ac_struct *acs)
{
    trie_node_free_children(acs->root);
    free(acs->root);
    free(acs);
}

int ac_search(char *str, ac_struct *acs, void (*call_back_f)(ac_w_data data, int pos))
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
        n = trie_node_lookup_children(acs->current, str[i]);
        if(n)
        {
            acs->current = n; 

            n = trie_node_get_failure(n);
            while(n != acs->root) // trace all other failures untill root
            {
                if(trie_node_is_final(n)) // hit, do something cool here
                {
				    if(call_back_f)
					    (*call_back_f)(trie_node_get_data(n), acs->counter);
                }
                n = trie_node_get_failure(n);
            }

            if(trie_node_is_final(acs->current)) // hit, do something cool here
            {
				if(call_back_f)
					(*call_back_f)(trie_node_get_data(acs->current), acs->counter);
            }
        }
        else
        {
            if(acs->current == acs->root)
                continue;

			acs->current = trie_node_get_failure(acs->current);
            acs->counter--;
            i--;
        }
    }    
    return 0;
}
