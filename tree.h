#ifndef _TREE_H_
#define _TREE_H_
#include <windows.h>

typedef enum { RED, BLACK } Color;
typedef enum { SEARCH, ADDNODE, MODIFY } op;
typedef struct RBTREE RBTREE;
struct RBTREE {
    int data;
    Color color;
    RBTREE *left, *right, *parent;
};
int *CreateArray(int size,int min, int max);
void FlushArray(int *arr, int size, int min, int max);
void RemoveNumsData(int *nums, int val,int *numsSize);
void expandArray(int **nums, int *capacity, int numsSize);
int stringToInt(const char *str, int *isValid);

void InsertTreeNode(RBTREE **T, int data);
void DeleteTreeNode(RBTREE** T, RBTREE* tNode); 
void ModifyTreeNode(RBTREE** T, RBTREE* tNode, int newValue);

void SaveRBTreeToFile(RBTREE *root, const char *filename); 
void LoadRBTreeFromFile(RBTREE **root, const char *filename);
void SaveRBTree(RBTREE *root, const char *filename);
void LoadRBTree(RBTREE **root, const char *filename); 
void BuildRBTree(RBTREE **T, int *nums, int numsSize);
void DestroyTree(RBTREE **T);

void PreorderTraversal(RBTREE *T);
void InorderTraversal(RBTREE *T);
void PostorderTraversal(RBTREE *T);
void LevelorderTraversal(RBTREE *T);

void PreorderTraversalMem(RBTREE *T, int **arr, int *arrSize) ;
void InorderTraversalMem(RBTREE *T, int **arr, int *arrSize) ;
void PostorderTraversalMem(RBTREE *T, int **arr, int *arrSize) ;
void LevelorderTraversalMem(RBTREE *T, int **arr, int *arrSize) ;

RBTREE *SearchTree(RBTREE *T, int data, int *pos);
void GetToPos(RBTREE *T, RBTREE **TreeNode, int pos);
void DisplayTree(HDC hdc, RBTREE* T, int pos);
void FlushPage(HWND hwnd, HDC hdc, RBTREE *tree, int *nums, int numsSize,int *arr, int arrSize);
// void DisplayTree(HDC hdc, RBTREE* T, int pos, int left, int top, int width, int heigh, int layer);
void MarkNode(HDC hdc, RBTREE *T, int pos);
void MarkTreeNode(HDC hdc, RBTREE *tree, int pos,int left, int top, int right, int bottom);
void ShowData(HDC hdc, int *nums, int numsSize, int dir, int rows, int left,int top, int dx, int dy);
void ShowTreeNet(HDC hdc, int left, int top, int width, int height, int layer);
int power_of_two(int n);
#endif