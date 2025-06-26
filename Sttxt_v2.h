#ifndef _VRE_APP_WIZARDTEMPLATE_
#define	_VRE_APP_WIZARDTEMPLATE_

#define		SUPPORT_BG

#include "vmio.h"
#include "vmchset.h"
#include "vmstdlib.h"
#include "vmsms.h"
#include "string.h"
#include <time.h>
//#include "vmmm.h"
#include "vmtimer.h"

//VMINT		layer_hdl[1];				////layer handle array.

void mre_read_sms();
static void mre_sms_read_callback_function(vm_sms_callback_t *callback_data);
static void mre_msg_content_display(void);
void create_auto_full_path_name(VMWSTR result, VMWSTR fname);
void checkFileExist(void);
void timer(int a);
void handle_sysevt(VMINT message, VMINT param);
void create_auto_filename(VMWSTR text, VMSTR extt);
void create_app_txt_filename(VMWSTR text, VMSTR extt);

#endif