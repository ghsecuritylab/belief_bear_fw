#include <rtthread.h>
#include "sysconfig.h"
syscfg_t syscfg;

void syscfg_save_mac_flash(void){
    u16 i;
    u16 *p_flash = (u16 *)&syscfg;
    FLASH_Unlock();
    syscfg.update_flag = 0x00;
    FLASH_ErasePage(SYSCFG_ADDR_FLASH);
    for( i=0 ; i<sizeof(syscfg) ;i+=2 ){
        FLASH_ProgramHalfWord((SYSCFG_ADDR_FLASH+i),*p_flash);
        p_flash++;
    }
    FLASH_Lock();
}

void syscfg_get_mac_flash(void){
    u16 j;

    syscfg.mac_addr_ch1 = *(vu32 *)(SYSCFG_ADDR_FLASH + 0);
    syscfg.mac_addr_ch2 = *(vu32 *)(SYSCFG_ADDR_FLASH + 4);
    syscfg.uplink_status = *(vu8 *)(SYSCFG_ADDR_FLASH + 8);
    syscfg.uplink_msg_ack_req = *(vu8 *)(SYSCFG_ADDR_FLASH + 9);
//#ifdef ENABLE_HOP
    syscfg.mac_hop_inited = *(vu8 *)(SYSCFG_ADDR_FLASH + 10);
    syscfg.mac_hop_enable = *(vu8 *)(SYSCFG_ADDR_FLASH + 11);
    syscfg.mac_hop_cur_idx = *(vu8 *)(SYSCFG_ADDR_FLASH + 12);
    syscfg.mac_hop_second = *(vu16 *)(SYSCFG_ADDR_FLASH + 13);
    for(j=0; j<16; j++){
        syscfg.mac_hop_nwids[j] = *(vu8 *)(SYSCFG_ADDR_FLASH + 15+j);
    }
//#endif    
    syscfg.update_flag = *(vu8 *)(SYSCFG_ADDR_FLASH + 30);
}
     

void soft_reset(void) {
	__set_FAULTMASK(1);
	NVIC_SystemReset();
}
