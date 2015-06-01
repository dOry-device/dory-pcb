#include "sys/alt_stdio.h"
#include "system.h"
#include "io.h"
 #include "altera_avalon_timer_regs.h"
#include <sys/alt_irq.h>

#define IRESETVAL 0 //Change the values if your register map is different than here
#define ICOUNTER 0
#define IRZ 1
#define ISTART 2
#define ISTOP 3
#define IIRQEN 4
#define ICLREOT 5
#define RESETVAL 0XFF000000 //Counter starts counting from this value
#define IRQENVAL 1
#define IRQDISVAL 0
#define CLREOTVAL 1
#define ARBITVAL 0X0000FFFF //Arbitrary writedata value used for addresses 1,2,3

int flag, snap;
int v= 0 ,d= 0;
void test_counter()
{
	IOWR(MYCOUNTER_0_BASE, IRESETVAL, RESETVAL);
	//Reset value is loaded
	IOWR(MYCOUNTER_0_BASE, IRZ, ARBITVAL);
	//Reset activated to load the counter with the reset value
	alt_printf("iCounter after reset= %x\n",IORD(MYCOUNTER_0_BASE, ICOUNTER));
	//Check that counter is loaded with the reset value
	IOWR(MYCOUNTER_0_BASE, ISTART, ARBITVAL);
	//Start the counter
	alt_printf("iCounter after start= %x\n",IORD(MYCOUNTER_0_BASE, ICOUNTER));
	//Read a value from the running counter
//	IOWR(MYCOUNTER_0_BASE, ISTOP, ARBITVAL);
//	alt_printf("iCounter after stop1= %x\n",IORD(MYCOUNTER_0_BASE, ICOUNTER));
//	alt_printf("iCounter after stop2= %x\n",IORD(MYCOUNTER_0_BASE, ICOUNTER));
//	//Two consecutive reads to test that the counter is stopped. They should give the same result
//	IOWR(MYCOUNTER_0_BASE, ISTART, ARBITVAL);
//	//Restart the counter
//	alt_printf("iCounter after restart1=%x\n",IORD(MYCOUNTER_0_BASE,ICOUNTER));
//	alt_printf("iCounter after restart2=%x\n",IORD(MYCOUNTER_0_BASE,ICOUNTER));
//	//Two consecutive reads to test that the counter is stopped. They should give different results
//	IOWR(MYCOUNTER_0_BASE, ISTOP, ARBITVAL);
}

static void my_isr(void* context)
{
	IOWR_ALTERA_AVALON_TIMER_SNAPL(TIMER_0_BASE, ARBITVAL);

	snap = IORD_ALTERA_AVALON_TIMER_SNAPL(TIMER_0_BASE);
	IOWR_ALTERA_AVALON_TIMER_SNAPL(TIMER_0_BASE, ARBITVAL);
	v = IORD_ALTERA_AVALON_TIMER_SNAPL(TIMER_0_BASE);
	IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE,0); //Clear interrupt (ITO)
	IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_0_BASE, 0); //CLEAR TO
	//IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE,7);
	flag=1; //Flag is a global variable
}


int main()
{
	void *NULL;
	alt_printf("Hello");
	alt_ic_isr_register(TIMER_0_IRQ_INTERRUPT_CONTROLLER_ID,TIMER_0_IRQ,my_isr,NULL,NULL);
	flag=0 ; //Flag is a global variable
	IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE,0);
	IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_0_BASE,0);//Clear control register
	//IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE,2); //Continuous mode ON
	IOWR_ALTERA_AVALON_TIMER_PERIODL(TIMER_0_BASE, 0xFFFF); //Set initial value
	IOWR_ALTERA_AVALON_TIMER_PERIODH(TIMER_0_BASE, 0x00FF);
	//IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE,3); //Enable timer interrupt
	IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE,7); //Start timer
	while(1)
	{
		// Normal program routine HERE…
		if(flag)
		{
			printf("a %u    d %x \n",0xffff-snap+1,0xffff-v+1);
			//alt_printf("c %x \n",snap);
			flag=0;
			IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE,7); //Enable IRQ and Start timer
		}
		d =IORD(TIMER_0_BASE,0x4);
//		alt_printf("b %x \n",IORD(TIMER_0_BASE,0x4));
//		alt_printf("d %x \n",IORD_ALTERA_AVALON_TIMER_STATUS(TIMER_0_BASE));
//		alt_printf("f %x \n",IORD_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE));
	};
	return 0;
}
