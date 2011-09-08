#ifndef	_MENU_H
#define	_MENU_H

//#include <hw_r.h>

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

#endif
