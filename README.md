libac
=====

Aho–Corasick string matching algorithm implementation in C for fun.

DFA Construction
=====

1. Build trie by search keys.
2. Find failure node for every nodes in the trie.
   The failure node B is the longest proper suffix for pattern in node A.
        Ex: Given root to node B has pattern "bcde", root to node C has pattern
        "cde" and root to node A has pattern "abcde", then node A's failure 
        node will be node B.
   If longest proper suffix cannot be found, set failure node to root.

Search
=====

Read the input character and transit through the DFA.
Check if current node or current node's failure node is a final state,
if so, generate hit event.
