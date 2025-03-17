#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "tree.h"
#include "treeres.h"

WNDPROC OldProc;

BOOL CALLBACK AboutDlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK HelpDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
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

			PostMessage(hwnd, WM_KEYDOWN,VK_RETURN,0);
			
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
				case VK_F3:
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
					PostMessage( hwnd, WM_KEYDOWN, VK_RETURN, 0 );
					break;
				case IDM_TREE_OPEN:
					ReadRBTree(&tree, "tree.dat");
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
				case IDM_TREE_DFS:

					break;
				case IDM_TREE_BFS:

					break;
				case IDM_TREE_DIJKSTRA:

					break;
				case IDM_TREE_FLOYD:

					break;
				case IDM_TREE_PRIM:

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
				case IDM_APP_HELP      :
					 DialogBox (hInstance, TEXT ("IDD_DIALOG_HELP"), hwnd, HelpDialogProc) ;
					break;
				case IDM_APP_ABOUT     :
					DialogBox (hInstance, TEXT ("IDD_DIALOG_ABOUT"), hwnd, AboutDlgProc) ;
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
// LRESULT CALLBACK EditSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
// 	if (uMsg == WM_KEYDOWN && wParam == VK_RETURN) {
// 		SendMessage(GetParent(hWnd), WM_COMMAND, MAKEWPARAM(IDOK, BN_CLICKED), 0);
// 		return 0;
// 	}
// 	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
// }

LRESULT CALLBACK EditSubClassProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    TCHAR szInput[256];
    switch (message) {
        // case WM_KEYDOWN:
        //     switch(wParam){
        //     	case VK_RETURN:
		// 			GetWindowText(hwnd, szInput, 256);
		// 			// MessageBox(hwnd, szInput, "Info", MB_OK);
		// 			SendMessage(GetParent(hwnd), WM_USER_INPUT, 0, (LPARAM)szInput); 
        //         	return 0; 	
    	// }
    	case WM_CHAR:
    		switch(wParam){
    			case VK_RETURN:
    				GetWindowText(hwnd, szInput, 256);
    				SendMessage(GetParent(hwnd), WM_USER_INPUT, 0, (LPARAM)szInput); 
    				return 0;
    	}
    	//编辑控件（EDIT）在处理WM_CHAR消息的回车键时发出声音，而处理WM_KEYDOWN消息时并不发出声音。
    }
    return CallWindowProc(OldProc, hwnd, message, wParam, lParam);
}
INT_PTR CALLBACK InputDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam) {
	HWND hEdit;
	TCHAR szInput[256];
	switch (msg) {
		case WM_INITDIALOG:
			hEdit = GetDlgItem(hDlg,IDC_INPUT_EDIT);
			// SetWindowSubclass(hEdit, EditSubclassProc, 0, 0);
			// OldProc = (WNDPROC)SetWindowLong(hEdit, GWL_WNDPROC, (LONG)SubClassProc);
			SetFocus(hEdit);
			return TRUE;
		case WM_COMMAND:
			switch(wParam){
				case IDOK:
					GetDlgItemText(hDlg, IDC_INPUT_EDIT, szInput, 256);
					SendMessage(GetParent(hDlg), WM_USER_INPUT, 0, (LPARAM)szInput);  
					EndDialog(hDlg, IDOK);
					return TRUE;
				case IDCANCEL:
					EndDialog(hDlg, IDCANCEL);
					return TRUE;
			}
			break;
	}
	return FALSE;
}
BOOL CALLBACK AboutDlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
     switch (message)
     {
     case WM_INITDIALOG :
          return TRUE ;
     case WM_COMMAND :
          switch (LOWORD (wParam))
          {
          case IDOK :
          case IDCANCEL :
               EndDialog (hDlg, 0) ;
               return TRUE ;
          }
          break ;
     }
     return FALSE ;
}
BOOL CALLBACK HelpDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HBRUSH hDlgBrush;
    COLORREF dialogBgColor;
    switch (uMsg) {
    case WM_INITDIALOG:
        // 获取对话框的背景颜色
        dialogBgColor = GetSysColor(COLOR_3DFACE);
        // 创建与对话框背景颜色相同的画刷
        hDlgBrush = CreateSolidBrush(dialogBgColor);
        return TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK:
        case IDCANCEL:
            // 销毁画刷
            DeleteObject(hDlgBrush);
            EndDialog(hwndDlg, LOWORD(wParam));
            return TRUE;
        }
        break;
    case WM_CTLCOLORSTATIC: {
        // 获取静态文本控件的句柄
        HWND hCtrl = (HWND)lParam;
        HDC hdcStatic = (HDC)wParam;
        int controlId = GetDlgCtrlID(hCtrl);

        switch (controlId) {
        case 3001:
            // 设置控件 ID 为 3001 的文本颜色为蓝色
            SetTextColor(hdcStatic, RGB(0, 0, 255));
            break;
        case 3002:
            // 设置控件 ID 为 3002 的文本颜色为红色
            SetTextColor(hdcStatic, RGB(128, 0, 255));
            break;
        default:
            // 其他控件设置为默认颜色（这里设置为黑色）
            SetTextColor(hdcStatic, RGB(0, 0, 0));
            break;
        }

        // 设置背景模式为透明
        SetBkMode(hdcStatic, TRANSPARENT);
        // 返回与对话框背景颜色相同的画刷
        return (INT_PTR)hDlgBrush;
    }
    }
    return FALSE;
}