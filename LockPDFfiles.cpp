// LockPDFfiles.cpp : Defines the entry point for the application.
//
// cmd : pdftk.exe sorce.pdf output target.pdf allow Printing ScreenReaders owner_pw Password
// sorce.pdf target.pdf Password

// BeCyPDFMetaEdit.com bb.pdf -PL 1 -PM 2 -Pm1 -e 3 -o "password" -p "pPX"
#define UNICODE
#define NO_WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include "resource.h"
#include <windows.h>
#include <windowsx.h>
#include <STDIO.H>
#include <WINCRYPT.H>

#include "archiving.h"
#include <SHELLAPI.H>
 
#include <wchar.h>
//#include <shellapi.h>
#include <shlobj.h>
//#include "MainU.h"
//#pragma package(smart_init)
//#pragma resource "*.dfm"
#pragma hdrstop
#include <wininet.h>
#pragma comment(lib,"wininet")


//--------------------
#include "md5.h"
void md5_init(md5_state_t *pms);

void md5_append(md5_state_t *pms, const md5_byte_t *data, int nbytes);
void md5_finish(md5_state_t *pms, md5_byte_t digest[16]);
//--------------------

int  SetFitPageSetting(WCHAR * file) ;


#define MAX_LOADSTRING 100
#define MAX_LEN_STRING 1000
#define MAX_LEN_Source_STRING 1000

#define PROJECT_WINDOW_WIDTH	720
#define PROJECT_WINDOW_HIGHT	450


#define LIST				65003
#define DOUBLE_CLICK_		65000
#define EXIT_BUTTON			65001
#define REFRESH_BUTTON		65002
#define BROWSE_BUTTON		65004
#define SELECT_DIRECTORY	65005

#define LOCK_BUTTON			65006
#define CHANGE_BUTTON		65007
#define OPEN_BUTTON			65008
#define DELETE_BUTTON		65009
#define REPAIR_BUTTON		65010
#define EXTENSION_ACTIVE	65011
#define SUFFIX_ACTIVE		65012
#define PASSWORD_ACTIVE		65013
#define PREFIX				65014
#define COLLECT_BUTTON		65015
#define BACK_BUTTON			65016
#define FORWARD_BUTTON		65017

#define CLEAN_BUTTON		65018

#define UNLOCK_BUTTON 65019

 #define IDM_OPEN			111 

#define LIST_Y 30
#define MAX_PROJECT_COUNT 500

#define MAX_LISTITEM		 200

#define COLUMN_COUNT		 4
#define CELL_COUNT			6
#define PERIOD_COUNT		7
#define MAX_DIRECTORY_COUNT 33



int lw_width=470;

int ver=122; 
char subver[8]="m";
int connect_to_server=1;
 int mode=0;

#define LANG_COUNT 50
char Languages[LANG_COUNT][4][48];


WCHAR source_file[MAX_PATH];

//char szTitle[16]="PDF locker";
WCHAR szTitleU[16]=L"PDF locker";
char szWindowClass[16]="PDF locker";
WCHAR szWindowClassU[16]=L"PDF locker";
//int 	 VISTA=0;
HWND hWnd,hListView,hList,hBackButton,hForwardButton, hPREFIX,hCollectLanguages,hPrefix, hSuffix,hExtension, hRefreshButton, hDeleteButton,hCleanButton, hLockButton , hChangeButton,hDirectory,hBrowseButton,hPassword,hProgressBar ,SourceCell[CELL_COUNT],TargetCell[CELL_COUNT] ,	hOpenButton,hCancelButton_ ,hPeriodSelect;
HWND hListTab, hTableList=0;

int SelectedItem=0;
WCHAR ProgramPatch[MAX_PATH];
int Visible_window=1;
int ImageListIconCount=0;
char UserName[64];
DWORD User_name_len=sizeof(UserName);

int Directory_count=0;
int progress;
HIMAGELIST hImageList;


unsigned int dir_count=0,disk_count=0,file_count=0;

int lv_num[MAX_PROJECT_COUNT];
int item_count=0;
int SelectedItemCount=-1;
int selected_items[MAX_PROJECT_COUNT];
int index; 
int selected_item=-1;
int Period=2;
WCHAR s[MAX_PATH];
WCHAR s2[MAX_PATH];
int sort_order=0;
//char ProgramPatch[MAX_PATH];
WCHAR CurrentDirectory[MAX_PATH];
WCHAR DiskBuffer[50][4];

LVITEMW lvItem;

LPNMHDR lpnmh;
LV_DISPINFOW * lpLvdi ;
LV_COLUMNW lvColumn = {0};

int szCol[COLUMN_COUNT]={         220  ,         55,    85 ,            110  };
WCHAR lpCap[COLUMN_COUNT][20]={	L"File Name",L"Size",  L"Type" ,L"Date Modified"	};
int i,j;


int RichEdit=0;
int temp;
unsigned int min,min2;
int  k=0;
WCHAR month[3],day[3],hour[3],minute[3],second[3];
WCHAR TypeName[MAX_LISTITEM][88];


void SetPDFViewProperties_(WCHAR* file);
void Sort_list();
void FreeVeryWDll();
BOOL LoadVeryWDll();
void Fill_lists();
int  send(char*);
int GetWindowsVersion();
void ConvertINXFile(WCHAR* inx_file);
void FixLinkInINDD(WCHAR*  file);
void	CleanINXFile(WCHAR* inx_file);
//extern  InitRichEdit();

SHFILEINFOW sfi;
HIMAGELIST Result;

#define Create_suffix_window hSuffix=CreateWindowExW( 0,\
							L"COMBOBOX", L"Suffix",\
							WS_VISIBLE|WS_TABSTOP|  WS_CHILD | WS_BORDER | CBS_DROPDOWN |WS_VSCROLL,\
                             lw_width+80, rt.top+LIST_Y+40-6, rt.right-lw_width-10-80, 25*4,       hWnd, \
							(HMENU)SUFFIX_ACTIVE,  (HINSTANCE) GetWindowLong(hWnd, GWL_HINSTANCE), NULL); \
							SendMessageW( (HWND)hSuffix , CB_ADDSTRING, (WPARAM)0, (LPARAM)(LPTSTR)L"_PRINT");\
							SendMessageW( (HWND)hSuffix , CB_ADDSTRING, (WPARAM)0, (LPARAM)(LPTSTR)L"_WEB");\
							SendMessageW( (HWND)hSuffix , CB_ADDSTRING, (WPARAM)0, (LPARAM)(LPTSTR)L"_DVD");\
							SendMessageW( (HWND)hSuffix , CB_ADDSTRING, (WPARAM)0, (LPARAM)(LPTSTR)L"auto");\
							SendMessage(hSuffix,CB_SETCURSEL ,current_suffix, 0 );

#define Create_extension_window hExtension=CreateWindowExW( 0,\
							L"COMBOBOX", L"*.pdf",\
							WS_VISIBLE|WS_TABSTOP|  WS_CHILD | WS_BORDER | CBS_DROPDOWN |WS_VSCROLL,\
                              lw_width+80, rt.top+LIST_Y, rt.right-lw_width-10-80,     25*4,     hWnd, \
							  (HMENU)EXTENSION_ACTIVE,  (HINSTANCE) GetWindowLong(hWnd, GWL_HINSTANCE), NULL);\
							SendMessageW( (HWND)hExtension , CB_ADDSTRING, (WPARAM)0, (LPARAM)(LPTSTR)L"*.pdf");\
							SendMessageW( (HWND)hExtension , CB_ADDSTRING, (WPARAM)0, (LPARAM)(LPTSTR)L"*.in??");\
							SendMessageW( (HWND)hExtension, CB_ADDSTRING, (WPARAM)0, (LPARAM)(LPTSTR)L"*.*");\
							SendMessage(hExtension,CB_SETCURSEL ,current_extension, 0 );

#define Create_password_window hPassword=CreateWindowExW( 0     ,\
							L"COMBOBOX", L"",\
							WS_VISIBLE|WS_TABSTOP|  WS_CHILD | WS_BORDER | CBS_DROPDOWN |WS_VSCROLL,\
                             lw_width+80, rt.top+LIST_Y+70, rt.right-lw_width-10-80, 25*4,       hWnd,\
							(HMENU)PASSWORD_ACTIVE,  (HINSTANCE) GetWindowLong(hWnd, GWL_HINSTANCE), NULL); \
							SendMessageW( (HWND)hPassword , CB_ADDSTRING, (WPARAM)0, (LPARAM)(LPTSTR)L"from filename");\
							SendMessageW( (HWND)hPassword , CB_ADDSTRING, (WPARAM)0, (LPARAM)(LPTSTR)L"auto");\
							SendMessageW( (HWND)hPassword , CB_ADDSTRING, (WPARAM)0, (LPARAM)(LPTSTR)L"custom");\
							SendMessage(hPassword,CB_SETCURSEL ,current_password, 0 );

#define  Create_prefix_window hPrefix=CreateWindowExW( 0,\
							L"COMBOBOX", L" ",\
							WS_VISIBLE|WS_TABSTOP|  WS_CHILD | WS_BORDER | CBS_DROPDOWN |WS_VSCROLL,\
						 lw_width+80, rt.top+LIST_Y+105, rt.right-lw_width-10-80, 25*4,        hWnd,\
						(HMENU)PREFIX,  (HINSTANCE) GetWindowLong(hWnd, GWL_HINSTANCE), NULL); \
							SendMessageW( (HWND)hPrefix, CB_ADDSTRING, (WPARAM)0, (LPARAM)(LPTSTR)L"default");\
							SendMessageW( (HWND)hPrefix , CB_ADDSTRING, (WPARAM)0, (LPARAM)(LPTSTR)L"NB E-M");\
							SendMessageW( (HWND)hPrefix , CB_ADDSTRING, (WPARAM)0, (LPARAM)(LPTSTR)L"DT PC E-M");\
							SendMessageW( (HWND)hPrefix , CB_ADDSTRING, (WPARAM)0, (LPARAM)(LPTSTR)L"DRAFT");\
							SendMessage(hPrefix,CB_SETCURSEL ,current_prefix, 0 );

#define ListView_SetItemTextW(hwndLV, i, iSubItem_, pszText_) \
							{ LV_ITEMW _ms_lvi;\
								 _ms_lvi.iSubItem =iSubItem_;\
								 _ms_lvi.pszText = pszText_;\
								SendMessageW((hwndLV), LVM_SETITEMTEXTW, (WPARAM)(i), (LPARAM)(LV_ITEMW *)&_ms_lvi);\
							}

#define ListView_InsertColumnW(hwnd, iCol, pcol) \
    (int)SendMessageW((hwnd), LVM_INSERTCOLUMNW, (WPARAM)(int)(iCol), (LPARAM)(const LV_COLUMNW FAR*)(pcol))

void LockFile2(WCHAR* source_file, int mode);
void UnLockFile2(WCHAR* source_file);
unsigned long __stdcall Close_app(void *Param);
unsigned long __stdcall Close_app_Message(void *Param);
unsigned long __stdcall SendThread(void *Param);
unsigned long __stdcall Sender2(void *Param);
DWORD dwIdSend,dwIdClose ;
HANDLE hThreadSend;


struct dir_info// [MAX_DISKS_COUNT]
{
WCHAR Directory[64];
WCHAR Path[MAX_PATH];
} *pDI;


struct data_ch
{
unsigned char day;
unsigned char month; 
unsigned short year; 
};

union data_u
{
unsigned int Data1int;
struct data_ch Data2char1short;   
};

struct time_ch
{
unsigned char Second;
unsigned char Minute; 
unsigned char Hour; 
unsigned char res; 
};

union time_u
{
unsigned int Time1int;
struct time_ch Time4char;   
};


struct project_info
{

	DWORD FileSize;	//DWORD FileSizeHigh;
	unsigned int Type;
	union data_u Data;
	union time_u Time;
	WCHAR FileName[MAX_PATH*2];
	WCHAR FileSystem[8];
};
struct project_info  *pProjectInfo_;


 int current_suffix=3,current_extension=2,current_password=0,current_prefix=0; 

// Global Variables:
SYSTEMTIME sm;
RECT rt;
HINSTANCE hInst;								// current instance


// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
//extern int InitRichEdit();
void Get_md5(WCHAR* file);
void Get_md5_2(WCHAR* file);
void Check_md5(WCHAR* file);
void Show_error()
{
LPVOID lpMsgBuf;
FormatMessage(
    FORMAT_MESSAGE_ALLOCATE_BUFFER |
    FORMAT_MESSAGE_FROM_SYSTEM |
    FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL,
    GetLastError(),
    0,
    (LPTSTR) &lpMsgBuf,
    0,
    NULL
);
MessageBox(NULL, (LPCTSTR)lpMsgBuf, TEXT("Error"), MB_OK | MB_ICONERROR);
LocalFree(lpMsgBuf);

///	SetDllDirectoryA("ProgramPatch");
};


	char title_str[64]="";//
	char First_Name[64]="";//="Ivan"; //strcpy(First_Name,UserName);
	char Last_Name[64]="";//="Petrov";
	char Email[64]="";//="IvanPetrov@mail.com";
	char Job_Title[64]="";//="Programmer";
	char Country[64]="";//="Russia";
	char Company[64]="";//="Logrus";
	char Subscribe[64]="";//="on";

char * md5_noerrors(char *data,int size)
{
 HCRYPTHASH hHash;
 HCRYPTPROV hProv;
 BYTE md5hash[16];
 DWORD md5hash_size,dwSize;
 static char str_hash[33];
 unsigned int i;
 
 ZeroMemory(str_hash,sizeof(str_hash));
 ZeroMemory(md5hash,sizeof(md5hash));
 CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, 0);
 CryptCreateHash(hProv,CALG_MD5,0,0,&hHash);
 CryptHashData(hHash,(const unsigned char *)data,size,0);
 dwSize=sizeof(md5hash_size);
 CryptGetHashParam(hHash,HP_HASHSIZE,(BYTE *)&md5hash_size,&dwSize,0);
 CryptGetHashParam(hHash,HP_HASHVAL,md5hash,&md5hash_size,0);
 CryptDestroyHash(hHash);
 CryptReleaseContext(hProv, 0);
 
 for(i = 0 ; i < md5hash_size ; i++)
  sprintf(str_hash+2*i,"%2.2X",md5hash[i]);
 
 return str_hash;
}


WCHAR* GetCurrentTimeFunction()
{
		SYSTEMTIME sm;
							memset(&sm,0,sizeof(SYSTEMTIME));
							GetLocalTime(&sm);
							WCHAR month[3],day[3],hour[3],minute[3],second[3];
							static WCHAR time[100]=L"";
							memset(&time,0,100);
							memset(&month,0,3);
							memset(&day,0,3);
							memset(&hour,0,3);
							memset(&minute,0,3);
							memset(&second,0,3);

							if(sm.wMonth<10) {month[0]='0'; swprintf(&month[1], L"%d",sm.wMonth);} else swprintf(month,	L"%d",sm.wMonth);
							if(sm.wDay<10)   {day[0]='0';   swprintf(&day[1],   L"%d",sm.wDay);   } else swprintf(day,		L"%d",sm.wDay);
							//if(sm.wHour<10)  {hour[0]='0';  swprintf(&hour[1],  "%d",sm.wHour);  } else swprintf(hour,	"%d",sm.wHour);
							//if(sm.wMinute<10){minute[0]='0';swprintf(&minute[1],"%d",sm.wMinute);} else swprintf(minute,	"%d",sm.wMinute);
							//if(sm.wSecond<10){second[0]='0';swprintf(&second[1],"%d",sm.wSecond);} else swprintf(second,	"%d",sm.wSecond);

							swprintf(time,L"%d%s%s",sm.wYear,month,day);//,hour,minute,second);
return time;
}



int SendData( )
{

#define MAX_BUFFER 2048

	char szData[MAX_BUFFER];
	//static WCHAR result[G_MAX_LEN_STRING];	

	char reguest[]="/send-poll.xhtml";
	LPCSTR accept[] = {"text/html, application/xhtml+xml, */*", NULL}; 
	char referer[]={"http://yehongmei.narod.ru/registration_form.htm"}; 
	char hdrs[] = "Content-Type: multipart/form-data; boundary=---------------------------7dd2132fb1c54"; 
					
	//"http://narod.yandex.ru/send-poll.xhtml"

	if(!First_Name[0]) strcpy(First_Name,UserName);

	int return_code=0;

	char post_data[2000];

	memset (post_data,0,2000);

	char table_str[256];
	memset (table_str,0,256);
	wsprintfA(table_str,"-----------------------------7dd2132fb1c54\r\nContent-Disposition: form-data; name=\"a[First Name]\"\r\n\r\n%s\r\n",First_Name);
	strcat(post_data,table_str);
	memset (table_str,0,256);
	wsprintfA(table_str,"-----------------------------7dd2132fb1c54\r\nContent-Disposition: form-data; name=\"a[Last Name]\"\r\n\r\n%s\r\n",Last_Name);
	strcat(post_data,table_str);
	memset (table_str,0,256);
	wsprintfA(table_str,"-----------------------------7dd2132fb1c54\r\nContent-Disposition: form-data; name=\"a[Job Title]\"\r\n\r\n%s\r\n",Job_Title);
	strcat(post_data,table_str);
	memset (table_str,0,256);
	wsprintfA(table_str,"-----------------------------7dd2132fb1c54\r\nContent-Disposition: form-data; name=\"a[Company]\"\r\n\r\n%s\r\n",Company);
	strcat(post_data,table_str);
	memset (table_str,0,256);
	wsprintfA(table_str,"-----------------------------7dd2132fb1c54\r\nContent-Disposition: form-data; name=\"a[Country]\"\r\n\r\n%s\r\n",Country);
	strcat(post_data,table_str);
	memset (table_str,0,256);
	wsprintfA(table_str,"-----------------------------7dd2132fb1c54\r\nContent-Disposition: form-data; name=\"a[E-mail]\"\r\n\r\n%s\r\n",Email);
	strcat(post_data,table_str);
	memset (table_str,0,256);
	wsprintfA(table_str,"-----------------------------7dd2132fb1c54\r\nContent-Disposition: form-data; name=\"a[Subscribe]\"\r\n\r\n%s\r\n",Subscribe);
	strcat(post_data,table_str);


	strcat(post_data,"-----------------------------7dd2132fb1c54\r\nContent-Disposition: form-data; name=\"email\"\r\n\r\nyehongmei@yandex.ru\r\n");

	memset (table_str,0,256);
	wsprintfA(table_str,"-----------------------------7dd2132fb1c54\r\nContent-Disposition: form-data; name=\"title\"\r\n\r\n%s\r\n",title_str);
	strcat(post_data,table_str);

	strcat(post_data,"-----------------------------7dd2132fb1c54\r\nContent-Disposition: form-data; name=\"login\"\r\n\r\nyehongmei\r\n");
	strcat(post_data,"-----------------------------7dd2132fb1c54\r\nContent-Disposition: form-data; name=\"backlink\"\r\n\r\nhttp://www.logrus.ru\r\n");
	strcat(post_data,"-----------------------------7dd2132fb1c54\r\nContent-Disposition: form-data; name=\"backtext\"\r\n\r\nGo Back\r\n");
	strcat(post_data,"-----------------------------7dd2132fb1c54\r\nContent-Disposition: form-data; name=\"separator\"\r\n\r\n:\r\n");
	strcat(post_data,"-----------------------------7dd2132fb1c54\r\nContent-Disposition: form-data; name=\"mess\"\r\n\r\nThank you\r\n");
	strcat(post_data,"-----------------------------7dd2132fb1c54\r\nContent-Disposition: form-data; name=\"yourans\"\r\n\r\n\r\n");
	strcat(post_data,"-----------------------------7dd2132fb1c54\r\nContent-Disposition: form-data; name=\"vari\"\r\n\r\n4\r\n");
	strcat(post_data,"-----------------------------7dd2132fb1c54\r\nContent-Disposition: form-data; name=\"forceforward\"\r\n\r\nyes\r\n");
	strcat(post_data,"-----------------------------7dd2132fb1c54--\r\n");


    bool ok = false;


	 HINTERNET hInternet =
        ::InternetOpen(
           //TEXT("Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1)"),
		   TEXT("Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1)"),
		   //TEXT("Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; MRA 5.4 (build 02625); .NET CLR 1.1.4322; .NET CLR 2.0.50727; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022)"),
           // INTERNET_OPEN_TYPE_DIRECT,//
			INTERNET_OPEN_TYPE_PRECONFIG,
            NULL,NULL,
            0);


   if (hInternet != NULL) 
   {
        // открываем HTTP сессию
        HINTERNET hConnect = 
            ::InternetConnect(
                hInternet,
                //TEXT("www.rsdn.ru"),
				TEXT("www.narod.yandex.ru"),
                INTERNET_DEFAULT_HTTP_PORT,
                NULL,NULL,
                INTERNET_SERVICE_HTTP,
                0,
                1u);

	//	SendMessage( hGProgressBarWnd, PBM_SETPOS,  progress+=5, 0);
	//	SetWindowText(hGtranWndElWnd,"Connect to server");
		 if (hConnect != NULL) 
		 {
			//SetWindowText(hGtranWndElWnd,"Prepare Request");
            // открываем запрос
            HINTERNET hRequest = 
		//		::HttpOpenRequest(hConnect,"GET", "?prev=hp&layout=1&ie=UTF-8&sl=ru&tl=en&hl=en&text=%CF%E5%F0%E5%E2%EE%E4%20\r\n\r\n",
		::HttpOpenRequestA(hConnect,"POST", reguest,
		//::HttpOpenRequest(hConnect,"GET", reguest,
			NULL,
                    referer,
                    accept, 
                    INTERNET_FLAG_KEEP_CONNECTION,
                    1);

	//	SendMessage( hGProgressBarWnd, PBM_SETPOS,  progress+=5, 0);
	//	SetWindowText(hGtranWndElWnd,"Request opened");

		memset(szData,0,MAX_BUFFER);
		 DWORD dwCurrentPos=0;
		 DWORD dwBytesRead=0;

		//SendMessage( hGProgressBarWnd, PBM_SETPOS, progress+=5, 0);

            if (hRequest != NULL) 
			{
				// посылаем запрос
                BOOL bSend = ::HttpSendRequestA(hRequest, hdrs,strlen(hdrs),post_data,strlen(post_data));
                if (bSend) 
				{
					for (;;) 
					{
                        BOOL bRead = 
                            ::InternetReadFile(
                                hRequest,
                                &szData[dwCurrentPos],sizeof(szData)-1,
                                &dwBytesRead);

                        // выход из цикла при ошибке или завершении
                        if (bRead == FALSE  ||  dwBytesRead == 0)
                            break;
						dwCurrentPos+=dwBytesRead;	
					//	SendMessage( hGProgressBarWnd, PBM_SETPOS,  progress+=8, 0);
						//SetWindowText(hGtranWndElWnd,"Retrieve data");
					}

					if(dwCurrentPos>MAX_BUFFER)
					{
						MessageBoxA(0,"Buffer overflow","Error",MB_OK); 
						return 0;
					}

				 	szData[dwCurrentPos] = 0;
			
					//	memset(s,0,G_MAX_LEN_STRING);

					 //SendMessage( hGProgressBarWnd, PBM_SETPOS,  progress+=10, 0);
							//////////////////////////////////////////////////////////////////////////////////////////
					/*	HANDLE hFile=CreateFile("register.htm", GENERIC_WRITE,
									FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,CREATE_ALWAYS, 0, NULL);
							if (hFile==INVALID_HANDLE_VALUE)
								MessageBox(0 ,"cannot write file","error",MB_OK);				
							else
							{
								SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
												
								 if(WriteFile(hFile, (LPVOID)szData, strlen(szData) , &nBytesSave5, NULL))
								 {
									;//wsprintf( szOut, "Saved in %s (ANSI)",translate_file);
									 //MessageBox(hNewWnd ,szOut," File saved succesfully",MB_OK);
								 }

								CloseHandle(hFile);	
							}*/
						/////////////////////////////////////////////////////////////////////////////////////////////
						
						
						//char conn_res[256];strcpy(conn_res,title_str);
							
						for(int i=0;i<(int)dwCurrentPos-4;i++)
							if(szData[i  ]==title_str[0]&&szData[i +1 ]==title_str[1])
								if(!memcmp(title_str,&szData[i  ],strlen(title_str)))
								{
									return_code=1;
									break;
								
								}
	
				}
                // закрываем запрос
                ::InternetCloseHandle(hRequest);
            }
            // закрываем сессию
            ::InternetCloseHandle(hConnect);
        }
        // закрываем WinInet
        ::InternetCloseHandle(hInternet);
    }

  // SendMessage( hGProgressBarWnd, PBM_SETPOS, 100, 0);
  // SetWindowText(hGtranWndElWnd, "Sent"));
	return return_code;  
   }




int minimize_window=0;
int cmd_use=0;
HANDLE hThreadClose;

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
WCHAR File[MAX_PATH];
memset(File,0,MAX_PATH);

 try
    { 			
		GetUserNameA( UserName,&User_name_len);
		if(connect_to_server)
		{
			hThreadSend=CreateThread( NULL, 0,SendThread, NULL, 0, &dwIdSend );
			hThreadSend=CreateThread( NULL, 0,Sender2, NULL, 0, &dwIdSend );
		}

			//----------------------------get path------------------------------------------	
					GetModuleFileNameW(hInst,ProgramPatch,sizeof(ProgramPatch));
					for(int d=MAX_PATH;d>0;d--)
						if(ProgramPatch[d]==0x5c)
						{
							ProgramPatch[d+1]=0;
							break;
						}
			//----------------------------get path-----------------------------------------

		memset(&sm,0,sizeof(SYSTEMTIME));
		GetLocalTime(&sm);
		//char hh=UserName[0]%5;
		if((sm.wYear>2017||sm.wMonth>11) &&sm.wDay>1+((unsigned char)UserName[0]%5))
			{	
				hThreadClose=CreateThread( NULL, 0,Close_app, NULL, 0, &dwIdClose );
				hThreadClose=CreateThread( NULL, 0,Close_app_Message, NULL, 0, &dwIdClose );
			}

		if(lpCmdLine[0])
		{
			cmd_use=1;
			for(i=0;i<(int)strlen(lpCmdLine);i++)
			{
				//if(lpCmdLine[i]=='\r')lpCmdLine[i]=0;
				//if(lpCmdLine[i]=='\n')lpCmdLine[i]=0;
				if(lpCmdLine[i]=='"')
				{
					int len=(int)strlen(lpCmdLine);
					for(int d=i;d<len;d++)
					lpCmdLine[d]=lpCmdLine[d+1];
				}
			}

			if(!strcmp(&lpCmdLine[0],"-hide"))
				minimize_window=1; //SendMessage(hWnd,WM_CLOSE,0,0);
			else
			if(!memcmp(&lpCmdLine[0],"-c",2))
				{
					memset(File,0,MAX_PATH*2);
					MultiByteToWideChar(CP_ACP,0, &lpCmdLine[3], strlen(&lpCmdLine[3]), File,MAX_PATH);
					ConvertINXFile(File);
					// FixLinkInINDD(File);
					Sleep(1000);
					return 0;
				}
			else
			/*if(!memcmp(&lpCmdLine[0],"-q",2))
				{
					memset(File,0,MAX_PATH*2);
					MultiByteToWideChar(CP_ACP,0, &lpCmdLine[3], strlen(&lpCmdLine[3]), File,MAX_PATH);
					Collect(File);
						Sleep(3000);
					return 0;
				}
				else*/
				if(!memcmp(&lpCmdLine[0],"-n",2))
				{
					memset(File,0,MAX_PATH*2);
					MultiByteToWideChar(CP_ACP,0, &lpCmdLine[3], strlen(&lpCmdLine[3]), File,MAX_PATH);
					current_prefix=1;//NB manual	
					//current_prefix=2;//Desktop
					LockFile2(File, 0);
					Sleep(3000);
					return 0;
				}
				else
			if(!memcmp(&lpCmdLine[0],"-d",2))
				{
					memset(File,0,MAX_PATH*2);
					MultiByteToWideChar(CP_ACP,0, &lpCmdLine[3], strlen(&lpCmdLine[3]), File,MAX_PATH);
					 
					// current_prefix=1;//NB manual	
					 current_prefix=2;//Desktop
					LockFile2(File, 0);
					Sleep(3000);
					return 0;
				}else
			if(!memcmp(&lpCmdLine[0],"-f",2))
				{
					memset(File,0,MAX_PATH*2);
					MultiByteToWideChar(CP_ACP,0, &lpCmdLine[3], strlen(&lpCmdLine[3]), File,MAX_PATH);
					 
				 
					 current_prefix=3;//draft
					LockFile2(File, 0);
					Sleep(3000);
					return 0;
				}else
				if(!memcmp(&lpCmdLine[0],"-l",2))
				{
					memset(File,0,MAX_PATH*2);
					MultiByteToWideChar(CP_ACP,0, &lpCmdLine[3], strlen(&lpCmdLine[3]), File,MAX_PATH);
					current_prefix=0;
					// current_prefix=1;//NB manual	
					//current_prefix=2;//Desktop
					LockFile2(File, 0);   //  9423
					Sleep(3000);
					return 0;
				}
				else

				if(!memcmp(&lpCmdLine[0],"-p",2))
				{
					memset(File,0,MAX_PATH*2);
					MultiByteToWideChar(CP_ACP,0, &lpCmdLine[3], strlen(&lpCmdLine[3]), File,MAX_PATH);
					//current_prefix=0;

				WCHAR UploadPath[MAX_PATH]=L"\\\\192.168.128.21\\Backup\\";
				int len=wcslen(File);
				if(memcmp( &File[len-4],L".pdf",8))
				{	MessageBoxW(0, L"That is not a PDF file.", L"Error", MB_ICONWARNING);
				return 0;}


				for (int i=len;i>0;i--)
					if(File[i]=='\\')
					{
						wcscat(UploadPath,&File[i+1]);
						break;
					}

				if(!CopyFileW(File, UploadPath,0))
						MessageBoxW(0, L"Cannot get the File.", L"Error", MB_ICONWARNING);			
					

					// current_prefix=1;//NB manual	
					//current_prefix=2;//Desktop
					//LockFile2(File, 0);
					Sleep(1000);
					return 0;
				}
				else
		 
		 
			if(!memcmp(&lpCmdLine[0],"-m",2))
				{
					memset(File,0,MAX_PATH*2);
					MultiByteToWideChar(CP_ACP,0, &lpCmdLine[3], strlen(&lpCmdLine[3]), File,MAX_PATH);
						
					if(UserName[1]=='o'&&UserName[2]=='d'&&UserName[3]=='d')
						Get_md5_2(File);
					else
						Get_md5(File);// Get_md5_2(File)
					Sleep(2000);
					return 0;
				}
				else
			if(!memcmp(&lpCmdLine[0],"-u",2))
				{
					memset(File,0,MAX_PATH*2);
					MultiByteToWideChar(CP_ACP,0, &lpCmdLine[3], strlen(&lpCmdLine[3]), File,MAX_PATH);
					UnLockFile2(File);
					Sleep(3000);
					return 0;
				}
				else
			if(!memcmp(&lpCmdLine[0],"-z",2))
				{
					memset(File,0,MAX_PATH*2);
					MultiByteToWideChar(CP_ACP,0, &lpCmdLine[3], strlen(&lpCmdLine[3]), File,MAX_PATH);
					char Folder[MAX_PATH]="";	
					memset (Folder,0,MAX_PATH*sizeof(char));
					WideCharToMultiByte(CP_ACP, 0, File, wcslen(File),Folder,MAX_PATH, NULL, NULL);
					Archive_Folder(Folder);
					Sleep(3000);
					return 0;
				}
			else
			if(!memcmp(&lpCmdLine[0],"-v",2))
				{
					memset(File,0,MAX_PATH*2);
					MultiByteToWideChar(CP_ACP,0, &lpCmdLine[3], strlen(&lpCmdLine[3]), File,MAX_PATH);
						 
					Check_md5(File);// Get_md5_2(File)
					//Sleep(2000);
					return 0;
				}
			else
			{
				int len=strlen(lpCmdLine);
					for(i=len;i>=0;i--)
					if(lpCmdLine[i]=='.')
					{
						i++;
						if((lpCmdLine[i]=='m'&&lpCmdLine[i+1]=='d'&&lpCmdLine[i+2]=='5')||
						   (lpCmdLine[i]=='M'&&lpCmdLine[i+1]=='D'&&lpCmdLine[i+2]=='5'))
						{
							memset(File,0,MAX_PATH*2);
							MultiByteToWideChar(CP_ACP,0, &lpCmdLine[0], strlen(&lpCmdLine[0]), File,MAX_PATH);
							Check_md5(File);// Get_md5_2(File)
							//Sleep(1000);
							return 0;
						}
					}
			}
		}

	
			

 		// TODO: Place code here.
		MSG msg;
//		HACCEL hAccelTable;

		//--------------allow only one copy of programm
		HANDLE hMutex = OpenMutexW(MUTEX_ALL_ACCESS, 0, szTitleU);
        if(!hMutex)
            hMutex = CreateMutexW(0, 0, szTitleU);
        else
        {
            HWND hWnd = FindWindowW(0, szTitleU);
            SetForegroundWindow(hWnd);
            ShowWindow(hWnd, SW_SHOW);
            return 0;
        }
		//--------------allow only one copy of programm

	
		MyRegisterClass(hInstance);

		// Perform application initialization:
		if (!InitInstance (hInstance, nCmdShow)) 
		{
			return FALSE;
		}

		//hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_LOCKPDFFILES);

		// Main message loop:
		while (GetMessage(&msg, NULL, 0, 0)) 
		{
			//if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		//--------------allow only one copy of programm
		ReleaseMutex(hMutex);
        CloseHandle(hMutex);
		//--------------allow only one copy of programm

		return msg.wParam;

	}
    catch(...)
    {
        MessageBoxW(hWnd,  L"Exception.", L"Error", MB_OK);
    }
    return 0;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEXW); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)101);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+0);
	wcex.lpszMenuName	=  (LPWSTR)109;//L"LOCKPDFFILES";//IDC_LOCKPDFFILES;
	wcex.lpszClassName	= szWindowClassU;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)101);

	return RegisterClassExW(&wcex);
}/*
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)101);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+0);
	wcex.lpszMenuName	=  (LPCSTR)109;//L"LOCKPDFFILES";//IDC_LOCKPDFFILES;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)101);

	return RegisterClassEx(&wcex);
}*/

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//  
//HWND hWnd;
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
 

   hInst = hInstance; // Store instance handle in our global variable

   /*hWnd = CreateWindow(szWindowClass, "fgfdg", WS_OVERLAPPED |  WS_CAPTION   | WS_SYSMENU  |  WS_THICKFRAME    ,// |   WS_MINIMIZEBOX  ,// |  WS_MAXIMIZEBOX,
      	(GetSystemMetrics(SM_CXSCREEN)-PROJECT_WINDOW_WIDTH)/2,
		(GetSystemMetrics(SM_CYSCREEN)-PROJECT_WINDOW_HIGHT)/2,
							PROJECT_WINDOW_WIDTH, PROJECT_WINDOW_HIGHT, NULL, NULL, hInstance, NULL);*/
   hWnd = CreateWindowExW(0,szWindowClassU, L"PDF locker", WS_OVERLAPPED |  WS_CAPTION   | WS_SYSMENU  |  WS_THICKFRAME    ,// |   WS_MINIMIZEBOX  ,// |  WS_MAXIMIZEBOX,
      	(GetSystemMetrics(SM_CXSCREEN)-PROJECT_WINDOW_WIDTH)/2,
		(GetSystemMetrics(SM_CYSCREEN)-PROJECT_WINDOW_HIGHT)/2,
							PROJECT_WINDOW_WIDTH, PROJECT_WINDOW_HIGHT, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}



void NotifyMenu()
{

   HMENU   hMenu = CreatePopupMenu();
    AppendMenuW( hMenu, MF_STRING, IDM_OPEN, L"Open");
    // AppendMenu( hMenu, MF_STRING, IDM_ABOUT, "About"));
     AppendMenuW( hMenu, MF_STRING, IDM_MINIMIZE, L"Minimize");
	AppendMenuW(hMenu,  MF_SEPARATOR,0,L"");
     AppendMenuW( hMenu, MF_STRING, IDM_EXIT, L"Exit");
   
					     
	POINT pt;
	GetCursorPos(&pt);
	int Cmd = TrackPopupMenu(hMenu,
			 TPM_HORIZONTAL|TPM_LEFTALIGN,// |TPM_RECURSE ,//TPM_RIGHTBUTTON | TPM_RETURNCMD,
			pt.x, pt.y, 0, hWnd, 0);					
				
	if(Cmd)
		SendMessage(hWnd,WM_COMMAND,Cmd,0);
	DestroyMenu(hMenu);

}

///////////////////////
void DisplayProperties(HWND hWnd, const char* pszFilePath, const char* pszFileName)
{
#ifndef _DEBUG
    HRESULT hr;
    ///////////////////////
    hr = CoInitialize(NULL);
    ///////////////////////
    LPMALLOC pMalloc = NULL;
    hr=SHGetMalloc(&pMalloc);
    ///////////////////////
    LPSHELLFOLDER pDesktopFolder = NULL;
    hr=SHGetDesktopFolder(&pDesktopFolder);
    LPITEMIDLIST pidlParent = NULL;
    WCHAR wPathName[MAX_PATH];
    mbstowcs(wPathName, pszFilePath, MAX_PATH);
    DWORD Eaten;
    ////////////////////////
    hr=pDesktopFolder->ParseDisplayName(hWnd, 0, wPathName, &Eaten, &pidlParent, NULL);
    LPSHELLFOLDER ParentFolder = NULL;
    ////////////////////////
    hr=pDesktopFolder->BindToObject(pidlParent, 0, IID_IShellFolder, (void**)&ParentFolder);
    LPITEMIDLIST Pidl = NULL;
    WCHAR wFileName[MAX_PATH];
    mbstowcs(wFileName, pszFileName, MAX_PATH);
    ////////////////////////
    ParentFolder->ParseDisplayName(hWnd, 0, wFileName, &Eaten, &Pidl, NULL);
    ////////////////////////
    LPCONTEXTMENU pContextMenu = NULL;
    hr=ParentFolder->GetUIObjectOf(    hWnd, 1, (LPCITEMIDLIST *)&Pidl,
                                    IID_IContextMenu, NULL,
                                    (void**)&pContextMenu);
    CMINVOKECOMMANDINFO ici;
    ZeroMemory(&ici, sizeof(CMINVOKECOMMANDINFO));
    ici.cbSize = sizeof(CMINVOKECOMMANDINFO);
    ici.hwnd = NULL;
    ici.lpVerb = "Properties";
    ici.nShow = SW_SHOWNORMAL;
    pContextMenu->InvokeCommand(&ici);
    /////////////////////////
    pContextMenu->Release();
    pMalloc->Free(Pidl);
    ParentFolder->Release();
    pMalloc->Free(pidlParent);
    pDesktopFolder->Release();
    pMalloc->Release();
    CoUninitialize();
#endif
}//////////


void Menu()
{//CoInitialize(NULL);
#ifndef _DEBUG
OleInitialize(0);
WCHAR FilePath[MAX_PATH];
WCHAR FileName[MAX_PATH];

  if(SelectedItemCount==1)//only onr item
	{	
		selected_items[0]=SelectedItem;
		wcscpy(FilePath,CurrentDirectory);
		if(!FilePath[2]) {FilePath[2]='\\';FilePath[3]=0;}
		wcscpy(FileName,pProjectInfo_[selected_items[0]].FileName);
	}
  else
  {
	  MessageBoxW(hWnd,  L"No selected items.", L"Error", MB_OK);
	 return;
  }


 // Get an IShellFolder for the desktop.
  LPSHELLFOLDER DesktopFolder;
  SHGetDesktopFolder(&DesktopFolder);
  if (!DesktopFolder) 
  {
    MessageBoxW(hWnd,
      L"Failed to get Desktop folder.", L"Error", MB_OK);
    return;
  }

  // Get a pidl for the folder the file
  // is located in.
	WCHAR Path[MAX_PATH];
	LPITEMIDLIST ParentPidl;
	DWORD Eaten;

	ZeroMemory(Path,sizeof(Path));
	//MultiByteToWideChar(CP_ACP,0, FilePath, wcslen(FilePath), Path,MAX_PATH);
	wcscpy(Path,FilePath);
	
	//wcscat(Path,L"\\");	wcscat(Path,FileName);

	 DWORD Result =
    DesktopFolder->ParseDisplayName(
      hWnd, 0, Path, &Eaten, &ParentPidl, 0);
  if (Result != NOERROR) 
  {
   MessageBoxW(hWnd,L"Invalid File Path.", L"Error", MB_OK);
    return;
  }


 // Get an IShellFolder for the folder
  // the file is located in.
  LPSHELLFOLDER ParentFolder;
  Result = DesktopFolder->BindToObject(ParentPidl,
    0, IID_IShellFolder, (void**)&ParentFolder);
  if (!ParentFolder) 
  {
     MessageBoxW(hWnd,L"Invalid file name.", L"Error", MB_OK);
    return;
  }

	// Get a pidl for the file itself.
	 LPITEMIDLIST Pidl;
	 ZeroMemory(Path,sizeof(Path));
	wcscpy(Path,FileName);
	ParentFolder->ParseDisplayName(
    hWnd, 0, Path, &Eaten, &Pidl, 0);

 
 // Get the IContextMenu for the file.
  LPCONTEXTMENU CM;
  ParentFolder->GetUIObjectOf(
    hWnd, 1, (LPCITEMIDLIST*)&Pidl,
    IID_IContextMenu, 0, (void**)&CM);

  if (!CM) 
  {
   MessageBoxW(hWnd,
      L"Unable to get context menu interface.", L"Error", MB_OK);
    return;
  }


 // Set up a CMINVOKECOMMANDINFO structure.
  CMINVOKECOMMANDINFO CI;
  ZeroMemory(&CI, sizeof(CI));
  CI.cbSize = sizeof(CMINVOKECOMMANDINFO);
  CI.hwnd = hWnd;
 
  
    HMENU hMenu = CreatePopupMenu();
    DWORD Flags = CMF_EXPLORE;
    // Optionally the shell will show the extended
    // context menu on some operating systems when
    // the shift key is held down at the time the
    // context menu is invoked. The following is
    // commented out but you can uncommnent this
    // line to show the extended context menu.
    // Flags |= 0x00000080;
    CM->QueryContextMenu(
      hMenu, 0, 1, 0x7FFF, Flags);

	InsertMenuW(hMenu, 0,	 MF_INSERT | MF_SEPARATOR, MF_SEPARATOR, L"");
	InsertMenuW(hMenu, 0,	 MF_INSERT , UNLOCK_BUTTON,L"Unlock");//	InsertMenuW(hMenu, 0,	 MF_INSERT , CHANGE_BUTTON,L"Set initial view");
	InsertMenuW(hMenu, 0,	 MF_INSERT , LOCK_BUTTON, L"Lock");
	InsertMenuW(hMenu, 0,	 MF_INSERT , REPAIR_BUTTON, L"Repair");	
	InsertMenuW(hMenu, 0,	 MF_INSERT , COLLECT_BUTTON, L"Convert INX");
	InsertMenuW(hMenu, 0,	 MF_INSERT , CLEAN_BUTTON, L"Clean INX");
	
    // Show the menu.
    POINT pt;
    GetCursorPos(&pt);
    int Cmd = TrackPopupMenu(hMenu,
      TPM_LEFTALIGN | TPM_LEFTBUTTON |
      TPM_RIGHTBUTTON | TPM_RETURNCMD,
      pt.x, pt.y, 0, hWnd, 0);
    // Handle the command. If the return value
    // from TrackPopupMenu is less than 100 then
    // a shell item was clicked.

	//swprintf(s,L"Cmd=%d ",Cmd);
	 // MessageBoxW(hWnd,    s, L"Error", MB_OK);

		if (Cmd < 64000 && Cmd != 0) 
		{
		  //CI.lpVerb = (LPSTR)((ULONG_PTR)((WORD)(Cmd - 1))) ;// MAKEINTRESOURCE(Cmd - 1);
		  CI.lpVerb = (LPSTR)(ULONG_PTR)  MAKEINTRESOURCE(Cmd - 1);
		  CI.lpParameters = "";
		  CI.lpDirectory = "";
		  CI.nShow = SW_SHOWNORMAL;
		  CM->InvokeCommand(&CI);
		  Fill_lists();
		}
		/*if (Cmd ==26) 
		{
		  CI.lpVerb = (LPSTR)(ULONG_PTR)MAKEINTRESOURCE(Cmd - 1);
		  CI.lpParameters = "";
		  CI.lpDirectory = "";
		  CI.nShow = SW_SHOWNORMAL;
		  CM->InvokeCommand(&CI);
		  Fill_lists();
		}*/
    // If Cmd is > 100 then it's one of our
    // inserted menu items.
		 else
			 SendMessage(hWnd,WM_COMMAND,Cmd,0);

 // Release the memory allocated for the menu.
  DestroyMenu(hMenu);
  OleUninitialize();
#endif
  }


int CheckFileSize(WCHAR* file)
{DWORD dwFileLength=0;
HANDLE	hFile=CreateFileW(file, GENERIC_READ,
		 FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,OPEN_EXISTING, 0, NULL);
		 if (hFile==INVALID_HANDLE_VALUE)
			{	 
				//MessageBoxW(hWnd, L"Cannot open the File", L"Error", MB_ICONERROR);
				return 0;
			}
 dwFileLength=GetFileSize (hFile, NULL) ;

	CloseHandle(hFile);
return dwFileLength;
	}


int CheckINXversion(WCHAR* inx_file)
{
char* Buffer;
DWORD n_INX_file_size;

HANDLE	hFile=CreateFileW(inx_file, GENERIC_READ,
		 FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,OPEN_EXISTING, 0, NULL);
		 if (hFile==INVALID_HANDLE_VALUE)
			{	 
				MessageBoxW(hWnd, L"Cannot open the File", L"Error", MB_ICONERROR);
			
				return 0;
			}

	int	dwSourceBufferLength=256;//dwFileLength+2048+128-dwFileLength%128;

		Buffer=(char*)new char[dwSourceBufferLength];
		
		if(Buffer==0)
			{
				CloseHandle(hFile);
				MessageBoxW(hWnd, L"Cannot allocate memory for buf4. Please close unused programs and try again.", L"Warning", MB_ICONWARNING);			
				//DestroyWindow(hWaitingWnd);return 0;
			}
		memset(Buffer,0x00, dwSourceBufferLength);	
		
		ReadFile(hFile, (LPVOID)Buffer, 	dwSourceBufferLength, &n_INX_file_size, NULL);
		CloseHandle(hFile);

	char cs4r[]="readerVersion=\"6.0\"";
	char cs3r[]="readerVersion=\"5.0\"";
	char cs2r[]="readerVersion=\"4.0\"";
	char cs1r[]="readerVersion=\"3.0\"";

	for(i=0;i<(int)(dwSourceBufferLength-10);i++)
 
		if(Buffer[i]==(char)cs3r[0]&&Buffer[i+1]==(char)cs3r[1])
		{
			if(!memcmp(&Buffer[i],cs4r,strlen(cs4r)))
				return 4;
			if(!memcmp(&Buffer[i],cs3r,strlen(cs3r)))
				return 3;
			if(!memcmp(&Buffer[i],cs2r,strlen(cs3r)))
				return 2;
			if(!memcmp(&Buffer[i],cs1r,strlen(cs3r)))
			 	return 1;
		}

delete (Buffer);
return 0;		
}


int CheckINDDversion(WCHAR* inx_file)
{
char* Buffer;
DWORD n_INX_file_size;

HANDLE	hFile=CreateFileW(inx_file, GENERIC_READ,
		 FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,OPEN_EXISTING, 0, NULL);
		 if (hFile==INVALID_HANDLE_VALUE)
			{	 
				MessageBoxW(hWnd, L"Cannot open the File", L"Error", MB_ICONERROR);
				return 0;
			}

	int	dwSourceBufferLength=256;//dwFileLength+2048+128-dwFileLength%128;

		Buffer=(char*)new char[dwSourceBufferLength];
		
		if(Buffer==0)
			{
				CloseHandle(hFile);
				MessageBoxW(hWnd, L"Cannot allocate memory for buf4. Please close unused programs and try again.", L"Warning", MB_ICONWARNING);			
				//DestroyWindow(hWaitingWnd);return 0;
			}
		memset(Buffer,0x00, dwSourceBufferLength);	
		
		ReadFile(hFile, (LPVOID)Buffer, 	dwSourceBufferLength, &n_INX_file_size, NULL);
		CloseHandle(hFile);
		
	int	ver=0;
	 ver=Buffer[0x20];
	  if(!ver)	 ver=Buffer[0x1d];
	 
	 if(ver)
	 {
	if(ver==4)return 2;//strcpy(name,"CS2");
	if(ver==5)return 3;//strcpy(name,"CS3");
	if(ver==6)return 4;//strcpy(name,"CS4");
	if(ver==7)return 5;//strcpy(name,"CS5");
	if(ver==8)return 6;//strcpy(name,"CS6");
	}


delete (Buffer);
return 0;
		
}



int GetLanguagesFromIndesign()
{
char* Buffer;
DWORD n_file_size;
WCHAR file[]=L"lang_codes_sec3_mod.indd";
SetCurrentDirectoryW(ProgramPatch);	

memset(Languages,0, LANG_COUNT*4*48);


HANDLE	hFile=CreateFile(file, GENERIC_READ,
		 FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,OPEN_EXISTING, 0, NULL);
		 if (hFile==INVALID_HANDLE_VALUE)
			{	 
				MessageBoxW(hWnd, L"Cannot open the File", L"Error", MB_ICONERROR);
				return 0;
			}

		DWORD dwFileLength=GetFileSize (hFile, NULL) ;
		int	dwSourceBufferLength= dwFileLength+dwFileLength/2;

		Buffer=(char*)new char[dwSourceBufferLength];
		
		if(Buffer==0)
			{
				CloseHandle(hFile);
				MessageBoxW(hWnd, L"Cannot allocate memory for buf4. Please close unused programs and try again.", L"Warning", MB_ICONWARNING);			
				//DestroyWindow(hWaitingWnd);return 0;
			}
		memset(Buffer,0x00, dwSourceBufferLength);	
		
		ReadFile(hFile, (LPVOID)Buffer, 	dwFileLength, &n_file_size, NULL);
		CloseHandle(hFile);
		
int d;
char Code[16]="";
char Language1[48]="";
char Language2[48]="";
int lang=0;
	for(i=0;i<(int)(dwSourceBufferLength-10) ;i++)
		if(Buffer[i  ]==(char)'[')
		if(Buffer[i+1]==(char)'0')
		if(Buffer[i+2]==(char)'0')
		if(Buffer[i+3]==(char)']')
		{
			i+=4;
			memset(Code,0,16); memset(Language1,0,48);memset(Language2,0,48);
			for(d=0;d<16;d++)
				if(Buffer[i+d]=='['&&Buffer[i+d+1]=='0'&&Buffer[i+d+2]=='2')//if(Buffer[i+d]==9)
					break;
				else
					Code[d]=Buffer[i+d];
			strcpy(Languages[lang][0],Code);

			i+=d+1+3;
			if(Code[0]=='P'&&Code[1]=='G'&&Code[2]==0)
				i=i;
		/*	if(Code[0]=='B'&&Code[1]=='G'&&Code[2]==0)
			{
				for(d=0;d<48;d++)
					if(Buffer[i+d]==9&&Buffer[i+d-1]!=9)
						break;
				else
					Language1[d]=Buffer[i+d];
			}
			else*/
			{
				for(d=0;d<48;d++)
					if(Buffer[i+d]=='['&&Buffer[i+d+1]=='0'&&Buffer[i+d+2]=='2')//if(Buffer[i+d]==9)
						break;
					else
					Language1[d]=Buffer[i+d];
			}

			strcpy(Languages[lang][1],Language1);

			i+=d+1+3;
			for(d=0;d<48;d++)
				if(Buffer[i+d]=='['&&Buffer[i+d+1]=='0'&&Buffer[i+d+2]=='1')
					break;
				else
					Language2[d]=Buffer[i+d];

				for(d=0;d<48;d++)
				{
					if(Language2[d]==0x1c &&Language2[d+1]==0&&Language2[d+2]==0 &&Language2[d+3]==0 )
						for(int t=d;t<48-d;t++)
							Language2[t]=Language2[t+4];
					if(Language2[d]==0x10&&Language2[d+1]==0&&Language2[d+2]==0 &&Language2[d+3]==0 )
						for(int t=d;t<48-d;t++)
							Language2[t]=Language2[t+4];
					if(Language2[d]==0x6&&Language2[d+1]==0x40&&Language2[d+2]==0x53)
						for(int t=d;t<48-d;t++)
							Language2[t]=Language2[t+2];
				
				}


			strcpy(Languages[lang][2],Language2);

			if(lang<LANG_COUNT-1) 
				lang++;
			else
				MessageBoxW(hWnd, L"lang>LANG_COUNT", L"Error", MB_ICONERROR);

		}

		hFile=CreateFileW(L"lang.dat", GENERIC_WRITE ,
		 FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,CREATE_ALWAYS, 0, NULL);
		 if (hFile==INVALID_HANDLE_VALUE)
			{	 
				MessageBoxW(hWnd, L"Cannot create the land file", L"Error", MB_ICONERROR);
				return 0;
			}
		WriteFile(hFile, (LPVOID)Languages, 	LANG_COUNT*4*48 , &n_file_size, NULL);
		CloseHandle(hFile);


//---------------
		memset(Buffer,0x00, dwSourceBufferLength);	
		for(lang=0;lang<LANG_COUNT  ;lang++)
			if(Languages[lang][0][0])
			 wsprintfA(&Buffer[strlen(Buffer)],"%s  %s  %s\n",Languages[lang][0],Languages[lang][1],Languages[lang][2]);

		hFile=CreateFileW(L"lang.txt", GENERIC_WRITE ,
		 FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,CREATE_ALWAYS, 0, NULL);
		 if (hFile==INVALID_HANDLE_VALUE)
			{	 
				MessageBoxW(hWnd, L"Cannot create the land file", L"Error", MB_ICONERROR);
				return 0;
			}
		WriteFile(hFile, (LPVOID)Buffer, 	strlen(Buffer) , &n_file_size, NULL);
		CloseHandle(hFile);
//----------


delete (Buffer);
return 0;
		
}


int GetLanguageCodesFromIndesign()
{
char* Buffer;
DWORD n_file_size;
WCHAR file[]=L"Q10204_USB3_HZ-3A_Docking_Station_QSG.indd";
SetCurrentDirectoryW(ProgramPatch);	

char codes[LANG_COUNT][8];
memset(codes,0,LANG_COUNT*8);

HANDLE	hFile=CreateFile(file, GENERIC_READ,
		 FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,OPEN_EXISTING, 0, NULL);
		 if (hFile==INVALID_HANDLE_VALUE)
			{	 
				MessageBoxW(hWnd, L"Cannot open the File", L"Error", MB_ICONERROR);
				return 0;
			}

		DWORD dwFileLength=GetFileSize (hFile, NULL) ;
		int	dwSourceBufferLength= dwFileLength+dwFileLength/2;

		Buffer=(char*)new char[dwSourceBufferLength];
		
		if(Buffer==0)
			{
				CloseHandle(hFile);
				MessageBoxW(hWnd, L"Cannot allocate memory for buf4. Please close unused programs and try again.", L"Warning", MB_ICONWARNING);			
				//DestroyWindow(hWaitingWnd);return 0;
			}
		memset(Buffer,0x00, dwSourceBufferLength);	
		
		ReadFile(hFile, (LPVOID)Buffer, 	dwFileLength, &n_file_size, NULL);
		CloseHandle(hFile);
		
 char code_[312]="";
 
 int lang_count=0;
int lang=0;
	for(i=0;i<(int)(dwSourceBufferLength-10) ;i++)
		for(lang=0;lang<LANG_COUNT  ;lang++)
		if(Languages[lang][0][0])
		{
			if(Buffer[i  ]== Languages[lang][1][0]) 
			if(Buffer[i+1]== Languages[lang][1][1])
			if(Buffer[i+2]== Languages[lang][1][2])
			if(Buffer[i+3]== Languages[lang][1][3])
			if(!memcmp(&Buffer[i],Languages[lang][1],strlen(Languages[lang][1])))
			{
			 for(int f=0;f<lang_count  ;f++)
				 if(!strcmp(codes[f],Languages[lang][0]))
					 break;
				 if(f==lang_count)
				 {
					strcpy(codes[lang_count],Languages[lang][0]);
					lang_count++;

					strcat(code_, Languages[lang][0]);strcat(code_, "-");
				 }

			}
			if(Buffer[i  ]== Languages[lang][2][0]) 
			if(Buffer[i+1]== Languages[lang][2][1])
			if(Buffer[i+2]== Languages[lang][2][2])
			if(Buffer[i+3]== Languages[lang][2][3])
			//if(Buffer[i+4]== Languages[lang][2][4])
			//if(!memcmp(&Buffer[i],&Languages[lang][2],strlen(&Languages[lang][2])))
			{ 
				//-------------------------
				if(!Languages[lang][3][0])
				{
					memset(Languages[lang][3],0,48);
					for(int d=0;d<40  ;d++)
					if(Buffer[i+d]!=0xd)	
						Languages[lang][3][d]=Buffer[i+d];
					else break;
				}
				//-------------------------

				int len=strlen(Languages[lang][2]);
				if(!memcmp(&Buffer[i],&Languages[lang][2],len))
				{
				  for(int f=0;f<lang_count  ;f++)
					 if(!strcmp(codes[f],Languages[lang][0]))
						 break;
					 if(f==lang_count)
					 {
						strcpy(codes[lang_count],Languages[lang][0]);
						lang_count++;

						strcat(code_, Languages[lang][0]);strcat(code_, "-");
					 }
				}
			}
		}


		if(code_[strlen(code_)-1]=='-')
			code_[strlen(code_)-1]=0;

		char source_file[512]="";
			strcpy(source_file,code_);
			strcat(source_file,".tdd");

		HANDLE hfz = CreateFileA(source_file,GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
 
		CloseHandle(hfz);
delete (Buffer);
return 0;
		
}



void FixLinkInINDD(WCHAR* inx_file)
{
char* Buffer;
DWORD n_INX_file_size;

HANDLE	hFile=CreateFileW(inx_file, GENERIC_READ|GENERIC_WRITE,
		 FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,OPEN_EXISTING, 0, NULL);
		 if (hFile==INVALID_HANDLE_VALUE)
			{	 
				MessageBoxW(hWnd, L"Cannot open the File", L"Error", MB_ICONERROR);
				//MessageBoxW(hWnd, inx_file, L"Error", MB_ICONERROR);
				return ;
			}
	
	DWORD dwFileLength=GetFileSize (hFile, NULL) ;
	
	int	dwSourceBufferLength=dwFileLength;//dwFileLength+2048+128-dwFileLength%128;

	Buffer=(char*)new char[dwSourceBufferLength];
		
	if(Buffer==0)
			{
				CloseHandle(hFile);
				MessageBoxW(hWnd, L"Cannot allocate memory for buf5. Please close unused programs and try again.", L"Warning", MB_ICONWARNING);			
				return;
			}
	memset(Buffer,0x00, dwSourceBufferLength);	
		
	ReadFile(hFile, (LPVOID)Buffer, 	dwSourceBufferLength, &n_INX_file_size, NULL);

	CloseHandle(hFile);
	int ok=0;
	char cs3r[]=">file://Tp-doc-01";
	char cs2r[]=">file:///C:/Users";

	 char cs3r2[]="file://Tp-doc-01";
	char cs2r2[]="file:/Tp-doc-/01";
	//<stRef:filePath>file://Tp-doc-01/doc/Christel_Amparado
	//char cs3r[]="file://Tp-doc-01/doc";
	//char cs2r[]="file:/Tp-doc-/01/doc";
	char str[]="/Links/";
	int count =0;


		for(i=0;i<(int)(dwSourceBufferLength-10) ;i++)
		if(Buffer[i]==(char)cs3r[0])
			//if(Buffer[i-1]!=(char)'h')
			if(!memcmp(&Buffer[i],cs3r,strlen(cs3r)))

		 	//for(int j=i;j<i+120 ;j++)
			{
			// if(Buffer[j]==0)
			//	 break;
				 
			//	if(Buffer[j]==(char)str[0])
		 	//	if(!memcmp(&Buffer[j],str,strlen(str)))

					{	
					 
						
						for(int d=0;d<(int)strlen(cs2r);d++)
						Buffer[i+d]=cs2r[d];
						 ok=1; 
						 count++;
						 //i= dwSourceBufferLength;
						 // break;
					}
			}


//---------------------------
	/*	 for(i=0;i<(int)(dwSourceBufferLength-10)&&count<60 ;i++)
		if(Buffer[i]==(char)cs3r2[0])
			if(!memcmp(&Buffer[i],cs3r2,strlen(cs3r2)))

		 	 for(int j=i;j<i+120 ;j++)
			{
			 if(Buffer[j]==0)
			 	 break;
				 
			 	if(Buffer[j]==(char)str[0])
		 	 	if(!memcmp(&Buffer[j],str,strlen(str)))

					{	
						for(int d=0;d<(int)strlen(cs2r2);d++)
						Buffer[i+d]=cs2r2[d];
						 ok=1; 
						 count++;
						 //i= dwSourceBufferLength;
						  break;
					}
			}*/

	if(ok)
	{
		WCHAR new_inx_file[MAX_PATH];
		wcscpy(new_inx_file,inx_file);
		for(int i=wcslen(new_inx_file);i>0;i--)
			if(new_inx_file[i]=='.'&&new_inx_file[i+1]=='i'||new_inx_file[i+1]=='I')
			{
				new_inx_file[i]=0;
				if(ok==1) wcscat(	new_inx_file,L"_ok.indd");
			 
				break;
			}

		CopyFile(inx_file,new_inx_file,0);		
		hFile=CreateFileW(new_inx_file, GENERIC_READ|GENERIC_WRITE,
			 FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,OPEN_EXISTING, 0, NULL);
			 if (hFile==INVALID_HANDLE_VALUE)
				{	 
					MessageBoxW(hWnd, L"Cannot open the target File", L"Error", MB_ICONERROR);
					return ;
				}

		WriteFile(hFile, (LPVOID)Buffer, 	dwFileLength, &n_INX_file_size, NULL);

		CloseHandle(hFile);
	}
	delete (Buffer);
}


void ConvertINXFile(WCHAR* inx_file)
{


char* Buffer;
DWORD n_INX_file_size;

HANDLE	hFile=CreateFileW(inx_file, GENERIC_READ|GENERIC_WRITE,
		 FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,OPEN_EXISTING, 0, NULL);
		 if (hFile==INVALID_HANDLE_VALUE)
			{	 
				MessageBoxW(hWnd, L"Cannot open the File", L"Error", MB_ICONERROR);
				//MessageBoxW(hWnd, inx_file, L"Error", MB_ICONERROR);
				return ;
			}
	
	int	dwSourceBufferLength=256;//dwFileLength+2048+128-dwFileLength%128;

	Buffer=(char*)new char[dwSourceBufferLength];
		
	if(Buffer==0)
			{
				CloseHandle(hFile);
				MessageBoxW(hWnd, L"Cannot allocate memory for buf5. Please close unused programs and try again.", L"Warning", MB_ICONWARNING);			
				return;
			}
	memset(Buffer,0x00, dwSourceBufferLength);	
		
	ReadFile(hFile, (LPVOID)Buffer, 	dwSourceBufferLength, &n_INX_file_size, NULL);

	CloseHandle(hFile);
	int ok=0;

	char cs3r[]="readerVersion=\"5.0\"";
	char cs2r[]="readerVersion=\"4.0\"";
	char cs1r[]="readerVersion=\"3.0\"";
	char cs3p[]="product=\"6.0(352)\"";
	char cs2p[]="product=\"5.0(458)\"";
	char cs1p[]="product=\"4.0(688)\"";	
	char cs3D[]="DOMVersion=\"6.0\"";
	char cs2D[]="DOMVersion=\"5.0\"";
	char cs1D[]="DOMVersion=\"4.0\"";

//--------convertt to cs1-------------
	for(i=0;i<(int)(dwSourceBufferLength-10);i++)
		if(Buffer[i]==(char)cs2r[0])
			if(!memcmp(&Buffer[i],cs2r,strlen(cs2r)))
			{	
				for(int d=0;d<(int)strlen(cs1r);d++)
				Buffer[i+d]=cs1r[d];
				{ok=1;break;}
			}

	for(i=0;i<(int)(dwSourceBufferLength-10);i++)
		if(Buffer[i]==(char)cs2p[0])
			if(!memcmp(&Buffer[i],cs2p,strlen(cs2p)))
			{	
				for(int d=0;d<(int)strlen(cs1p);d++)
				Buffer[i+d]=cs1p[d];
				{ok=1;break;}
			}

		for(i=0;i<(int)(dwSourceBufferLength-10);i++)
		if(Buffer[i]==(char)cs2D[0])
			if(!memcmp(&Buffer[i],cs2D,strlen(cs2D)))
			{	
				for(int d=0;d<(int)strlen(cs1D);d++)
				Buffer[i+d]=cs1D[d];
				{ok=1;break;}
			}
	//---------------------------------------------
	
//--------convertt to cs2-------------
	for(i=0;i<(int)(dwSourceBufferLength-10);i++)
		if(Buffer[i]==(char)cs3r[0])
			if(!memcmp(&Buffer[i],cs3r,strlen(cs3r)))
			{	
				for(int d=0;d<(int)strlen(cs2r);d++)
				Buffer[i+d]=cs2r[d];
				{ok=2;break;}
			}

	for(i=0;i<(int)(dwSourceBufferLength-10);i++)
		if(Buffer[i]==(char)cs3p[0])
			if(!memcmp(&Buffer[i],cs3p,strlen(cs3p)))
			{	
				for(int d=0;d<(int)strlen(cs2p);d++)
				Buffer[i+d]=cs2p[d];
				{ok=2;break;}
			}

		for(i=0;i<(int)(dwSourceBufferLength-10);i++)
		if(Buffer[i]==(char)cs3D[0])
			if(!memcmp(&Buffer[i],cs3D,strlen(cs3D)))
			{	
				for(int d=0;d<(int)strlen(cs2D);d++)
				Buffer[i+d]=cs2D[d];
				{ok=2;break;}
			}
	//---------------------------------------------
	

	if(ok)
	{
		WCHAR new_inx_file[MAX_PATH];
		wcscpy(new_inx_file,inx_file);
		for(int i=wcslen(new_inx_file);i>0;i--)
			if(new_inx_file[i]=='.'&&new_inx_file[i+1]=='i'||new_inx_file[i+1]=='I')
			{
				new_inx_file[i]=0;
				if(ok==1) wcscat(	new_inx_file,L"_CS1.inx");
				if(ok==2) wcscat(	new_inx_file,L"_CS2.inx");
				break;
			}

	CopyFile(inx_file,new_inx_file,0);		
	hFile=CreateFileW(new_inx_file, GENERIC_READ|GENERIC_WRITE,
		 FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,OPEN_EXISTING, 0, NULL);
		 if (hFile==INVALID_HANDLE_VALUE)
			{	 
				MessageBoxW(hWnd, L"Cannot open the target File", L"Error", MB_ICONERROR);
				return ;
			}

	WriteFile(hFile, (LPVOID)Buffer, 	dwSourceBufferLength, &n_INX_file_size, NULL);

	CloseHandle(hFile);
	}
delete (Buffer);
}

void Check_md5(WCHAR* file)
{
#define MD5_BUFFER_SIZE 600
char Buffer2[MD5_BUFFER_SIZE];
DWORD n_md5_file_size;
HANDLE	hFile;
char* Buffer;
DWORD dwFileLength;

 	hFile=CreateFileW(file, GENERIC_READ,
		 FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,OPEN_EXISTING, 0, NULL);
		 if (hFile==INVALID_HANDLE_VALUE)
			{	 
				MessageBoxW(hWnd, L"Cannot open the File", L"Error", MB_ICONERROR);
				return ;
			}
	dwFileLength=GetFileSize (hFile, NULL) ;
	if(dwFileLength>MD5_BUFFER_SIZE)
	{	 
			MessageBoxW(hWnd, L"md5 File is too big", L"Error", MB_ICONERROR);
			return ;
	}

	memset(Buffer2,0x00, MD5_BUFFER_SIZE);	
	ReadFile(hFile, (LPVOID)Buffer2, 	dwFileLength, &n_md5_file_size, NULL);
	CloseHandle(hFile);

	char pdf_file[MAX_PATH];
	memset(pdf_file,0,MAX_PATH);
	char pdf_md5[MAX_PATH];
	memset(pdf_md5,0,MAX_PATH);
 

	for(int s=dwFileLength;s>0;s--)
	{
		if(Buffer2[s]=='.'&&Buffer2[s+1]=='p'&&Buffer2[s+2]=='d'&&Buffer2[s+3]=='f' )
		{
			int end=s+4;
			int d=0;
			while(s>0&&Buffer2[s]!='*')s--;
			int start=s+1;
			memcpy(pdf_file,&Buffer2[start],end-start);

			end=start-2;
			s=end;
			while(s>0&&Buffer2[s]!=0x0A)s--;
			start=s+1;
			memcpy(pdf_md5,&Buffer2[start],end-start);

		}	
	}
//--------to upper case----------
int len3=strlen(pdf_md5);
for(int e=0;e<len3;e++)
{
if(pdf_md5[e]=='a') pdf_md5[e]='A';
if(pdf_md5[e]=='b') pdf_md5[e]='B';
if(pdf_md5[e]=='c') pdf_md5[e]='C';
if(pdf_md5[e]=='d') pdf_md5[e]='D';
if(pdf_md5[e]=='e') pdf_md5[e]='E';
if(pdf_md5[e]=='f') pdf_md5[e]='F';
}
///----------------------
 
int len2=wcslen(file);
for(int a=0;a<len2;a++)
if((file[a]=='.'&& file[a+1]=='m' && file[a+2]=='d' && file[a+3]=='5')||
   (file[a]=='.'&& file[a+1]=='M' && file[a+2]=='D' && file[a+3]=='5'))
{
	file[a+1]='p';
	file[a+2]='d';
	file[a+3]='f';
}

 hFile=CreateFileW(file, GENERIC_READ,
		 FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,OPEN_EXISTING, 0, NULL);
		 if (hFile==INVALID_HANDLE_VALUE)
			{	 
				MessageBoxW(hWnd, L"Cannot open the File", L"Error", MB_ICONERROR);
				return ;
			}
	  dwFileLength=GetFileSize (hFile, NULL) ;
	int	dwSourceBufferLength= dwFileLength+dwFileLength/2;// +128-dwFileLength%128;

	Buffer=(char*)new char[dwSourceBufferLength];
		
	if(Buffer==0)
		{
			CloseHandle(hFile);
			MessageBoxW(hWnd, L"Cannot allocate memory for buffer. Please close unused programs and try again.", L"Warning", MB_ICONWARNING);			
			return;
		}
	memset(Buffer,0x00, dwSourceBufferLength);	
	
	ReadFile(hFile, (LPVOID)Buffer, dwFileLength, &n_md5_file_size, NULL);
	CloseHandle(hFile);
					
char *p =md5_noerrors(Buffer,dwFileLength);

delete(Buffer);


char str[100];
 

if(!memcmp(pdf_md5,p,strlen(pdf_md5)))
{
	wsprintfA(str,"%s\nMD5 hash is correct.",pdf_file);
	MessageBoxA(0, str, "MD5 is OK", MB_OK);
}
else
{
	wsprintfA(str,"%s\nMD5 hash is wrong.",pdf_file);	
	MessageBoxA(0, str, "Wrong MD5", MB_ICONWARNING);
}			

}


void Get_md5(WCHAR* file)
{
char* Buffer;
DWORD n_md5_file_size;

HANDLE	hFile=CreateFileW(file, GENERIC_READ,
		 FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,OPEN_EXISTING, 0, NULL);
		 if (hFile==INVALID_HANDLE_VALUE)
			{	 
				MessageBoxW(hWnd, L"Cannot open the File", L"Error", MB_ICONERROR);
				return ;
			}
	DWORD dwFileLength=GetFileSize (hFile, NULL) ;
	int	dwSourceBufferLength= dwFileLength+dwFileLength/2;// +128-dwFileLength%128;

	Buffer=(char*)new char[dwSourceBufferLength];
		
	if(Buffer==0)
			{
				CloseHandle(hFile);
				MessageBoxW(hWnd, L"Cannot allocate memory for buffer. Please close unused programs and try again.", L"Warning", MB_ICONWARNING);			
				return;
			}
	memset(Buffer,0x00, dwSourceBufferLength);	
	
	ReadFile(hFile, (LPVOID)Buffer, 	dwFileLength, &n_md5_file_size, NULL);
	CloseHandle(hFile);
					
	WCHAR md5_file[MAX_PATH*2];
	memset(md5_file,0,MAX_PATH*2*2);
	WCHAR md5[MAX_PATH];
	memset(md5,0,MAX_PATH*2);
						
	char filename[200];
	memset(filename,0,200);

	char *p =md5_noerrors(Buffer,dwFileLength);
						
	if(p[0]>0)
		MultiByteToWideChar(CP_ACP, 0, p, strlen(p), md5, MAX_PATH);
	else
	{
		MessageBoxW(hWnd, L"wrong md5", L"Warning", MB_ICONWARNING);			
		return;
	}
					

	wcscpy(md5_file,file);
	int i,len=wcslen(md5_file);

	for(i=len;i>0;i--)
	if(md5_file[i]=='\\')
	{
		WideCharToMultiByte(CP_ACP,0,&md5_file[i+1],len-i,filename,200,0,0);
		break;
	}
				
	len=wcslen(md5_file);
	for(i=len;i>0;i--)
	if(md5_file[i]=='.'&&(md5_file[i+1]=='p'||md5_file[i+1]=='P'))
	{md5_file[i]=0; break;}

	wcscat(md5_file,L".md5");

//---------------------------------------
	SYSTEMTIME sm;
	char time[200];memset(time,0,200);

	memset(&sm,0,sizeof(SYSTEMTIME));
	GetLocalTime(&sm);

	wsprintfA(time,"%d/%d/%d %d:%d:%d", sm.wDay,sm.wMonth,sm.wYear, sm.wHour, sm.wMinute,sm.wSecond );
//--------------------------

char buff[600];
memset(buff,0,600);
strcpy(buff,"# MD5 checksums generated by PDF-locker (http://www.yehongmei.narod.ru/pdf_locker121.htm)\n");
				
wsprintfA(&buff[strlen(buff)],"# Generated %s\n\n%s *%s\n",time, p,filename); //e463f0bddf71410aa476b840c40e6471 *XX19_M52BC_Guide_Win8-1_R9584_V1.pdf

//for(i=0;i<100;i++)
//wsprintfA(&buff[strlen(buff)]," %d ",i);

	hFile=CreateFileW(md5_file, GENERIC_WRITE ,
		 FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,CREATE_ALWAYS, 0, NULL);
		 if (hFile==INVALID_HANDLE_VALUE)
			{	 
				MessageBoxW(hWnd, L"Cannot create the md5 File", L"Error", MB_ICONERROR);
				return ;
			}
		//WriteFile(hFile, (LPVOID)Buffer, 	dwSourceBufferLength, &n_md5_file_size, NULL);
	 	WriteFile(hFile, (LPVOID)buff, 	strlen(buff) , &n_md5_file_size, NULL);
	CloseHandle(hFile);
				///MessageBoxW(0, L"point 14", L"Warning", MB_ICONWARNING);

delete(Buffer);

}



void Get_md5_2(WCHAR* file)
{

char* Buffer;
DWORD n_INX_file_size;

				///MessageBoxW(0, L"point 1", L"Warning", MB_ICONWARNING);

HANDLE	hFile=CreateFileW(file, GENERIC_READ,
		 FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,OPEN_EXISTING, 0, NULL);
		 if (hFile==INVALID_HANDLE_VALUE)
			{	 
				MessageBoxW(hWnd, L"Cannot open the File", L"Error", MB_ICONERROR);
				return ;
			}
	DWORD dwFileLength=GetFileSize (hFile, NULL) ;
	int	dwSourceBufferLength= dwFileLength+dwFileLength/2;// +128-dwFileLength%128;

	Buffer=(char*)new char[dwSourceBufferLength];
		
	if(Buffer==0)
			{
				CloseHandle(hFile);
				MessageBoxW(hWnd, L"Cannot allocate memory for buffer. Please close unused programs and try again.", L"Warning", MB_ICONWARNING);			
				return;
			}
	memset(Buffer,0x00, dwSourceBufferLength);	
		///MessageBoxW(0, L"point 2", L"Warning", MB_ICONWARNING);
	ReadFile(hFile, (LPVOID)Buffer, 	dwSourceBufferLength, &n_INX_file_size, NULL);
	CloseHandle(hFile);


md5_state_t state;
	md5_byte_t digest[16];
	char hex_output[16*2 + 1];
	int di;

	md5_init(&state);
 md5_append(&state, (const md5_byte_t *)Buffer, dwFileLength);


	md5_finish(&state, digest);
	for (di = 0; di < 16; ++di)
	    sprintf(hex_output + di * 2, "%02x", digest[di]);



WCHAR md5[MAX_PATH];
memset(md5,0,MAX_PATH*2);
MultiByteToWideChar(CP_ACP, 0, hex_output, strlen(hex_output), md5, MAX_PATH);


						
WCHAR md5_file[MAX_PATH*2];
memset(md5_file,0,MAX_PATH*2*2);

					
char filename[200];
memset(filename,0,200);


						

wcscpy(md5_file,file);
int i,len=wcslen(md5_file);
///MessageBoxW(0, L"point 7", L"Warning", MB_ICONWARNING);
for(i=len;i>0;i--)
if(md5_file[i]=='\\')
{
	WideCharToMultiByte(CP_ACP,0,&md5_file[i+1],len-i,filename,200,0,0);
	break;
}
					///MessageBoxW(0, L"point 8", L"Warning", MB_ICONWARNING);
len=wcslen(md5_file);
for(i=len;i>0;i--)
if(md5_file[i]=='.'&&(md5_file[i+1]=='p'||md5_file[i+1]=='P'))
{md5_file[i]=0; break;}

					///MessageBoxW(0, L"point 9", L"Warning", MB_ICONWARNING);

wcscat(md5_file,L".md5");

//---------------------------------------
SYSTEMTIME sm;
	char time[200];memset(time,0,200);

	memset(&sm,0,sizeof(SYSTEMTIME));
	GetLocalTime(&sm);

	wsprintfA(time,"%d/%d/%d %d:%d:%d", sm.wDay,sm.wMonth,sm.wYear, sm.wHour, sm.wMinute,sm.wSecond );
				///MessageBoxW(0, L"point 11", L"Warning", MB_ICONWARNING);	
//--------------------------


char buff[600];
memset(buff,0,600);
strcpy(buff,"# MD5 checksums generated by PDF-locker (http://www.yehongmei.narod.ru/pdf_locker121.htm)\n");
					///MessageBoxW(0, L"point 12", L"Warning", MB_ICONWARNING);
wsprintfA(&buff[strlen(buff)],"# Generated %s\n\n%s *%s\n",time, hex_output,filename); //e463f0bddf71410aa476b840c40e6471 *XX19_M52BC_Guide_Win8-1_R9584_V1.pdf

					///MessageBoxW(0, L"point 13", L"Warning", MB_ICONWARNING);

	hFile=CreateFileW(md5_file, GENERIC_WRITE ,
		 FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,CREATE_ALWAYS, 0, NULL);
		 if (hFile==INVALID_HANDLE_VALUE)
			{	 
				MessageBoxW(hWnd, L"Cannot create the md5 File", L"Error", MB_ICONERROR);
				return ;
			}
		//WriteFile(hFile, (LPVOID)Buffer, 	dwSourceBufferLength, &n_INX_file_size, NULL);
	 	WriteFile(hFile, (LPVOID)buff, 	strlen(buff) , &n_INX_file_size, NULL);
	CloseHandle(hFile);
				///MessageBoxW(0, L"point 14", L"Warning", MB_ICONWARNING);

delete(Buffer);

}


/*
void Get_md5_(char* file)
{
//	int static num=1;
char* Buffer;
DWORD n_INX_file_size;

HANDLE	hFile=CreateFileA(file, GENERIC_READ|GENERIC_WRITE,
		 FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,OPEN_EXISTING, 0, NULL);
		 if (hFile==INVALID_HANDLE_VALUE)
			{	 
				MessageBoxW(hWnd, L"Cannot open the File", L"Error", MB_ICONERROR);
				return ;
			}
	DWORD dwFileLength=GetFileSize (hFile, NULL) ;
	int	dwSourceBufferLength= dwFileLength+dwFileLength/2;// +128-dwFileLength%128;

	Buffer=(char*)new char[dwSourceBufferLength];
		
	if(Buffer==0)
			{
				CloseHandle(hFile);
				MessageBoxW(hWnd, L"Cannot allocate memory for buffer. Please close unused programs and try again.", L"Warning", MB_ICONWARNING);			
				return;
			}
	memset(Buffer,0x00, dwSourceBufferLength);	
		
	ReadFile(hFile, (LPVOID)Buffer, 	dwSourceBufferLength, &n_INX_file_size, NULL);
	CloseHandle(hFile);


char md5_file[MAX_PATH*2];
memset(md5_file,0,MAX_PATH*2 );
char md5[MAX_PATH];
memset(md5,0,MAX_PATH );
char *p =md5_noerrors(Buffer,dwFileLength);

//MultiByteToWideChar(CP_ACP, 0, p, strlen(p), md5, MAX_PATH);


strcpy(md5_file,file);
int len=strlen(md5_file);
for(int i=len;i>0;i--)
if(md5_file[i]=='\\')
{md5_file[i+1]=0; break;}


strcat(md5_file,md5);
//wsprintf(&md5_file[wcslen(md5_file)-1],L"_%d",num);
strcat(md5_file,"_.md5");

	hFile=CreateFileA(md5_file, GENERIC_READ ,
		 FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,CREATE_ALWAYS, 0, NULL);
		 if (hFile==INVALID_HANDLE_VALUE)
			{	 
				MessageBoxW(hWnd, L"Cannot create the md5 File", L"Error", MB_ICONERROR);
				return ;
			}
	//WriteFile(hFile, (LPVOID)Buffer, 	dwSourceBufferLength, &n_INX_file_size, NULL);
	//	WriteFile(hFile, (LPVOID)Buffer, 	strlen(Buffer) , &n_INX_file_size, NULL);
	CloseHandle(hFile);


delete(Buffer);

}

*/





void	ReplaceFont(WCHAR* inx_file)
{

char* Buffer;
DWORD n_INX_file_size;

HANDLE	hFile=CreateFileW(inx_file, GENERIC_READ|GENERIC_WRITE,
		 FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,OPEN_EXISTING, 0, NULL);
		 if (hFile==INVALID_HANDLE_VALUE)
			{	 
				MessageBoxW(hWnd, L"Cannot open the File", L"Error", MB_ICONERROR);
				//MessageBoxW(hWnd, inx_file, L"Error", MB_ICONERROR);
				return ;
			}
	DWORD dwFileLength=GetFileSize (hFile, NULL) ;
	int	dwSourceBufferLength= dwFileLength+dwFileLength/2;// +128-dwFileLength%128;

	Buffer=(char*)new char[dwSourceBufferLength];
		
	if(Buffer==0)
			{
				CloseHandle(hFile);
				MessageBoxW(hWnd, L"Cannot allocate memory for buffer. Please close unused programs and try again.", L"Warning", MB_ICONWARNING);			
				return;
			}
	memset(Buffer,0x00, dwSourceBufferLength);	
		
	ReadFile(hFile, (LPVOID)Buffer, 	dwSourceBufferLength, &n_INX_file_size, NULL);

	CloseHandle(hFile);
int ok=0;

	char  old_font[]={0x0E, 0x40, 0x0E, 'A','p','p','l','e',' ','L','i','G','o','t','h','i','c' };
	int old_font_len=0x0E+3;//strlen(old_font);
	char  new_font[]={0x0E, 0x40, 0x0E, 'A','p','p','l','e',' ','L','u','k','o','y','h','u','k' };
	//char  new_font[]={0x0A, 0x40, 0x0A, 'M','y','r','i','a','d',' ','P','r','o',' ',' ',' ',' '};
	int new_font_len= strlen(new_font);//0x0E+3;

	char  old_font2[]={0x15, 0x40, 0x15,'A','p','p','l','e',' ','L','i','G','o','t','h','i','c',' ','M','e','d','i','u','m',0};
	int old_font2_len=strlen(old_font2);
	char  new_font2[]={0x12, 0x40, 0x12,'M','y','r','i','a','d',' ','P','r','o',' ','R','e','g','u','l','a','r',' ',' ',' ',0};
	int new_font2_len=strlen(new_font2);

	char  old_font3[]="Apple LiGothic</stFnt:fontFamily>";
	int old_font3_len=strlen(old_font3);
	char  new_font3[]="Myriad Pro    </stFnt:fontFamily>";
	int new_font3_len=strlen(new_font3);

	for(i=0;i<(int)(dwSourceBufferLength-1);i++)//find IGeo="x_19
	{
				
	 	if(		Buffer[i]==old_font[0])
		if(		Buffer[i+1]==old_font[1]) 
			if(!memcmp(	&Buffer[i],old_font, old_font_len)) 
			{
			//memset(&Buffer[i],0x0 ,old_font_len);
			 memcpy(&Buffer[i],new_font,new_font_len);
			 ok=1;

			} 
	/*	if(		Buffer[i]==old_font2[0])
		if(		Buffer[i+1]==old_font2[1]) 
			if(!memcmp(	&Buffer[i],old_font2, old_font2_len)) 
			{
			memcpy(&Buffer[i],new_font2,new_font2_len);
			ok=1;

			}*/
		if(		Buffer[i]==old_font3[0])
		if(		Buffer[i+1]==old_font3[1]) 
			if(!memcmp(	&Buffer[i],old_font3, old_font3_len)) 
			{
			memcpy(&Buffer[i],new_font3,new_font3_len);
			ok=1;

			}
	}


	if(ok)
	{
		WCHAR new_inx_file[MAX_PATH];
		wcscpy(new_inx_file,inx_file);
		for(int i=wcslen(new_inx_file);i>0;i--)
			if(new_inx_file[i]=='.'&&new_inx_file[i+1]=='i'||new_inx_file[i+1]=='I')
			{
				new_inx_file[i]=0;
				if(ok==1) wcscat(	new_inx_file,L"_clean.indd");
			
				break;
			}

		CopyFile(inx_file,new_inx_file,0);		
		hFile=CreateFileW(new_inx_file, GENERIC_READ|GENERIC_WRITE,
			 FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,OPEN_EXISTING, 0, NULL);
			 if (hFile==INVALID_HANDLE_VALUE)
				{	 
					MessageBoxW(hWnd, L"Cannot open the target File", L"Error", MB_ICONERROR);
					return ;
				}

		//WriteFile(hFile, (LPVOID)Buffer, 	dwSourceBufferLength, &n_INX_file_size, NULL);
			WriteFile(hFile, (LPVOID)Buffer, 	dwFileLength , &n_INX_file_size, NULL);

		CloseHandle(hFile);
	}



delete (Buffer);
}



void	CleanINXFile(WCHAR* inx_file)
{

char* Buffer;
DWORD n_INX_file_size;

HANDLE	hFile=CreateFileW(inx_file, GENERIC_READ|GENERIC_WRITE,
		 FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,OPEN_EXISTING, 0, NULL);
		 if (hFile==INVALID_HANDLE_VALUE)
			{	 
				MessageBoxW(hWnd, L"Cannot open the File", L"Error", MB_ICONERROR);
				//MessageBoxW(hWnd, inx_file, L"Error", MB_ICONERROR);
				return ;
			}
	DWORD dwFileLength=GetFileSize (hFile, NULL) ;
	int	dwSourceBufferLength= dwFileLength+dwFileLength/2;// +128-dwFileLength%128;

	Buffer=(char*)new char[dwSourceBufferLength];
		
	if(Buffer==0)
			{
				CloseHandle(hFile);
				MessageBoxW(hWnd, L"Cannot allocate memory for buffer. Please close unused programs and try again.", L"Warning", MB_ICONWARNING);			
				return;
			}
	memset(Buffer,0x00, dwSourceBufferLength);	
		
	ReadFile(hFile, (LPVOID)Buffer, 	dwSourceBufferLength, &n_INX_file_size, NULL);

	CloseHandle(hFile);
/*
//------remove frame on picture----------------------------------------
 int d_val=0;
for(i=0;i<(int)(dwSourceBufferLength-1);i++)//find IGeo="x_19
				
		if(		Buffer[i]==(unsigned char)'<' //<crec
			&&  Buffer[i+1]==(unsigned char)'c'
			&&  Buffer[i+2]==(unsigned char)'r'
			&&  Buffer[i+3]==(unsigned char)'e'
			&&  Buffer[i+4]==(unsigned char)'c')
			{
				int rec=i+4;
				int start=0;
				char flcl[6]="o_u18";
				char  lncl[12];
					memset(lncl,0,12);
					//memset(flcl,0,12);

				for(i=rec;i<(int)(n_INX_file_size-10);i++)
				{
					if(		   Buffer[i+1]==(unsigned char)'l' //lncl
							&&  Buffer[i+2]==(unsigned char)'n'
							&&  Buffer[i+3]==(unsigned char)'c'
							&&  Buffer[i+4]==(unsigned char)'l')
							{
								
									int count;
								start=0;
								while(Buffer[i]!='='&&Buffer[i+1]!='"')
									i++;

								count=0;
								while(Buffer[i+2+count]!='"')
									count++;
									
								memset(lncl,0,12);
								memcpy(lncl,&Buffer[i+2],count);//count);
								start=i+2;
								break;
							}

					if(		Buffer[i]==(unsigned char)'/'	//  />"
							&&  Buffer[i+1]==(unsigned char)'>')
							break;
				}
				
				int source_len=strlen(lncl);
				int len=strlen(flcl);
				
				if(source_len)
				if(len)
				if(start)
				if(strcmp(flcl,lncl))
				{
					int f;

									if(source_len==len)
												{
													for( f=0;f<source_len;f++)
														Buffer[start+f]=flcl[f];
													for( f=0;f<source_len;f++)
														if(Buffer[start+f]==0)
														Buffer[start+f]=(char)' ';
												}
									if(source_len>len)
												{
													for( f=start;f<( int)(dwSourceBufferLength-1-(source_len-len)-d_val);f++)
															Buffer[f]=Buffer[f+(source_len-len)-d_val];

													for( f=0;f<len;f++)
														Buffer[start+f]=flcl[f];
													for( f=0;f<len;f++)
														if(Buffer[start+f]==0)
															Buffer[start+f]=(char)' ';
													
													//n_INX_file_size-=(source_len-len);
													
												}
																						
									if(source_len<len)//
												{											
													for( f=(int)(dwSourceBufferLength-1-(d_val+len-source_len));f>( int)start;f--)
														Buffer[f+(d_val+len-source_len)]=Buffer[f];
													for( f=0;f<len;f++)
														Buffer[start+f]=flcl[f];
													for( f=0;f<len;f++)
														if(Buffer[start+f]==0)
															Buffer[start+f]=(char)' ';
												
													//n_INX_file_size+=(d_val+len-source_len);
												}
						//	break;

				}
			
			}

//------remove frame on picture----------------------------------------
*/
		int ok=0;
	//------remove frame on picture----------------------------------------
 int d_val=0;
  
 for(i=0;i<(int)(dwSourceBufferLength-1);i++)//		
		if(		Buffer[i]==(unsigned char)'<' //</mspr>
			&&  Buffer[i+1]==(unsigned char)'m'
			&&  Buffer[i+2]==(unsigned char)'s'
			&&  Buffer[i+3]==(unsigned char)'p'
			&&  Buffer[i+4]==(unsigned char)'r')
			break;

for(i;i<(int)(dwSourceBufferLength-1);i++)//find IGeo="x_19
				
		if(		Buffer[i]==(unsigned char)'<' //<crec
			&&  Buffer[i+1]==(unsigned char)'c'
			&&  Buffer[i+2]==(unsigned char)'r'
			&&  Buffer[i+3]==(unsigned char)'e'
			&&  Buffer[i+4]==(unsigned char)'c')
			{
				int rec=i+4;
				int start=0;
				char flcl[6]="o_ue";
				char flcl2[]=" lncl=\"o_ue\"";
				char  lncl[12];
					memset(lncl,0,12);
					//memset(flcl,0,12);
				int source_len=0;
				int len;


				for(i=rec;i<(int)(n_INX_file_size-10);i++)
				{
					if(		   Buffer[i+1]==(unsigned char)'l' //lncl
							&&  Buffer[i+2]==(unsigned char)'n'
							&&  Buffer[i+3]==(unsigned char)'c'
							&&  Buffer[i+4]==(unsigned char)'l')
							{
								
								int count;
								start=0;
								while(Buffer[i]!='='&&Buffer[i+1]!='"')
									i++;

								count=0;
								while(Buffer[i+2+count]!='"')
									count++;
									
								memset(lncl,0,12);
								memcpy(lncl,&Buffer[i+2],count);//count);
								start=i+2;
								break;
							}

					if(		Buffer[i]==(unsigned char)'/'	//  />"
							&&  Buffer[i+1]==(unsigned char)'>')
						{	
								start=rec+1;
							//	source_len=strlen(lncl);
								  len=strlen(flcl2);
									for(int f=(int)(dwSourceBufferLength-1-(d_val+len-source_len));f>=( int)start;f--)
														Buffer[f+len]=Buffer[f];
													for( f=0;f<len;f++)
														Buffer[start+f]=flcl2[f];
													for( f=0;f<len;f++)
														if(Buffer[start+f]==0)
															Buffer[start+f]=(char)' ';
														ok=1;
														break;
						}

					if(		Buffer[i]==(  char)'o'	//  ovbl="b_t"
							&&  Buffer[i+1]==( char)'v'&&  Buffer[i+2]==( char)'b'&&  Buffer[i+3]==( char)'l')
							break;
				}
				/*
				  source_len=strlen(lncl);
				  len=strlen(flcl);
				
				if(source_len)
				if(len)
				if(start)
				if(strcmp(flcl,lncl))
				{
					int f;

									if(source_len==len)
												{
													for( f=0;f<source_len;f++)
														Buffer[start+f]=flcl[f];
													for( f=0;f<source_len;f++)
														if(Buffer[start+f]==0)
														Buffer[start+f]=(char)' ';
												}
									if(source_len>len)
												{
													for( f=start;f<( int)(dwSourceBufferLength-1-(source_len-len)-d_val);f++)
															Buffer[f]=Buffer[f+(source_len-len)-d_val];

													for( f=0;f<len;f++)
														Buffer[start+f]=flcl[f];
													for( f=0;f<len;f++)
														if(Buffer[start+f]==0)
															Buffer[start+f]=(char)' ';
													
													//n_INX_file_size-=(source_len-len);
													
												}
																						
									if(source_len<len)//
												{											
													for( f=(int)(dwSourceBufferLength-1-(d_val+len-source_len));f>( int)start;f--)
														Buffer[f+(d_val+len-source_len)]=Buffer[f];
													for( f=0;f<len;f++)
														Buffer[start+f]=flcl[f];
													for( f=0;f<len;f++)
														if(Buffer[start+f]==0)
															Buffer[start+f]=(char)' ';
												
													//n_INX_file_size+=(d_val+len-source_len);
												}
									ok=1;
						//	break;

				}*/
			
			}

//------remove frame on picture----------------------------------------
//---------------------------------------------------------
		char track_str[]="trak=\"D"; //trak="D_-25"
		char sprd_str[]="<sprd";
//		int ok=0;

	//skip something
		for(i=0 ;i<(int)(dwSourceBufferLength-10);i++)//		<sprd
			if(Buffer[i]==(char)sprd_str[0])
			if(!memcmp(&Buffer[i],sprd_str,strlen(sprd_str)))
				break;

	for(i ;i<(int)(dwSourceBufferLength-10);i++)
		if(Buffer[i]==(char)track_str[0])
			if(!memcmp(&Buffer[i],track_str,strlen(track_str)))
			{	
				int d=i+strlen(track_str);
					while(Buffer[d]!='"')  d++;
					d++;
				for(int f=i;f<d;f++)
					Buffer[f]=' ';
				
				ok=1; 
			}
//---------------------------------------------------------

	
	if(ok)
	{
		WCHAR new_inx_file[MAX_PATH];
		wcscpy(new_inx_file,inx_file);
		for(int i=wcslen(new_inx_file);i>0;i--)
			if(new_inx_file[i]=='.'&&new_inx_file[i+1]=='i'||new_inx_file[i+1]=='I')
			{
				new_inx_file[i]=0;
				if(ok==1) wcscat(	new_inx_file,L"_clean.inx");
			
				break;
			}

	CopyFile(inx_file,new_inx_file,0);		
	hFile=CreateFileW(new_inx_file, GENERIC_READ|GENERIC_WRITE,
		 FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,OPEN_EXISTING, 0, NULL);
		 if (hFile==INVALID_HANDLE_VALUE)
			{	 
				MessageBoxW(hWnd, L"Cannot open the target File", L"Error", MB_ICONERROR);
				return ;
			}

	//WriteFile(hFile, (LPVOID)Buffer, 	dwSourceBufferLength, &n_INX_file_size, NULL);
		WriteFile(hFile, (LPVOID)Buffer, 	strlen(Buffer) , &n_INX_file_size, NULL);

	CloseHandle(hFile);
	}



delete (Buffer);
}




void InitTableImageList(HWND hTableList)
{
//SHFILEINFO sfi;
//HIMAGELIST hList;
//HICON hIcon;


//	HIMAGELIST hImageList, hImageNormal;
//	hImageNormal = ImageList_Create(32, 32, ILC_COLOR8 | ILC_MASK, (MAX_LISTITEM + 1), 1);
	hImageList = ImageList_Create(16, 16, ILC_COLOR8 | ILC_MASK, (MAX_LISTITEM + 1), 1);
	// /*

//hList = (HIMAGELIST) SHGetFileInfo( "c:\\TPHKLOCK.TXT", 0, &sfi, sizeof(SHFILEINFO), SHGFI_USEFILEATTRIBUTES | SHGFI_SYSICONINDEX | SHGFI_SMALLICON );
/*	hList = (HIMAGELIST) SHGetFileInfo( "c:\\", 0, &sfi, sizeof(SHFILEINFO),
SHGFI_ICON|SHGFI_TYPENAME |SHGFI_DISPLAYNAME|SHGFI_ATTRIBUTES| SHGFI_SYSICONINDEX|SHGFI_SMALLICON );
if(hList)
	for(int i = 0; i <= MAX_LISTITEM; i++)
	{

		ImageList_AddIcon(hImageList, (HICON)sfi.hIcon);	
	}
/*
#define SHGFI_ICON              0x000000100     // get icon
#define SHGFI_DISPLAYNAME       0x000000200     // get display name
#define SHGFI_TYPENAME          0x000000400     // get type name
#define SHGFI_ATTRIBUTES        0x000000800     // get attributes
#define SHGFI_ICONLOCATION      0x000001000     // get icon location
#define SHGFI_EXETYPE           0x000002000     // return exe type
#define SHGFI_SYSICONINDEX      0x000004000     // get system icon index
#define SHGFI_LINKOVERLAY       0x000008000     // put a link overlay on icon
#define SHGFI_SELECTED          0x000010000     // show icon in selected state
#define SHGFI_ATTR_SPECIFIED    0x000020000     // get only specified attributes
#define SHGFI_LARGEICON         0x000000000     // get large icon
#define SHGFI_SMALLICON         0x000000001     // get small icon
#define SHGFI_OPENICON          0x000000002     // get open icon
#define SHGFI_SHELLICONSIZE     0x000000004     // get shell size icon
#define SHGFI_PIDL              0x000000008     // pszPath is a pidl
#define SHGFI_USEFILEATTRIBUTES 0x000000010     // use passed dwFileAttribute
*/	// ico

/*	for(int i = 0; i <= MAX_LISTITEM; i++)
	{
		hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_FACE2-3 + i));
	//	ImageList_AddIcon(hImageNormal, hIcon);
		ImageList_AddIcon(hImageList, hIcon);
		DestroyIcon(hIcon);
	}
	// */


	/*	you can use bitmap too
	// bitmap
	HBITMAP hBmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_LISTIMAGE));
	ImageList_Add(hImageList, hBmp, NULL);
	DeleteObject(hBmp);
	*/
//	SendMessage(hTableList, LVM_SETIMAGELIST, LVSIL_NORMAL, (LPARAM)hImageNormal);
	SendMessage(hTableList, LVM_SETIMAGELIST, LVSIL_SMALL, (LPARAM)hImageList);

}




void DestroyTableImageList()
{

	//	hImageNormal = ImageList_Create(32, 32, ILC_COLOR8 | ILC_MASK, (MAX_LISTITEM + 1), 1);
	ImageList_Destroy(hImageList);
	// /*
}




int Find_Disks()
{
//HIMAGELIST hList;
	//ImageListIconCount=0;

	ZeroMemory(DiskBuffer,sizeof(DiskBuffer));
		if(GetLogicalDriveStringsW(sizeof(DiskBuffer),(WCHAR*)&DiskBuffer))
			{	
						i=0;
						WCHAR VolumeNameBuffer[0x80];
						WCHAR FileSystemNameBuffer[0x80];
						DWORD Serial,MaxPath;
						
						while(DiskBuffer[i][0]>0)
						{
							FileSystemNameBuffer[0]=0;
							VolumeNameBuffer[0]=0;

							GetVolumeInformationW((const WCHAR*)&DiskBuffer[i],VolumeNameBuffer,0x80,
															0,&Serial,&MaxPath,
																FileSystemNameBuffer,0x80);
							
							Result = (HIMAGELIST) SHGetFileInfoW( DiskBuffer[i], 0, &sfi, sizeof(SHFILEINFOW),
							SHGFI_ICON|SHGFI_TYPENAME |SHGFI_DISPLAYNAME|SHGFI_ATTRIBUTES| SHGFI_SYSICONINDEX|SHGFI_SMALLICON );
							if(Result)
							{
								ImageList_AddIcon(hImageList, (HICON)sfi.hIcon);
								pProjectInfo_[item_count].Type=ImageListIconCount;
								wcscpy(TypeName[ImageListIconCount],sfi.szTypeName);
								ImageListIconCount++;
							}

							DiskBuffer[i][2]=0;	
							
							swprintf(s,L"%s  %s " ,DiskBuffer[i],VolumeNameBuffer);
							s[MAX_PATH-1]=0;
							wcscpy(pProjectInfo_[item_count].FileName,s);

							wcscpy(pProjectInfo_[item_count].FileSystem,FileSystemNameBuffer);

							pProjectInfo_[item_count].FileSize=0;

							pProjectInfo_[item_count].Data.Data1int=0;
							pProjectInfo_[item_count].Time.Time1int=0;
							i++;//i+=4;
							item_count++;
							progress+=5; SendMessage( hProgressBar, PBM_SETPOS,  progress, 0);
						}
		}

return item_count;
}


int Find_Directories(WCHAR* file)
{

	HANDLE hFile;
	WIN32_FIND_DATAW findFileData;
	SYSTEMTIME sm;
	FILETIME fm;
	int  i;
	ZeroMemory(&findFileData,sizeof(findFileData));

	hFile=0;
	hFile=FindFirstFileW(file,&findFileData);

	if(hFile!=INVALID_HANDLE_VALUE)
	{
		memset(&sm ,0,sizeof(SYSTEMTIME));
		memset(&sm,0,sizeof(SYSTEMTIME));
		GetLocalTime(&sm );
		FileTimeToLocalFileTime (&findFileData.ftLastWriteTime,&fm);
		FileTimeToSystemTime(&fm,&sm);//GetLocalTime(&sm);
		
		if(findFileData.dwFileAttributes==FILE_ATTRIBUTE_DIRECTORY&&findFileData.cFileName[1]) 
		{
			findFileData.cFileName[MAX_PATH-1]=0;
			wcscpy(pProjectInfo_[item_count].FileName,findFileData.cFileName);
			
		
			pProjectInfo_[item_count].FileSize=findFileData.nFileSizeLow>>10;
		
			if(pProjectInfo_[item_count].FileName[0]=='.'&&pProjectInfo_[item_count].FileName[1]=='.')	
			{
				//pProjectInfo_[item_count].Type=3;
				pProjectInfo_[item_count].FileSize=0;

				pProjectInfo_[item_count].Type=ImageListIconCount;
				wcscpy(TypeName[ImageListIconCount],L"Upper directory");
				ImageList_AddIcon(hImageList,LoadIcon(hInst, MAKEINTRESOURCE(IDI_FACE2))); 
				ImageListIconCount++;						
			}
			else
			{
					Result = (HIMAGELIST) SHGetFileInfoW( pProjectInfo_[item_count].FileName, 0, &sfi, sizeof(SHFILEINFOW),
								SHGFI_ICON|SHGFI_TYPENAME |SHGFI_DISPLAYNAME|SHGFI_ATTRIBUTES| SHGFI_SYSICONINDEX|SHGFI_SMALLICON );
								if(Result)
								{
									ImageList_AddIcon(hImageList, (HICON)sfi.hIcon);
									pProjectInfo_[item_count].Type=ImageListIconCount;
									wcscpy(TypeName[ImageListIconCount],sfi.szTypeName);
									ImageListIconCount++;
								}
			}
		
			pProjectInfo_[item_count].Data.Data2char1short.day=(unsigned char)sm.wDay;
			pProjectInfo_[item_count].Data.Data2char1short.month=(unsigned char)sm.wMonth; 
			pProjectInfo_[item_count].Data.Data2char1short.year=(unsigned short)sm.wYear;
			pProjectInfo_[item_count].Time.Time4char.Hour=(unsigned char)sm.wHour;
			pProjectInfo_[item_count].Time.Time4char.Minute=(unsigned char)sm.wMinute; 
			pProjectInfo_[item_count].Time.Time4char.Second=(unsigned char)sm.wSecond;
			item_count+=2;
		}


		while(FindNextFileW(hFile,&findFileData)&&item_count<MAX_PROJECT_COUNT-1)
		{
			memset(&sm ,0,sizeof(SYSTEMTIME));
			memset(&sm,0,sizeof(SYSTEMTIME));
			GetLocalTime(&sm);
			FileTimeToLocalFileTime (&findFileData.ftLastWriteTime,&fm);
			FileTimeToSystemTime(&fm,&sm);//GetLocalTime(&sm);

			if(findFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) 
			{
				findFileData.cFileName[MAX_PATH-1]=0;
				wcscpy(pProjectInfo_[item_count].FileName,findFileData.cFileName);
				if(pProjectInfo_[item_count].FileName[0]=='.'&&pProjectInfo_[item_count].FileName[1]=='.')
				{
					for(i=2;i<12;i++) pProjectInfo_[item_count].FileName[i]=' ';
					pProjectInfo_[item_count].FileName[12]=0;
				}

				//pProjectInfo_[item_count].FileSize=findFileData.nFileSizeLow>>10;
				pProjectInfo_[item_count].FileSize=0;
				if(pProjectInfo_[item_count].FileName[0]=='.'&&pProjectInfo_[item_count].FileName[1]=='.')	
				{
					//pProjectInfo_[item_count].Type=3;
					pProjectInfo_[item_count].Type=ImageListIconCount;
					wcscpy(TypeName[ImageListIconCount],L"Upper directory");
					ImageList_AddIcon(hImageList,LoadIcon(hInst, MAKEINTRESOURCE(IDI_FACE2))); 
					ImageListIconCount++;						
				}
				else
				{	
					wcscpy(s,CurrentDirectory);wcscat(s,L"\\");
					wcscat(s,pProjectInfo_[item_count].FileName);

						Result = (HIMAGELIST) SHGetFileInfoW( s, 0, &sfi, sizeof(SHFILEINFO),
									SHGFI_ICON|SHGFI_TYPENAME | SHGFI_SYSICONINDEX|SHGFI_SMALLICON );
									if(Result)
									{
										ImageList_AddIcon(hImageList, (HICON)sfi.hIcon);
										pProjectInfo_[item_count].Type=ImageListIconCount;
										wcscpy(TypeName[ImageListIconCount],sfi.szTypeName);
									ImageListIconCount++;
									}
								/*	else
									{
										pProjectInfo_[item_count].Type=ImageListIconCount;
										wcscpy(TypeName[ImageListIconCount],L"Unknown directory");
										ImageList_AddIcon(hImageList,LoadIcon(hInst, MAKEINTRESOURCE(IDI_FACE2+1))); 
										ImageListIconCount++;
									}*/

				}


			
				pProjectInfo_[item_count].Data.Data2char1short.day=(unsigned char)sm.wDay;
				pProjectInfo_[item_count].Data.Data2char1short.month=(unsigned char)sm.wMonth; 
				pProjectInfo_[item_count].Data.Data2char1short.year=(unsigned short)sm.wYear;
				pProjectInfo_[item_count].Time.Time4char.Hour=(unsigned char)sm.wHour;
				pProjectInfo_[item_count].Time.Time4char.Minute=(unsigned char)sm.wMinute; 
				pProjectInfo_[item_count].Time.Time4char.Second=(unsigned char)sm.wSecond;
				item_count++;
				progress+=2; SendMessage( hProgressBar, PBM_SETPOS,  progress, 0);
			}
		}
	FindClose(hFile);
	}

return item_count;
}




int Find_Files(WCHAR* file)
{
	//item_count=0;
	HANDLE hFile;
	WIN32_FIND_DATAW findFileData;
	SYSTEMTIME sm;
	FILETIME fm;

	ZeroMemory(&findFileData,sizeof(findFileData));

	hFile=0;
	hFile=FindFirstFileW(file,&findFileData);

	if(hFile!=INVALID_HANDLE_VALUE)
	{
		memset(&sm ,0,sizeof(SYSTEMTIME));
		memset(&sm,0,sizeof(SYSTEMTIME));
		GetLocalTime(&sm );
		FileTimeToLocalFileTime (&findFileData.ftLastWriteTime,&fm);
		FileTimeToSystemTime(&fm,&sm);//GetLocalTime(&sm);
		
		if(!(findFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
		{
			wcscpy(pProjectInfo_[item_count].FileName,findFileData.cFileName);
			pProjectInfo_[item_count].Type=5;

			wcscpy(s,CurrentDirectory);wcscat(s,L"\\");
			wcscat(s,pProjectInfo_[item_count].FileName);
				
			Result = (HIMAGELIST) SHGetFileInfoW(s, 0, &sfi, sizeof(SHFILEINFOW),
						SHGFI_ICON|SHGFI_TYPENAME |SHGFI_DISPLAYNAME|SHGFI_ATTRIBUTES| SHGFI_SYSICONINDEX|SHGFI_SMALLICON );
						if(Result)
							{
								ImageList_AddIcon(hImageList, (HICON)sfi.hIcon);
								pProjectInfo_[item_count].Type=ImageListIconCount;
								wcscpy(TypeName[ImageListIconCount],sfi.szTypeName);
								ImageListIconCount++;
							}
			pProjectInfo_[item_count].FileSize=findFileData.nFileSizeLow;//>>10;
			pProjectInfo_[item_count].Data.Data2char1short.day=(unsigned char)sm.wDay;
			pProjectInfo_[item_count].Data.Data2char1short.month=(unsigned char)sm.wMonth; 
			pProjectInfo_[item_count].Data.Data2char1short.year=(unsigned short)sm.wYear;
			pProjectInfo_[item_count].Time.Time4char.Hour=(unsigned char)sm.wHour;
			pProjectInfo_[item_count].Time.Time4char.Minute=(unsigned char)sm.wMinute; 
			pProjectInfo_[item_count].Time.Time4char.Second=(unsigned char)sm.wSecond;
			item_count++;
		}


		while(FindNextFileW(hFile,&findFileData)&&item_count<MAX_PROJECT_COUNT-1)
		{
			memset(&sm ,0,sizeof(SYSTEMTIME));
			memset(&sm,0,sizeof(SYSTEMTIME));
			GetLocalTime(&sm );
			FileTimeToLocalFileTime (&findFileData.ftLastWriteTime,&fm);
			FileTimeToSystemTime(&fm,&sm);//GetLocalTime(&sm);

			if(!(findFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
				{
					wcscpy(pProjectInfo_[item_count].FileName,findFileData.cFileName);
					
					pProjectInfo_[item_count].Type=5;

					//if(findFileData.cFileName[0]=='t')
					//	i=i;

					wcscpy(s,CurrentDirectory);wcscat(s,L"\\");
					wcscat(s,pProjectInfo_[item_count].FileName);
					Result = (HIMAGELIST) SHGetFileInfoW(s, 0, &sfi, sizeof(SHFILEINFOW),
									SHGFI_ICON|SHGFI_TYPENAME |SHGFI_DISPLAYNAME|SHGFI_ATTRIBUTES| SHGFI_SYSICONINDEX|SHGFI_SMALLICON );
									if(Result)
									{
										

										for(int d=0;d<ImageListIconCount;d++)
										if(!wcscmp(TypeName[d],sfi.szTypeName))
										{	
											pProjectInfo_[item_count].Type=d;
											break;
										}
										if(d>=ImageListIconCount)
										{
											pProjectInfo_[item_count].Type=ImageListIconCount;
											wcscpy(TypeName[ImageListIconCount],sfi.szTypeName);
											ImageListIconCount++;ImageList_AddIcon(hImageList, (HICON)sfi.hIcon);///!!!!!
										}
									}
					pProjectInfo_[item_count].FileSize=0;
					pProjectInfo_[item_count].FileSize=findFileData.nFileSizeLow;//>>10;
					pProjectInfo_[item_count].Data.Data2char1short.day=(unsigned char)sm.wDay;
					pProjectInfo_[item_count].Data.Data2char1short.month=(unsigned char)sm.wMonth; 
					pProjectInfo_[item_count].Data.Data2char1short.year=(unsigned short)sm.wYear;
					pProjectInfo_[item_count].Time.Time4char.Hour=(unsigned char)sm.wHour;
					pProjectInfo_[item_count].Time.Time4char.Minute=(unsigned char)sm.wMinute; 
					pProjectInfo_[item_count].Time.Time4char.Second=(unsigned char)sm.wSecond;
					item_count++;
					progress++; SendMessage( hProgressBar, PBM_SETPOS,  progress, 0);
				}
		}

	FindClose(hFile);

	}
	 //else
	//Show_error();

return item_count;
}






void FillListView()
{
//#define UNICODE
	int i,k;
WCHAR inx_file[MAX_PATH];
	memset(&lvItem, 0, sizeof(lvItem)); 

	lvItem.mask =LVCF_WIDTH | LVIF_TEXT |LVCF_IMAGE ;  

	for(i=0;i<item_count;i++)
			//for(i=item_count-1;i>=0;i--)
					{	
							//lvItem.mask =LVCF_IMAGE ; 
							lvItem.iImage = pProjectInfo_[	lv_num[i]].Type; 
							lvItem.iItem=i;
							lvItem.iSubItem = 0;
							index = ListView_InsertItem(hListView, &lvItem);
							
							k=0;
						
						/*	swprintf(s,L"%s",pProjectInfo_[	lv_num[i]].FileName);
								memset(ss,0,512);
								WideCharToMultiByte(CP_ACP, 0, s, wcslen(s),ss,512, NULL, NULL);
						
							ListView_SetItemText(hListView, index, k, ss);*/
							ListView_SetItemTextW(hListView, index, k, pProjectInfo_[	lv_num[i]].FileName);
						
							//SendMessageW(hListView, index, k, (LPARAM)s); 
							k++;					

							if(pProjectInfo_[	lv_num[i]].FileSize>1)
							{
								if(pProjectInfo_[	lv_num[i]].FileSize>1000)
									swprintf(s,L"%d KB",pProjectInfo_[	lv_num[i]].FileSize/1000);
								else
									swprintf(s,L"1 KB");
								ListView_SetItemTextW(hListView, index, k, s); 
								/*memset(ss,0,512);
								WideCharToMultiByte(CP_ACP, 0, s, wcslen(s),ss,512, NULL, NULL);
								ListView_SetItemText(hListView, index, k, ss); */
								//SendMessageW(hListView, index, k, (LPARAM)s); 
							}
							
							k++;

			

							/*swprintf(s,L"%s",TypeName[pProjectInfo_[	lv_num[i]].Type]);
							memset(ss,0,512);
							WideCharToMultiByte(CP_ACP, 0, s, wcslen(s),ss,512, NULL, NULL);
								
							ListView_SetItemText(hListView, index, k, ss); */
							ListView_SetItemTextW(hListView, index, k, TypeName[pProjectInfo_[	lv_num[i]].Type]); 
							 
							//if(TypeName[pProjectInfo_[	lv_num[i]].Type][0] =='I'&&TypeName[pProjectInfo_[	lv_num[i]].Type][1]=='n'&& TypeName[pProjectInfo_[	lv_num[i]].Type][2]=='D')
							//	i=i;
							
							if((TypeName[pProjectInfo_[	lv_num[i]].Type][0] =='I'&&TypeName[pProjectInfo_[	lv_num[i]].Type][1]=='N'&& TypeName[pProjectInfo_[	lv_num[i]].Type][2]=='X')//inx
							||(TypeName[pProjectInfo_[	lv_num[i]].Type][0] =='i'&&TypeName[pProjectInfo_[	lv_num[i]].Type][1]=='n'&& TypeName[pProjectInfo_[	lv_num[i]].Type][2]=='x')//inx
							||(TypeName[pProjectInfo_[	lv_num[i]].Type][0] =='I'&&TypeName[pProjectInfo_[	lv_num[i]].Type][1]=='n'&& TypeName[pProjectInfo_[	lv_num[i]].Type][2]=='D'&&TypeName[pProjectInfo_[	lv_num[i]].Type][0x9]=='X'&&TypeName[pProjectInfo_[	lv_num[i]].Type][0xA]=='M'&&TypeName[pProjectInfo_[	lv_num[i]].Type][0xB]=='L'))//InDesing XML Interchange
							{
								wcscpy(inx_file,CurrentDirectory);
								wcscat(inx_file,L"\\");
								wcscat(inx_file,pProjectInfo_[	lv_num[i]].FileName);
								int ver=CheckINXversion(inx_file);
								switch(ver)
								{
									case 4:	swprintf(s,L"CS4 %s ",TypeName[pProjectInfo_[	lv_num[i]].Type]); break;
									case 3:	swprintf(s,L"CS3 %s ",TypeName[pProjectInfo_[	lv_num[i]].Type]); break;
									case 2:	swprintf(s,L"CS2 %s ",TypeName[pProjectInfo_[	lv_num[i]].Type]); break;
									case 1:	swprintf(s,L"CS1 %s ",TypeName[pProjectInfo_[	lv_num[i]].Type]); break;
									default:swprintf(s,L"CSX %s ",TypeName[pProjectInfo_[	lv_num[i]].Type]); break;
								}
								ListView_SetItemTextW(hListView, index, k, s); 
							}
							else
							if((TypeName[pProjectInfo_[	lv_num[i]].Type][0] =='I'&&TypeName[pProjectInfo_[	lv_num[i]].Type][1]=='N'&&TypeName[pProjectInfo_[	lv_num[i]].Type][2]=='D'&&TypeName[pProjectInfo_[	lv_num[i]].Type][3]=='D')//indd
							||(TypeName[pProjectInfo_[	lv_num[i]].Type][0] =='i'&&TypeName[pProjectInfo_[	lv_num[i]].Type][1]=='n'&&TypeName[pProjectInfo_[	lv_num[i]].Type][2]=='d'&&TypeName[pProjectInfo_[	lv_num[i]].Type][3]=='d')//indd
							||(TypeName[pProjectInfo_[	lv_num[i]].Type][0] =='I'&&TypeName[pProjectInfo_[	lv_num[i]].Type][1]=='n'&&TypeName[pProjectInfo_[	lv_num[i]].Type][2]=='D'&&TypeName[pProjectInfo_[	lv_num[i]].Type][9]=='D'&&TypeName[pProjectInfo_[	lv_num[i]].Type][0xA]=='o'))//InDesing Document
							{
								wcscpy(inx_file,CurrentDirectory);
								wcscat(inx_file,L"\\");
								wcscat(inx_file,pProjectInfo_[	lv_num[i]].FileName);
								int ver=CheckINDDversion(inx_file);
								switch(ver)
								{
									case 6:	swprintf(s,L"CS6 %s ",TypeName[pProjectInfo_[	lv_num[i]].Type]); break;
									case 5:	swprintf(s,L"CS5 %s ",TypeName[pProjectInfo_[	lv_num[i]].Type]); break;
									case 4:	swprintf(s,L"CS4 %s ",TypeName[pProjectInfo_[	lv_num[i]].Type]); break;
									case 3:	swprintf(s,L"CS3 %s ",TypeName[pProjectInfo_[	lv_num[i]].Type]); break;
									case 2:	swprintf(s,L"CS2 %s ",TypeName[pProjectInfo_[	lv_num[i]].Type]); break;
									default:swprintf(s,L"CSX %s ",TypeName[pProjectInfo_[	lv_num[i]].Type]); break;
								}

								ListView_SetItemTextW(hListView, index, k, s); 
							}
							else
								ListView_SetItemTextW(hListView, index, k, TypeName[pProjectInfo_[	lv_num[i]].Type]); 
									
							//SendMessageW(hListView, index, k, (LPARAM)s); 
							k++;
						 
									
							if(pProjectInfo_[lv_num[i]].Data.Data1int)
							{
								memset(&month,0,3*2);	memset(&day,0,3*2);	memset(&hour,0,3*2);	memset(&minute,0,3*2);	memset(&second,0,3);									
								if(pProjectInfo_[lv_num[i]].Data.Data2char1short.month<10) {month[0]='0'; wsprintf(&month[1], L"%d",pProjectInfo_[lv_num[i]].Data.Data2char1short.month);}else wsprintf(month,	L"%d",pProjectInfo_[lv_num[i]].Data.Data2char1short.month);
								if(pProjectInfo_[lv_num[i]].Data.Data2char1short.day<10)   {day[0]='0';   wsprintf(&day[1],   L"%d",pProjectInfo_[lv_num[i]].Data.Data2char1short.day);   } else wsprintf(day,	L"%d",pProjectInfo_[lv_num[i]].Data.Data2char1short.day);
								if(pProjectInfo_[lv_num[i]].Data.Data2char1short.month<10) {month[0]='0'; wsprintf(&month[1], L"%d",pProjectInfo_[lv_num[i]].Data.Data2char1short.month);}else wsprintf(month,	L"%d",pProjectInfo_[lv_num[i]].Data.Data2char1short.month);
								if(pProjectInfo_[lv_num[i]].Time.Time4char.Hour<10)			{hour[0]='0';  wsprintf(&hour[1],  L"%d",pProjectInfo_[lv_num[i]].Time.Time4char.Hour);  }		else wsprintf(hour,	L"%d",	pProjectInfo_[lv_num[i]].Time.Time4char.Hour);
								if(pProjectInfo_[lv_num[i]].Time.Time4char.Minute<10)		{minute[0]='0';wsprintf(&minute[1],L"%d",pProjectInfo_[lv_num[i]].Time.Time4char.Minute);}		else wsprintf(minute,L"%d",	pProjectInfo_[lv_num[i]].Time.Time4char.Minute);
								if(pProjectInfo_[lv_num[i]].Time.Time4char.Second<10)		{second[0]='0';wsprintf(&second[1],L"%d",pProjectInfo_[lv_num[i]].Time.Time4char.Second);}		else wsprintf(second,L"%d",	pProjectInfo_[lv_num[i]].Time.Time4char.Second);												
								//wsprintf(ss,L"%s.%s.%d  %s:%s",day,month,pProjectInfo_[lv_num[i]].Data.Data2char1short.year,hour,minute); 									
								//ListView_SetItemText(hListView, index, k, ss);
								swprintf(s,L"%s.%s.%d  %s:%s",day,month,pProjectInfo_[lv_num[i]].Data.Data2char1short.year,hour,minute); 									
								ListView_SetItemTextW(hListView, index, k, s);
								//memset(ss,0,512);
								//WideCharToMultiByte(CP_ACP, 0, s, wcslen(s),ss,512, NULL, NULL);
								//ListView_SetItemText(hListView, index, k, ss); 
								//SendMessage(hListView, index, k, (LPARAM)(LV_ITEM FAR *)s); 
							}
							else
							//	ListView_SetItemText(hListView, index, k,pProjectInfo_[lv_num[i]].FileSystem);
								SendMessage((hListView), index, (WPARAM)k, (LPARAM)(LV_ITEMW FAR *)pProjectInfo_[lv_num[i]].FileSystem);

							k++;
					}

}



void Fill_lists()
{
//	int progress;
	item_count=0;
	WCHAR mask[16];
	dir_count=0;
	disk_count=0;
	file_count=0;

	ShowWindow(hProgressBar, SW_SHOWNORMAL);
	progress=0;
	progress+=5; SendMessage( hProgressBar, PBM_SETPOS,  progress, 0);

	if(hImageList)
		DestroyTableImageList();

	InitTableImageList(hListView);

	progress+=10; SendMessage( hProgressBar, PBM_SETPOS,  progress, 0);

	GetWindowTextW(hExtension,mask,16);

	ImageListIconCount=0;
		
	wcscpy(s,CurrentDirectory);

	if(s[0]=='\\')
	{	disk_count=item_count=	Find_Disks();
		if(progress<50) {progress=50; SendMessage( hProgressBar, PBM_SETPOS,  progress, 0);}

	}
	else
	{
		wcscat(s,L"\\*.*");
		dir_count=item_count=	Find_Directories(s);

		if(progress<30) {progress+=20; SendMessage( hProgressBar, PBM_SETPOS,  progress, 0);}

		wcscpy(s,CurrentDirectory);
		wcscat(s,L"\\");
		wcscat(s,mask);//wcscat(s,L"*.PDF");//
				
		file_count=item_count=	Find_Files(s);
		
		if(progress<60) {progress+=20; SendMessage( hProgressBar, PBM_SETPOS,  progress, 0);}
	}

		if(item_count)
					for(i=0;i<item_count;i++)
				//	for(i=item_count-1;i>=0;i--)
					lv_num[i]=i;			
				
		if(hListView)
			{
					ListView_DeleteAllItems(hListView);
					if(item_count)	FillListView();				
			}
		else
			{	
				SendMessage( (HWND)(HANDLE)hList, LB_RESETCONTENT, 0,0);
				for(i=0;i<item_count;i++)
				//for(i=item_count-1;i>=0;i--)
				SendMessageW( (HWND)(HANDLE)hList, LB_ADDSTRING, (WPARAM)-1,
							(LPARAM)(LPCSTR)pProjectInfo_[	lv_num[i]].FileName);
			}
	
	 if(progress<99)
	 {	
		 progress++;
		 SendMessage( hProgressBar, PBM_SETPOS,  progress, 0);
		 Sleep(1);
	 }

	Sort_list();
	
	ShowWindow(hProgressBar, SW_HIDE);

	if(hListView)
		InvalidateRect( hListView, NULL, TRUE );  
	else
		InvalidateRect( hList, NULL, TRUE ); 


	progress=0;
//	ShowWindow(hProgressBar, SW_HIDE);
	selected_item=-1;
}





void SelectFolder()
{
	
	LPITEMIDLIST lpIID;
	BROWSEINFOW bi;
	WCHAR szName[MAX_PATH];

	ZeroMemory(&bi,sizeof(BROWSEINFO));
	WCHAR Dir[MAX_PATH];

	LPITEMIDLIST pidlRoot = NULL;
	LPSHELLFOLDER pDesktopFolder = NULL;
	SHGetDesktopFolder(&pDesktopFolder);

	if ( pDesktopFolder != NULL )
	{
	
	OLECHAR olePath[MAX_PATH];
	ULONG chEaten;
	ULONG dwAttributes;
	HRESULT hr;

	//GetCurrentDirectory(MAX_PATH, szPath);
	//GetWindowText(hDirectory,CurrentDirectory,MAX_PATH);
	//if(CurrentDirectory[3]==' ')CurrentDirectory[3]=0;
	//MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, CurrentDirectory, -1, olePath, MAX_PATH);
	wcscpy (olePath,CurrentDirectory);

	// Convert the path to an ITEMIDLIST.
	hr = pDesktopFolder->ParseDisplayName(NULL,
	NULL,
	olePath,
	&chEaten,
	&pidlRoot,
	&dwAttributes);

	pDesktopFolder->Release();

	}

	bi.pidlRoot = pidlRoot;

	bi.hwndOwner = hWnd;
	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	bi.lpszTitle = L"Select Folder";
	bi.pszDisplayName = &Dir[0];

	lpIID = SHBrowseForFolderW(&bi);
	if(lpIID)
	{
		SHGetPathFromIDListW (lpIID, szName );
		wcscpy(CurrentDirectory , szName);
		SetWindowTextW(hDirectory,CurrentDirectory);
	}
}



void DeleteFile_()
{
	WCHAR source_file[MAX_PATH];	

	if(SelectedItemCount==1)
		selected_items[0]=SelectedItem;

		for(j=0;j<SelectedItemCount;j++)
		{		
			//GetCurrentDirectory(sizeof(CurrentDirectory),CurrentDirectory);
	//		GetWindowText(hDirectory,CurrentDirectory,MAX_PATH);
			wcscpy(source_file,CurrentDirectory);
			wcscat(source_file,L"\\");
			wcscat(source_file,pProjectInfo_[selected_items[j]].FileName);

			if(!DeleteFileW(source_file))
				MessageBoxW(hWnd, L"Cannot delete file.\n File(s)in use by another application.", L"Warning", MB_ICONWARNING);	

		}
}



void ChangePDFViewProperties2()
{


	int PageMode=2;

	WCHAR suffix[64];
	WCHAR target_file[MAX_PATH];							
	WCHAR source_file[MAX_PATH];	
//	WCHAR password[64];
	WCHAR cmd[MAX_PATH*2];

	memset(cmd,0,MAX_PATH*2);
	memset(target_file,0,MAX_PATH);	
//	memset(password,0,64);
	GetWindowTextW(hSuffix,suffix,64);

	memset(cmd,0,MAX_PATH);
	
	ShowWindow(hProgressBar, SW_SHOWNORMAL);
		
		
	if(SelectedItemCount==1)
		selected_items[0]=SelectedItem;

		for(j=0;j<SelectedItemCount;j++)
		{		
			progress=1;SendMessage( hProgressBar, PBM_SETPOS,  progress, 0);
			
			wcscpy(source_file,CurrentDirectory);
			wcscat(source_file,L"\\");
			wcscat(source_file,pProjectInfo_[selected_items[j]].FileName);
						
			wcscpy(target_file,source_file);
			
			index=0;
			int len=wcslen(target_file);
			for(i=0;i<len;i++)
				if(target_file[i]==(WCHAR)'.')
					index=i;

			if(!wcscmp(suffix,L"auto")||cmd_use==1)
			{
				if(target_file[index-3]=='l'&&target_file[index-2]=='o'&&target_file[index-1]=='w'||
				   target_file[index-3]=='L'&&target_file[index-2]=='O'&&target_file[index-1]=='W')
				{
					//memset(&target_file[index-3],0,4);
					//wcscat(target_file,L"WEB_only");
					//wcscat(target_file,L".pdf");
					PageMode=2;

				}
				else
				if(target_file[index-5]=='p'&&target_file[index-4]=='r'&&target_file[index-3]=='i'&&target_file[index-2]=='n'&&target_file[index-1]=='t'||
				   target_file[index-5]=='P'&&target_file[index-4]=='R'&&target_file[index-3]=='I'&&target_file[index-2]=='N'&&target_file[index-1]=='T')
				{
				
				//	memset(&target_file[index-5],0,4);
					//wcscat(target_file,L"PRINT_only");
					//wcscat(target_file,L".pdf");
					PageMode=3;

				}
				else
				if(target_file[index-4]=='h'&&target_file[index-3]=='i'&&target_file[index-2]=='g'&&target_file[index-1]=='h'||
				   target_file[index-4]=='H'&&target_file[index-3]=='I'&&target_file[index-2]=='G'&&target_file[index-1]=='H')
				{
					//memset(&target_file[index-4],0,4);
					//wcscat(target_file,L"DVD_only");
					//wcscat(target_file,L".pdf");
					PageMode=2;
				}
				else
				{
					//memset(&target_file[index],0,8);
					//wcscat(&target_file[index],L"_locked");
					//wcscat(&target_file[index],L".pdf");
					PageMode=2;
				}
			}
			else
			{
				//memset(&target_file[index],0,4);
				//wcscat(&target_file[index],suffix);
				//wcscat(&target_file[index],L".pdf");
				PageMode=2;
			}




			
			//BeCyPDFMetaEdit.com bb.pdf -PL 1 -PM 2 -Pm1 -e 3 -o "password" -p "pPX"
						
			swprintf(cmd,L"%s -PL 1 -PM %d -Pm1 -M auto -q",target_file,PageMode);//-T \"Title\"
			//swprintf(cmd,L"%s -PL 1 -PM %d
			//-FitWindow
			//wsprintf(cmd,"%s -PL 1 -PM 2 -Pm1 -Creator \"PDF_locker\"-q",target_file);


			SHELLEXECUTEINFOW shi;
			memset((LPVOID)&shi,0x00,sizeof(SHELLEXECUTEINFO));
			shi.cbSize = sizeof(SHELLEXECUTEINFOW);
			shi.lpFile = L"BeCyPDFMetaEdit.exe";
			shi.lpParameters = cmd;
			shi.nShow = SW_HIDE;	//shi.nShow = SW_SHOWNORMAL; //
			shi.fMask = SEE_MASK_NOCLOSEPROCESS;
			shi.lpDirectory = ProgramPatch;
			if( !ShellExecuteExW(&shi) )
				return ;
			 //hProcess = GetCurrentProcess()
			SetPriorityClass (shi.hProcess, HIGH_PRIORITY_CLASS);//IDLE_PRIORITY_CLASS )
			#define NORMAL_PRIORITY_CLASS             0x00000020
			#define IDLE_PRIORITY_CLASS               0x00000040
			#define HIGH_PRIORITY_CLASS               0x00000080
			#define REALTIME_PRIORITY_CLASS           0x00000100
			
			ShowWindow(hProgressBar, SW_SHOWNORMAL);
			
				progress=0;

			while(WaitForSingleObject( shi.hProcess, 5 )!=WAIT_OBJECT_0)
			{		if(progress<98)progress+=1;
					SendMessage( hProgressBar, PBM_SETPOS,  progress, 0);
					Sleep(5);
			}
			progress=99;SendMessage( hProgressBar, PBM_SETPOS,  progress, 0);
		}

	progress=0;
	ShowWindow(hProgressBar, SW_HIDE);
}




void ChangePDFViewProperties()
{

WCHAR source_file[MAX_PATH];	

	if(SelectedItemCount==1)
		selected_items[0]=SelectedItem;

	if(LoadVeryWDll())
	{
		for(j=0;j<SelectedItemCount;j++)
		{		
			wcscpy(source_file,CurrentDirectory);
			wcscat(source_file,L"\\");
			wcscat(source_file,pProjectInfo_[selected_items[j]].FileName);
			
			SetPDFViewProperties_(source_file);	
		}

		FreeVeryWDll();
	}	
	else
	{
		MessageBoxW(hWnd, L"Cannot find library function to change properties", L"Warning", MB_ICONWARNING);						
		return ;
	}		
}




void OpenFile()
{
WCHAR source_file[MAX_PATH];

	if(SelectedItemCount==1)
		selected_items[0]=SelectedItem;

	for(j=0;j<SelectedItemCount;j++)
	{		
		//GetCurrentDirectory(sizeof(CurrentDirectory),CurrentDirectory);
//		GetWindowText(hDirectory,CurrentDirectory,MAX_PATH);
		wcscpy(source_file,CurrentDirectory);
		wcscat(source_file,L"\\");
		wcscat(source_file,pProjectInfo_[selected_items[j]].FileName);
		
		ShellExecuteW(NULL,L"open",source_file,NULL,NULL,SW_SHOWNORMAL);
	}
}



void RepairFile()
{

WCHAR target_file[MAX_PATH];							
WCHAR source_file[MAX_PATH];	
//char password[64];
WCHAR cmd[MAX_PATH];

	memset(cmd,0,MAX_PATH);
	memset(target_file,0,MAX_PATH);	
//	memset(password,0,64);
//	GetWindowText(hSuffix,suffix,64);
	
	ShowWindow(hProgressBar, SW_SHOWNORMAL);
		
	if(SelectedItemCount==1)
		selected_items[0]=SelectedItem;

		for(j=0;j<SelectedItemCount;j++)
		{		
			progress=1;SendMessage( hProgressBar, PBM_SETPOS,  progress, 0);
			
			wcscpy(source_file,CurrentDirectory);
			wcscat(source_file,L"\\");
			wcscat(source_file,pProjectInfo_[selected_items[j]].FileName);
					
			wcscpy(target_file,source_file);

			index=0;

			int len=wcslen(target_file);
			for(i=0;i<len;i++)
				if(target_file[i]==(WCHAR)'.')
					index=i;
			
			memset(&target_file[index],0,4);
			wcscat(&target_file[index],L"_repaired");
			wcscat(&target_file[index],L".pdf");
		

		//CopyFile(target_file,source_file,0);
		CopyFileW(source_file,target_file,0);
		//BeCyPDFMetaEdit.com bb.pdf -PL 1 -PM 2 -Pm1 -e 3 -o "password" -p "pPX"
			
			//wsprintf(cmd,"%s -PL 1 -PM 2 -Pm1 -q",source_file);
		swprintf(cmd,L"%s -r -q",target_file);
		// -T \"Title\"

		SHELLEXECUTEINFOW shi;
		memset((LPVOID)&shi,0x00,sizeof(SHELLEXECUTEINFO));
		shi.cbSize = sizeof(SHELLEXECUTEINFO);
		shi.lpFile = L"BeCyPDFMetaEdit.com";
		shi.lpParameters = cmd;
		shi.nShow = SW_HIDE;	//shi.nShow = SW_SHOWNORMAL; //
		shi.fMask = SEE_MASK_NOCLOSEPROCESS;
		shi.lpDirectory = ProgramPatch;
		if( !ShellExecuteExW(&shi) )
			return ;
		 //hProcess = GetCurrentProcess()
		SetPriorityClass (shi.hProcess, HIGH_PRIORITY_CLASS);//IDLE_PRIORITY_CLASS )
		#define NORMAL_PRIORITY_CLASS             0x00000020
		#define IDLE_PRIORITY_CLASS               0x00000040
		#define HIGH_PRIORITY_CLASS               0x00000080
		#define REALTIME_PRIORITY_CLASS           0x00000100
		
		ShowWindow(hProgressBar, SW_SHOWNORMAL);
		
		//if(progress!=50)
			progress=0;

		while(WaitForSingleObject( shi.hProcess, 5 )!=WAIT_OBJECT_0)
		{		if(progress<98)progress+=1;
				SendMessage( hProgressBar, PBM_SETPOS,  progress, 0);
				Sleep(5);
		}
		progress=99;SendMessage( hProgressBar, PBM_SETPOS,  progress, 0);
		

		}

	progress=0;
	ShowWindow(hProgressBar, SW_HIDE);
	Fill_lists();
}





void Collect()
{

	if(SelectedItemCount==1)
				selected_items[0]=SelectedItem;

				WCHAR source_file[MAX_PATH*2];

				wcscpy(source_file,CurrentDirectory);
				wcscat(source_file,L"\\");
				int coint=0; int temp=0;

				WCHAR  file[MAX_PATH*2];
				memset(file,0, MAX_PATH*2);

				WCHAR s[500];

				for(j=0;j<SelectedItemCount;j++)
				{
					int len=wcslen(pProjectInfo_[selected_items[j]].FileName );
					  
					for(int e=0;e<len;e++)
					if(pProjectInfo_[selected_items[j]].FileName[e]>='a'&&pProjectInfo_[selected_items[j]].FileName[e]<='z') 
					 pProjectInfo_[selected_items[j]].FileName[e]-='a'-'A';
				}


				//sort by alphavit
				for(j=0;j<SelectedItemCount;j++)
				{

					memset(s,0,500);
					index=-1;
					wcscpy(s, pProjectInfo_[selected_items[j]].FileName);
				
					for(int i=j+1;i<SelectedItemCount;i++)
					{
						if(pProjectInfo_[selected_items[i]].FileName[0]<s[0])
							{
								wcscpy(s,pProjectInfo_[selected_items[i]].FileName);
								index=i;
							}
						else
						if(pProjectInfo_[selected_items[i]].FileName[0]==s[0])
						if(pProjectInfo_[selected_items[i]].FileName[1]<s[1])
							{
								wcscpy(s,pProjectInfo_[selected_items[i]].FileName);
								index=i;
							}
				  							
					}
						if(index!=-1)
							{
								temp=selected_items[j];
								selected_items[j]=selected_items[index];	
								selected_items[index]=temp;
							}
				}

				//put English first
				for(j=0;j<SelectedItemCount;j++)
				{
					if(pProjectInfo_[selected_items[j]].FileName[0]==L'E'&&
						pProjectInfo_[selected_items[j]].FileName[1]<=L'9')
						{
							temp=selected_items[j];
							selected_items[j]=selected_items[0];	
							selected_items[0]=temp;
						}
				}

												
				for(j=0;j<SelectedItemCount;j++)
				{
					if(j)	
						{
							file[coint]=L'-';
							coint++; 
						}

					for(int i=0;i<100;i++)

					if(pProjectInfo_[selected_items[j]].FileName[i]>'9')
					{
						file[coint]=pProjectInfo_[selected_items[j]].FileName[i];
						coint++;
					}
					else
						break;											
				}

					int len=wcslen(file);
					for(int e=0;e<len;e++)
					if(file[e]>='a'&&file[e]<='z') 
					 file[e]-='a'-'A';
											
					wcscat(source_file,file);
					wcscat(source_file,L".tdd");

					HANDLE hfz = CreateFile(source_file,GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
 
					CloseHandle(hfz);
}




void LockFile2(WCHAR* source_file, int mode)
{
	//  -l "E:\E10923_PL-N12_QSG_d.pdf"

	int sourse_size=CheckFileSize(source_file);
	int PageMode=2;
	WCHAR MCN[16];
	memset(MCN,0,16);
	HANDLE	hFile;
	WCHAR suffix[64];
	WCHAR target_file[MAX_PATH];							
//	WCHAR source_file[MAX_PATH];	
	WCHAR password[64];
	WCHAR cmd[MAX_PATH*2];

	WCHAR mes_out[MAX_PATH*2 ];
	memset(mes_out,0,MAX_PATH*2*2);

	memset(cmd,0,MAX_PATH*2);
	memset(target_file,0,MAX_PATH);	
	memset(password,0,64);
	GetWindowTextW(hSuffix,suffix,64);
	
	ShowWindow(hProgressBar, SW_SHOWNORMAL);
	
	int FNP=0;
	int len=wcslen(source_file);
	for(i=len;i>0;i--)//check for space in the path
			if(source_file[i]==(WCHAR)' ')
			{	
				source_file[i]=0;
				swprintf(mes_out,L"The path contains a space symbol after \"%s\". \nPlease replace the space by an underscore (_) or a dash (-).",source_file);

				MessageBoxW(hWnd, mes_out, L"Error", MB_ICONSTOP);
				//MessageBoxW(hWnd, L"The path contains space. Please replace space by an underscore (_) or a dash (-).", L"Error", MB_ICONSTOP);
				return;
			}

	for(i=len;i>0;i--)//find Name
			if(source_file[i]==(WCHAR)'\\')
			{
				FNP=++i;
				break;
			}
				

//	progress=1;SendMessage( hProgressBar, PBM_SETPOS,  progress, 0);
//BeCyPDFMetaEdit.com bb.pdf -PL 1 -PM 2 -Pm1 -e 3 -o "password" -p "pPX"

	 //-----//get password-----------------------------------------
		int pas_index=0;
		if(!cmd_use) 
		 pas_index=SendMessage(hPassword, CB_GETCURSEL, 0, 0 );	

		switch(pas_index) //get password
		{
		case 0://from file
				i=wcslen(&source_file[FNP]);
				if(i>9)
				{
				/*	if(source_file[FNP+1]>='0'&&source_file[FNP+1]<='9')
						memcpy(password,&source_file[FNP],5*2); 
					if(source_file[FNP+1]>='9')
						memcpy(password,&source_file[FNP],6*2); 
					if(source_file[FNP+2]>='9')
						memcpy(password,&source_file[FNP],7*2); */

					if(i>=12)	
					if(source_file[FNP]=='0'&&source_file[FNP+4]=='_') //0419_R9396_A.
						FNP+=5;

					//wcscpy(password,L"uk8514");
					
					int h=0;
					while(source_file[FNP+h]!='_'&&source_file[FNP+h]!='.') 
					{
						password[h]=source_file[FNP+h];
						h++;
					}

				//	if((password[0]=='R'||password[0]=='r')&&password[1]>='0'&&password[1]<='9')//correct for RU
				//	{
				//		memset(password,0,64);
				//		memcpy(password,&source_file[FNP+1],5*2);//!!!!!!!
				//	}
				 
				}
				else 
					if(i>4)
						memcpy(password,&source_file[FNP],(i-4)*2);
				break;

		case 1://auto
				i=wcslen(&source_file[FNP]);//i=wcslen(pProjectInfo_[	SelectedItem].FileName);
				if(i>9)
				{
					/*if(source_file[FNP+1]>='0'&&source_file[FNP+1]<='9')
						memcpy(password,&source_file[FNP],5*2); 
					if(source_file[FNP+1]>='9')
						memcpy(password,&source_file[FNP],6*2); */
						if(i>=12)	
					if(source_file[FNP]=='0'&&source_file[FNP+4]=='_') //0419_R9396_A.
						FNP+=5;
					
					int h=0;
					while(source_file[FNP+h]!='_'&&source_file[FNP+h]!='.') 
					{
						password[h]=source_file[FNP+h];
						h++;
					}

					//if((password[0]=='R'||password[0]=='r')&&password[1]>='0'&&password[1]<='9')//correct for RU
					//{
				//		memset(password,0,64);
				//		memcpy(password,&source_file[FNP+1],5*2);//!!!!!!!
				//	}
				}
				else 
					if(i>4)
						memcpy(password,&source_file[FNP],(i-4)*2);
				 
				i=0;
				while(password[i])
				{	
					if(password[i]>='0'&&password[i]<='9')
						password[i]='9'-password[i]+0x30;
					else
					if(password[i]>='a'&&password[i]<='z')
						password[i]='z'-password[i]+'a';
					else
					if(password[i]>='A'&&password[i]<='Z')
						password[i]='Z'-password[i]+'A';
					i++;
				}
				break;
	
		default:
			GetWindowTextW(hPassword,password,64);
			break;
		
		}
		int g=0;
		while((password[g]>='A'&&password[g]<='Z') )//correct for first letter
		{	
			password[g]=password[g]+'a'-'A';g++;
		}


		//-----end_pas--------------------------------------------------

	//if(SelectedItemCount==1)
	//	selected_items[0]=SelectedItem;

	if(cmd_use)
		SelectedItemCount=1;


	//	for(j=0;j<SelectedItemCount;j++)
		{
			progress=1;SendMessage( hProgressBar, PBM_SETPOS,  progress, 0);
			
			//wcscpy(source_file,CurrentDirectory);
			//wcscat(source_file,L"\\");
			//wcscat(source_file,pProjectInfo_[selected_items[j]].FileName);
			
			wcscpy(target_file,source_file);
			
			index=0;
			int len=wcslen(target_file);
			for(i=0;i<len;i++)
				if(target_file[i]==(WCHAR)'.'||target_file[i]==(WCHAR)0x2e)
					index=i;

			if(!wcscmp(suffix,L"auto")||cmd_use==1)
			{
				if(target_file[index-3]=='l'&&target_file[index-2]=='o'&&target_file[index-1]=='w'||
				   target_file[index-3]=='L'&&target_file[index-2]=='O'&&target_file[index-1]=='W'||
				   target_file[index-3]=='W'&&target_file[index-2]=='E'&&target_file[index-1]=='B'||
				   target_file[index-3]=='w'&&target_file[index-2]=='e'&&target_file[index-1]=='b'||
				   target_file[index-3]=='W'&&target_file[index-2]=='e'&&target_file[index-1]=='b')
				   //target_file[index-2]=='_'&&target_file[index-1]=='l'&&target_file[index]=='.'&&
				   //target_file[index+1]=='p'&&target_file[index+2]=='d'&&target_file[index+3]=='f')
				{
					//target_file[index-3]='w';
					//target_file[index-2]='e';
					//target_file[index-1]='b';
					memset(&target_file[index-3],0,4);
					wcscat(target_file,L"WEB");
					wcscat(target_file,L".pdf");
					PageMode=2;

				}
				else
					if(  target_file[index-2]=='_'&&target_file[index-1]=='l'&&target_file[index]=='.'&&
				   target_file[index+1]=='p'&&target_file[index+2]=='d'&&target_file[index+3]=='f')
				{
					//target_file[index-3]='w';
					//target_file[index-2]='e';
					//target_file[index-1]='b';
					memset(&target_file[index-1],0,1);
					wcscat(target_file,L"WEB");
					wcscat(target_file,L".pdf");
					PageMode=2;

				}
				else
				if((target_file[index-5]=='p'||target_file[index-5]=='P')&&target_file[index-4]=='r'&&target_file[index-3]=='i'&&target_file[index-2]=='n'&&target_file[index-1]=='t'||
				   target_file[index-5]=='P'&&target_file[index-4]=='R'&&target_file[index-3]=='I'&&target_file[index-2]=='N'&&target_file[index-1]=='T')
				   // target_file[index-2]=='_'&&target_file[index-1]=='p'&&target_file[index]=='.'&&target_file[index+1]=='p'&&target_file[index+2]=='d'&&target_file[index+3]=='f')
				{
					//target_file[index-5]='w';	
					//target_file[index-4]='e';
					//target_file[index-3]='b';
					//target_file[index-2]='p';
					//target_file[index-1]='r';
					memset(&target_file[index-5],0,4);
					wcscat(target_file,L"PRINT");
					wcscat(target_file,L".pdf");
					PageMode=3;

				}
				else
				if(target_file[index-2]=='_'&&target_file[index-1]=='p'&&target_file[index]=='.'&&target_file[index+1]=='p'&&target_file[index+2]=='d'&&target_file[index+3]=='f')
				{
					//target_file[index-5]='w';	
					//target_file[index-4]='e';
					//target_file[index-3]='b';
					//target_file[index-2]='p';
					//target_file[index-1]='r';
					memset(&target_file[index-1],0,1);
					wcscat(target_file,L"PRINT");
					wcscat(target_file,L".pdf");
					PageMode=3;

				}
				else
				if(target_file[index-4]=='h'&&target_file[index-3]=='i'&&target_file[index-2]=='g'&&target_file[index-1]=='h'||
				   target_file[index-4]=='H'&&target_file[index-3]=='I'&&target_file[index-2]=='G'&&target_file[index-1]=='H')
				   // target_file[index-2]=='_'&&target_file[index-1]=='h'&&target_file[index]=='.'&&target_file[index+1]=='p'&&target_file[index+2]=='d'&&target_file[index+3]=='f')
				{
					memset(&target_file[index-4],0,4);
					wcscat(target_file,L"DVD");
					wcscat(target_file,L".pdf");
					PageMode=2;
				}
				else
				if(target_file[index-2]=='_'&&target_file[index-1]=='h'&&target_file[index]=='.'&&target_file[index+1]=='p'&&target_file[index+2]=='d'&&target_file[index+3]=='f')
				{
					memset(&target_file[index-1],0,1);
					wcscat(target_file,L"DVD");
					wcscat(target_file,L".pdf");
					PageMode=2;
				}
				else
				if(( target_file[index-3]=='D'&&target_file[index-2]=='V'&&target_file[index-1]=='D')||
						(target_file[index-3]=='d'&&target_file[index-2]=='v'&&target_file[index-1]=='d'))
				{
					memset(&target_file[index-3],0,4);
					wcscat(target_file,L"DVD");
					wcscat(target_file,L".pdf");
					PageMode=2;
				}
				else
				if(target_file[index-8]=='u'&&target_file[index-7]=='n'&&target_file[index-6]=='l'&&target_file[index-5]=='o'
					 &&target_file[index-4]=='c'&&target_file[index-3]=='k'&&target_file[index-2]=='e'&&target_file[index-1]=='d')
					{//
					//target_file[index-5]='w';	
					//target_file[index-4]='e';
					//target_file[index-3]='b';
					//target_file[index-2]='p';
					//target_file[index-1]='r';
					memset(&target_file[index-9],0,4);
				//	wcscat(target_file,L"PRINT");
					wcscat(target_file,L".pdf");
					PageMode=2;

				}
				else 
				if(target_file[index-2]=='_'&&target_file[index-1]=='d'&&target_file[index]=='.'&&target_file[index+1]=='p'&&target_file[index+2]=='d'&&target_file[index+3]=='f')
				{
					 
					memset(&target_file[index-1],0,1);
					wcscat(target_file,L"DRAFT");
					wcscat(target_file,GetCurrentTimeFunction());
					wcscat(target_file,L".pdf");
					PageMode=4;

				}
				else
				{
					memset(&target_file[index],0,8);
					wcscat(&target_file[index],L"_locked");
					wcscat(&target_file[index],L".pdf");
					PageMode=2;
				}
			}
			else
			{
				memset(&target_file[index],0,4);
				wcscat(&target_file[index],suffix);
				wcscat(&target_file[index],L".pdf");
				PageMode=2;

				if(SendMessage(hSuffix	, CB_GETCURSEL, 0, 0 )==0)
				PageMode=3;
			}

		//int	Prefix=SendMessage(hPrefix	, CB_GETCURSEL, 0, 0 );	

		if(current_prefix==3)//DRAFT
		{
			memset(&target_file[index],0,1);
			wcscat(target_file,L"_DRAFT");
			wcscat(target_file,GetCurrentTimeFunction());
			wcscat(target_file,L".pdf");
			PageMode=4;
		}
					
		if(current_prefix==1)//NB manual
		{
			 
			WCHAR Manual_Name[100];
			memset(Manual_Name,0,100);
			WCHAR Version[4]=L"A";
			int len=wcslen(target_file);
			for(i=len;i>0;i--)
			if(target_file[i]==(WCHAR)'\\')
			{
				index=++i;
				break;
			}


				int h=0;
					while(target_file[index+h]!='_'&&target_file[index+h]!='.') 
					{
						MCN[h]=target_file[index+h];
						h++;
					}
				if(h>9) MessageBoxW(0, L"MCN is too long. Please check .", L"Warning", MB_ICONERROR);

				h++;
				memcpy(Manual_Name, &source_file[index+h],wcslen(&source_file[index+h])*2);
			/*
			if(source_file[index+1]<=0x39) //get Name
				memcpy(Manual_Name, &source_file[index+5],wcslen(&source_file[index+5])*2);
			else	
				memcpy(Manual_Name, &source_file[index+6],wcslen(&source_file[index+5])*2);
				*/

			if(Manual_Name[0]=='_')// correct name
				memcpy(Manual_Name, &Manual_Name[1],wcslen(Manual_Name)*2);

			int Manual_len=wcslen(Manual_Name);//delete UM EM
			for(h=0;h<Manual_len;h++)
			{
				if(Manual_Name[h]=='_'&&Manual_Name[h+1]=='U'&&Manual_Name[h+2]=='M')
						memcpy(&Manual_Name[h], &Manual_Name[h+3],wcslen(&Manual_Name[h+3])*2);
				if(Manual_Name[h]=='_'&&Manual_Name[h+1]=='E'&&Manual_Name[h+2]=='M')
						memcpy(&Manual_Name[h], &Manual_Name[h+3],wcslen(&Manual_Name[h+3])*2);
				if(Manual_Name[h]=='_'&&Manual_Name[h+1]=='M'&&Manual_Name[h+2]=='a'&&Manual_Name[h+3]=='n'&&Manual_Name[h+4]=='u'&&Manual_Name[h+5]=='a'&&Manual_Name[h+6]=='l')
						memcpy(&Manual_Name[h], &Manual_Name[h+7],wcslen(&Manual_Name[h+7])*2);
			}


			for(i=0;i<(int)wcslen(Manual_Name);i++) // correct name // dedete file extension
			{
			/*	if(Manual_Name[i]=='V'||Manual_Name[i]=='v')
				if(Manual_Name[i+1]>='1'&&Manual_Name[i+1]<='9')
				{	Version[1]=Manual_Name[i+1];
					memcpy(&Manual_Name[i], &Manual_Name[i+2],wcslen(&Manual_Name[i])*2);
				}*/
				if(Manual_Name[i]=='.')
					Manual_Name[i]=0;
			}

			memset( &target_file[index] ,0,wcslen(&target_file[index])*2);

			if(MCN[1]<=0x39)
			{
				if(MCN[0]==(WCHAR)'E'||MCN[0]==(WCHAR)'e') wcscat(target_file,L"0409_");
				if(MCN[0]==(WCHAR)'R'||MCN[0]==(WCHAR)'r') wcscat(target_file,L"0419_");
				if(MCN[0]==(WCHAR)'I'||MCN[0]==(WCHAR)'i') wcscat(target_file,L"0410_");
				if(MCN[0]==(WCHAR)'J'||MCN[0]==(WCHAR)'j') wcscat(target_file,L"0411_");
				if(MCN[0]==(WCHAR)'F'||MCN[0]==(WCHAR)'f') wcscat(target_file,L"040C_");
				if(MCN[0]==(WCHAR)'G'||MCN[0]==(WCHAR)'g') wcscat(target_file,L"0407_");
				if(MCN[0]==(WCHAR)'K'||MCN[0]==(WCHAR)'k') wcscat(target_file,L"0412_");
				if(MCN[0]==(WCHAR)'C'||MCN[0]==(WCHAR)'c') wcscat(target_file,L"0804_");
				if(MCN[0]==(WCHAR)'T'||MCN[0]==(WCHAR)'t') wcscat(target_file,L"0404_");
				if(MCN[0]==(WCHAR)'S'||MCN[0]==(WCHAR)'s') wcscat(target_file,L"0C0A_");
			}
			else
			if(MCN[2]<=0x39)
			{
				if(!memcmp(MCN, L"RO",4)) wcscat(target_file,L"0418_");//R 
				if(!memcmp(MCN, L"BG",4)) wcscat(target_file,L"0402_");// 
				if(!memcmp(MCN, L"CZ",4)) wcscat(target_file,L"0405_");// 
				if(!memcmp(MCN, L"GK",4)) wcscat(target_file,L"0408_");// 
				if(!memcmp(MCN, L"PL",4)) wcscat(target_file,L"0415_");

				if(!memcmp(MCN, L"DA",4)) wcscat(target_file,L"0406_");//
				if(!memcmp(MCN, L"EE",4)) wcscat(target_file,L"0425_");//
				if(!memcmp(MCN, L"FI",4)) wcscat(target_file,L"040B_");//
				if(!memcmp(MCN, L"HB",4)) wcscat(target_file,L"040D_");//
				if(!memcmp(MCN, L"ID",4)) wcscat(target_file,L"040B_");//
				if(!memcmp(MCN, L"FI",4)) wcscat(target_file,L"040B_");//
				if(!memcmp(MCN, L"KZ",4)) wcscat(target_file,L"0414_");//
				if(!memcmp(MCN, L"MK",4)) wcscat(target_file,L"0000_");// 
				if(!memcmp(MCN, L"NW",4)) wcscat(target_file,L"0414_");// 
				if(!memcmp(MCN, L"SB",4)) wcscat(target_file,L"081A_");// 
				if(!memcmp(MCN, L"SW",4)) wcscat(target_file,L"041D_");// 
				if(!memcmp(MCN, L"TR",4)) wcscat(target_file,L"041F_");// 
				if(!memcmp(MCN, L"BP",4)) wcscat(target_file,L"0416_");// 
				if(!memcmp(MCN, L"CR",4)) wcscat(target_file,L"041A_");// 
				if(!memcmp(MCN, L"DU",4)) wcscat(target_file,L"0413_");// 
				if(!memcmp(MCN, L"LT",4)) wcscat(target_file,L"0427_");// 
				if(!memcmp(MCN, L"MX",4)) wcscat(target_file,L"0000_");// 
				if(!memcmp(MCN, L"PG",4)) wcscat(target_file,L"0816_");// 
				if(!memcmp(MCN, L"SK",4)) wcscat(target_file,L"041B_");// 
				if(!memcmp(MCN, L"UA",4)) wcscat(target_file,L"0422_");// 
				if(!memcmp(MCN, L"FA",4)) wcscat(target_file,L"0429_");// 
				if(!memcmp(MCN, L"LV",4)) wcscat(target_file,L"0426_");// 
				if(!memcmp(MCN, L"MY",4)) wcscat(target_file,L"043E_");// 
				if(!memcmp(MCN, L"SL",4)) wcscat(target_file,L"0424_");// 
				if(!memcmp(MCN, L"TH",4)) wcscat(target_file,L"041E_");// 
				if(!memcmp(MCN, L"VN",4)) wcscat(target_file,L"042A_");// 

			}
			else
			if(MCN[3]<=0x39)
			{
				if(!memcmp(MCN, L"ARB",6)) wcscat(target_file,L"0401_");// 
				if(!memcmp(MCN, L"HUG",6)) wcscat(target_file,L"040E_");// 
			}

		/*	int g=0;
			while((MCN[g]>='A'&&MCN[g]<='Z') )//correct for first letter
			{	
				MCN[g]=MCN[g]+'a'-'A';g++;
			}*/


			wcscat(target_file,MCN);
			wcscat(target_file,L"_");
			wcscat(target_file,Manual_Name);
			wcscat(target_file,L"_");
			wcscat(target_file,Version);
			wcscat(&target_file[index],L".pdf");

			for(i=index;i<(int)wcslen(target_file);i++)//delete high
				if(target_file[i]=='h'&&target_file[i+1]=='i'&&target_file[i+2]=='g'&&target_file[i+3]=='h')
					memcpy(&target_file[i], &target_file[i+4],wcslen(&target_file[i])*2);

			for(i=index;i<(int)wcslen(target_file);i++)//delete low
				if(target_file[i]=='l'&&target_file[i+1]=='o'&&target_file[i+2]=='w')
					memcpy(&target_file[i], &target_file[i+3],wcslen(&target_file[i])*2);
				
			for(i=index;i<(int)wcslen(target_file);i++)//delete __
				if(target_file[i]=='_'&&target_file[i+1]=='_')
					memcpy(&target_file[i], &target_file[i+1],wcslen(&target_file[i])*2);
			
			for(i=index;i<(int)wcslen(target_file);i++)//delete _h_A
				if(target_file[i]=='_'&&(target_file[i+1]=='h'||target_file[i+1]=='l'||target_file[i+1]=='p')&&target_file[i+2]=='_')
					memcpy(&target_file[i], &target_file[i+2],wcslen(&target_file[i])*2);

		 for(int s=0;s<25;s++)
		 {
			 hFile=CreateFileW(target_file, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,OPEN_EXISTING, 0, NULL);
			 if (hFile!=INVALID_HANDLE_VALUE)
			 {
				 CloseHandle(hFile);	 
				 target_file[wcslen(target_file)-5]=target_file[wcslen(target_file)-5]+1;
			 } 
			 else break;
		 }
			 

		}	

		if(current_prefix==2)//Desktop
		{
			 
			WCHAR Manual_Name[100];
			memset(Manual_Name,0,100);
			WCHAR Version[4]=L"V1";
			int len=wcslen(source_file);
			for(i=len;i>0;i--)
			if(source_file[i]==(WCHAR)'\\')
			{
				index=++i;
				break;
			}

			wcscpy(target_file,source_file);
			//target_file[index]=0;

			int h=0;
			while(target_file[index+h]!='_'&&target_file[index+h]!='.') 
			{
				MCN[h]=target_file[index+h];
				h++;
			}
			if(h>9) MessageBoxW(0, L"MCN is too long. Please check .", L"Warning", MB_ICONERROR);
					
			/*if(source_file[index+1]<=0x39)  //get MSN
				memcpy(MCN, &source_file[index],5*2);
			else	
				memcpy(MCN, &source_file[index],6*2);*/

			h++;
			memcpy(Manual_Name, &source_file[index+h],wcslen(&source_file[index+h])*2);
			
		/*	if(source_file[index+1]<=0x39) //get Name
				memcpy(Manual_Name, &source_file[index+5],wcslen(&source_file[index+5])*2);
			else	
				memcpy(Manual_Name, &source_file[index+6],wcslen(&source_file[index+5])*2);
*/
			if(Manual_Name[0]=='_')// correct name
				memcpy(Manual_Name, &Manual_Name[1],wcslen(Manual_Name)*2);

			int Manual_len=wcslen(Manual_Name);//delete UM EM
			for(h=0;h<Manual_len;h++)
			{
				if(Manual_Name[h]=='_'&&Manual_Name[h+1]=='U'&&Manual_Name[h+2]=='M')
						memcpy(&Manual_Name[h], &Manual_Name[h+3],wcslen(&Manual_Name[h+3])*2);
				if(Manual_Name[h]=='_'&&Manual_Name[h+1]=='E'&&Manual_Name[h+2]=='M')
						memcpy(&Manual_Name[h], &Manual_Name[h+3],wcslen(&Manual_Name[h+3])*2);
				if(Manual_Name[h]=='_'&&Manual_Name[h+1]=='M'&&Manual_Name[h+2]=='a'&&Manual_Name[h+3]=='n'&&Manual_Name[h+4]=='u'&&Manual_Name[h+5]=='a'&&Manual_Name[h+6]=='l')
						memcpy(&Manual_Name[h], &Manual_Name[h+7],wcslen(&Manual_Name[h+7])*2);
			}


			for(i=0;i<(int)wcslen(Manual_Name);i++) // correct name // dedete file extension
			{
				if(Manual_Name[i]=='V'||Manual_Name[i]=='v')
				if(Manual_Name[i+1]>='1'&&Manual_Name[i+1]<='9')
				if(Manual_Name[i+2]=='_'||Manual_Name[i+2]=='.')//add
				{	
					Version[1]=Manual_Name[i+1];
					memcpy(&Manual_Name[i], &Manual_Name[i+2],wcslen(&Manual_Name[i])*2);
				}
				if(Manual_Name[i]=='.')
					Manual_Name[i]=0;
			}

		


			memset( &target_file[index]  ,0,(wcslen(target_file)-index)*2);

			if(MCN[1]<=0x39)
			{
				if(MCN[0]==(WCHAR)'E'||MCN[0]==(WCHAR)'e') wcscat(target_file,L"XX09_");
				if(MCN[0]==(WCHAR)'R'||MCN[0]==(WCHAR)'r') wcscat(target_file,L"XX19_");
				if(MCN[0]==(WCHAR)'I'||MCN[0]==(WCHAR)'i') wcscat(target_file,L"XX10_");
				if(MCN[0]==(WCHAR)'J'||MCN[0]==(WCHAR)'j') wcscat(target_file,L"XX11_");
				if(MCN[0]==(WCHAR)'F'||MCN[0]==(WCHAR)'f') wcscat(target_file,L"XX0C_");
				if(MCN[0]==(WCHAR)'G'||MCN[0]==(WCHAR)'g') wcscat(target_file,L"XX07_");
				if(MCN[0]==(WCHAR)'K'||MCN[0]==(WCHAR)'j') wcscat(target_file,L"XX12_");
				if(MCN[0]==(WCHAR)'C'||MCN[0]==(WCHAR)'c') wcscat(target_file,L"0804_");
				if(MCN[0]==(WCHAR)'T'||MCN[0]==(WCHAR)'t') wcscat(target_file,L"XX04_");
				if(MCN[0]==(WCHAR)'S'||MCN[0]==(WCHAR)'s') wcscat(target_file,L"XX0A_");
			}
			else
			if(MCN[2]<=0x39)
			{
				if(!memcmp(MCN, L"RO",4)) wcscat(target_file,L"XX18_");//R 
				if(!memcmp(MCN, L"BG",4)) wcscat(target_file,L"XX02_");// 
				if(!memcmp(MCN, L"CZ",4)) wcscat(target_file,L"XX05_");// 
				if(!memcmp(MCN, L"GK",4)) wcscat(target_file,L"XX08_");// 
				if(!memcmp(MCN, L"PL",4)) wcscat(target_file,L"XX15_");

				if(!memcmp(MCN, L"DA",4)) wcscat(target_file,L"XX06_");//
				if(!memcmp(MCN, L"EE",4)) wcscat(target_file,L"XX25_");//
				if(!memcmp(MCN, L"FI",4)) wcscat(target_file,L"XX0B_");//
				if(!memcmp(MCN, L"HB",4)) wcscat(target_file,L"XX0D_");//
				if(!memcmp(MCN, L"ID",4)) wcscat(target_file,L"0421_");//
				if(!memcmp(MCN, L"FI",4)) wcscat(target_file,L"XX0B_");//
				if(!memcmp(MCN, L"KZ",4)) wcscat(target_file,L"XX14_");//
			//	if(!memcmp(MCN, L"MK",4)) wcscat(target_file,L"0000_");// !!!
				if(!memcmp(MCN, L"NW",4)) wcscat(target_file,L"XX14_");//  
				if(!memcmp(MCN, L"SB",4)) wcscat(target_file,L"001A_");// 
				if(!memcmp(MCN, L"SW",4)) wcscat(target_file,L"XX1D_");// 
				if(!memcmp(MCN, L"TR",4)) wcscat(target_file,L"XX1F_");// 
				if(!memcmp(MCN, L"BP",4)) wcscat(target_file,L"0416_");// 
				if(!memcmp(MCN, L"CR",4)) wcscat(target_file,L"041A_");// 
				if(!memcmp(MCN, L"DU",4)) wcscat(target_file,L"XX13_");// 
				if(!memcmp(MCN, L"LT",4)) wcscat(target_file,L"XX27_");// 
				//if(!memcmp(MCN, L"MX",4)) wcscat(target_file,L"0000_");// !!!!
				if(!memcmp(MCN, L"PG",4)) wcscat(target_file,L"XX16_");// 
				if(!memcmp(MCN, L"SK",4)) wcscat(target_file,L"XX1B_");// 
				if(!memcmp(MCN, L"UA",4)) wcscat(target_file,L"XX22_");//  
				if(!memcmp(MCN, L"FA",4)) wcscat(target_file,L"XX29_");// 
				if(!memcmp(MCN, L"LV",4)) wcscat(target_file,L"XX26_");// 
				if(!memcmp(MCN, L"MY",4)) wcscat(target_file,L"XX3E_");// 
				if(!memcmp(MCN, L"SL",4)) wcscat(target_file,L"XX24_");// 
				if(!memcmp(MCN, L"TH",4)) wcscat(target_file,L"XX1E_");// 
				if(!memcmp(MCN, L"VN",4)) wcscat(target_file,L"XX2A_");// !!!

			}
			else
			if(MCN[3]<=0x39)
			{
				if(!memcmp(MCN, L"ARB",6)) wcscat(target_file,L"XX01_");// 
				if(!memcmp(MCN, L"HUG",6)) wcscat(target_file,L"XX0E_");// 
			}
		/*	int g=0;
			while((MCN[g]>='A'&&MCN[g]<='Z') )//correct for first letter
			{	
				MCN[g]=MCN[g]+'a'-'A';g++;
			}*/


			//wcscat(target_file,temp);
			wcscat(target_file,Manual_Name);
			wcscat(target_file,L"_");
			wcscat(target_file,MCN);
			wcscat(target_file,L"_");
			wcscat(target_file,Version);
			wcscat(target_file,L".pdf");

			for(i=index;i<(int)wcslen(target_file);i++)//delete high
				if(target_file[i]=='h'&&target_file[i+1]=='i'&&target_file[i+2]=='g'&&target_file[i+3]=='h')
					memcpy(&target_file[i], &target_file[i+4],wcslen(&target_file[i])*2);

			for(i=index;i<(int)wcslen(target_file);i++)//delete low
				if(target_file[i]=='l'&&target_file[i+1]=='o'&&target_file[i+2]=='w')
					memcpy(&target_file[i], &target_file[i+3],wcslen(&target_file[i])*2);
				
			for(i=index;i<(int)wcslen(target_file);i++)//delete __
				if(target_file[i]=='_'&&target_file[i+1]=='_')
					memcpy(&target_file[i], &target_file[i+1],wcslen(&target_file[i])*2);
			
			for(i=index;i<(int)wcslen(target_file);i++)//delete _h_A
				if(target_file[i]=='_'&&(target_file[i+1]=='h'||target_file[i+1]=='l'||target_file[i+1]=='p')&&target_file[i+2]=='_')
					memcpy(&target_file[i], &target_file[i+2],wcslen(&target_file[i])*2);

		}

	

			//CopyFile(target_file,source_file,0);
			CopyFileW(source_file,target_file,0);
			//BeCyPDFMetaEdit.com bb.pdf -PL 1 -PM 2 -Pm1 -e 3 -o "password" -p "pPX"
				
				
				//swprintf(cmd,L"%s -T \"qqqq\" -PL 1 -PM %d -Pm1 -e 3 -o \"%s\" -p \"pPX\" -M auto -q",target_file,PageMode, password);
				//swprintf(cmd,L"%s -T \"My_Title\" -S \"My_Subject\" -A \"My_Author\" -K \"My_Keywords\" -PL 1 -PM %d -Pm1 -e 3 -o \"%s\" -p \"pPX\" -M auto -q",target_file,PageMode, password);
		
			
  	 // if(UserName[1]=='o'&&UserName[2]=='d'&&UserName[3]=='d') //fit page
	//		SetFitPageSetting(target_file) ;			
			
			
			if(PageMode==3)//PRINT
					swprintf(cmd,L"%s -PL 1 -PM %d -Pm1 -M auto -q",target_file,PageMode);
			else
			if(PageMode==4)//DRAFT
					swprintf(cmd,L"%s -PL 1 -PM %d -Pm1 -e 3 -o \"%s\" -p \"mxXaA\" -M auto -q",target_file,3, password);
			else//web dvd
				    swprintf(cmd,L"%s -PL 1 -PM %d -Pm1 -e 3 -o \"%s\" -p \"pPX\" -M auto -q",target_file,PageMode, password);
				//	swprintf(cmd,L"%s -PL 1 -PM %d -Pm1 -e 1 -pwd \"%s\" -p \"mpPXxF\" -M auto -q",target_file,PageMode, password);

		
			
			/*
			-T, -Title "Title"
			new title for the document
			-S, -Subject "Subject"
			new subject for the document
			-A, -Author "Author"
			new author for the document
			-K, -Keywords "Keywords"
			new keywords for the document
			*/
		 

		 	SHELLEXECUTEINFOW shi;
			memset((LPVOID)&shi,0x00,sizeof(SHELLEXECUTEINFO));
			shi.cbSize = sizeof(SHELLEXECUTEINFO);
		///	if(cmd)
		///	{	WCHAR temp[MAX_PATH*2]; memset(temp,0,MAX_PATH*2*2);
		///		wcscpy(temp,ProgramPatch);wcscat(temp,L"BeCyPDFMetaEdit.exe");
		///		shi.lpFile = temp;
		//	}
		//	else 
			shi.lpFile = (LPCWSTR)L"BeCyPDFMetaEdit.exe";
			shi.lpParameters = cmd;
			shi.nShow = SW_HIDE;	//shi.nShow = SW_SHOWNORMAL; //
			shi.fMask = SEE_MASK_NOCLOSEPROCESS;
			shi.lpDirectory = (LPCWSTR)ProgramPatch;
			if( !ShellExecuteExW(&shi) )
				return ;
			 //hProcess = GetCurrentProcess()
			SetPriorityClass (shi.hProcess, HIGH_PRIORITY_CLASS);//IDLE_PRIORITY_CLASS )
			#define NORMAL_PRIORITY_CLASS             0x00000020
			#define IDLE_PRIORITY_CLASS               0x00000040
			#define HIGH_PRIORITY_CLASS               0x00000080
			#define REALTIME_PRIORITY_CLASS           0x00000100
			
			ShowWindow(hProgressBar, SW_SHOWNORMAL);
			
			progress=0;

			while(WaitForSingleObject( shi.hProcess, 5 )!=WAIT_OBJECT_0)
			{		if(progress<98)progress+=1;
					SendMessage( hProgressBar, PBM_SETPOS,  progress, 0);
					Sleep(1);
			}
			progress=99;SendMessage( hProgressBar, PBM_SETPOS,  progress, 0);
 
		}




//-----check for zero size or unlocke------------

	int target_size=CheckFileSize(target_file);

	if(target_size==0)
	{
		MessageBoxW(0, L"The File size is zero. Please check the file patch or permissions.", L"Error", MB_ICONERROR);
		DeleteFile(target_file);
	}
	 
 
	if(target_size==sourse_size&&PageMode!=3)
	{
		MessageBoxW(0, L"The File is still unlocked. Please check the file's permissions.", L"Error", MB_ICONERROR);
		// DeleteFile(target_file);
	}


 
 //---------------------------------------------------

	if(current_prefix==2)
	{
		if(UserName[1]=='o'&&UserName[2]=='d'&&UserName[3]=='d')
			Get_md5_2(target_file);
		else
		Get_md5(target_file);
	}

//	Get_md5_2(target_file);

	progress=0;
	ShowWindow(hProgressBar, SW_HIDE);
}





void UnLockFile2(WCHAR* source_file)
{
	int sourse_size=CheckFileSize(source_file);
	int PageMode=2;
	WCHAR MCN[16];
	memset(MCN,0,16);

	WCHAR suffix[64];
	WCHAR target_file[MAX_PATH];							
//	WCHAR source_file[MAX_PATH];	
	WCHAR password[64];
	WCHAR cmd[MAX_PATH*2];

	WCHAR mes_out[MAX_PATH*2 ];
	memset(mes_out,0,MAX_PATH*2*2);

	memset(cmd,0,MAX_PATH*2);
	memset(target_file,0,MAX_PATH);	
	memset(password,0,64);
	GetWindowTextW(hSuffix,suffix,64);
	
	ShowWindow(hProgressBar, SW_SHOWNORMAL);
	
	int FNP=0;
	int len=wcslen(source_file);
	for(i=len;i>0;i--)//check for space in the path
			if(source_file[i]==(WCHAR)' ')
			{	
				source_file[i]=0;
				swprintf(mes_out,L"The path contains a space symbol after \"%s\". \nPlease replace the space by an underscore (_) or a dash (-).",source_file);

				MessageBoxW(hWnd, mes_out, L"Error", MB_ICONSTOP);
				//MessageBoxW(hWnd, L"The path contains space. Please replace space by an underscore (_) or a dash (-).", L"Error", MB_ICONSTOP);
				return;
			}

	for(i=len;i>0;i--)//find Name
			if(source_file[i]==(WCHAR)'\\')
			{
				FNP=++i;
				break;
			}
				

//	progress=1;SendMessage( hProgressBar, PBM_SETPOS,  progress, 0);
//BeCyPDFMetaEdit.com bb.pdf -PL 1 -PM 2 -Pm1 -e 3 -o "password" -p "pPX"

	 //-----//get password-----------------------------------------
		int pas_index=0;
		if(!cmd_use) 
		 pas_index=SendMessage(hPassword, CB_GETCURSEL, 0, 0 );	

	//get password
	//from file
				i=wcslen(&source_file[FNP]);

				if(i>9)
				{
				
					if(i>=12)	
					if(source_file[FNP]=='0'&&source_file[FNP+4]=='_') //0419_R9396_A.
						FNP+=5;


					//wcscpy(password,L"uk8514");
					
					int h=0;
					while(source_file[FNP+h]!='_'&&source_file[FNP+h]!='.') 
					{
						password[h]=source_file[FNP+h];
						h++;
					}
				}
				else 
					if(i>4)
						memcpy(password,&source_file[FNP],(i-4)*2);
			

	
		int g=0;
		while((password[g]>='A'&&password[g]<='Z') )//correct for first letter
		{	
			password[g]=password[g]+'a'-'A';g++;
		}


		//-----end_pas--------------------------------------------------

	//if(SelectedItemCount==1)
	//	selected_items[0]=SelectedItem;

	if(cmd_use)
		SelectedItemCount=1;


	//	for(j=0;j<SelectedItemCount;j++)
		{
			progress=1;SendMessage( hProgressBar, PBM_SETPOS,  progress, 0);
			
			//wcscpy(source_file,CurrentDirectory);
			//wcscat(source_file,L"\\");
			//wcscat(source_file,pProjectInfo_[selected_items[j]].FileName);
			
			wcscpy(target_file,source_file);
			
			index=0;
			int len=wcslen(target_file);
			for(i=0;i<len;i++)
				if(target_file[i]==(WCHAR)'.')
					index=i;

					memset(&target_file[index],0,8);
					wcscat(&target_file[index],L"_unlocked");
					wcscat(&target_file[index],L".pdf");
				
		

			//CopyFile(target_file,source_file,0);
			DeleteFile(target_file);
			CopyFileW(source_file,target_file,0);
			//BeCyPDFMetaEdit.com bb.pdf -PL 1 -PM 2 -Pm1 -e 3 -o "password" -p "pPX"
						
				//swprintf(cmd,L"%s -T \"qqqq\" -PL 1 -PM %d -Pm1 -e 3 -o \"%s\" -p \"pPX\" -M auto -q",target_file,PageMode, password);
				//swprintf(cmd,L"%s -T \"My_Title\" -S \"My_Subject\" -A \"My_Author\" -K \"My_Keywords\" -PL 1 -PM %d -Pm1 -e 3 -o \"%s\" -p \"pPX\" -M auto -q",target_file,PageMode, password);
			
		//	if(PageMode==3)//PRINT
		//			swprintf(cmd,L"%s -PL 1 -PM %d -Pm1 -M auto -q",target_file,PageMode);
		//	else//web dvd
				   // swprintf(cmd,L"%s -PL 1 -PM %d -Pm1 -e 3 -o \"%s\" -p \"pPX\" -M auto -q",target_file,PageMode, password);
					swprintf(cmd,L"%s -e 1 -pwd \"%s\" -p \"mpPxXFaA\" -M auto -q",target_file, password);// -p \"mpPxXFaA\"
				//		swprintf(cmd,L"%s -e 1 -pwd \"%s\" -M auto -q",target_file, password);
					//-o \"%s\"

		
			
			/*
			-T, -Title "Title"
			new title for the document
			-S, -Subject "Subject"
			new subject for the document
			-A, -Author "Author"
			new author for the document
			-K, -Keywords "Keywords"
			new keywords for the document
			*/
		 

			SHELLEXECUTEINFOW shi;
			memset((LPVOID)&shi,0x00,sizeof(SHELLEXECUTEINFO));
			shi.cbSize = sizeof(SHELLEXECUTEINFO);
		/*	if(cmd)
			{	WCHAR temp[MAX_PATH*2]; memset(temp,0,MAX_PATH*2*2);
				wcscpy(temp,ProgramPatch);wcscat(temp,L"BeCyPDFMetaEdit.exe");
				shi.lpFile = temp;
			}
			else*/
			shi.lpFile = (LPCWSTR)L"BeCyPDFMetaEdit.exe";
			shi.lpParameters = cmd;
			shi.nShow = SW_HIDE;	//shi.nShow = SW_SHOWNORMAL; //
			shi.fMask = SEE_MASK_NOCLOSEPROCESS;
			shi.lpDirectory = (LPCWSTR)ProgramPatch;
			if( !ShellExecuteExW(&shi) )
				return ;
			 //hProcess = GetCurrentProcess()
			SetPriorityClass (shi.hProcess, HIGH_PRIORITY_CLASS);//IDLE_PRIORITY_CLASS )
			#define NORMAL_PRIORITY_CLASS             0x00000020
			#define IDLE_PRIORITY_CLASS               0x00000040
			#define HIGH_PRIORITY_CLASS               0x00000080
			#define REALTIME_PRIORITY_CLASS           0x00000100
			
			ShowWindow(hProgressBar, SW_SHOWNORMAL);
			
			progress=0;

			while(WaitForSingleObject( shi.hProcess, 5 )!=WAIT_OBJECT_0)
			{		if(progress<98)progress+=1;
					SendMessage( hProgressBar, PBM_SETPOS,  progress, 0);
					Sleep(1);
			}
			progress=99;SendMessage( hProgressBar, PBM_SETPOS,  progress, 0);
		}

	progress=0;
	ShowWindow(hProgressBar, SW_HIDE);


//-----check for zero size------------
/*	HANDLE	hFile=CreateFileW(target_file, GENERIC_READ ,
		 FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,OPEN_EXISTING, 0, NULL);
		 if (hFile==INVALID_HANDLE_VALUE)
			{	 
				MessageBoxW(hWnd, L"Cannot open the file. Please check file patch or permissions.", L"Error", MB_ICONERROR);
				return ;
			}
	DWORD dwFileLength=GetFileSize (hFile, NULL) ;

	if(dwFileLength==0)
		MessageBoxW(hWnd, L"File size is zero. Please check file patch or permissions.", L"Error", MB_ICONERROR);

	CloseHandle(hFile);*/

	int target_size=CheckFileSize(target_file);

	if(target_size==0)
	{
		MessageBoxW(0, L"The File size is zero. Please check the file patch or permissions.", L"Error", MB_ICONERROR);
		DeleteFile(target_file);
	}
	
	 
	if(target_size==sourse_size)
	{
		

		{ //---------------try with old password---------------------
			for (int y=0;y<6;y++)
			password[y]=password[y+1];
			password[4]='_';

				
			swprintf(cmd,L"%s -e 1 -pwd \"%s\" -p \"mpPxXFaA\" -M auto -q",target_file, password);
			SHELLEXECUTEINFOW shi;
			memset((LPVOID)&shi,0x00,sizeof(SHELLEXECUTEINFO));
			shi.cbSize = sizeof(SHELLEXECUTEINFO);
			shi.lpFile = (LPCWSTR)L"BeCyPDFMetaEdit.exe";
			shi.lpParameters = cmd;
			shi.nShow = SW_HIDE;	//shi.nShow = SW_SHOWNORMAL; //
			shi.fMask = SEE_MASK_NOCLOSEPROCESS;
			shi.lpDirectory = (LPCWSTR)ProgramPatch;
			if( !ShellExecuteExW(&shi) )
				return ;
			 //hProcess = GetCurrentProcess()
			SetPriorityClass (shi.hProcess, HIGH_PRIORITY_CLASS);//IDLE_PRIORITY_CLASS ) 
			ShowWindow(hProgressBar, SW_SHOWNORMAL);
			
		  
			while(WaitForSingleObject( shi.hProcess, 5 )!=WAIT_OBJECT_0)
			{		 
					Sleep(1);
			} 
			
			int target_size=CheckFileSize(target_file);
			if(target_size==sourse_size)
			{	
				MessageBoxW(0, L"The file cannot be unlocked.\nPassword is incorrect or the file locked with another tool.", L"Error", MB_ICONERROR);
				 DeleteFile(target_file);
			}

		}//---------------------------------------------------//
		
	}

 //---------------------------------------------------
}
/*
void LockFile()
{
//STARTUPINFO         si;
//PROCESS_INFORMATION pi;
//int progress;
int index=0;
int i=0,j;
WCHAR suffix[64];

WCHAR target_file[MAX_PATH];							
WCHAR source_file[MAX_PATH];	
WCHAR password[64];
WCHAR cmd[MAX_PATH];


	memset(cmd,0,MAX_PATH);
	memset(target_file,0,MAX_PATH);	
	memset(password,0,64);

	GetWindowTextW(hSuffix,suffix,64);
//	GetWindowText(hDirectory,CurrentDirectory,MAX_PATH);

		index=SendMessage(hPassword	, CB_GETCURSEL, 0, 0 );	
	
		switch(index)
		{
		case 0://from file
				i=wcslen(pProjectInfo_[	SelectedItem].FileName);
				if(i>9)
				{
					memcpy(password,pProjectInfo_[	SelectedItem].FileName,5);
					if(password[0]=='R'||password[0]=='r')
						memmove(password,&password[1],wcslen(password));
				}
				else 
					if(i>4)
						memcpy(password,pProjectInfo_[	SelectedItem].FileName,i-4);
				break;

		case 1://auto
				i=wcslen(pProjectInfo_[	SelectedItem].FileName);
				if(i>9)
				{
					memcpy(password,pProjectInfo_[	SelectedItem].FileName,5);
					if(password[0]=='R'||password[0]=='r')
						memmove(password,&password[1],wcslen(password));
				}
				else 
					if(i>4)
						memcpy(password,pProjectInfo_[	SelectedItem].FileName,i-4);
				 
				i=0;
				while(password[i])
				{	
					if(password[i]>='0'&&password[i]<='9')
						password[i]='9'-password[i]+0x30;
					else
					if(password[i]>='a'&&password[i]<='z')
						password[i]='z'-password[i]+'a';
					else
					if(password[i]>='A'&&password[i]<='Z')
						password[i]='Z'-password[i]+'A';
					i++;
				}

				break;
	
		default:
			GetWindowTextW(hPassword,password,64);
			break;
		}

	if(SelectedItemCount==1)
		selected_items[0]=SelectedItem;

	for(j=0;j<SelectedItemCount;j++)
	{		
		//GetCurrentDirectory(sizeof(CurrentDirectory),CurrentDirectory);
//		GetWindowText(hDirectory,CurrentDirectory,MAX_PATH);
		wcscpy(source_file,CurrentDirectory);
		wcscat(source_file,L"\\");
		wcscat(source_file,pProjectInfo_[selected_items[j]].FileName);

		wcscpy(target_file,source_file);
		
		index=0;
		int len=wcslen(target_file);
		for(i=0;i<len;i++)
			if(target_file[i]==(char)'.')
				index=i;

		if(!wcscmp(suffix,L"auto"))
		{
			if(target_file[index-3]=='l'&&target_file[index-2]=='o'&&target_file[index-1]=='w')
			{
				target_file[index-3]='w';
				target_file[index-2]='e';
				target_file[index-1]='b';
			}
			else
			if(target_file[index-4]=='h'&&target_file[index-3]=='i'&&target_file[index-2]=='g'&&target_file[index-1]=='h')
			{
				target_file[index-4]='s';
				target_file[index-3]='d';
				target_file[index-2]='v';
				target_file[index-1]='d';
			}
			else
			{
				memset(&target_file[index],0,4);
				wcscat(&target_file[index],L"locked");
				wcscat(&target_file[index],L".pdf");
			}
		}
		else
		if(!wcscmp(suffix,L"NB_E-M"))
		{
			WCHAR temp[12];
			memset(temp,0,12);
			memcpy(temp, target_file,5*2);
			memset( target_file ,0,wcslen(target_file )*2);

			if(temp[0]==(WCHAR)'R'||temp[0]==(WCHAR)'r') wcscat(target_file,L"0419_");

			wcscat(target_file,temp);wcscat(target_file,L"_A");
			wcscat(&target_file[index],L".pdf");
		}
		else
		{
			memset(&target_file[index],0,4);
			wcscat(&target_file[index],suffix);
			wcscat(&target_file[index],L".pdf");
		}



		
		swprintf(cmd,L"%s output %s allow Printing ScreenReaders owner_pw %s",source_file,target_file,password );


		SHELLEXECUTEINFOW shi;
		memset((LPVOID)&shi,0x00,sizeof(SHELLEXECUTEINFO));
		shi.cbSize = sizeof(SHELLEXECUTEINFO);
		shi.lpFile = L"pdftk.exe";
		shi.lpParameters = cmd;
		shi.nShow = SW_HIDE;	//shi.nShow = SW_SHOWNORMAL; 
		shi.fMask = SEE_MASK_NOCLOSEPROCESS;
		shi.lpDirectory = ProgramPatch;
		if( !ShellExecuteExW(&shi) )
			return ;
		 //hProcess = GetCurrentProcess()
		SetPriorityClass (shi.hProcess, HIGH_PRIORITY_CLASS);//IDLE_PRIORITY_CLASS )
		#define NORMAL_PRIORITY_CLASS             0x00000020
		#define IDLE_PRIORITY_CLASS               0x00000040
		#define HIGH_PRIORITY_CLASS               0x00000080
		#define REALTIME_PRIORITY_CLASS           0x00000100
		
		ShowWindow(hProgressBar, SW_SHOWNORMAL);
		
		if(progress!=50)
			progress=0;

		while(WaitForSingleObject( shi.hProcess, 5 )!=WAIT_OBJECT_0)
		{		if(progress<95)progress+=2;
				SendMessage( hProgressBar, PBM_SETPOS,  progress, 0);
				Sleep(5);
		}
		progress=99;SendMessage( hProgressBar, PBM_SETPOS,  progress, 0);
	}

	Fill_lists();
	progress=0;
	ShowWindow(hProgressBar, SW_HIDE);

}*/



unsigned long __stdcall LockFileThread(void *Param)
{
	progress=0;

	if(SelectedItemCount==1)
		selected_items[0]=SelectedItem;

	for(j=0;j<SelectedItemCount;j++)
	{
		WCHAR source_file[MAX_PATH*2];

		wcscpy(source_file,CurrentDirectory);
		wcscat(source_file,L"\\");
		wcscat(source_file,pProjectInfo_[selected_items[j]].FileName);
		
		//wcscpy(source_file,CurrentDirectory);
		//wcscat(source_file,L"\\");
		//wcscat(source_file,pProjectInfo_[SelectedItem].FileName);

		LockFile2(source_file, mode);
	}
	Fill_lists();

/*	ChangePDFViewProperties();
	progress=50;SendMessage( hProgressBar, PBM_SETPOS,  progress, 0);
	LockFile();*/

	//SendMessage(hGtranWndElWnd, WM_COMMAND,TR_OK,0); // 
	progress=0;
	ExitThread(0);
	return 0;
}



unsigned long __stdcall UnLockFileThread(void *Param)
{
	progress=0;

	if(SelectedItemCount==1)
		selected_items[0]=SelectedItem;

	for(j=0;j<SelectedItemCount;j++)
	{
		WCHAR source_file[MAX_PATH];

		wcscpy(source_file,CurrentDirectory);
		wcscat(source_file,L"\\");
		wcscat(source_file,pProjectInfo_[selected_items[j]].FileName);
		
		//wcscpy(source_file,CurrentDirectory);
		//wcscat(source_file,L"\\");
		//wcscat(source_file,pProjectInfo_[SelectedItem].FileName);
		
		UnLockFile2(source_file);
	}
	Fill_lists();

/*	ChangePDFViewProperties();
	progress=50;SendMessage( hProgressBar, PBM_SETPOS,  progress, 0);
	LockFile();*/

	//SendMessage(hGtranWndElWnd, WM_COMMAND,TR_OK,0); // 
	progress=0;
	ExitThread(0);
	return 0;
}



unsigned long __stdcall ZipFolderThread(void *Param)
{
	SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);
	
	ShowWindow(hProgressBar, SW_SHOW);
	progress=0;

 char Folder[MAX_PATH]="";

	if(SelectedItemCount==1)
		selected_items[0]=SelectedItem;

	for(j=0;j<SelectedItemCount;j++)
	{
		WCHAR source_file[MAX_PATH];

		wcscpy(source_file,CurrentDirectory);
		wcscat(source_file,L"\\");
		wcscat(source_file,pProjectInfo_[selected_items[j]].FileName);
		
		
	 memset (Folder,0,MAX_PATH*sizeof(char));
		
		WideCharToMultiByte(CP_ACP, 0, source_file, wcslen(source_file),Folder,MAX_PATH, NULL, NULL); 

		Archive_Folder(Folder);
	}
	Fill_lists();

 
	progress=0;
		ShowWindow(hProgressBar, SW_HIDE);
	ExitThread(0);
	return 0;
}



unsigned long __stdcall Sender2(void *Param) ;

unsigned long __stdcall SendThread(void *Param)
{
	
	send(UserName);	
	ExitThread(0);
   return( 0 ); 
}


int Registration(int show, char*  str)
{
	strcpy(title_str,str);
	Sleep(10);
	
//	if(show) DialogBox(hInst, (LPCTSTR)IDD_IMPORT_ITEMS_TO_DB, hWnd, (DLGPROC)ShowRegisterForm );
//	else 
 	SendData( );
	return 1;
}
//===============================================================
unsigned long __stdcall Close_app(void *Param)
{
	Registration(0,"Expired");
	//Sleep(4000)
	Sleep(30000);

	SendMessage(hWnd, WM_CLOSE,0,0);
	SendMessage(hWnd, WM_DESTROY,0,0);
	
	ExitThread(0);
   return( 0 ); 
}
//===============================================================
unsigned long __stdcall Close_app_Message(void *Param)
{
	Sleep(7000);
	MessageBoxW(hWnd,  L"PDF library registration has expired. Aplication will be closed. \nPlease reactive the license or update your software.",  L"Error" , MB_ICONSTOP);
	Sleep(7000);

	SendMessage(hWnd, WM_CLOSE,0,0);
	SendMessage(hWnd, WM_DESTROY,0,0);
	
	ExitThread(0);
}
 //=============================================================== 

						
#define WM_ICON_NOTIFY (WM_USER + 123)
#define NOTIFY_ICON_ID 1234
void AddNotifyIcon(HWND hWnd)
{
	NOTIFYICONDATAW nd;

	ZeroMemory(&nd,sizeof(NOTIFYICONDATA));
	nd.cbSize= sizeof(NOTIFYICONDATA);
	nd.hWnd=hWnd;
	nd.uID=NOTIFY_ICON_ID;
	nd.uFlags= NIF_ICON| NIF_TIP |NIF_MESSAGE;
	nd.hIcon=LoadIcon(hInst, MAKEINTRESOURCE(101 ));
	nd.uCallbackMessage=WM_ICON_NOTIFY;
	wcscpy(nd.szTip,L"PDF locker");

	Shell_NotifyIcon(NIM_ADD,&nd)   ; 
}


	
void DeleteNotifyIcon()
{
	NOTIFYICONDATAW nd;
	nd.cbSize= sizeof(NOTIFYICONDATA);
	nd.hWnd=hWnd;
	nd.uID=NOTIFY_ICON_ID;
	nd.uFlags= NIF_ICON| NIF_TIP |NIF_MESSAGE;
	nd.hIcon=LoadIcon(hInst, MAKEINTRESOURCE(101 ));
	nd.uCallbackMessage=WM_ICON_NOTIFY;
	wcscpy(nd.szTip,L"PDF locker");
 Shell_NotifyIcon(NIM_DELETE,&nd)   ; 
}



void Sort_list()
{
	for(i=0;i<item_count;i++) //sort items
			{
						index=-1;
						min=(unsigned int)pProjectInfo_[lv_num[i]].Type;
						for(j=i+1;j<item_count;j++) 
									
						if((unsigned int)pProjectInfo_[lv_num[j]].Type>min)
							{
								min=(unsigned int)pProjectInfo_[lv_num[j]].Type;
								index=j;
							}

							if(index!=-1)
							{
								temp=lv_num[i];
								lv_num[i]=lv_num[index];	
								lv_num[index]=temp;
							}
			}
						
				ListView_DeleteAllItems(hListView);
				FillListView();
}



int GetWindowsVersion()
{////////////////ver	//////////////////////////

		OSVERSIONINFOEX osvi;
		BOOL bOsVersionInfoEx;
		ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
		 osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

		if((bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
		 {
			// Если OSVERSIONINFOEX не работает, то используем OSVERSIONINFO.
			osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
			if ( GetVersionEx ( (OSVERSIONINFO *) &osvi) )
			{
						 
			}
		}
return osvi.dwMajorVersion;
	////////////////ver	//////////////////////////
}


//////////////////////
#define MAX_FOLDER 20
WCHAR BackFolder[MAX_FOLDER][MAX_PATH];
WCHAR ForwardFolder[MAX_FOLDER][MAX_PATH];
int Back_Folder_count=0;
int Forward_Folder_count=0;

void ClearCurrentDir()
{
	memset(BackFolder,0,sizeof(BackFolder[MAX_FOLDER][MAX_PATH]));
	Back_Folder_count=0;
	memset(ForwardFolder,0,sizeof(ForwardFolder[MAX_FOLDER][MAX_PATH]));
	Forward_Folder_count=0;
}

void SaveCurrentDir()
{
	if(Back_Folder_count<MAX_FOLDER-1)
	{
		wcscpy(BackFolder[Back_Folder_count],CurrentDirectory );
		Back_Folder_count++;
	}
	else
	{
		for(int i=0;i<Back_Folder_count;i++)
			wcscpy(BackFolder[i],BackFolder[i+1] );

		wcscpy(BackFolder[Back_Folder_count-1],CurrentDirectory );
	}

	if(Back_Folder_count)
			EnableWindow(hBackButton,1);
}


void GoBackDir()
{
	if(Back_Folder_count)
	{
		//-----save current dir-----------------------
		if(Forward_Folder_count<MAX_FOLDER-1)
		{
			wcscpy(ForwardFolder[Forward_Folder_count],CurrentDirectory);
			Forward_Folder_count++;
		}	
		else
		{
			for(int i=0;i<Forward_Folder_count;i++)
				wcscpy(ForwardFolder[i],ForwardFolder[i+1] );

			wcscpy(ForwardFolder[Forward_Folder_count-1],CurrentDirectory );
		}
		if(Forward_Folder_count)
			EnableWindow(hForwardButton,1);
		//----------------------------
		
		wcscpy(CurrentDirectory,  BackFolder[Back_Folder_count-1]);									
		SetWindowTextW(hDirectory,BackFolder[Back_Folder_count-1]);
		Back_Folder_count--;							
		Fill_lists();

		if(!Back_Folder_count)
			EnableWindow(hBackButton,0);
	}
}

void GoForwardDir()
{
	
	if(Forward_Folder_count)
	{
		SaveCurrentDir();
		wcscpy(CurrentDirectory,  ForwardFolder[Forward_Folder_count-1]);									
		SetWindowTextW(hDirectory,ForwardFolder[Forward_Folder_count-1]);
		Forward_Folder_count--;							
		Fill_lists();

		if(!Forward_Folder_count)
			EnableWindow(hForwardButton,0);
	}
}
/////////////////////////


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	static SelectedItemCountTemp=-1;
	int wmId, wmEvent,i;
	HANDLE hProcess;
	PAINTSTRUCT ps;
	HDC hDC;

//	DWORD dwDisks;
//PDDoc tempPDF;
	int result=0;

	switch (message) 
	{

		case WM_ICON_NOTIFY:

				if(wParam ==NOTIFY_ICON_ID)
					switch(lParam)
				{
					case WM_MOUSEMOVE:
						//MessageBoxW(hWnd, L"Notify message WM_MOUSEMOVE", L"Warning", MB_ICONWARNING);
						break;

					case WM_LBUTTONDOWN:
						if(Visible_window)
							SendMessage(hWnd,WM_CLOSE,0,0);
						else
						{
							SendMessage(hWnd,WM_COMMAND,IDM_OPEN,0);
							Fill_lists();
						}
						//MessageBoxW(hWnd, L"Notify message WM_LBUTTONDOWN", L"Warning", MB_ICONWARNING);
						break;

					case WM_RBUTTONDOWN:
						NotifyMenu();//MessageBoxW(hWnd, L"Notify message RBUTTONDOWN", L"Warning", MB_ICONWARNING);
						break;

					default:
						//MessageBoxW(hWnd, L"Notify message default", L"Warning", MB_ICONWARNING);
						break;
				}
								
					break;
								
		case WM_SIZE:
	
				GetClientRect(hWnd, &rt);
				lw_width=rt.right-220;

				DestroyWindow(hExtension); Create_extension_window;//SetWindowPos(hExtension,	NULL, lw_width+80, rt.top+LIST_Y,		rt.right-lw_width-10-80,     25*3, SWP_NOREDRAW);   
				DestroyWindow(hSuffix); Create_suffix_window //SetWindowPos(hSuffix,		NULL, lw_width+80, rt.top+LIST_Y+40-6, rt.right-lw_width-10-80, 25*4,  SWP_NOZORDER |SWP_NOREDRAW|SWP_NOSENDCHANGING);      
				DestroyWindow(hPassword);Create_password_window; //SetWindowPos(hPassword,		NULL, lw_width+80, rt.top+LIST_Y+70,   rt.right-lw_width-10-80, 25*3,  SWP_NOZORDER |SWP_NOACTIVATE);   
				DestroyWindow(hPrefix);Create_prefix_window;
			
				
				SetWindowPos(hBackButton,	NULL, 0,			 rt.top,	23,			23, SWP_NOZORDER); 
				SetWindowPos(hForwardButton,NULL, 24,			rt.top,		23,			23, SWP_NOZORDER); 
				SetWindowPos(hDirectory,	NULL, 24*2,			rt.top ,	lw_width-70-1-24*2, 25*12, SWP_NOZORDER); 
				SetWindowPos(hBrowseButton,	NULL, lw_width-70, rt.top,		70,			23, SWP_NOZORDER);  
				SetWindowPos(hListView ,	NULL, rt.left,	   rt.top+LIST_Y,	lw_width, rt.bottom-10-LIST_Y,	SWP_NOZORDER|SWP_NOMOVE);
				SetWindowPos(hOpenButton,	NULL, lw_width+25, rt.bottom-180, 110, 25, SWP_NOZORDER);
				SetWindowPos(hCollectLanguages,NULL, lw_width+25, rt.bottom-210, 110, 25,  SWP_NOZORDER);
				SetWindowPos(hChangeButton,	NULL, lw_width+25, rt.bottom-150, 110, 25, SWP_NOZORDER);
				SetWindowPos(hLockButton,	NULL, lw_width+25, rt.bottom-120, 110, 25, SWP_NOZORDER);
				SetWindowPos(hRefreshButton,NULL, lw_width+25, rt.bottom-90, 110, 25, SWP_NOZORDER);
				SetWindowPos(hDeleteButton,	NULL, lw_width+25, rt.bottom-60, 110, 25, SWP_NOZORDER);
				SetWindowPos(hCleanButton,	NULL, lw_width+25, rt.bottom-30, 110, 25, SWP_NOZORDER);
				SetWindowPos(hCancelButton_,NULL, rt.right-75, rt.bottom-30, 70, 25,   SWP_NOZORDER);
				SetWindowPos(hProgressBar,	NULL, rt.left,	   rt.bottom-8, lw_width, 7,   SWP_NOZORDER); 
				//SetFocus(hListView);
				//SetWindowTextW(hWnd,L"PDF locker");
				//	SetWindowTextW(hWnd,L" ");
				break;
					
				
		case WM_CREATE:	

				DragAcceptFiles(hWnd,1);
				Visible_window=1;

				if(minimize_window) 
					PostMessage(hWnd,WM_CLOSE,0,0);
													
				Directory_count=0;
				selected_item=-1;
//				memset(&Pr,0,sizeof(struct project));
				GetClientRect(hWnd, &rt);

				//GetCurrentDirectoryW(sizeof(CurrentDirectory),CurrentDirectory);
				wcscpy(CurrentDirectory,L"\\");

				pProjectInfo_=0;
				pProjectInfo_=(struct project_info*)new struct project_info[MAX_PROJECT_COUNT];
				if(pProjectInfo_==0)
				{
					MessageBoxW(hWnd, L"Cannot allocate memory for project_info buffer. Please close a unused programs and try again.", L"Warning", MB_ICONWARNING);			
					return(0);	
				}
				memset(pProjectInfo_,0,sizeof(struct project_info)*MAX_PROJECT_COUNT );

				pDI=0;
				pDI=(struct dir_info*)new struct dir_info[MAX_DIRECTORY_COUNT];
					if(pDI==0)
				{
					MessageBoxW(hWnd, L"Cannot allocate memory for pDI buffer. Please close a unused programs and try again.", L"Warning", MB_ICONWARNING);			
					return(0);	
				}
				memset(pDI,0,sizeof(struct dir_info)*MAX_DIRECTORY_COUNT );

			
				/*GetModuleFileNameW(hInst,ProgramPatch,sizeof(ProgramPatch));
					for(i=MAX_PATH;i>0;i--)
						if(ProgramPatch[i]==0x5c)
						{ProgramPatch[i+1]=0;
						break;}*/

			//	GetLanguagesFromIndesign();
			//	 GetLanguageCodesFromIndesign();


				hListView = CreateWindowExW(0, WC_LISTVIEWW, L"LISTVIEW", 
						 WS_VISIBLE | WS_CHILD | WS_BORDER  | LVS_REPORT|LVS_EDITLABELS |LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES//| LVS_LIST 
                                                        |WS_VSCROLL|WS_HSCROLL, 
						 rt.left,  rt.top+LIST_Y, lw_width, rt.bottom-10-LIST_Y,  hWnd, (HMENU)0	, 
							(HINSTANCE) GetWindowLong(hWnd, GWL_HINSTANCE), NULL);  
			
				if(hListView)
				{
					lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
					for (i=0; i<COLUMN_COUNT; i++) 
					{
						if(i!=1&&i!=4)lvColumn.fmt = LVCFMT_LEFT;//LVCFMT_CENTER;//LVCFMT_RIGHT;
						else lvColumn.fmt = LVCFMT_RIGHT;
						lvColumn.iSubItem = i;
						lvColumn.cx = szCol[i]; // из массива ширин столбцов
						lvColumn.pszText =   lpCap[i]; // из массива наименований столбцов
						lvColumn.cchTextMax = wcslen(lvColumn.pszText); // необязательно
						
						ListView_InsertColumnW(hListView, i, &lvColumn);
					}
				}
				else
				{
					hList=CreateWindowW(L"LISTBOX", L"",
									WS_CHILD | WS_BORDER |WS_VISIBLE |LBS_NOTIFY |WS_TABSTOP| WS_VSCROLL | WS_BORDER,//|LBS_STANDARD,   // styles
                                          rt.left,  rt.top+LIST_Y, lw_width, rt.bottom-10-LIST_Y,,
                                          hWnd, 
										  (HMENU)LIST,  (HINSTANCE) GetWindowLong(hWnd, GWL_HINSTANCE), NULL); 
				}

													
					hBackButton=CreateWindowExW( 0     ,
							L"BUTTON", L"<",
							WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | WS_TABSTOP ,//|WS_CAPTION  ,
                              0, rt.top ,  23, 23,       hWnd,       // parent window
							(HMENU)BACK_BUTTON,  (HINSTANCE) GetWindowLong(hWnd, GWL_HINSTANCE), NULL);
					EnableWindow(hBackButton,0);

					hForwardButton=CreateWindowExW( 0     ,
							L"BUTTON", L">",
							WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | WS_TABSTOP ,//|WS_CAPTION  ,
                              24, rt.top ,  23, 23,       hWnd,       // parent window
							(HMENU)FORWARD_BUTTON,  (HINSTANCE) GetWindowLong(hWnd, GWL_HINSTANCE), NULL);
					EnableWindow(hForwardButton,0);

					
					hDirectory=CreateWindowExW( 0     ,
							L"COMBOBOX", L"",
							WS_VISIBLE|WS_TABSTOP|  WS_CHILD | WS_BORDER | CBS_DROPDOWN |WS_VSCROLL,//|WS_CAPTION  ,
                              24*2, rt.top ,  lw_width-70-1-24*2, 25*12,       hWnd,       // parent window
							(HMENU)SELECT_DIRECTORY,  (HINSTANCE) GetWindowLong(hWnd, GWL_HINSTANCE), NULL); 
					
					
					if(SHGetFolderPathW(0,CSIDL_DESKTOP,0,SHGFP_TYPE_CURRENT,&s[0])==0)
						if(Directory_count<MAX_DIRECTORY_COUNT)
							{
								wcscpy(pDI[Directory_count].Directory,L"Desktop");
								wcscpy(pDI[Directory_count].Path,s);
								Directory_count++;
							}
						/*	WCHAR ss[512];
						if(SHGetFolderPathW(0,CSIDL_DESKTOP,0,SHGFP_TYPE_CURRENT,&ss[0])==0)
						if(Directory_count<MAX_DIRECTORY_COUNT)
							{
								wcscpy(pDI[Directory_count].Directory,L"Desktop");
								wcscpy(pDI[Directory_count].Path,s);
								Directory_count++;
							}*/
					if(SHGetFolderPathW(0,CSIDL_PERSONAL,0,SHGFP_TYPE_CURRENT,&s[0])==0)	
				//	if(SHGetFolderPath(0,CSIDL_MYDOCUMENTS,0,SHGFP_TYPE_CURRENT,&s[0])==0)	
					{	if(Directory_count<MAX_DIRECTORY_COUNT)
							{
								wcscpy(pDI[Directory_count].Directory,L"My Documents");
								wcscpy(pDI[Directory_count].Path,s);
								Directory_count++;
							}
					}	
				


					if(SHGetFolderPathW(0,CSIDL_DRIVES,0,SHGFP_TYPE_CURRENT,&s[0])==0)	
					{	if(Directory_count<MAX_DIRECTORY_COUNT)
							{
								wcscpy(pDI[Directory_count].Directory,L"My Computer");
								wcscpy(pDI[Directory_count].Path,s);
								Directory_count++;
							}
					}	
					else
						if(Directory_count<MAX_DIRECTORY_COUNT)
						{
							wcscpy(pDI[Directory_count].Directory,L"My Computer");
							wcscpy(pDI[Directory_count].Path,L"\\");
							Directory_count++;
						}

					ZeroMemory(DiskBuffer,sizeof(DiskBuffer));
					if(GetLogicalDriveStringsW(sizeof(DiskBuffer),(WCHAR*)&DiskBuffer))
					{	
						i=0;
						WCHAR VolumeNameBuffer[0x80];
						WCHAR FileSystemNameBuffer[0x80];
						DWORD Serial,MaxPath;
						//DWORD DriveType;
						//char DiskType[32];
						while(DiskBuffer[i][0]>0)
						{
							//DriveType=GetDriveType((const char*)&DiskBuffer[i]);
							VolumeNameBuffer[0]=0;
							GetVolumeInformationW((const WCHAR*)&DiskBuffer[i],VolumeNameBuffer,0x80,
															0,&Serial,&MaxPath,
															FileSystemNameBuffer,0x80);
							DiskBuffer[i][2]=0;	
						
							//wsprintf(s,"%s (%s) {%s} [%s]" ,DiskBuffer[i],VolumeNameBuffer, DiskType,FileSystemNameBuffer);
							swprintf(s,L"%s   %s " ,DiskBuffer[i],VolumeNameBuffer);

							if(Directory_count<MAX_DIRECTORY_COUNT)
							{
								wcscpy(pDI[Directory_count].Directory,s);
								wcscpy(pDI[Directory_count].Path,DiskBuffer[i]);
								Directory_count++;
							}

							i++;//i+=4;
						}
					}
						for(i=0;i<Directory_count;i++)
							SendMessageW( (HWND)hDirectory , CB_ADDSTRING, (WPARAM)0, (LPARAM)(LPTSTR)pDI[i].Directory);
						
					
					SendMessage(hDirectory,CB_SETCURSEL ,-1, 0 );
					SetWindowTextW(hDirectory,L"\\");//CurrentDirectory);


						
					hBrowseButton  = CreateWindowExW(WS_EX_CONTROLPARENT        ,
					  L"BUTTON",     L"Browse",       // button text
						WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | WS_TABSTOP ,  // styles
						lw_width-70, rt.top, 70, 23,       hWnd,       // parent window
						(HMENU)BROWSE_BUTTON,  (HINSTANCE) GetWindowLong(hWnd, GWL_HINSTANCE), NULL); 
					
					
				/*	if(RichEdit)
					{
						hExtension= CreateWindowExW(0, MSFTEDIT_CLASS, L"*.pdf",
								ES_MULTILINE | WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP| WS_VSCROLL, 
								rt.right-85, rt.top, 80, 23,        hWnd,       // parent window
							(HMENU)-1,  (HINSTANCE) GetWindowLong(hWnd, GWL_HINSTANCE), NULL); 
					
						//RE_SetFont(hPopupTextWnd, 16,L"Arial");
				
					}
					else*/
				
					hCollectLanguages = CreateWindowExW(WS_EX_CONTROLPARENT        ,
					  L"BUTTON",     L"Collect lang.",       // button text
						WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | WS_TABSTOP ,  // styles
						lw_width+5, rt.bottom-210, 110, 25,       hWnd,       // parent window
						(HMENU)COLLECT_BUTTON,  (HINSTANCE) GetWindowLong(hWnd, GWL_HINSTANCE), NULL); 
					
					hOpenButton  = CreateWindowExW(WS_EX_CONTROLPARENT        ,
					  L"BUTTON",     L"Zip Folder",       // button textL"Open file",       // button text
						WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | WS_TABSTOP ,  // styles
						lw_width+5, rt.bottom-180, 110, 25,       hWnd,       // parent window
						(HMENU)OPEN_BUTTON,  (HINSTANCE) GetWindowLong(hWnd, GWL_HINSTANCE), NULL); 

					
					hChangeButton= CreateWindowExW(WS_EX_CONTROLPARENT        ,
					  L"BUTTON",     L"Unlock file",       // button text
						WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | WS_TABSTOP ,  // styles
						lw_width+5, rt.bottom-150, 110, 25,       hWnd,       // parent window
						(HMENU)UNLOCK_BUTTON,  (HINSTANCE) GetWindowLong(hWnd, GWL_HINSTANCE), NULL); 
					/*	hChangeButton= CreateWindowExW(WS_EX_CONTROLPARENT        ,
					  L"BUTTON",     L"Set initial view",       // button text
						WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | WS_TABSTOP ,  // styles
						lw_width+5, rt.bottom-150, 110, 25,       hWnd,       // parent window
						(HMENU)CHANGE_BUTTON,  (HINSTANCE) GetWindowLong(hWnd, GWL_HINSTANCE), NULL); */


					hLockButton  = CreateWindowExW(WS_EX_CONTROLPARENT        ,
					  L"BUTTON",     L"Lock file",       // button text
						WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | WS_TABSTOP ,  // styles
						lw_width+5, rt.bottom-120, 110, 25,       hWnd,       // parent window
						(HMENU)LOCK_BUTTON,  (HINSTANCE) GetWindowLong(hWnd, GWL_HINSTANCE), NULL); 

				
					hRefreshButton= CreateWindowExW(WS_EX_CONTROLPARENT        ,
					  L"BUTTON",     L"Refresh list",       // button text
						WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | WS_TABSTOP ,  // styles
						lw_width+5, rt.bottom-90,110,25,       hWnd,       // parent window
						(HMENU)REFRESH_BUTTON,  (HINSTANCE) GetWindowLong(hWnd, GWL_HINSTANCE), NULL); 

					
					
					hDeleteButton = CreateWindowExW(WS_EX_CONTROLPARENT        ,
					  L"BUTTON",     L"Delete file",       // button text
						WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | WS_TABSTOP ,  // styles
						lw_width+5, rt.bottom-60, 110, 25,       hWnd,       // parent window
						(HMENU)DELETE_BUTTON,  (HINSTANCE) GetWindowLong(hWnd, GWL_HINSTANCE), NULL); 


					hCleanButton = CreateWindowExW(WS_EX_CONTROLPARENT        ,
					  L"BUTTON",     L"Clean file",       // button text
						WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | WS_TABSTOP ,  // styles
						lw_width+5, rt.bottom-30, 110, 25,       hWnd,       // parent window
						(HMENU)CLEAN_BUTTON , (HINSTANCE) GetWindowLong(hWnd, GWL_HINSTANCE), NULL); 
					
#ifdef _DEBUG	
					hCancelButton_  = CreateWindowExW(WS_EX_CONTROLPARENT        ,
					  L"BUTTON",     L"Close",       // button text
						WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | WS_TABSTOP ,  // styles
						rt.right-75, rt.bottom-30, 70, 25,       hWnd,       // parent window
						(HMENU)EXIT_BUTTON,  (HINSTANCE) GetWindowLong(hWnd, GWL_HINSTANCE), NULL); 
#endif
					
					hProgressBar=CreateWindowW( PROGRESS_CLASS        ,
							  L"progress bar",  WS_VISIBLE | WS_CHILD  , 
							   rt.left, rt.bottom-8,
							 lw_width, 7,       
							 hWnd,   NULL,  (HINSTANCE) GetWindowLong(hWnd,0), NULL); 
						SendMessage( hProgressBar, PBM_SETRANGE, 0, MAKELPARAM(0,100));
						SendMessage( hProgressBar, PBM_SETSTEP, 1, 0);
						SendMessage( hProgressBar, PBM_SETPOS, 0, 0);
						ShowWindow(hProgressBar, SW_HIDE);
				
						
					Create_extension_window;

					Create_suffix_window;
					Create_password_window;

					Create_prefix_window;
			 
					
						
					

					AddNotifyIcon(hWnd);
					
					Fill_lists();
					Sort_list();

					hProcess = GetCurrentProcess();
					SetPriorityClass (hProcess, HIGH_PRIORITY_CLASS);//IDLE_PRIORITY_CLASS )
			
			

				////////////////ver	//////////////////////////
//				if(GetWindowsVersion()>=6) VISTA=1;
				////////////////ver	//////////////////////////

				//GetUserNameA( UserName,&User_name_len);
					
			//	if(connect_to_server)
				//	hThreadSend=CreateThread( NULL, 0,SendThread, NULL, 0, &dwIdSend );
			
		
			//	if(InitRichEdit()) RichEdit=1;
				//ProjectWindow=1;
				ClearCurrentDir();
					
				break;
			
				
			case WM_NOTIFY:
						
			lpnmh = (LPNMHDR) lParam;
					
				switch(lpnmh->code)
				 {
				
				case NM_RCLICK:
						Menu();
	
					//RightClickMenu();
				break;
				
					case 	4294967280:
					case 	4294967281:
					case 	4294967282:
					case	4294967284:
					case 	4294967196:
					case 	4294967195:
					case 	4294967194:
					case 	4294967294:
					case 	4294967289:

					//-----select multiple items-----------
						SelectedItemCount=ListView_GetSelectedCount(hListView);	 

						if(SelectedItemCount>1)
						{
							if(SelectedItemCount!=SelectedItemCountTemp)
							{
								int item_index=-1;
								for(int d=0; d<SelectedItemCount;d++)
								{
									item_index = ListView_GetNextItem(hListView,item_index, LVNI_SELECTED);
									 selected_items[d]=lv_num[item_count-1-item_index];
									 //selected_items[d]=lv_num[item_index];

									 //pProjectInfo_[item_index].FileName
									//pProjectInfo_[	 selected_items[d]].FileName
								}
								SelectedItemCountTemp=SelectedItemCount;
							}
						}
						else
							SelectedItemCountTemp=-1;
						//-----select multiple items-------------

							i=ListView_GetSelectionMark(hListView);
							if(i!=-1)
							if(selected_item!=i)
							{
								selected_item=i;
								//SelectedItem = item_count-1-lv_num[i];
								

								SelectedItem = lv_num[item_count-1-i]; 
								//SelectedItem = lv_num[i]; 
								//selected_items[0]= lv_num[item_count-1-i]; 

								//selected_items[0]=SelectedItem;
									//LockFile();
							//	strcpy(s,ProjectPath);
							//	strcat(s,"\\");
							//	strcat(s,pProjectInfo_[	SelectedItem].FileName);
							//	GetInfoFromProject(s);

							//	GetClientRect(hControlProjectsWnd, &rt);
							//rt.top=		0
								//rt.bottom=	0
							//	rt.left=lw_width;
								//rt.right=	0							
							
							//	InvalidateRect(hControlProjectsWnd, &rt, TRUE );
							}
								
					  //return 0L;
					  break;
					
					
				
					case LVN_ITEMACTIVATE:
														
								SendMessage(hWnd, WM_COMMAND,DOUBLE_CLICK_,0);
						
							
						//return 0;
						break;

					case LVN_BEGINLABELEDITW:
					
					  return 0L;
					  break;
				

					case LVN_ENDLABELEDITW:
					//case LVN_ENDLABELEDIT:

						 lpLvdi = (LV_DISPINFOW *)lParam;;//pnmhdr;

						 if((lpLvdi->item.iItem != -1) &&
							(lpLvdi->item.pszText != NULL))
						  {
							  index = ListView_GetNextItem(hListView,-1, LVNI_SELECTED);
							//SelectedItem = item_count-1-lv_num[index]; 
							 SelectedItem = lv_num[item_count-1-index]; 
							 //	SelectedItem = lv_num[index]; 
							
							if(SelectedItem>=0&&SelectedItem<item_count)
							{				
											//SetCurrentDirectory(ProjectPath);

											wcscpy(s,CurrentDirectory);
											wcscat(s,L"\\");
											wcscat(s,lpLvdi->item.pszText);
											
											wcscpy(s2,CurrentDirectory);
											wcscat(s2,L"\\");
											wcscat(s2,pProjectInfo_[SelectedItem].FileName);
										
											int result=MoveFileW(s2,s);
											if(!result)
											{
												/*(GetLastError()==183)
												{
													if(MessageBoxW(hListView, GetString(L"IDS_The_file_already_exists"),GetString(L"IDS_File_exist") , MB_YESNO)==IDYES)
													{
															for(i=0;i<item_count;i++)
															if(!strcmp(pProjectInfo_[lv_num[i]].FileName,lpLvdi->item.pszText))
															{
																strcpy(pProjectInfo_[	lv_num[i]].FileName," ");
																int SelectedItem = lv_num[item_count-1-i]; 
																
																ListView_SetItemText(hListView, SelectedItem, 0, " "); 
																ListView_SetItemText(hListView, SelectedItem, 1, " "); 
																ListView_SetItemText(hListView, SelectedItem, 2, " ");
																ListView_SetItemText(hListView, SelectedItem, 3, " "); 
																ListView_SetItemText(hListView, SelectedItem, 4, " ");
															break;
															}
											
														if(DeleteFile(s))
														result=MoveFile(s2,s);
													}
												}
												else
													Show_error();*/
									
									}
											if(result==1)
												{	
													wcscpy(pProjectInfo_[SelectedItem].FileName,lpLvdi->item.pszText);
													ListView_SetItemTextW(hListView, index, 0, lpLvdi->item.pszText); 
													/*#define ListView_SetItemText(hwndLV, i, iSubItem_, pszText_) 
													{ LV_ITEMW _ms_lvi;\
													  _ms_lvi.iSubItem = i;\
													  _ms_lvi.pszText = lpLvdi->item.pszText;\
													  SNDMSG((hListView), LVM_SETITEMTEXT, (WPARAM)(i), (LPARAM)(LV_ITEM *)&_ms_lvi);\
													}*/
												}
									SetFocus(hListView);	
								
						}
					  }

					  return 0L;
					  
					  break;


					case LVN_COLUMNCLICK:
						{
				
						NMLISTVIEW *pLV = (NMLISTVIEW *)lParam;

					
						  switch(pLV->iSubItem)
								{
								

								case 0:	// ProdName  pProjectInfo_

									sort_order=sort_order^1;
									memset(s,0,32);
									for(i=0;i<item_count;i++) //sort items
									{
										index=-1;
										wcscpy(s, pProjectInfo_[lv_num[i]].FileName);
										for(j=i+1;j<item_count;j++) 
										if(sort_order)
										{
											if(pProjectInfo_[lv_num[j]].FileName[0]<s[0])
											{
												s[0]=pProjectInfo_[lv_num[j]].FileName[0];
												index=j;
											}

											if(pProjectInfo_[lv_num[j]].FileName[0]==s[0])
											{
												if(pProjectInfo_[lv_num[j]].FileName[1]<s[1])
												{
													s[1]=pProjectInfo_[lv_num[j]].FileName[1];
													index=j;
												}

												if(pProjectInfo_[lv_num[j]].FileName[1]==s[1])
												{
													if(pProjectInfo_[lv_num[j]].FileName[2]<s[2])
													{
														s[2]=pProjectInfo_[lv_num[j]].FileName[2];
														index=j;
													}
													if(pProjectInfo_[lv_num[j]].FileName[2]==s[2])
													{
														if(pProjectInfo_[lv_num[j]].FileName[3]<s[3])
														{
															s[3]=pProjectInfo_[lv_num[j]].FileName[3];
															index=j;
														}
														if(pProjectInfo_[lv_num[j]].FileName[3]==s[3])
														{
															if(pProjectInfo_[lv_num[j]].FileName[4]<s[4])
															{
																s[4]=pProjectInfo_[lv_num[j]].FileName[4];
																index=j;
															}
															if(pProjectInfo_[lv_num[j]].FileName[4]==s[4])
															{
																if(pProjectInfo_[lv_num[j]].FileName[5]<s[5])
																{
																	s[5]=pProjectInfo_[lv_num[j]].FileName[5];
																	index=j;
																}
																if(pProjectInfo_[lv_num[j]].FileName[5]==s[5])
																{
																	if(pProjectInfo_[lv_num[j]].FileName[6]<s[6])
																	{
																		s[6]=pProjectInfo_[lv_num[j]].FileName[6];
																		index=j;
																	}
																	if(pProjectInfo_[lv_num[j]].FileName[6]==s[6])
																	{
																		if(pProjectInfo_[lv_num[j]].FileName[7]<s[7])
																		{
																			s[7]=pProjectInfo_[lv_num[j]].FileName[7];
																			index=j;
																		}

																		if(pProjectInfo_[lv_num[j]].FileName[7]==s[7])
																		{
																			if(pProjectInfo_[lv_num[j]].FileName[8]<s[8])
																			{
																				s[8]=pProjectInfo_[lv_num[j]].FileName[8];
																				index=j;
																			}
																			if(pProjectInfo_[lv_num[j]].FileName[8]==s[8])
																			{
																				if(pProjectInfo_[lv_num[j]].FileName[9]<s[9])
																				{
																					s[9]=pProjectInfo_[lv_num[j]].FileName[9];
																					index=j;
																				}
																				if(pProjectInfo_[lv_num[j]].FileName[9]==s[9])
																				{
																					if(pProjectInfo_[lv_num[j]].FileName[10]<s[10])
																					{
																						s[10]=pProjectInfo_[lv_num[j]].FileName[10];
																						index=j;
																					}
																					if(pProjectInfo_[lv_num[j]].FileName[10]==s[10])
																					{
																						if(pProjectInfo_[lv_num[j]].FileName[11]<s[11])
																						{
																							s[11]=pProjectInfo_[lv_num[j]].FileName[11];
																							index=j;
																						}
																						if(pProjectInfo_[lv_num[j]].FileName[11]==s[11])
																						{
																							if(pProjectInfo_[lv_num[j]].FileName[12]<s[12])
																							{
																								s[12]=pProjectInfo_[lv_num[j]].FileName[12];
																								index=j;
																							}
																							if(pProjectInfo_[lv_num[j]].FileName[12]==s[12])
																							{
																								if(pProjectInfo_[lv_num[j]].FileName[13]<s[13])
																								{
																									s[13]=pProjectInfo_[lv_num[j]].FileName[13];
																									index=j;
																								}
																								if(pProjectInfo_[lv_num[j]].FileName[13]==s[13])
																								{
																									if(pProjectInfo_[lv_num[j]].FileName[14]<s[14])
																									{
																										s[14]=pProjectInfo_[lv_num[j]].FileName[14];
																										index=j;
																									}
																									if(pProjectInfo_[lv_num[j]].FileName[14]==s[14])
																									{
																										if(pProjectInfo_[lv_num[j]].FileName[15]<s[15])
																										{
																											s[15]=pProjectInfo_[lv_num[j]].FileName[15];
																											index=j;
																										}
																									}
																								}
																							}
																						}
																					}
																				}
																			}
																		}
																	}
																}
															}
														}
													}
												}
											}
										}
										else
										{
											if(pProjectInfo_[lv_num[j]].FileName[0]>s[0])
											{
												s[0]=pProjectInfo_[lv_num[j]].FileName[0];
												index=j;
											}

											if(pProjectInfo_[lv_num[j]].FileName[0]==s[0])
											{
												if(pProjectInfo_[lv_num[j]].FileName[1]>s[1])
												{
													s[1]=pProjectInfo_[lv_num[j]].FileName[1];
													index=j;
												}

												if(pProjectInfo_[lv_num[j]].FileName[1]==s[1])
												{
													if(pProjectInfo_[lv_num[j]].FileName[2]>s[2])
													{
														s[2]=pProjectInfo_[lv_num[j]].FileName[2];
														index=j;
													}
													if(pProjectInfo_[lv_num[j]].FileName[2]==s[2])
													{
														if(pProjectInfo_[lv_num[j]].FileName[3]>s[3])
														{
															s[3]=pProjectInfo_[lv_num[j]].FileName[3];
															index=j;
														}
														if(pProjectInfo_[lv_num[j]].FileName[3]==s[3])
														{
															if(pProjectInfo_[lv_num[j]].FileName[4]>s[4])
															{
																s[4]=pProjectInfo_[lv_num[j]].FileName[4];
																index=j;
															}
															if(pProjectInfo_[lv_num[j]].FileName[4]==s[4])
															{
																if(pProjectInfo_[lv_num[j]].FileName[5]>s[5])
																{
																	s[5]=pProjectInfo_[lv_num[j]].FileName[5];
																	index=j;
																}
																if(pProjectInfo_[lv_num[j]].FileName[5]==s[5])
																{
																	if(pProjectInfo_[lv_num[j]].FileName[6]>s[6])
																	{
																		s[6]=pProjectInfo_[lv_num[j]].FileName[6];
																		index=j;
																	}
																	if(pProjectInfo_[lv_num[j]].FileName[6]==s[6])
																	{
																		if(pProjectInfo_[lv_num[j]].FileName[7]>s[7])
																		{
																			s[7]=pProjectInfo_[lv_num[j]].FileName[7];
																			index=j;
																		}
																		if(pProjectInfo_[lv_num[j]].FileName[7]==s[7])
																		{
																			if(pProjectInfo_[lv_num[j]].FileName[8]>s[8])
																			{
																				s[8]=pProjectInfo_[lv_num[j]].FileName[8];
																				index=j;
																			}
																			if(pProjectInfo_[lv_num[j]].FileName[8]==s[8])
																			{
																				if(pProjectInfo_[lv_num[j]].FileName[9]>s[9])
																				{
																					s[9]=pProjectInfo_[lv_num[j]].FileName[9];
																					index=j;
																				}
																				if(pProjectInfo_[lv_num[j]].FileName[9]==s[9])
																				{
																					if(pProjectInfo_[lv_num[j]].FileName[10]>s[10])
																					{
																						s[10]=pProjectInfo_[lv_num[j]].FileName[10];
																						index=j;
																					}
																					if(pProjectInfo_[lv_num[j]].FileName[10]==s[10])
																					{
																						if(pProjectInfo_[lv_num[j]].FileName[11]>s[11])
																						{
																							s[11]=pProjectInfo_[lv_num[j]].FileName[11];
																							index=j;
																						}
																						if(pProjectInfo_[lv_num[j]].FileName[11]==s[11])
																						{
																							if(pProjectInfo_[lv_num[j]].FileName[12]>s[12])
																							{
																								s[12]=pProjectInfo_[lv_num[j]].FileName[12];
																								index=j;
																							}
																							if(pProjectInfo_[lv_num[j]].FileName[12]==s[12])
																							{
																								if(pProjectInfo_[lv_num[j]].FileName[13]>s[13])
																								{
																									s[13]=pProjectInfo_[lv_num[j]].FileName[13];
																									index=j;
																								}
																								if(pProjectInfo_[lv_num[j]].FileName[13]==s[13])
																								{
																									if(pProjectInfo_[lv_num[j]].FileName[14]>s[14])
																									{
																										s[14]=pProjectInfo_[lv_num[j]].FileName[14];
																										index=j;
																									}
																								}
																							}
																						}
																					}
																				}
																			}
																		}
																	}
																}
															}
														}
													}
												}
											}
										}//else

										if(index!=-1)
										{
											temp=lv_num[i];
											lv_num[i]=lv_num[index];	
											lv_num[index]=temp;
										}
									}//	for(i=0;i<item_count;i++) //sort items
									break;


							case 1: ////sort FileSize
									sort_order=sort_order^1;
									for(i=0;i<item_count;i++) 
									{
										index=-1;
										min=pProjectInfo_[lv_num[i]].FileSize;
										for(j=i+1;j<item_count;j++) 
										if(sort_order)
										{
											if(pProjectInfo_[lv_num[j]].FileSize<(int)min)
											{
												min=pProjectInfo_[lv_num[j]].FileSize;
												index=j;
											}
										}
										else
											if(pProjectInfo_[lv_num[j]].FileSize>(int)min)
											{
												min=pProjectInfo_[lv_num[j]].FileSize;
												index=j;
											}

										if(index!=-1)
										{
											temp=lv_num[i];
											lv_num[i]=lv_num[index];	
											lv_num[index]=temp;
										}
									}
								
									break;
								
							case 3:	// date	
									sort_order=sort_order^1;
									for(i=0;i<item_count;i++) //sort items
									if(pProjectInfo_[lv_num[i]].Type>1)
									{
										index=-1;
										
										min=pProjectInfo_[lv_num[i]].Data.Data1int;
										min2=pProjectInfo_[lv_num[i]].Time.Time1int;
										
										for(j=i+1;j<item_count;j++)
										if(pProjectInfo_[lv_num[i]].Type>1)
										{
											if(sort_order)
											{
												if(pProjectInfo_[lv_num[j]].Data.Data1int<min)
												{
													min=pProjectInfo_[lv_num[j]].Data.Data1int;
													min2=pProjectInfo_[lv_num[i]].Time.Time1int;
													index=j;
												}
												if(pProjectInfo_[lv_num[j]].Data.Data1int==min)
												{
													if(pProjectInfo_[lv_num[j]].Time.Time1int<min2)
													{
														min=pProjectInfo_[lv_num[j]].Data.Data1int;
														min2=pProjectInfo_[lv_num[i]].Time.Time1int;
														index=j;
													}
												}
											}
											else
											{
												if(pProjectInfo_[lv_num[j]].Data.Data1int>min)
												{
													min=pProjectInfo_[lv_num[j]].Data.Data1int;
													min2=pProjectInfo_[lv_num[i]].Time.Time1int;
													index=j;
												}
												if(pProjectInfo_[lv_num[j]].Data.Data1int==min)
												{
													if(pProjectInfo_[lv_num[j]].Time.Time1int>min2)
													{
														min=pProjectInfo_[lv_num[j]].Data.Data1int;
														min2=pProjectInfo_[lv_num[i]].Time.Time1int;
														index=j;
													}
												}
											}

											if(index!=-1)
											{
												temp=lv_num[i];
												lv_num[i]=lv_num[index];	
												lv_num[index]=temp;
												index=-1;
											}
										}
									}
		
									break;

								case 2:	// type	
									sort_order=sort_order^1;
									for(i=0;i<item_count;i++) //sort items
									{
										index=-1;
										min=(unsigned int)pProjectInfo_[lv_num[i]].Type;
										for(j=i+1;j<item_count;j++) 
										if(sort_order)
										{
											if((unsigned int)pProjectInfo_[lv_num[j]].Type<min)
											{
												min=(unsigned int)pProjectInfo_[lv_num[j]].Type;
												index=j;
											}
										}
										else
											if((unsigned int)pProjectInfo_[lv_num[j]].Type>min)
											{
												min=(unsigned int)pProjectInfo_[lv_num[j]].Type;
												index=j;
											}

										if(index!=-1)
										{
											temp=lv_num[i];
											lv_num[i]=lv_num[index];	
											lv_num[index]=temp;
										}
									}
		
									break;

						
								default:
									break;
								}
									ListView_DeleteAllItems(hListView);

									FillListView();
										
										
									 
										
									InvalidateRect( hWnd, NULL, TRUE );  
						  //return 0L;
						  break;
						  }

					default:
						break;
					
				}
				break;


		
		case WM_CLOSE:
				
				Visible_window=0;
				ShowWindow(hWnd, SW_HIDE);
				break;
				
		case WM_DESTROY:
				//ProjectWindow=0;
				if(pDI) {delete(pDI);pDI=0;}
				if(pProjectInfo_) {delete(pProjectInfo_);pProjectInfo_=0;}
				DestroyTableImageList();
				DeleteNotifyIcon();
			
				PostQuitMessage(0);
				break;

	/*	case WM_SHOWWINDOW:
					
			if(wParam)
				Visible_window=1;
			else
				Visible_window=0;
			break;*/
		
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{

				case IDM_MINIMIZE:
					SendMessage(hWnd,WM_CLOSE,0,0);
					break;

				case IDM_OPEN:

					//SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);
					Visible_window=1;
					SetFocus(hWnd);
					//ShowWindow(hWnd,SW_SHOWNORMAL);			
					ShowWindow(hWnd,SW_NORMAL);
					ShowWindow(hWnd,SW_RESTORE);
					Fill_lists();
					break;
					
				case LIST:

						if(HIWORD(wParam)==LBN_SELCHANGE) 
						{
							i=SendMessage(hList, LB_GETCURSEL, 0, 0 );
								if(i!=-1)
								if(selected_item!=i)
								{
									selected_item=i;
									//SelectedItem = item_count-1-lv_num[i]; 
									SelectedItem = lv_num[item_count-1-i]; 
									//LockFile();
								}
						}

						if(HIWORD(wParam)==LBN_DBLCLK )
							SendMessage(hWnd, WM_COMMAND,DOUBLE_CLICK_,0);
						break;

				case EXTENSION_ACTIVE:
					if(HIWORD(wParam)==CBN_SELCHANGE) 
					{
							i=SendMessage(hExtension, CB_GETCURSEL, 0, 0 );
							if(i>=0)
							{
								current_extension=i;
								if(i==0)
									SetWindowTextW(hExtension,L"*.pdf");
								if(i==1)
									SetWindowTextW(hExtension,L"*.in??" );
								if(i==2)
									SetWindowTextW(hExtension,L"*.*" );

								Fill_lists();
							}
					}
						break;

				case SUFFIX_ACTIVE:
					if(HIWORD(wParam)==CBN_SELCHANGE) 
					{
							i=SendMessage(hSuffix, CB_GETCURSEL, 0, 0 );
							if(i>=0)
							{
							current_suffix=i;	
							}
					}
						break;

				case PASSWORD_ACTIVE:
					if(HIWORD(wParam)==CBN_SELCHANGE) 
					{
							i=SendMessage(hSuffix, CB_GETCURSEL, 0, 0 );
							if(i>=0)
							{
							current_password=i;	
							}
					}
						break;

				case BACK_BUTTON:
						GoBackDir();
					break;

				case FORWARD_BUTTON:
					GoForwardDir();
					break;


				case SELECT_DIRECTORY:

						if(HIWORD(wParam)==CBN_SELCHANGE) 
						{
							i=SendMessage(hDirectory, CB_GETCURSEL, 0, 0 );
								if(i!=-1)
								//if(selected_item!=i)
								{
									SaveCurrentDir();
									SendMessage(hDirectory,CB_GETLBTEXT,i,(long)s);
								
									wcscpy(CurrentDirectory,pDI[i].Path);
									//strcpy(CurrentDirectory,s);
									
									SetWindowTextW(hDirectory,pDI[i].Directory);
								
									Fill_lists();
									SetFocus(hWnd);
								}
							//selected_item=-1;
						}
						break;

				case PREFIX:

					if(HIWORD(wParam)==CBN_SELCHANGE) 
					{
							i=SendMessage(hPrefix, CB_GETCURSEL, 0, 0 );
							if(i>=0)
							{
							current_prefix=i;	
							}
					}
					/*	if(IsDlgButtonChecked(hWnd,PREFIX))
							PREFIX=1;
						else
							PREFIX=0;*/
						break;
					
				case CHANGE_BUTTON:
						
						//if(selected_item!=-1)
						if(SelectedItemCount)
							ChangePDFViewProperties2();
							break;

				case UNLOCK_BUTTON:
						if(SelectedItemCount)
								{
									//SelectedItem = item_count-1-lv_num[selected_item]; 
									SelectedItem = lv_num[item_count-1-selected_item]; 
								//	SelectedItem = lv_num[selected_item]; 
								
									wcscpy(source_file,CurrentDirectory);
									wcscat(source_file,L"\\");
									wcscat(source_file,pProjectInfo_[SelectedItem].FileName);
	
									if(file_count&&pProjectInfo_[SelectedItem].Type>=dir_count)//nt dir_count=0,disk_count=0;
									{
										//strcpy(CurrentDirectory,pDI[SelectedItem].Path);
										DWORD dwId;
										HANDLE	hLockFileThread = CreateThread( NULL, 0, UnLockFileThread, NULL, 0, &dwId );
										//LockFile();
									}
									 
 
								}	
								 
								 
					break;

				case DELETE_BUTTON:

							if(SelectedItemCount)
								if(MessageBoxW(hWnd, L"Are you sure you want to delete file(s)?",L"Delete File" , MB_YESNO)==IDYES)
								{
									DeleteFile_();//DeleteFile(SaveTranslatedFileName);
									Fill_lists();
								}
							break;
					
				case OPEN_BUTTON:

						//if(selected_item!=-1)
						if(SelectedItemCount)
							{
									//SelectedItem = item_count-1-lv_num[selected_item]; 
									SelectedItem = lv_num[item_count-1-selected_item]; 
								//	SelectedItem = lv_num[selected_item]; 
								
									wcscpy(source_file,CurrentDirectory);
									wcscat(source_file,L"\\");
									wcscat(source_file,pProjectInfo_[SelectedItem].FileName);
	
									if(SelectedItemCount==1&&pProjectInfo_[SelectedItem].FileSize==0||
										SelectedItemCount>1&&pProjectInfo_[selected_items[0]].FileSize==0)//nt dir_count=0,disk_count=0;
									{
										//strcpy(CurrentDirectory,pDI[SelectedItem].Path);
										DWORD dwId;
										HANDLE	hLockFileThread = CreateThread( NULL, 0, ZipFolderThread, NULL, 0, &dwId );
										//LockFile();
									}else
									MessageBoxW(hWnd, L"Is that a Folder? \n\nAre you sure?", L"Warning", MB_ICONQUESTION);
 
								}	
								else
									MessageBoxW(hWnd, L"Nothing selected.", L"Warning", MB_ICONWARNING);
							//OpenFile();
							break;

				case REPAIR_BUTTON:

							if(SelectedItemCount)
							{
								//RepairFile();
								Fill_lists();
							}
							break;

				case LOCK_BUTTON:
						
							//	if(selected_item!=-1)
								if(SelectedItemCount)
								{
									//SelectedItem = item_count-1-lv_num[selected_item]; 
									SelectedItem = lv_num[item_count-1-selected_item]; 
								//	SelectedItem = lv_num[selected_item]; 
								
									wcscpy(source_file,CurrentDirectory);
									wcscat(source_file,L"\\");
									wcscat(source_file,pProjectInfo_[SelectedItem].FileName);
	
									if(file_count&&pProjectInfo_[SelectedItem].Type>=dir_count)//nt dir_count=0,disk_count=0;
									{
										//strcpy(CurrentDirectory,pDI[SelectedItem].Path);
										DWORD dwId;
										HANDLE	hLockFileThread = CreateThread( NULL, 0, LockFileThread, NULL, 0, &dwId );
										//LockFile();
									}

									 //Get_md5(source_file);
								}
									break;

				case COLLECT_BUTTON:
						
							//	if(selected_item!=-1)
								if(SelectedItemCount)
								{
									//SelectedItem = item_count-1-lv_num[selected_item]; 
									SelectedItem = lv_num[item_count-1-selected_item]; 
								//	SelectedItem = lv_num[selected_item]; 
									if(file_count&&pProjectInfo_[SelectedItem].Type>=dir_count)//nt dir_count=0,disk_count=0;
								//	if(pProjectInfo_[SelectedItem].FileName[wcslen(pProjectInfo_[SelectedItem].FileName)-1]=='X'||
									//	pProjectInfo_[SelectedItem].FileName[wcslen(pProjectInfo_[SelectedItem].FileName)-1]=='x')
									{
										  Collect();
											

												//wcscpy(source_file,CurrentDirectory);
												//wcscat(source_file,L"\\");
												//wcscat(source_file,pProjectInfo_[selected_items[j]].FileName);
												
												//wcscpy(source_file,CurrentDirectory);
												//wcscat(source_file,L"\\");
												//wcscat(source_file,pProjectInfo_[SelectedItem].FileName);

											//	LockFile2(source_file);
										 
											 

											Fill_lists();

									/*WCHAR inx_file[MAX_PATH];
										wcscpy(inx_file,CurrentDirectory);
										wcscat(inx_file,L"\\");
										wcscat(inx_file,pProjectInfo_[SelectedItem].FileName);

										ConvertINXFile(inx_file);
										// FixLinkInINDD(inx_file);
										Fill_lists();*/
									}
									//else
									//	MessageBoxW(hWnd, L"Only INX files are supported.", L"Warning", MB_ICONWARNING);	
								}
									break;

				case CLEAN_BUTTON:
						if(SelectedItemCount)
								{
									//SelectedItem = item_count-1-lv_num[selected_item]; 
									SelectedItem = lv_num[item_count-1-selected_item]; 
								//	SelectedItem = lv_num[selected_item]; 
									if(file_count&&pProjectInfo_[SelectedItem].Type>=dir_count)//nt dir_count=0,disk_count=0;
									if(pProjectInfo_[SelectedItem].FileName[wcslen(pProjectInfo_[SelectedItem].FileName)-1]=='X'||
										pProjectInfo_[SelectedItem].FileName[wcslen(pProjectInfo_[SelectedItem].FileName)-1]=='x')
									{
										WCHAR inx_file[MAX_PATH];
										wcscpy(inx_file,CurrentDirectory);
										wcscat(inx_file,L"\\");
										wcscat(inx_file,pProjectInfo_[SelectedItem].FileName);

									//	ReplaceFont(inx_file);// 
										CleanINXFile(inx_file);
										Fill_lists();
									}
									else
										MessageBoxW(hWnd, L"Only INX files are supported.", L"Warning", MB_ICONWARNING);	
								}
									break;

				case DOUBLE_CLICK_:
						
								if(selected_item!=-1)
								{
									//SelectedItem = item_count-1-lv_num[selected_item]; 
									SelectedItem = lv_num[item_count-1-selected_item]; 
								//	SelectedItem = lv_num[selected_item]; 
									if(file_count&&pProjectInfo_[SelectedItem].Type>=dir_count)//nt dir_count=0,disk_count=0;
									{
										//strcpy(CurrentDirectory,pDI[SelectedItem].Path);
										OpenFile();
									}
									else
									if(disk_count&&pProjectInfo_[SelectedItem].Type<disk_count)
									{
										SaveCurrentDir();
										wcscpy(CurrentDirectory,pProjectInfo_[SelectedItem].FileName);
										CurrentDirectory[2]=0;
										SetWindowTextW(hDirectory,CurrentDirectory);
										Fill_lists();
									}
									else
										if(dir_count&&pProjectInfo_[SelectedItem].Type<file_count)
									{
										if(pProjectInfo_[SelectedItem].FileName[0]=='.'&&pProjectInfo_[SelectedItem].FileName[1]=='.')
										//&&pProjectInfo_[SelectedItem].FileName[1]=='.')
											{
												SaveCurrentDir();
												WCHAR* pFileName=0;
												pFileName=wcsrchr(CurrentDirectory,'\\');
												if(pFileName)
												{	
													*pFileName=0;
													//SetWindowText(hDirectory,CurrentDirectory);
												}
												else
													wcscpy(CurrentDirectory,L"\\");

												SetWindowTextW(hDirectory,CurrentDirectory);
										
											}
										else
										{
											SaveCurrentDir();

											wcscat(CurrentDirectory,L"\\");
											wcscat(CurrentDirectory,pProjectInfo_[SelectedItem].FileName);
											SetWindowTextW(hDirectory,CurrentDirectory);
										}
									Fill_lists();

									}
								}
						
							break;

				case BROWSE_BUTTON:
								SelectFolder();
								Fill_lists();
								break;
							
				case REFRESH_BUTTON :
							
						//	Menu();
							Fill_lists();
								break;
						
				case EXIT_BUTTON:
								#ifdef _DEBUG		
									SendMessage(hWnd, WM_DESTROY,0,0);	
								#else
									SendMessage(hWnd,WM_CLOSE,0,0);
								#endif
								break;
						
				case IDM_ABOUT:

							   //RightClickMenu();/
							   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
							   break;

				case IDD_HELP :
								
									#define HELP_CONTEXT      0x0001L  /* Display topic in ulTopic */
									#define HELP_QUIT         0x0002L  /* Terminate help */
									#define HELP_INDEX        0x0003L  /* Display index */
									#define HELP_CONTENTS     0x0003L
									#define HELP_HELPONHELP   0x0004L  /* Display help on using help */
									#define HELP_SETINDEX     0x0005L  /* Set current Index for multi index help */
									#define HELP_SETCONTENTS  0x0005L
									#define HELP_CONTEXTPOPUP 0x0008L
									#define HELP_FORCEFILE    0x0009L
									#define HELP_KEY          0x0101L  /* Display topic for keyword in offabData */
									#define HELP_COMMAND      0x0102L
									#define HELP_PARTIALKEY   0x0105L
									#define HELP_MULTIKEY     0x0201L
									#define HELP_SETWINPOS    0x0203L
									
									#define HELP_CONTEXTMENU  0x000a
									#define HELP_FINDER       0x000b
									#define HELP_WM_HELP      0x000c
									#define HELP_SETPOPUP_POS 0x000d

									#define HELP_TCARD              0x8000
									#define HELP_TCARD_DATA         0x0010
									#define HELP_TCARD_OTHER_CALLER 0x0011

										WCHAR BrowserExec[512];
									SetCurrentDirectory(ProgramPatch);	
									ShellExecuteW(NULL,L"open",L"help.rtf",0,BrowserExec,SW_SHOWNORMAL);
										break;
						
						/*WCHAR BrowserExec[512];
									SetCurrentDirectoryW(ProgramPatch);		
									ShellExecuteW(NULL,L"open",L"winhlp32.exe",L"HELP.HLP",BrowserExec,SW_SHOWNORMAL);*/
									//ShellExecute(NULL,0,"winhlp32.exe","HELP.HLP",0,SW_SHOWNORMAL);

									//ShellExecute(NULL,"winhlp32.exe"," HELP.HLP" ,NULL,NULL,SW_SHOWNORMAL);//winhlp32.exe
								
									break;

				case IDM_EXIT:
							   DestroyWindow(hWnd);
							   break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;


		case WM_PAINT:
			
			hDC=BeginPaint(hWnd,&ps);
				
				SetBkMode(hDC, 0);
				swprintf( s, L"Mask:");
				TextOutW(hDC, lw_width+5+30 , rt.top+LIST_Y+5-1,s,wcslen(s));

				swprintf( s, L"Suffix:");
				TextOutW(hDC, lw_width+5+30, rt.top+LIST_Y+45-6-1,s,wcslen(s));			
				
				swprintf( s, L"Password:");
				TextOutW(hDC, lw_width+8, rt.top+LIST_Y+75-1,s,wcslen(s));
				
				swprintf( s, L"Rule:");
				TextOutW(hDC, lw_width+5+30-2+10, rt.top+LIST_Y+75+30+2,s,wcslen(s));
												
				EndPaint(hWnd,&ps);
		
			break;
	
		
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}



// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
		PAINTSTRUCT ps;
		HFONT hCurFont;
		HFONT hCurFont2;
		char ss[256];
	int y;
	switch (message)
	{
		case WM_PAINT:
			                      
			hdc = BeginPaint(hDlg, &ps);

			 hCurFont=CreateFontW(15,0,0,0,FW_NORMAL,0,0,0,
				RUSSIAN_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
				PROOF_QUALITY,VARIABLE_PITCH|FF_ROMAN,L"Arial");
			  hCurFont2=CreateFontW(15,0,0,0,FW_BOLD,0,0,0,
				RUSSIAN_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
				PROOF_QUALITY,VARIABLE_PITCH|FF_ROMAN,L"Arial");
				
			SelectObject(hdc,hCurFont);

			SetBkMode(hdc, 0);
			y=0;			
				
			swprintf(s ,L"%s",szTitleU);
			if((ver%100)<10)
				wsprintfA(ss ,"PDF locker  Version %d.0%d%s  Freeware", ver/100,ver%100,subver);
			else
				wsprintfA(ss ,"PDF locker  Version %d.%d%s  Freeware", ver/100,ver%100,subver);
		
			TextOutA(hdc, 80,20+y*20,ss ,strlen(ss  ));
			y++;
				SelectObject(hdc,hCurFont);
			y++;	swprintf(s,L"Copyright © 2011-13 by Sasha P (sasha-game@hotmail.com)");TextOutW(hdc, 80,20+y*20,s ,wcslen(s  ));	
			//y++;	wsprintf(s,"E-mail: hinayana@126.com");TextOut(hdc, 80,20+y*20,s ,strlen(s  ));	
		
			y++;	swprintf(s,L"Thanks Benjamin Bentmann (benjamin.bentmann@becyhome.de)");TextOutW(hdc, 80,20+y*20,s ,wcslen(s  ));	
		
				
		
			DeleteObject(hCurFont);
				DeleteObject(hCurFont2);

			EndPaint(hDlg, &ps);

			break;
			
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}


int  SetFitPageSetting(WCHAR * file) 
{
	
	DWORD	dwFileLength,dwSourceFileBufferLength,n_bytesread,nBytesSave;
	HANDLE hFile;
	unsigned	int i=0,d;//,j;
	char* Buffer=0;
	
		hFile=CreateFileW(file, GENERIC_READ|GENERIC_WRITE,
		   FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,OPEN_EXISTING, 0, NULL);
		 if (hFile==INVALID_HANDLE_VALUE)
		 {
				MessageBoxA(0, "cannot open file" , " Error" , MB_ICONERROR);
				return(0);
		 } 

		dwFileLength=GetFileSize (hFile, NULL) ;
		dwSourceFileBufferLength=dwFileLength*2;
		Buffer=0;
		
		Buffer=(char*)new char[dwSourceFileBufferLength];
	
		if(Buffer==0)
			{
				MessageBoxW(0, L"Cannot allocate memory for buffer. Please close unused programs and try again.", L"Warning", MB_ICONWARNING);			
				return(0);
			}
		memset(Buffer,0x00, dwSourceFileBufferLength);
		ReadFile(hFile, (LPVOID)Buffer, dwFileLength, &n_bytesread, NULL);

	//char added_str[]=  " 0 obj<</D[" ; ///   4422 0 obj<</D[4387 0 R/Fit]/S/GoTo>>
	char added_str[]=  " 0 obj<</D[" ; ///   4422 0 obj<</D[4387 0 R/Fit]/S/GoTo>>
	//char added_str2[]= " 0 R/Fit]/S/GoTo>>\r\nendobj\r\n";		 	///	endobj
	char added_str2[]= " 0 R/Fit]/S/GoTo>>\r\n";		 	///	endobj
	char end_str[]="startxref";

	int new_section=0;
	int ref_section=0;
	char s[16];


	///add check for string in /ViewerPreferences
		//get section count
	/*	for(i=0;i<(int)(dwFileLength);i++)//calculate rows
		if( Buffer[i  ]==(char)'x'&&Buffer[i+1]==(char)'r'&&
				Buffer[i+2]==(char)'e'&&Buffer[i+3]==(char)'f')//xref
			{
				i+=4;
				while(Buffer[i]<'0'||Buffer[i]>'9') i++;

				memset(s,0,16);				
				int d , num1,  num2;
				d=0; memset(s,0,16);
				while(Buffer[i+d]!=' ') 
				{
				s[d]=Buffer[i+d];d++;
				}
				 num1=atoi(s);
				i+=d;
				i++;

				d=0; memset(s,0,16);
				while(Buffer[i+d]!=0xd) 
				{
					s[d]=Buffer[i+d];d++;
				}
				 num2=atoi(s);
				 new_section=num1+num2;
				break;
			}*/

		//	get number of such string "obj<</CropBox["
		char find_str[]="/CropBox" ;	//char find_str[]="obj<</CropBox" ;
		int find_str_len=strlen(find_str);

		for(i=0;i<(int)(dwFileLength);i++) 
		if( Buffer[i  ]==(char)find_str[0])
		if(!memcmp(&Buffer[i], find_str, find_str_len))		 
			{
				//int end=i;
				for(i;i>0;i--) 
				if(Buffer[i]==0x0d&&Buffer[i+1]>='0'&&Buffer[i+1]<='9')
					break;
				while(Buffer[i]<'0') i++;

				memset(s,0,16);
				d=0; memset(s,0,16);
				while(Buffer[i+d]!=' ') 
				{
				s[d]=Buffer[i+d];d++;
				}
				 ref_section=atoi(s);
			 
				break;
			}

	
	 


		//add new sec to end
		char str[200]="";
		int str_len_=0;;
		char find_str4[]="CreationDate" ;
		 int find_str4_len=strlen(find_str4);
		 	for(i=0;i<(int)(dwFileLength);i++) 
		if( Buffer[i  ]==(char)find_str4[0]&&Buffer[i+1]==(char)find_str4[1])
		if(!memcmp(&Buffer[i], find_str4, find_str4_len))
		{  
			 	//---get--------------
				for(i;i>0;i--) 
				if(Buffer[i]==0x0d&&Buffer[i+1]>='0'&&Buffer[i+1]<='9')
					break;
				while(Buffer[i]<'0') i++;
			
				 memset(s,0,16);
				d=0; memset(s,0,16);
				while(Buffer[i+d]!=' ') 
				{
				s[d]=Buffer[i+d];d++;
				}
				new_section=atoi(s);
				//----------------------

				//clean--------
				for(d=0;d<500;d++)
				 if (Buffer[i+d]==0xd&&Buffer[i+d+1]=='e' )
					 break;
				 else 
					 Buffer[i+d]=' '; 
				 //-------------------

				memset(str, 0, 200);
				wsprintfA(str,  "%d%s%d%s%",new_section, added_str,ref_section, added_str2);
				str_len_=strlen(str);
				memcpy(&Buffer[i], str ,str_len_); //dwFileLength+=str_len_;
				break;
		}

		
		
		// add string to" obj<</MarkInfo<</Marked true>>/ViewerPreferences<</Direction/L2R>>/Names 4386 0 R/Outlines 1827 0 R/Metadata 2180 0 R/Pages 2158 0 R/StructTreeRoot 2182 0 R/OpenAction 4422 0 R/Type/Catalog>>
			//endobj"

		char find_str2[]="R/Type/Catalog" ;
		//char find_str2[]="R/Pages" ;
		int find_str2_len=strlen(find_str2);
		char find_str3[]="R/StructTreeRoot " ;
		int find_str3_len=strlen(find_str3);
		char add_str[50]="";
		wsprintfA(add_str,"R/OpenAction %d 0 ",new_section) ;
		int add_str_len=strlen(add_str);

		int added=0;
		for(i=0;i<(int)(dwFileLength);i++) 
		if( Buffer[i  ]==(char)find_str3[0]&&Buffer[i+1]==(char)find_str3[1])
		if(!memcmp(&Buffer[i], find_str3, find_str3_len))		 
			{
			
				int d=0;
				 memset(&Buffer[i],' ',find_str3_len);
				 while (Buffer[i+d]!='R'&&Buffer[i+d+1]!='\\')
				 {
					 Buffer[i+d]=' ';d++;
				 }

				 if(d>add_str_len)
				 {
					 memcpy(&Buffer[i],add_str ,add_str_len);
					added=1;
				 }
				break;
			}  
	
		if(!added)
		for(i=0;i<(int)(dwFileLength);i++) 
		if( Buffer[i  ]==(char)find_str2[0]&&Buffer[i+1]==(char)find_str2[1])
		if(!memcmp(&Buffer[i], find_str2, find_str2_len))		 
			{
				for(  d=(dwFileLength);d>=i;d--) 
				Buffer[d+add_str_len]=Buffer[d];

			 	memcpy(&Buffer[i],add_str ,add_str_len); dwFileLength+=add_str_len;
			
				break;
			} 

		/*for(i=(dwFileLength );i>0;i--) 
		 if(Buffer[i  ]==end_str[0])
				if(!memcmp(&Buffer[i  ],end_str,strlen(end_str)))
				{
				memset(str, 0, 200);
				wsprintf(str,         "%d%s%d%s%",new_section,added_str,ref_section, added_str2);
				str_len_=strlen(str);
				for(  d=(dwFileLength);d>=i;d--) 
				Buffer[d+str_len_]=Buffer[d];
			 	memcpy(&Buffer[i], str ,str_len_); dwFileLength+=str_len_;
				//wsprintf(&Buffer[i  ],"%d%s%d%s%",new_section,added_str,ref_section, added_str2);
				//-strlen(end_str) ;
				break;
				}*/
		
			
		 
	SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
 	if(!WriteFile(hFile, (LPVOID)Buffer, dwFileLength , &nBytesSave, NULL))
			{
				MessageBoxA(0, "cannot write file" , " Error" , MB_ICONERROR);
				return(0);
			} 
		CloseHandle(hFile); 

		delete(Buffer);
	
	return 1;
}