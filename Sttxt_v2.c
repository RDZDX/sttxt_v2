#include "Sttxt_v2.h"

vm_sms_msg_data_t *message_data = NULL;
VMINT8 *content_buff;
VMWCHAR f_wname[100];
VMFILE f_write;
VMUINT nwrite;
VMINT trigeris = 0;
VMINT trigeris1 = 0;
VMINT trigeris2 = 0;

void vm_main(void) {

    vm_reg_sysevt_callback(handle_sysevt);
    checkFileExist();
}

void handle_sysevt(VMINT message, VMINT param) {

    switch (message) {
        case VM_MSG_CREATE:
            break;
        case VM_MSG_PAINT:
            if (trigeris2 == 0) {vm_pmng_set_bg();}
            trigeris2 = 1;
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


    for (i = 0; i<sms_k; ++i){

        message_id = vm_sms_get_msg_id(VM_SMS_BOX_INBOX, i);
        vm_sms_read_msg(message_id, 0, message_data, mre_sms_read_callback_function, NULL);
    }

    vm_free(message_data->content_buff);
    vm_free(message_data);
    //message_data = NULL;
    if (i == sms_k) {trigeris1 = 1;}
}

static void mre_sms_read_callback_function(vm_sms_callback_t *callback_data) {

    VMWCHAR file_name[100];
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
    VMCHAR ascii_data[161]; //160 characters (GSM-7 encoding) or 70 characters (UCS-2 encoding) 
    VMWCHAR f_wname1[100];
    VMUINT16 content_buffer_size;
    VMINT8 *content_buffer;

    content_buffer_size = message_data->content_buff_size;
    content_buffer = message_data->content_buff;

    vm_ucs2_to_ascii (ascii_data, content_buffer_size, (VMWSTR)content_buffer);
    //vm_ucs2_to_ascii (ascii_data, wstrlen((VMWSTR)content_buffer) + 1, (VMWSTR)content_buffer);

    if (trigeris == 1) {
       create_auto_filename(file_name);
       create_auto_full_path_name(f_wname1, file_name);
       f_write = vm_file_open(f_wname1, MODE_CREATE_ALWAYS_WRITE, FALSE);
       vm_file_write(f_write, ascii_data, strlen(ascii_data), &nwrite);
    } else {
       f_write = vm_file_open(f_wname, MODE_APPEND, FALSE);
       if (f_write < 0){
          f_write = vm_file_open(f_wname, MODE_CREATE_ALWAYS_WRITE, FALSE);
       }
       vm_file_write(f_write, ascii_data, strlen(ascii_data), &nwrite);
       vm_file_write(f_write, "\n",1, &nwrite);
    }
    vm_file_close(f_write);

    if (trigeris1 == 1){vm_create_timer_ex(500,timer);}
}

void create_app_txt_filename(VMWSTR text) {

    VMWCHAR fullPath[100];
    VMWCHAR appName[100];
    VMWCHAR wfile_extension[8];
    VMCHAR file_extension[4] = "txt";

    vm_get_exec_filename(fullPath);
    vm_get_filename(fullPath, appName);
    vm_ascii_to_ucs2(wfile_extension, (strlen(file_extension) + 1) * 2, file_extension);
    vm_wstrncpy(text, appName, vm_wstrlen(appName) - 3);
    vm_wstrcat(text, wfile_extension);

}

void create_auto_filename(VMWSTR text) {

    struct vm_time_t curr_time;
    VMCHAR fAutoFileName[100];
    int rand1 = rand() % 99;
    vm_get_time(&curr_time);
    //sprintf(fAutoFileName, "%02d%02d%02d%02d%02d.txt", curr_time.mon, curr_time.day, curr_time.hour, curr_time.min, curr_time.sec);
    sprintf(fAutoFileName, "%02d%02d%02d%02d%02d.txt", curr_time.mon, curr_time.day, curr_time.hour, rand1, curr_time.sec);
    vm_ascii_to_ucs2(text, (strlen(fAutoFileName) + 1) * 2, fAutoFileName);

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

    create_app_txt_filename(appName);
    create_auto_full_path_name(f_wname, appName);

    f_write = vm_file_open(f_wname, MODE_READ, FALSE);

    if (f_write < 0) {
        vm_file_close(f_write);
        trigeris = 1;
    } else {
        trigeris = 0;
        vm_file_close(f_write);
    }
    mre_read_sms();
}

void timer(int a){
     vm_delete_timer_ex(a);
     //vm_vibrator_once();
     vm_exit_app();
}