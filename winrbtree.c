#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "tree.h"
#include "treeres.h"

WNDPROC OldProc;
// LRESULT CALLBACK EditSubClassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
LRESULT CALLBACK EditSubClassProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK InputDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow){
	static TCHAR szAppName[]=TEXT("HelloWin");
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;
	wndclass.style=CS_HREDRAW|CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL,IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL,IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName="TREEMENU";
	wndclass.lpszClassName=szAppName;
	if(!RegisterClass(&wndclass)){
		MessageBox(NULL,TEXT("This program requires Windows NT!"),szAppName,MB_ICONERROR);
		return 0;
	}
	hwnd = CreateWindow(szAppName,TEXT("The Hello Program"),WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,NULL,NULL,hInstance, NULL);
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	while(GetMessage(&msg,NULL,0,0)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam ){
	HINSTANCE hInstance;
	HDC hdc;
	PAINTSTRUCT ps;
	// RECT rect;
	UINT state;
	TCHAR* pszInput;
	int value, isValid, posx, posy;
	char buf[100];
	RBTREE *treeNode;
	static HBITMAP hBitmap = NULL;
	static HDC hMemDC = NULL;
	static int cxClient, cyClient; // 用于存储客户区的大小	
	static RBTREE *tree=NULL,*activeNode=NULL;
	static int *nums,numsSize,capacity, pos;
	static int *arr=NULL, arrSize=0;
	static HPEN *hPen;
	static HFONT hFont;
	static HMENU hMenu;
	static HWND hwndEdit,hLabel;
	static int op;
	switch(message){
		case WM_CREATE:
			hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
			hwndEdit = CreateWindow("EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL, 200,350,100,20, hwnd, (HMENU)IDEDIT, hInstance, NULL);
			ShowWindow(hwndEdit,SW_HIDE);
			OldProc = (WNDPROC)SetWindowLong(hwndEdit, GWL_WNDPROC, (LONG)EditSubClassProc);
			hLabel=CreateWindow("static","Search",WS_CHILD | WS_VISIBLE | ES_RIGHT | ES_AUTOHSCROLL, 100,350,90,20, hwnd, (HMENU)IDLABEL, hInstance, NULL);
			ShowWindow(hLabel,SW_HIDE);
			// numsSize=rand()%100;
			capacity=150;
			numsSize=100;
			nums = CreateArray(capacity, 0, 100);
			arr=(int*)malloc(numsSize*sizeof(int));
			BuildRBTree(&tree,nums,numsSize);
			arrSize=0;
			InorderTraversalMem(tree, &arr, &arrSize);			
			hPen=malloc(32*sizeof(HPEN));
			for(int i=0;i<32;i++){
				hPen[i]=CreatePen(PS_SOLID,1,RGB(8*i,0,0));
			}
			hFont=CreateFont(-8,-4,0,0,0,0,0,0,0,0,0,0,0,0);
			hMenu=GetMenu(hwnd);
			hdc = GetDC(hwnd);
    		hMemDC = CreateCompatibleDC(hdc);
    		SelectObject(hMemDC, hFont);
    		ReleaseDC(hwnd, hdc);
			return 0;
		case WM_SIZE:
			cxClient = LOWORD(lParam);
			cyClient = HIWORD(lParam);
			if (hBitmap) DeleteObject(hBitmap);
			hdc = GetDC(hwnd);
			hBitmap = CreateCompatibleBitmap(hdc, cxClient, cyClient);
			ReleaseDC(hwnd, hdc);
			SelectObject(hMemDC, hBitmap);
			FlushPage(hwnd, hMemDC, tree, nums, numsSize, arr, arrSize);
			return 0;	
		case WM_PAINT:
		    hdc = BeginPaint(hwnd, &ps);
		    if (hMemDC && hBitmap) {
		        BitBlt(hdc, 0, 0, cxClient, cyClient, hMemDC, 0, 0, SRCCOPY);
		    }
		    // ShowTreeNet(hdc,120,10,1024,30,9);
		    sprintf(buf,"%d/%d",numsSize,capacity);
		    TextOut(hdc,0,0,buf,strlen(buf));
		    EndPaint(hwnd, &ps);
		    return 0;
		case WM_TIMER:
			PostMessage(hwnd, WM_KEYDOWN, VK_RETURN, 0);
			return 0;	
		case WM_KEYDOWN:
			switch(wParam){
				case VK_ESCAPE:
					PostMessage(hwnd, WM_DESTROY,0,0);
					break;
				case VK_RETURN:
					// numsSize=rand()%capacity;
					FlushArray(nums,numsSize,0,100);
					BuildRBTree(&tree, nums, numsSize);
					arrSize=0;
					InorderTraversalMem(tree, &arr, &arrSize);
					FlushPage(hwnd, hMemDC, tree, nums, numsSize, arr, arrSize);
					break;
				case VK_F5:
					PostMessage(hwnd, WM_COMMAND,IDM_EDIT_SEARCH,0);
					break;	
				case VK_INSERT:
					PostMessage(hwnd, WM_COMMAND,IDM_EDIT_ADD,0);
					break;	
				case VK_DELETE:
					PostMessage(hwnd, WM_COMMAND,IDM_EDIT_DEL,0);
					break;	
				case VK_F4:
					PostMessage(hwnd, WM_COMMAND,IDM_EDIT_MODIFY,0);
					break;
				case VK_F2:
					LoadRBTreeFromFile(&tree, "tree2.dat");
					if(nums)free(nums);
					nums=(int*)malloc(capacity*sizeof(int));
					numsSize=0;
					PreorderTraversalMem(tree, &nums, &numsSize) ;
					arrSize=0;
					InorderTraversalMem(tree, &arr, &arrSize);
					FlushPage(hwnd, hMemDC, tree, nums, numsSize, arr, arrSize);
					break;
				case VK_F3:
					SaveRBTreeToFile(tree, "tree2.dat");
					PostMessage(hwnd, WM_COMMAND,IDM_TREE_SAVE,0);
					break;	
			}
			return 0;	
		case WM_LBUTTONDOWN:
			posx=LOWORD(lParam);
			posy=HIWORD(lParam);
			if(120<posx && posx<1164 && 10<posy && posy<250){
				int layer, dxpos, uxpos;
				layer=posy/30;
				dxpos=1024>>layer;
				uxpos=(posx-120)/dxpos;
				pos=(1<<layer)+uxpos ;
				GetToPos(tree, &activeNode, pos);
				sprintf(buf, "(x,y)=(%3d , %d) layer=%d dxpos=%d uxpos=%d, pos=%d data=%d ", posx, posy, layer, dxpos, uxpos,pos, (treeNode==NULL)?-1:treeNode->data);
				SetWindowText(hwnd,buf);
				MarkTreeNode(hMemDC, tree, pos,120, 10, 1164,280);
				InvalidateRect(hwnd, NULL, FALSE);
			}
			return 0;	
		case WM_COMMAND:
			switch(wParam){
				case IDM_TREE_NEW:
					PostMessage(hwnd, WM_KEYDOWN,VK_RETURN,0);
					break;
				case IDM_TREE_OPEN:
					LoadRBTree(&tree, "tree.dat");
					if(nums)free(nums);
					nums=(int*)malloc(capacity*sizeof(int));
					numsSize=0;
					PreorderTraversalMem(tree, &nums, &numsSize) ;
					arrSize=0;
					InorderTraversalMem(tree, &arr, &arrSize);
					FlushPage(hwnd, hMemDC, tree, nums, numsSize, arr, arrSize);					
					break;
				case IDM_TREE_SAVE:
					SaveRBTree(tree, "tree.dat");
					break;
				case IDM_APP_EXIT:
					PostMessage( hwnd, WM_DESTROY, 0, 0 );
					break;
				case IDM_EDIT_UNDO:
					break;
				case IDM_EDIT_ADD:
					state = GetMenuState(hMenu, IDM_TIMER_ON, MF_BYCOMMAND); 
					if(state!=-1){ // The Menu exist 
						if(state & MF_CHECKED) { // if The TIMER is ON, Turn it off first 
							PostMessage(hwnd, WM_COMMAND, IDM_TIMER_OFF,0);
							//PostMessage(hwnd, WM_COMMAND, IDM_EDIT_SEARCH,0);
						}
						SetWindowText(hLabel,"Add");
						ShowWindow(hLabel,SW_SHOW);
						ShowWindow(hwndEdit,SW_SHOW);
						SetFocus(hwndEdit);
						op=ADDNODE;
					}
					break;
				case IDM_EDIT_DEL:
					state = GetMenuState(hMenu, IDM_TIMER_ON, MF_BYCOMMAND); 
					if(state!=-1){
						if(state & MF_CHECKED) {// if The TIMER is ON, Turn it off first 
							PostMessage(hwnd, WM_COMMAND, IDM_TIMER_OFF,0);
						}
						if(activeNode){
							RemoveNumsData(nums, activeNode->data,&numsSize);
							DeleteTreeNode(&tree,activeNode);
							activeNode=NULL;
							arrSize=0;
							InorderTraversalMem(tree, &arr, &arrSize);						
						}
						FlushPage(hwnd, hMemDC, tree, nums, numsSize, arr, arrSize);
					}
					break;
				case IDM_EDIT_MODIFY:
					state = GetMenuState(hMenu, IDM_TIMER_ON, MF_BYCOMMAND); 
					if(state!=-1){
						if(state & MF_CHECKED) {// if The TIMER is ON, Turn it off first 
							PostMessage(hwnd, WM_COMMAND, IDM_TIMER_OFF,0);
							PostMessage(hwnd, WM_COMMAND, IDM_EDIT_SEARCH,0);
						}else{
							SetWindowText(hLabel,"Modify");
							ShowWindow(hLabel,SW_SHOW);
							ShowWindow(hwndEdit,SW_SHOW);
							SetFocus(hwndEdit);
							op=MODIFY;
						}
					}
					break;
				case IDM_EDIT_SEARCH:
					state = GetMenuState(hMenu, IDM_TIMER_ON, MF_BYCOMMAND); 
					if(state!=-1){
						if(state & MF_CHECKED) {// if The TIMER is ON, Turn it off first 
							PostMessage(hwnd, WM_COMMAND, IDM_TIMER_OFF,0);
							PostMessage(hwnd, WM_COMMAND, IDM_EDIT_SEARCH,0);
						}else{
							SetWindowText(hLabel,"Search");
							ShowWindow(hLabel,SW_SHOW);
							ShowWindow(hwndEdit,SW_SHOW);
							SetFocus(hwndEdit);
							op=SEARCH;
						}
					}
					break;
				case IDM_TIMER_ON:
					CheckMenuItem(hMenu, IDM_TIMER_ON, MF_BYCOMMAND | MF_CHECKED);
					CheckMenuItem(hMenu, IDM_TIMER_OFF, MF_BYCOMMAND | MF_UNCHECKED);
					SetTimer(hwnd, ID_TIMER, 3000, NULL);
					break;
				case IDM_TIMER_OFF:
					CheckMenuItem(hMenu, IDM_TIMER_ON, MF_BYCOMMAND | MF_UNCHECKED);
					CheckMenuItem(hMenu, IDM_TIMER_OFF, MF_BYCOMMAND | MF_CHECKED);				
					KillTimer(hwnd, ID_TIMER);
					break;
				case IDM_APP_HELP:
					
					break;
				case IDM_APP_ABOUT:

					break;
			}
			return 0;    
		case WM_USER_INPUT:
			pszInput = (TCHAR*)lParam;
			ShowWindow(hLabel,SW_HIDE);
			ShowWindow(hwndEdit,SW_HIDE);
			SetWindowText(hwndEdit,"");
			value=stringToInt(pszInput, &isValid);
			if(isValid){
				switch(op){
					case SEARCH:
						pos=1;
						treeNode=SearchTree( tree, value, &pos );
						if(treeNode==NULL){
							MessageBox(NULL, "Not found the data in the tree.","OKOK",MB_OK);
							return 0;
						}
						MarkTreeNode(hMemDC, tree, pos,120, 10, 1164,280);
						break;
					case ADDNODE: 
						expandArray(&nums, &capacity, numsSize);
						nums[numsSize++]=value;
						InsertTreeNode(&tree, value);
						activeNode=NULL;
						arrSize=0;
						InorderTraversalMem(tree, &arr, &arrSize);
						FlushPage(hwnd, hMemDC, tree, nums, numsSize, arr, arrSize);						
						break;
					case MODIFY: 
						if(activeNode){
							//Delete the node
							RemoveNumsData(nums, activeNode->data,&numsSize);
							DeleteTreeNode(&tree,activeNode);
							//Insert the value
							expandArray(&nums, &capacity, numsSize);
							nums[numsSize++]=value;
							InsertTreeNode(&tree, value);
							activeNode=NULL;
							arrSize=0;
							InorderTraversalMem(tree, &arr, &arrSize);
							FlushPage(hwnd, hMemDC, tree, nums, numsSize, arr, arrSize);							
						}
						break;
				}
				InvalidateRect(hwnd, NULL, FALSE);
			}
			return 0;
		case WM_DESTROY:
			if (arr) free(arr);
			if (nums) free(nums);
			if (tree) DestroyTree(&tree);
		    if (hBitmap) DeleteObject(hBitmap);
			if (hMemDC) DeleteDC(hMemDC);
			PostQuitMessage(0);
			return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
LRESULT CALLBACK EditSubClassProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    TCHAR szInput[256];
    switch (message) {
    	case WM_CHAR:
    		switch(wParam){
    			case VK_RETURN:
    				GetWindowText(hwnd, szInput, 256);
    				SendMessage(GetParent(hwnd), WM_USER_INPUT, 0, (LPARAM)szInput); 
    				return 0;
    	}
    }
    return CallWindowProc(OldProc, hwnd, message, wParam, lParam);
}

