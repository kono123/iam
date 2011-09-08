#ifndef	_VCL_H
#define	_VCL_H

#include "hardware.h"
#include <lcd.h>
//#include <menu.h>
#include <dispatch.h>
//#include <msgbox.h>
//#include <enumbox.h>
//#include <inputbox.h>
//#include <wizard.h>

#define VCL_RET_OK       1
#define VCL_RET_CANCEL   2
#define VCL_RET_ERROR    3
#define VCL_RET_OK2      4
#define VCL_RET_POWER_ERROR    0x81
#define VCL_RET_WRITE_ERROR    0x82

#define MENU_ICON             "\x0F"
#define LISTBOX_ICON          "\x8D"
#define MEMO_ICON             "\x8F"

int VCL_Open(void);
int VCL_SetMutex(void);
int VCL_FreeMutex(void);
void VCL_WaitMutex(int mutex);
void VCL_SetRetVal(int val);
int VCL_GetRetVal(void);
void VCL_SetTimer(int val);

void OkCancelKbdHandler(U8 key);
void OkKbdHandler(U8 key);
void CancelKbdHandler(U8 key);

//void IntProgressBar(U8 x,U8 y,U8 len,U16 val,U16 maxval);
void IntProgressBarToStr(char *str,U8 len,U16 val,U16 maxval);
int IntProgressBar(const char *Title,U8 len);

// inputbox.c
#define INPUTBOX_STR       1
#define INPUTBOX_NUM       2
#define INPUTBOX_TIME      3
#define INPUTBOX_DATE      4

int InputBox(const char *Title, char *iostr, int flag);

// info.c
int Info(char *str1,char *str2,char *str3,char *str4);
// memo.c
int Memo(const char *text,int flag);
char *ListBox(char **text,U8 lines);

// kbdblock.c
int Kbdblock(U8 key);

// enumbox.c
#define ENUM_BOX_OK       1
#define ENUM_BOX_CANCEL   -1

#define ENATTR_INVERSE  VATTR_INVERSE
//#define MBATTR_INVERSE  VATTR_UNDERLINE

#define MAX_ENUM_LIST_ITEMS 8

int EnumBox(char *Title,U8 *initval,U8 items, ...);

// messagebox.c
#define MBF_OKCANCEL    1
#define MBF_OK          0

//#define MSGBOX_OK       1
//#define MSGBOX_CANCEL   2

#define MBATTR_INVERSE  VATTR_INVERSE
//#define MBATTR_INVERSE  VATTR_UNDERLINE

int MsgBox(char *Title,int flag);
int MsgBoxEx(char *Title,char *str2,int flag);
int WaitTimerBox(char *Title,int sec);

//  menu.c
#define MAX_LIST_ITEMS 4

typedef struct _menuitem_tag {
//  const struct _menu_tag *CildMenu;
  int (*MenuHandler) (void);
//  unsigned char ShortCut;
//  unsigned char Enable;
  int Visible;
//  unsigned char Tag;
//  unsigned char Id;
  char Caption[18];
} TMenuItem, *PMenuItem;

typedef struct _menu_tag {
  const struct _menu_tag *Parent;
  U8 Items;
//  char Caption[18];
//  void *CtrlHandler;
//  int (*MenuHandler) (void *);
  TMenuItem Item[MAX_LIST_ITEMS];
} TMENU, *PMENU;

#define MEMU_ATTR_INVERSE  VATTR_INVERSE

void MainMenuKbdHandler(U8 key);

int Menu(TMENU const *pmenu);
int StartMainMenu(TMENU const *pmenu);

//  wizard.c
#define WIZARD_CANCEL -1
#define WIZARD_PARAM_ERROR -2
#define MAX_WIZARD_ITEMS 8

typedef struct _step_wizard_tag {
  char Caption[19];
  char Help[19];
  U8   Type;
  int (*GetParam) (char *);
  int (*EditParam) (void *);
  int (*ErrorParam) (void);
} TItemWizard, *PItemWizard;

typedef struct _wizard_tag {
  char Items;
  U8   Type;
//  int (*WizardHandler) (void *);
  TItemWizard Item[MAX_WIZARD_ITEMS];
} TWizard, *PWizard;

int Wizard(TWizard const *ptr,int item);

void ShowHourglass(void);
void HideHourglass(void);

#endif
