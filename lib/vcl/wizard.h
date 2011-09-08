#ifndef	_WIZARD_H
#define	_WIZARD_H

//#include <hw_r.h>
#include <vcl.h>

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

#endif
