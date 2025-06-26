#include "Sttxt_v2.h"

vm_sms_msg_data_t *message_data = NULL;
VMWCHAR f_wname[100] = {0};
VMFILE f_write;
VMUINT nwrite;
VMBOOL trigeris = VM_FALSE;
VMBOOL trigeris1 = VM_FALSE;
VMBOOL trigeris2 = VM_FALSE;

void vm_main(void) {

    vm_reg_sysevt_callback(handle_sysevt);
    checkFileExist();

}

void handle_sysevt(VMINT message, VMINT param) {

    switch (message) {
        case VM_MSG_CREATE:
            break;
        case VM_MSG_PAINT:
            if (trigeris2 == VM_FALSE) {vm_pmng_set_bg();}
            trigeris2 = VM_TRUE;
            break;
	case VM_MSG_HIDE:	
            break;
        case VM_MSG_QUIT:
            break;
    }
}

void mre_read_sms() {

    VMUINT16 msg_index = 0;
    VMINT16 message_id;
    VMINT16 sms_k;
    int i;

    sms_k = vm_sms_get_box_size(VM_SMS_BOX_INBOX);

    message_data = vm_malloc(sizeof(vm_sms_msg_data_t));
    memset(message_data, 0, sizeof(vm_sms_msg_data_t));
    message_data->content_buff = (VMINT8 *)vm_malloc((500 + 1) * 2);
    memset(message_data->content_buff, 0, (500 + 1) * 2);
    message_data->content_buff_size = 500 * 2;


    for (i = 0; i < sms_k; ++i){

        message_id = vm_sms_get_msg_id(VM_SMS_BOX_INBOX, i);
        vm_sms_read_msg(message_id, 0, message_data, mre_sms_read_callback_function, NULL);
    }

    vm_free(message_data->content_buff);
    vm_free(message_data);
    //message_data = NULL;
    if (i == sms_k) {trigeris1 = VM_TRUE;}
}

static void mre_sms_read_callback_function(vm_sms_callback_t *callback_data) {

    if (callback_data->result == 1) {
        if (callback_data->cause == VM_SMS_CAUSE_NO_ERROR) {
            switch (callback_data->action) {
                case VM_SMS_ACTION_NONE:
                    break;
                case VM_SMS_ACTION_READ:
                    mre_msg_content_display();
                    return;
                default:
                    break;
            }
        } else {
        }
    } else {
    }

}

static void mre_msg_content_display(void) {

    VMWCHAR file_name[100]; 
    VMCHAR ascii_data[1002]; //1sms size 160 ascii GSM-7 | 70 UCS-2 ? ; 1sms 153 Total 459 | 1sms 70 Total 201 ???
    VMWCHAR f_wname1[100];

    vm_chset_convert(VM_CHSET_UTF16LE, VM_CHSET_UTF8, (VMSTR)message_data->content_buff, ascii_data, vm_wstrlen((VMWSTR)message_data->content_buff) + 1);
    //vm_chset_convert(VM_CHSET_UCS2, VM_CHSET_UTF8, (VMSTR)message_data->content_buff, ascii_data, vm_wstrlen((VMWSTR)message_data->content_buff) + 1);


    if (trigeris == VM_TRUE) {
       create_auto_filename(file_name, "txt");
       create_auto_full_path_name(f_wname1, file_name);

       f_write = vm_file_open(f_wname1, MODE_CREATE_ALWAYS_WRITE, FALSE);
       vm_file_write(f_write, ascii_data, strlen(ascii_data), &nwrite);
    } else {
       f_write = vm_file_open(f_wname, MODE_APPEND, FALSE);
       if (f_write < 0){
          f_write = vm_file_open(f_wname, MODE_CREATE_ALWAYS_WRITE, FALSE);
       }
       vm_file_write(f_write, ascii_data, strlen(ascii_data), &nwrite);
       vm_file_write(f_write, "\n", 1, &nwrite);
    }
    vm_file_close(f_write);

    if (trigeris1 == VM_TRUE){
       message_data = NULL;
       vm_create_timer_ex(500, timer);
    }
}

void create_auto_full_path_name(VMWSTR result, VMWSTR fname) {

    VMINT drv;
    VMCHAR fAutoFileName[100];
    VMWCHAR wAutoFileName[100];

    if ((drv = vm_get_removable_driver()) < 0) {
       drv = vm_get_system_driver();
    }

    sprintf(fAutoFileName, "%c:\\", drv);
    vm_ascii_to_ucs2(wAutoFileName, (strlen(fAutoFileName) + 1) * 2, fAutoFileName);
    vm_wstrcat(wAutoFileName, fname);
    vm_wstrcpy(result, wAutoFileName);

}

void checkFileExist(void) {

    VMWCHAR appName[100];

    create_app_txt_filename(appName, "txt");
    create_auto_full_path_name(f_wname, appName);
    if (vm_file_get_attributes(f_wname) == -1) {trigeris = VM_TRUE;}
    mre_read_sms();
}

void timer(int a){
     vm_delete_timer_ex(a);
     //vm_vibrator_once();
     vm_exit_app();
}

void create_auto_filename(VMWSTR text, VMSTR extt) {

    struct vm_time_t curr_time;
    VMCHAR fAutoFileName[100];
    int rand1 = rand() % 99;

    vm_get_time(&curr_time);
    sprintf(fAutoFileName, "%02d%02d%02d%02d%02d.%s", curr_time.mon, curr_time.day, curr_time.hour, rand1, curr_time.sec, extt);
    vm_ascii_to_ucs2(text, (strlen(fAutoFileName) + 1) * 2, fAutoFileName);

}

void create_app_txt_filename(VMWSTR text, VMSTR extt) {

    VMWCHAR fullPath[100];
    VMWCHAR appName[100];
    VMWCHAR wfile_extension[8];

    vm_get_exec_filename(fullPath);
    vm_get_filename(fullPath, appName);
    vm_ascii_to_ucs2(wfile_extension, 8, extt);
    vm_wstrncpy(text, appName, vm_wstrlen(appName) - 3);
    vm_wstrcat(text, wfile_extension);

}