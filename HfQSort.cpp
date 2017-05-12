#include "HfQSort.h"

#include <stdlib.h>
struct HF_Stack
{
    int left;
    int right;
};

#define HF_STACK_PUSH(le,ri) (++top, top->left = (le), top->right = (ri))
#define HF_STACK_POP(le,ri) ((le) = top->left, (ri) = top->right, --top)
#define HF_STACK_NONEMPTY (top > stack)
#define HF_STACK_SIZE (top - stack)

#define HF_MAX_STACK 1024

#define HF_CHECK_STACK_SIZE \
do {\
    if (HF_STACK_SIZE >= HF_MAX_STACK) {\
        HYDEBUG_QUIT("FATAL ERROR! stack size = %lu\n", HF_STACK_SIZE);\
    }\
}while(0)


static int sort_hf_tree_one_qsort(HuffmanNode* tree, int left, int right, huffmanNode_cmp cmp);

void sort_hf_tree_qsort(HuffmanNode* tree, int num, huffmanNode_cmp cmp) {
    HF_Stack stack[HF_MAX_STACK];
    HF_Stack * top = stack;

    int left = 0;
    int right = num - 1;
    int pos = 0;

    HF_STACK_PUSH(left, right);

    while (HF_STACK_NONEMPTY) {

        HF_STACK_POP(left , right);

        if (left >= right)
            continue;

        pos = sort_hf_tree_one_qsort(tree, left, right, cmp);
        HF_STACK_PUSH(left, pos - 1);
        HF_CHECK_STACK_SIZE;
        HF_STACK_PUSH(pos + 1, right);
        HF_CHECK_STACK_SIZE;
    }
}


static
int sort_hf_tree_one_qsort(HuffmanNode* tree, int left, int right, huffmanNode_cmp cmp) {


    if (left >= right) {
        return -1;
    }

    int i = left;
    int j = right + 1;

    HuffmanNode* pivot = tree + left;
    HuffmanNode tmp_node;


    do {
        do { ++i; } while(cmp(tree + i, pivot) < 0);
        do { --j; } while(cmp(tree + j, pivot) > 0);

        if (i < j) {
            HF_TREE_SWAP_POINTER(tree + i, tree + j, &tmp_node);
        }


    } while (i < j);

    HF_TREE_SWAP_POINTER(tree + left, tree + j, &tmp_node);

    return j;
}
