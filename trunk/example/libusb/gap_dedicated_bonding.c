
//*****************************************************************************
//
// minimal setup for SDP client over USB or UART
//
//*****************************************************************************

#include "btstack-config.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include <btstack/hci_cmds.h>
#include <btstack/run_loop.h>

#include "hci.h"
#include "gap.h"
#include "btstack_memory.h"
#include "hci_dump.h"

// static bd_addr_t remote = {0x04,0x0C,0xCE,0xE4,0x85,0xD3};
static bd_addr_t remote = {0x84, 0x38, 0x35, 0x65, 0xD1, 0x15};

static void packet_handler (uint8_t packet_type, uint8_t *packet, uint16_t size){
    printf("packet_handler type %u, packet[0] %x\n", packet_type, packet[0]);

    if (packet_type != HCI_EVENT_PACKET) return;

    switch (packet[0]) {
        case BTSTACK_EVENT_STATE:
            // bt stack activated, get started 
            if (packet[2] == HCI_STATE_WORKING){
                gap_dedicated_bonding(remote, 1);
            }
            break;
        case GAP_DEDICATED_BONDING_COMPLETED:
            printf("GAP Dedicated Bonding Complete, status %u\n", packet[2]);
        default:
            break;
    }
}

static void btstack_setup(){
    /// GET STARTED ///
    btstack_memory_init();
    run_loop_init(RUN_LOOP_POSIX);
    
    hci_dump_open("/tmp/hci_dump.pklg", HCI_DUMP_PACKETLOGGER);
   
    hci_transport_t    * transport = hci_transport_usb_instance();
    hci_uart_config_t  * config = NULL;
    bt_control_t       * control   = NULL;    

    remote_device_db_t * remote_db = (remote_device_db_t *) &remote_device_db_memory;
    hci_init(transport, config, control, remote_db);
    hci_register_packet_handler(packet_handler);    
    // turn on!
    hci_power_control(HCI_POWER_ON);
}

int main(void){
    btstack_setup();
    run_loop_execute(); 
    return 0;
}
