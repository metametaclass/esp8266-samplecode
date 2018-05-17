#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "user_config.h"
#include "driver/uart.h"

#include "ip_addr.h"
#include "espconn.h"
#include "user_interface.h"

#include "sniffer.h"


#define CHECK(rc, msg) \
  if(!rc){\
     os_printf("%s failed\n", msg);\
  }


// System task

#define user_procTaskPrio        0
#define user_procTaskQueueLen    1

static void user_procTask(os_event_t *events);
os_event_t    user_procTaskQueue[user_procTaskQueueLen];


//timer
volatile os_timer_t     timer;

//post-init callback
static void post_user_init_func();


// Timer callback function, called when the timer expires
void timer_func(void *arg)
{
    // hopping channels 1-14
    uint8 new_channel = wifi_get_channel() + 1;
    if (new_channel > 14) {
      new_channel = 1;
    }
    os_printf("channel:%d\n", new_channel);

    bool rc = wifi_set_channel(new_channel);
    CHECK(rc, "wifi_set_channel");
}

// Called once the system intialization is complete
void post_user_init_func()
{
    os_printf("post_user_init_func() called\r\n");

    if(wifi_station_disconnect()) {
       os_printf("wifi_station_disconnect: ok\r\n");
    } else {
       os_printf("wifi_station_disconnect: fail\r\n");
    }

    os_printf(" -> Promisc mode setup ... ");
    wifi_promiscuous_enable(0); 
    wifi_set_promiscuous_rx_cb(promisc_cb);
    wifi_promiscuous_enable(1);
    os_printf("done.\n");

    // Create a timer which repeats itself and fires every 10 seconds.
    os_timer_disarm(&timer);

    // Assign a callback function and arm it
    os_timer_setfn(&timer, (os_timer_func_t *) timer_func, NULL);
    os_timer_arm(&timer, 10000, 1);
}


void ICACHE_FLASH_ATTR user_rf_pre_init(void)
{
}


void ICACHE_FLASH_ATTR user_init(void)
{
    uart_init(115200, 115200);
    os_delay_us(100);
    bool rc;

    uart0_sendStr("*** Monitor mode test ***\r\n");

    os_printf(" -> disable autoconnect ... ");
    rc = wifi_station_set_auto_connect(0);
    CHECK(rc, "wifi_station_set_auto_connect")
    os_printf("done.\n");

    system_init_done_cb(&post_user_init_func);
 
    os_printf(" -> Set opmode ... ");
    wifi_set_opmode( STATION_MODE );
    os_printf("done.\n");
    
    //Start os task
    //system_os_task(loop, user_procTaskPrio,user_procTaskQueue, user_procTaskQueueLen);

    os_printf(" -> Init finished!\n\n");
}