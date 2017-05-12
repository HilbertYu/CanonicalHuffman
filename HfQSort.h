#ifndef HFQSORT_H
#define HFQSORT_H

#include "HuffmanBase.h"


typedef int (*huffmanNode_cmp)(const HuffmanNode*, const HuffmanNode*);
void sort_hf_tree_qsort(HuffmanNode* tree, int num, huffmanNode_cmp cmp);


#endif /* end of include guard: HFQSORT_H */


