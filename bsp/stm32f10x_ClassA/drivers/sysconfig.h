#ifndef __SYSCONFIG_H__
#define __SYSCONFIG_H__

#include <stm32f10x.h>
/*
#define SYSCFG_ADDR_MACADDR                     0x08080D00
#define SYSCFG_ADDR_MAC_HOP_ENABLE              0x08080D14
#define SYSCFG_ADDR_MAC_HOP_CUR_IDX             0x08080D18
#define SYSCFG_ADDR_MAC_HOP_NWID_LEN            0x08080D1C
#define SYSCFG_ADDR_MAC_HOP_SECONDS             0x08080D20
#define SYSCFG_ADDR_MAC_HOP_NWID                0x08080D40
#define SYSCFG_ADDR_MAC_HOP_INITED              0x08080DF0

#define SYSCFG_ADDR_UPLINK_MSG_ACK_REQUEST      0x08080DF4
#define SYSCFG_ADDR_UPLINK_STATUS               0x08080DF8
*/
// FLASH_ErasePage = 1K = 1024 = 0x400
#define SYSCFG_ADDR_FLASH                       0x0801F000
//default byte not alignment 
//condition : The default is a continuous memory area
typedef __packed struct _syscfg_t{

    u32 mac_addr_ch1;
    u32 mac_addr_ch2;
    u8  uplink_status;
    u8  uplink_msg_ack_req;
    
//#ifdef ENABLE_HOP
    u8  mac_hop_inited;
    u8  mac_hop_enable;
    u8  mac_hop_cur_idx;
    u16 mac_hop_second;
    u8  mac_hop_nwids[16];
    
//#endif   
    u8 update_flag;
}syscfg_t;
extern syscfg_t syscfg;
//Save before comparing whether to change
void syscfg_save_mac_flash(void);
void syscfg_get_mac_flash(void);

void soft_reset(void);

#endif /* __SYSCONFIG_H__ */
