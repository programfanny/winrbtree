#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "tree.h"

int *CreateArray(int size,int min, int max){
	int *arr;
	arr = (int*)malloc(size*sizeof(int));
	for(int i=0;i<size;i++){
		arr[i]=rand()%(max-min)+min;
	}
	return arr;
}
void FlushArray(int *arr, int size, int min, int max){
	for(int i=0;i<size;i++){
		arr[i]=rand()%(max-min)+min;
	}	
}
void PreorderTraversal(RBTREE *T) {
	if (T == NULL) return;
	printf("%d ", T->data);
	PreorderTraversal(T->left);
	PreorderTraversal(T->right);
}
void InorderTraversal(RBTREE *T) {
	if (T == NULL) return;
	InorderTraversal(T->left);
	printf("%d ", T->data);
	InorderTraversal(T->right);
}
void PostorderTraversal(RBTREE *T) {
	if (T == NULL) return;
	PostorderTraversal(T->left);
	PostorderTraversal(T->right);
	printf("%d ", T->data);
}
void PreorderTraversalMem(RBTREE *T, int **arr, int *arrSize) {
	if (T == NULL) return;    
	(*arrSize)++;
	// arr是一个大小动态变化的区域
	// *arr = (int*)realloc(*arr, (*arrSize) * sizeof(int));
	// arr是一个大小固定的区域
	(*arr)[(*arrSize) - 1] = T->data;
	PreorderTraversalMem(T->left, arr, arrSize);
	PreorderTraversalMem(T->right, arr, arrSize);
}
void InorderTraversalMem(RBTREE *T, int **arr, int *arrSize) {
	if (T == NULL) return;
	InorderTraversalMem(T->left, arr, arrSize);
	(*arrSize)++;
	// arr是一个大小动态变化的区域
	// *arr = (int*)realloc(*arr, (*arrSize) * sizeof(int));
	// arr是一个大小固定的区域
	(*arr)[(*arrSize) - 1] = T->data;
	InorderTraversalMem(T->right, arr, arrSize);
}
void PostorderTraversalMem(RBTREE *T, int **arr, int *arrSize) {
	if (T == NULL) return;
	PostorderTraversalMem(T->left, arr, arrSize);
	PostorderTraversalMem(T->right, arr, arrSize);
	(*arrSize)++;
	// arr是一个大小动态变化的区域
	// *arr = (int*)realloc(*arr, (*arrSize) * sizeof(int));
	// arr是一个大小固定的区域
	(*arr)[(*arrSize) - 1] = T->data;
}
RBTREE* createNode(int data, Color color, RBTREE *left, RBTREE *right, RBTREE *parent) {
	RBTREE *node = (RBTREE*)malloc(sizeof(RBTREE));
	node->data = data;
	node->color = color;
	node->left = left;
	node->right = right;
	node->parent = parent;
	return node;
}

RBTREE* findMin(RBTREE* node) {
	while (node->left != NULL) {
		node = node->left;
	}
	return node;
}
void leftRotate(RBTREE **T, RBTREE *x) {
	RBTREE *y = x->right;
	x->right = y->left;
	if (y->left != NULL) {
		y->left->parent = x;
	}
	y->parent = x->parent;
	if (x->parent == NULL) {
		*T = y;
	} else if (x == x->parent->left) {
		x->parent->left = y;
	} else {
		x->parent->right = y;
	}
	y->left = x;
	x->parent = y;
}

void rightRotate(RBTREE **T, RBTREE *y) {
	RBTREE *x = y->left;
	y->left = x->right;
	if (x->right != NULL) {
		x->right->parent = y;
	}
	x->parent = y->parent;
	if (y->parent == NULL) {
		*T = x;
	} else if (y == y->parent->right) {
		y->parent->right = x;
	} else {
		y->parent->left = x;
	}
	x->right = y;
	y->parent = x;
}

void insertFixup(RBTREE **T, RBTREE *z) {
	while (z->parent != NULL && z->parent->color == RED) {
		if (z->parent == z->parent->parent->left) {
			RBTREE *y = z->parent->parent->right;
			if (y != NULL && y->color == RED) {
				z->parent->color = BLACK;
				y->color = BLACK;
				z->parent->parent->color = RED;
				z = z->parent->parent;
			} else {
				if (z == z->parent->right) {
					z = z->parent;
					leftRotate(T, z);
				}
				z->parent->color = BLACK;
				z->parent->parent->color = RED;
				rightRotate(T, z->parent->parent);
			}
		} else {
			RBTREE *y = z->parent->parent->left;
			if (y != NULL && y->color == RED) {
				z->parent->color = BLACK;
				y->color = BLACK;
				z->parent->parent->color = RED;
				z = z->parent->parent;
			} else {
				if (z == z->parent->left) {
					z = z->parent;
					rightRotate(T, z);
				}
				z->parent->color = BLACK;
				z->parent->parent->color = RED;
				leftRotate(T, z->parent->parent);
			}
		}
	}
	(*T)->color = BLACK;
}
void InsertTreeNode(RBTREE **T, int data) {
	RBTREE *z = createNode(data, RED, NULL, NULL, NULL);
	RBTREE *y = NULL;
	RBTREE *x = *T;
	while (x != NULL) {
		y = x;
		if (z->data < x->data) {
			x = x->left;
		} else {
			x = x->right;
		}
	}

	z->parent = y;
	if (y == NULL) {
		*T = z;
	} else if (z->data < y->data) {
		y->left = z;
	} else {
		y->right = z;
	}
	insertFixup(T, z);
}

void deleteFixup(RBTREE** T, RBTREE* x, RBTREE* xParent) {
	while (x != *T && (x == NULL || x->color == BLACK)) {
		if (x == xParent->left) {
			RBTREE* w = xParent->right; // x 的兄弟节点
			if (w->color == RED) {
				// Case 1: 兄弟节点是红色
				w->color = BLACK;
				xParent->color = RED;
				leftRotate(T, xParent);
				w = xParent->right;
			}
			if ((w->left == NULL || w->left->color == BLACK) &&
				(w->right == NULL || w->right->color == BLACK)) {
				// Case 2: 兄弟节点的两个子节点都是黑色
				w->color = RED;
				x = xParent;
				xParent = x->parent;
			} else {
				if (w->right == NULL || w->right->color == BLACK) {
					// Case 3: 兄弟节点的左子节点是红色，右子节点是黑色
					if (w->left != NULL) {
						w->left->color = BLACK;
					}
					w->color = RED;
					rightRotate(T, w);
					w = xParent->right;
				}
				// Case 4: 兄弟节点的右子节点是红色
				w->color = xParent->color;
				xParent->color = BLACK;
				if (w->right != NULL) {
					w->right->color = BLACK;
				}
				leftRotate(T, xParent);
				x = *T;
			}
		} else {
			// 对称的情况
			RBTREE* w = xParent->left;
			if (w->color == RED) {
				w->color = BLACK;
				xParent->color = RED;
				rightRotate(T, xParent);
				w = xParent->left;
			}
			if ((w->right == NULL || w->right->color == BLACK) &&
				(w->left == NULL || w->left->color == BLACK)) {
				w->color = RED;
				x = xParent;
				xParent = x->parent;
			} else {
				if (w->left == NULL || w->left->color == BLACK) {
					if (w->right != NULL) {
						w->right->color = BLACK;
					}
					w->color = RED;
					leftRotate(T, w);
					w = xParent->left;
				}
				w->color = xParent->color;
				xParent->color = BLACK;
				if (w->left != NULL) {
					w->left->color = BLACK;
				}
				rightRotate(T, xParent);
				x = *T;
			}
		}
	}
	if (x != NULL) {
		x->color = BLACK;
	}
}
void DeleteTreeNode(RBTREE** T, RBTREE* tNode) {
	if (tNode == NULL || *T == NULL) {
		return; // 如果节点为空或树为空，直接返回
	}
	RBTREE* y = tNode; // y 是要删除的节点
	RBTREE* x;         // x 是 y 的子节点
	RBTREE* xParent;   // x 的父节点
	Color yOriginalColor = y->color;

	if (tNode->left == NULL) {
		// Case 1: 没有左子节点
		x = tNode->right;
		xParent = tNode->parent;
		if (x != NULL) {
			x->parent = tNode->parent;
		}
		if (tNode->parent == NULL) {
			*T = x; // 如果删除的是根节点
		} else if (tNode == tNode->parent->left) {
			tNode->parent->left = x;
		} else {
			tNode->parent->right = x;
		}
	} else if (tNode->right == NULL) {
		// Case 2: 没有右子节点
		x = tNode->left;
		xParent = tNode->parent;
		if (x != NULL) {
			x->parent = tNode->parent;
		}
		if (tNode->parent == NULL) {
			*T = x; // 如果删除的是根节点
		} else if (tNode == tNode->parent->left) {
			tNode->parent->left = x;
		} else {
			tNode->parent->right = x;
		}
	} else {
		// Case 3: 有两个子节点
		y = findMin(tNode->right); // 找到后继节点
		yOriginalColor = y->color;
		x = y->right;
		xParent = y->parent;
		if (y->parent == tNode) {
			xParent = y;
		} else {
			if (x != NULL) {
				x->parent = y->parent;
			}
			y->parent->left = x;
			y->right = tNode->right;
			tNode->right->parent = y;
		}
		if (tNode->parent == NULL) {
			*T = y; // 如果删除的是根节点
		} else if (tNode == tNode->parent->left) {
			tNode->parent->left = y;
		} else {
			tNode->parent->right = y;
		}
		y->parent = tNode->parent;
		y->color = tNode->color;
		y->left = tNode->left;
		tNode->left->parent = y;
	}

	// 如果删除的节点是黑色，需要修复红黑树的性质
	if (yOriginalColor == BLACK) {
		deleteFixup(T, x, xParent);
	}

	// 释放被删除节点的内存
	free(tNode);
}
// There are some BUGS here.
void ModifyTreeNode(RBTREE** T, RBTREE* tNode, int newValue) {
	if (tNode == NULL) return; 
	//int oldValue = tNode->data;
	tNode->data = newValue;

	if ((tNode->left != NULL && tNode->left->data > newValue) ||
		(tNode->right != NULL && tNode->right->data < newValue)) {
		DeleteTreeNode(T, tNode);
		InsertTreeNode(T, newValue);
	}
}

void BuildRBTree(RBTREE **T, int *nums, int numsSize) {
	*T = NULL;
	for (int i = 0; i < numsSize; i++) {
		InsertTreeNode(T, nums[i]);
	}
}
void DestroyTree(RBTREE **T){
	if(*T==NULL)return;
	DestroyTree(&(*T)->left);
	DestroyTree(&(*T)->right);
	free(*T);
	*T=NULL;
}
RBTREE *SearchTree(RBTREE *T, int data, int *pos){
	RBTREE *current = T; 
	while (current != NULL) {
		if (data == current->data) {
			return current; 
		} else if (data < current->data) {
			current = current->left;
			*pos =*pos*2;
		} else {
			current = current->right;
			*pos =*pos*2+1; 
		}
	}
	return NULL;
}
void MarkNode(HDC hdc, RBTREE *T, int pos){
	if(T==NULL)return;
	int h, y;
	if(pos==0)y=0;
	else{
		h=power_of_two(pos);
		y=1<<h;
	}
	int x=pos-y;
	int w=1024/y;
	HBRUSH hNullBrush=GetStockObject(NULL_BRUSH);
	HBRUSH hOldBrush=SelectObject(hdc, hNullBrush);
	HPEN hPen=CreatePen(PS_SOLID, 2, RGB(192,0,0));
	HPEN hOldPen = SelectObject(hdc, hPen);    
	Ellipse(hdc, 120 + x * w + w / 2 - 7, 10 + h * 30 - 1, 120 + x * w + w / 2 + 10, 10 + h * 30 + 13);
	SelectObject(hdc, hOldPen);
	SelectObject(hdc, hOldBrush);
	DeleteObject(hPen);
}
int power_of_two(int n){
	if(n<=0)return 0;
	int power=0;
	while(n>0){
		n=n>>1;
		power++;
	}
	return power-1;
} 
int stringToInt(const char *str, int *isValid) {  
	if (strcmp(str, "null") == 0) {  
		*isValid = 0;  
		return 0;  
	}  
	char *endptr;  
	long result = strtol(str, &endptr, 10);  
	if (*endptr != '\0' || result > INT_MAX || result < INT_MIN) {  
		*isValid = 0;  
		return 0;  
	}  
	*isValid = 1;  
	return (int)result;  
}
RBTREE* buildTreeFromLevelOrder(const char *input) {
	// 去掉首尾括号
	int len = strlen(input);
	char processed[len - 2 + 1];
	strncpy(processed, input + 1, len - 2);
	processed[len - 2] = '\0';
	// 分割字符串
	char *token = strtok(processed, ",");
	if (token == NULL) return NULL;
	// 检查根节点是否为null
	int isValid;
	int rootVal = stringToInt(token, &isValid);
	if (!isValid) return NULL;
	// 创建根节点
	RBTREE *root = (RBTREE *)malloc(sizeof(RBTREE));
	root->data = rootVal;
	root->left = NULL;
	root->right = NULL;
	// 使用队列辅助构建二叉树
	RBTREE **queue = (RBTREE **)malloc(1000 * sizeof(RBTREE *));
	int front = 0, rear = 0;
	queue[rear++] = root;
	while ((token = strtok(NULL, ",")) != NULL) {
		RBTREE *current = queue[front++];
		int leftIsValid, rightIsValid;
		int leftVal = stringToInt(token, &leftIsValid);
		token = strtok(NULL, ",");
		int rightVal = (token != NULL) ? stringToInt(token, &rightIsValid) : 0;
		if (leftIsValid) {
			current->left = (RBTREE *)malloc(sizeof(RBTREE));
			current->left->data = leftVal;
			current->left->left = NULL;
			current->left->right = NULL;
			queue[rear++] = current->left;
		}
		if (rightIsValid) {
			current->right = (RBTREE *)malloc(sizeof(RBTREE));
			current->right->data = rightVal;
			current->right->left = NULL;
			current->right->right = NULL;
			queue[rear++] = current->right;
		}
	}
	free(queue);
	return root;
}

// void DisplayTree(HDC hdc, RBTREE* T, int pos, int left, int top, int width, int heigh, int layer)
void DisplayTree(HDC hdc, RBTREE* T, int pos){
	if(T==NULL)return;
	int h, y;
	if(pos==0)y=0;
	else{
		h=power_of_two(pos);
		y=1<<h;
	}
	int x=pos-y;
	int w=1024/y;
	char buf[10];
	if(h<8){//树的深度最多只显示8层
		if(h){        
			if(pos%2==0){
				MoveToEx(hdc,120+x*w+w/2,10+h*30,NULL);
				LineTo(hdc,120+x*w+w-4,10+h*30-20);
			}else{
				MoveToEx(hdc,120+x*w+w/2,10+h*30,NULL);
				LineTo(hdc,120+x*w+4,10+h*30-20);
			}
		}
		if (T->color == RED) {
			//SetTextColor(hdc, RGB(255, 0, 0)); // 红色文本
			SetBkColor(hdc, RGB(255, 200, 200)); // 浅红色背景
		} else {
			//SetTextColor(hdc, RGB(0, 0, 0)); // 黑色文本
			SetBkColor(hdc, RGB(200, 200, 200)); // 浅灰色背景
		}    
		HBRUSH hRedBrush = CreateSolidBrush(RGB(255, 200, 200)); // 浅红色画刷
		HBRUSH hBlackBrush = CreateSolidBrush(RGB(200, 200, 200)); // 浅灰色画刷
		HPEN hRedPen=CreatePen(PS_SOLID, 2, RGB(255,192,192));
		HPEN hBlackPen=CreatePen(PS_SOLID,2,RGB(192,192,192));
		HPEN hOldPen;
		if (T->color == RED) {
			SelectObject(hdc, hRedBrush);
			hOldPen = SelectObject(hdc,hRedPen);
			//SetTextColor(hdc,RGB(255,0,0));
		} else {
			SelectObject(hdc, hBlackBrush);
			hOldPen = SelectObject(hdc,hBlackPen);
		}
		Ellipse(hdc, 120 + x * w + w / 2 - 7, 10 + h * 30 - 1, 120 + x * w + w / 2 + 10, 10 + h * 30 + 13);
		sprintf(buf,"%d",T->data);
		TextOut(hdc,120+x*w+w/2-4,10+h*30,buf,strlen(buf));     
		SelectObject(hdc, GetStockObject(WHITE_BRUSH));
		SelectObject(hdc, hOldPen);
		SetBkColor(hdc, RGB(255,255,255));
		DeleteObject(hRedBrush);
		DeleteObject(hBlackBrush);  
		DeleteObject(hRedPen);
		DeleteObject(hBlackPen);
	}  
	if (T->left == NULL) {
		if(h<8){
			MoveToEx(hdc, 120 + (pos - (1 << h)) * w + w / 2 , 10 + h * 30 +13, NULL);
			LineTo(hdc, 120 + (pos * 2 - (1 << (h + 1))) * w/2 + w / 4 , 10 + (h + 1) * 30 + 4);
			HBRUSH hBlackBrush = CreateSolidBrush(RGB(100, 100, 100));
			SelectObject(hdc, hBlackBrush);
			Ellipse(hdc, 120 + (pos * 2 - (1 << (h + 1))) * w/2 + w / 4-2 , 10 + (h + 1) * 30 +4,120 + (pos * 2 - (1 << (h + 1))) * w/2 + w / 4+2 , 10 + (h + 1) * 30 + 8);
			SelectObject(hdc, GetStockObject(WHITE_BRUSH));
			DeleteObject(hBlackBrush);
		}
	} else {
		DisplayTree(hdc, T->left, pos * 2);
	}
   if (T->right == NULL) {
   		if(h<8){
			MoveToEx(hdc, 120 + (pos - (1 << h)) * w + w / 2 , 10 + h * 30 +13, NULL);
			LineTo(hdc, 120 + (pos * 2 + 1 - (1 << (h + 1))) * w/2 + w / 4 , 10 + (h + 1) * 30 + 4);
			HBRUSH hBlackBrush = CreateSolidBrush(RGB(100, 100, 100));
			SelectObject(hdc, hBlackBrush);
			Ellipse(hdc, 120 + (pos * 2 + 1 - (1 << (h + 1))) * w/2 + w / 4-2 , 10 + (h + 1) * 30 +4, 120 + (pos * 2 + 1 - (1 << (h + 1))) * w/2 + w / 4 + 2, 10 + (h + 1) * 30 + 8);
			SelectObject(hdc, GetStockObject(WHITE_BRUSH));
			DeleteObject(hBlackBrush);
		}
	} else {
		DisplayTree(hdc, T->right, pos * 2 + 1);
	}    
}
void ShowData(HDC hdc, int *nums, int numsSize, int dir, int rows,  int left,int top, int dx, int dy){
	char buf[10];
	for (int i = 0; i < numsSize; i++) {
		sprintf(buf, "%d ", nums[i]);
		if(dir==1)
			TextOut(hdc, left + i / rows * dx, top + i % rows * dy, buf, strlen(buf));
		else
			TextOut(hdc, left + i % rows * dx, top + i / rows * dy, buf, strlen(buf));
	}
}
void ShowTreeNet(HDC hdc, int left, int top, int width, int height, int layer){
	int pow=1;
	for(int i=0;i<layer;i++){
		MoveToEx(hdc,left,i*height,NULL);
		LineTo(hdc,left+width,i*height);
		pow*=2;
		for(int j=0;j<=pow;j++){
			MoveToEx(hdc,left+j*width/pow,i*height,NULL);
			LineTo(hdc, left+j*width/pow,(layer-1)*height);
		}
	}    
}
void MarkTreeNode(HDC hdc, RBTREE *tree, int pos,int left, int top, int right, int bottom){
	RECT rect;
	rect.left=left;
	rect.top=top;
	rect.right=right;
	rect.bottom=bottom;
	FillRect(hdc, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));
	DisplayTree(hdc,tree,1);
	MarkNode( hdc, tree, pos );  
}
void GetToPos(RBTREE *T, RBTREE **TreeNode, int pos) {
	if (T == NULL || pos < 1) {
		*TreeNode = NULL; 
		return;
	}
	RBTREE *current = T; 
	int mask = 1 << 30; 
	while ((pos & mask) == 0) {
		mask >>= 1;
	}
	mask >>= 1;
	while (mask != 0) {
		if (current == NULL) {
			*TreeNode = NULL;
			return;
		}
		if ((pos & mask) == 0) {
			current = current->left; 
		} else {
			current = current->right;
		}
		mask >>= 1; 
	}
	*TreeNode = current;
}
void FlushPage(HWND hwnd, HDC hdc, RBTREE *tree, int *nums, int numsSize,int *arr, int arrSize){
	RECT rect;
	GetClientRect(hwnd, &rect);
	FillRect(hdc, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));
	ShowData(hdc, nums, numsSize, 1, 25, 10, 20, 20, 15);
	DisplayTree(hdc,tree,1);
	ShowData(hdc, arr, arrSize, 0, 50, 120, 300, 20, 15 );
	InvalidateRect(hwnd, NULL, FALSE);
}
void RemoveNumsData(int *nums, int val,int *numsSize){
	int i=0;
	while(nums[i]!=val){i++;}
	while(i+1<*numsSize){
		nums[i]=nums[i+1];i++;
	}
	(*numsSize)--;
}
void expandArray(int **nums, int *capacity, int numsSize) {
    if (numsSize < *capacity) {
        return;
    }
    int newCapacity = *capacity * 3 / 2;
    int *newNums = realloc(*nums, newCapacity * sizeof(int));
    if (newNums == NULL) {
        free(*nums);
        printf("Memory allocation failed.\n");
        exit(1);
    }
    *nums = newNums;
    *capacity = newCapacity;
}

RBTREE *ReadNode(FILE *file) {
    char line[100];
    if (fgets(line, sizeof(line), file) == NULL) {
        return NULL;
    }
    if (strcmp(line, "-1\n") == 0) {
        return NULL;
    }
    int data;
    Color color;
    if (sscanf(line, "%d %s", &data, line) != 2) {
        fprintf(stderr, "Invalid file format\n");
        return NULL;
    }
    if (strcmp(line, "RED") == 0) {
        color = RED;
    } else if (strcmp(line, "BLACK") == 0) {
        color = BLACK;
    } else {
        fprintf(stderr, "Invalid color in file\n");
        return NULL;
    }
    RBTREE *newNode = (RBTREE*)malloc(sizeof(RBTREE));
    if (newNode == NULL) {
        perror("Failed to allocate memory");
        return NULL;
    }
    newNode->data = data;
    newNode->color = color;
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->left = ReadNode(file);
    newNode->right = ReadNode(file);
    return newNode;
}
void ReadRBTree(RBTREE **root, char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open file");
        *root = NULL;
        return;
    }
    *root = ReadNode(file);
    fclose(file);
}

void SaveNode(RBTREE *node, FILE *file) {
    if (node == NULL) {
        fprintf(file, "-1\n");
        return;
    }
    fprintf(file, "%d %s\n", node->data, node->color == RED ? "RED" : "BLACK");
    SaveNode(node->left, file);
    SaveNode(node->right, file);
}
void SaveRBTree(RBTREE *root, char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Failed to open file");
        return;
    }
    SaveNode(root, file);
    fclose(file);
}