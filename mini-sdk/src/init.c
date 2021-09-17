#include "resets.h"
#include "gpio.h"
#include "uart.h"
#include "nvic.h"
#include "xosc.h"
#include "lib.h"
#include "systick.h"
#include "shell.h"
#include "malloc.h"
 
 /*

SECTIONS {
  . = 0x20000000;

  .text : {
    *(.text*);
  }

  .data : {
    *(.data*);
  }

  .bss : {
    *(.bss*);
  }
}


.global reset
reset:
 
	ldr r0, =0x20040000
	mov sp, r0

	bl init
	b .
	*/


char *text="kfjrhgjrhfejhfjkhfr";

struct  cmdent  {     /* Entry in command table */
  char  *cname;     /* Name of command    */
  bool cbuiltin;   /* Is this a builtin command? */
};

const struct  cmdent  cmdtab[] = {
  {"echo",  true},
  {"cat",   false},
  {"clear", true},
  {"date",  false},
  {"devdump", false},
  {"echo",  false},
  {"exit",  true},
  {"help",  false},
  {"kill",  true},
  {"memdump", false},
  {"memstat", true}, /* Make built-in */
  {"ps",    false},
  {"sleep", false},
  {"uptime",  false},
  {"?",   false},
  {"fopen",   false},
  {"fclose",   false},
  {"fcreat",   false},
  {"fread",   false},
  {"fwrite",   false},
  {"ls",   false},
  {"cd",   false},
  {"pwd",   false},
  {"cat",   false},
  {"rm",   false},
  {"mkdir",   false},
  {"touch",   false},
  {"mount",   false},
  {"writegpio",   false},
  {"readgpio",   false},
  
};

uint32_t  ncmd = sizeof(cmdtab) / sizeof(struct cmdent);

 




void init() {
  typedef void (*function_t)();
    extern function_t __init_array_start[0];
    extern function_t __init_array_end[0];

    for (function_t* c=__init_array_start; c < __init_array_end; ++c)
    {
       (*c)();
    }
    
	xosc_init();
	IO_WR(CLOCKS_BASE+0x48, 0x880); // clk_peri enble

	reset_release_wait(RESET_IO_BANK0);
	reset_release_wait(RESET_PADS_BANK0);
	reset_release_wait(RESET_UART0);

	uart_init(0, 115200);
	gpio_init(0, GPIO_FUNC_UART);
	gpio_init(1, GPIO_FUNC_UART);
	gpio_dir(0, 1);

	nvic_init();

	UART_TxHead = 0;
    UART_TxTail = 0;
    UART_RxHead = 0;
    UART_RxTail = 0;

	uart_intr_enable(0, 1, 0);
	nvic_register_irq(IRQ_UART0, irq_uart0);
	nvic_enable(IRQ_UART0);
 
    gpio_init(25, GPIO_FUNC_SIO);
	gpio_dir(25, 1);
    

    malloc_init(KMALLOC_START, KMALLOC_LENGTH);
    malloc_debug();

	systick_init();
	systick_set(12000000);
   
printf("%s\n",text );


	while(1){
         printf("%s",SHELL_PROMPT);
         while(!uart0_available());
          while(uart0_available()){
              len = readBytes(buf,sizeof(buf));
        }
        if (len == EOF) {
           break;
        }
        if (len <= 1) {
            continue;
        }

        buf[len] = SH_NEWLINE;  /* terminate line */

  

    ntok = lexan(buf, len, tokbuf, &tlen, tok, toktyp);

    /* Handle parsing error */

    if (ntok == SYSERR) {
      printf("%s\n",SHELL_SYNERRMSG);
      continue;
    }

    /* If line is empty, go to next input line */

    if (ntok == 0) {
      printf( "\n");
      continue;
    }
    
    for (j = 0; j < ncmd; j++) {
      src = cmdtab[j].cname;
      cmp = tokbuf;
      diff = false;
      while (*src != NULLCH) {
        if (*cmp != *src) {
          diff = true;
          break;
        }
        src++;
        cmp++;
      }
      if (diff || (*cmp != NULLCH)) {
        continue;
      } else {
        break;
      }
    }

    /* Handle command not found */

    if (j >= ncmd) {
        printf("command %s not found %d\n",tokbuf,len);
        continue;
    }else{
        printf("ok");
        printf(" %d ",j);
      for (int i=0; i<ntok; i++) {
          args[i] = &tokbuf[tok[i]];
          printf("%s",args[i]);
          printf(" ");
      }
      if(!strcmp(args[0],"kill")){
      	systick_disable();
      	PEND_SV();
      }else if(!strcmp(args[0],"exit")){
        //reset();
      }
      printf("\n");

        
    }

 

	}
}


/*

#define SYS_CSR    (PPB_BASE + 0xE010)
#define SYS_RVR    (PPB_BASE + 0xE014)
#define SYS_CVR    (PPB_BASE + 0xE018)
#define SYS_CALIB  (PPB_BASE + 0xE01C)
#define NVIC_ISER   (PPB_BASE + 0xE100)
#define NVIC_ICER   (PPB_BASE + 0xE180)
#define NVIC_ISPR   (PPB_BASE + 0xE200)
#define NVIC_ICPR   (PPB_BASE + 0xE280)


reg_wr(CLOCKS_BASE+0x78, 0);

reg_wr(XOSC_BASE+0x00, 0xaa0);
reg_wr(XOSC_BASE+0x0c, 47);
reg_wr(XOSC_BASE+0x00+0x2000, 0xfab000);

while(1) {
	if (((reg_rd(XOSC_BASE+0x04)) & 0x80000000)!=0){
		break;
	}
}

reg_wr(CLOCKS_BASE+0x30, 2);
reg_wr(CLOCKS_BASE+0x3c, 0);
reg_wr(CLOCKS_BASE+0x48, 0x880);

reset_release_wait(RESET_IO_BANK0);
reset_release_wait(RESET_PADS_BANK0);
reset_release_wait(RESET_UART0);


uart_init();

reg_wr(SYS_CSR, 4);
reg_wr(SYS_RVR, 12000000-1);
reg_wr(SYS_CVR, 12000000-1);
reg_wr(SYS_CSR, 7);

reg_wr(NVIC_ISER, 1 << 13);

asm volatile("cpsie i");

gpio_init(0, GPIO_FUNC_UART);
gpio_init(1, GPIO_FUNC_UART);

*/


/*
gpio irq


void irq13() {
	gpio_set(20, 0);
	delay(1000000);
	gpio_set(20, 1);
	gpio_int_ack(7);
}


	gpio_init(18, GPIO_FUNC_SIO);
	gpio_init(19, GPIO_FUNC_SIO);
	gpio_init(20, GPIO_FUNC_SIO);
	gpio_dir(18, 1);
	gpio_dir(19, 1);
	gpio_dir(20, 1);

	gpio_init(0, GPIO_FUNC_SIO);
	gpio_init(1, GPIO_FUNC_SIO);
	gpio_init(2, GPIO_FUNC_SIO);
	gpio_pullup(0, 1);
	gpio_pullup(1, 1);
	gpio_pullup(2, 1);

	gpio_init(7, GPIO_FUNC_SIO);
	gpio_pullup(7, 1);
	delay(10000);
	gpio_int_set(7, 1, GPIO_INT_EDGE_FALL);
	nvic_init();
	nvic_register_irq(13, irq13);
	nvic_enable(13);

	gpio_set(19, 1);
	gpio_set(20, 1);
	*/
