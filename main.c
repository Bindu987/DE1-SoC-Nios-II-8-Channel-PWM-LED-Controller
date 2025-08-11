#include <stdio.h>
#include <unistd.h>
#include "system.h"
#include "altera_avalon_pio_regs.h"

// Qsys symbols
#define PWM_CUSTOM_BASE PWM_CUSTOM_0_BASE
#define KEYS_BASE_IO KEYS_BASE

// PWM config (2 kHz)
#define CLK_HZ 50000000u
#define PWM_HZ 2000u
#define PWM_DIV (CLK_HZ / PWM_HZ)
#define DUTY_HIGH ((PWM_DIV*80)/100) // ~80%
#define DUTY_MED ((PWM_DIV*50)/100) // ~50%
#define DUTY_LOW ((PWM_DIV*10)/100) // ~10%

static inline void pwm_write_word(unsigned wofs, unsigned val){
 IOWR_32DIRECT(PWM_CUSTOM_BASE, (wofs<<2), val);
}
static void set_all_pwm(unsigned div, unsigned duty){
 if(div < 1) div = 1;
 if(duty > div) duty = div;
 for(int i=0;i<8;i++){
 pwm_write_word((i<<1)+0, div);
 pwm_write_word((i<<1)+1, duty);
 }
}

static inline void debounce_delay(void){ usleep(15000); } // ~15 ms
int main(void){
 set_all_pwm(PWM_DIV, DUTY_MED);
 printf("PWM ~%u Hz. Use keys such as key [3]=High ,[2]=Med and then [1]=Low (KEY0 ignored)\n",
 (unsigned)(CLK_HZ/PWM_DIV));
 // Read once; force KEY0 high so it can never trigger logic

 unsigned prev_keys = IORD_ALTERA_AVALON_PIO_DATA(KEYS_BASE_IO) & 0x0F;
 prev_keys |= 0x01; // force bit0 = 1 (ignored)
 while(1){

	 unsigned keys = IORD_ALTERA_AVALON_PIO_DATA(KEYS_BASE_IO) & 0x0F;
 keys |= 0x01; // ALWAYS ignore KEY0 (force not-pressed)
 // Active-low >> invert; now 1 = pressed. Consider only bits 3..1

 unsigned now_pressed = ((~keys) & 0x0E);
 unsigned prev_pressed = ((~prev_keys) & 0x0E);
 unsigned new_press = now_pressed & ~prev_pressed;

 if(new_press){
 debounce_delay();
 keys = IORD_ALTERA_AVALON_PIO_DATA(KEYS_BASE_IO) & 0x0F;
 keys |= 0x01; // keep KEY0 ignored
 now_pressed = ((~keys) & 0x0E);

 if (now_pressed & 0x08){ // KEY3
 set_all_pwm(PWM_DIV, DUTY_HIGH);
 printf("KEY3 isPRESSED: HIGH (div=%u, duty=%u)\n", PWM_DIV, DUTY_HIGH);
 } else if (now_pressed & 0x04){ // KEY2
 set_all_pwm(PWM_DIV, DUTY_MED);
 printf("KEY2 is PRESSED: MED (div=%u, duty value=%u)\n", PWM_DIV, DUTY_MED);
 } else if (now_pressed & 0x02){ // KEY1
 set_all_pwm(PWM_DIV, DUTY_LOW);
 printf("KEY1 is PRESSED: LOW (div=%u, duty value=%u)\n", PWM_DIV, DUTY_LOW);
 }
 }

 prev_keys = keys;
 usleep(3000); // 3 ms poll
 }
 return 0;
}
