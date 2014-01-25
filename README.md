libac
=====

Aho–Corasick string matching algorithm implementation in C for fun.

### DFA Construction

1. Build trie by search keys.
2. Find failure node for every nodes in the trie.
   The failure node, say B, of a node, say A, must be the longest proper suffix for pattern in node A.
        Ex: Given node B has pattern "bcde", node C has pattern
        "cde" node A has pattern "abcde", then node A's failure 
        node will be node B.
   However, if longest proper suffix cannot be found, set failure node to root.

### Search

While doing searching, we read the input file by each character and, accordingly, transit the DFA state. 
Then at every state we transited to, check if the current node or the failure node of the current node is a final state, if so, generate hit event.

