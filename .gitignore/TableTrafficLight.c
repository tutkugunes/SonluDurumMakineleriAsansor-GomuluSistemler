/* 
 * PE0: Positive logic switch1 (UP)
 * PE1: Positive logic switch2 (DOWN)
 * PB0: Positive logic LED1 	 (RED)
 * PB1: Positive logic LED2		 (GREEN)
 * PB2: Positive logic LED3		 (YELLOW)
 *
 *************************************/
	
/* Define Port B addresses*/
#define GPIO_PORTB_DATA_R       (*((volatile unsigned long *)0x400053FC)) 
#define GPIO_PORTB_DIR_R        (*((volatile unsigned long *)0x40005400))
#define GPIO_PORTB_AFSEL_R      (*((volatile unsigned long *)0x40005420))
#define GPIO_PORTB_PUR_R        (*((volatile unsigned long *)0x40005510))
#define GPIO_PORTB_DEN_R        (*((volatile unsigned long *)0x4000551C))
#define GPIO_PORTB_LOCK_R       (*((volatile unsigned long *)0x40005520))
#define GPIO_PORTB_CR_R         (*((volatile unsigned long *)0x40005524))
#define GPIO_PORTB_AMSEL_R      (*((volatile unsigned long *)0x40005528))
#define GPIO_PORTB_PCTL_R       (*((volatile unsigned long *)0x4000552C))
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
#define Leds										(*((volatile unsigned long *)0x400050FC))
	

/* Define Port E addresses*/
#define GPIO_PORTE_DIR_R        (*((volatile unsigned long *)0x40024400))
#define GPIO_PORTE_AFSEL_R      (*((volatile unsigned long *)0x40024420))
#define GPIO_PORTE_PUR_R 				(*((volatile unsigned long *)0x40024510))
#define GPIO_PORTE_DEN_R        (*((volatile unsigned long *)0x4002451C))
#define GPIO_PORTE_AMSEL_R      (*((volatile unsigned long *)0x40024528))
#define GPIO_PORTE_PCTL_R       (*((volatile unsigned long *)0x4002452C))
#define Buttons									(*((volatile unsigned long *)0x4002401C))


void ports_Init(void){
	
	unsigned long volatile delay;
	SYSCTL_RCGC2_R |= 0x12; 				// activate clock for Port B,E
  delay = SYSCTL_RCGC2_R; 				// allow time for clock to start

	/*Port B init  */
  GPIO_PORTB_LOCK_R = 0x4C4F434B;   // unlock port
  GPIO_PORTB_CR_R = 0x07;           // allow changes to PB2-0
	GPIO_PORTB_PCTL_R = 0x00000000;   // clear PCTL
  GPIO_PORTB_AMSEL_R &= ~0x07;      // disable analog on PB2-0
  GPIO_PORTB_AFSEL_R &= ~0x07;      // disable alt funct on PB2-0
  GPIO_PORTB_DEN_R |= 0x07;         // enable digital I/O on PB2-0
	GPIO_PORTB_DIR_R |= 0x07;         // PB2-0 outputs

	/* Port E init  */
	GPIO_PORTE_PCTL_R = 0x00000000;   // clear PCTL
  GPIO_PORTE_AMSEL_R &= ~0x03;      // disable analog on PE1-0
  GPIO_PORTE_AFSEL_R &= ~0x03;      // disable alt funct on PE1-0
  GPIO_PORTE_PUR_R &= ~0x03;        // disableb pull-up on PE1-0
  GPIO_PORTE_DEN_R |= 0x03;         // enable digital I/O on PE1-0
	GPIO_PORTE_DIR_R &= ~0x03;        // PE1-0 inputs

}
typedef struct State{
	unsigned long Out; 					//output for leds 
	unsigned long Time; 			// delay time
	unsigned long Next[4]; 	// 2 input equal 4 next state
} SType;

#define red 0    
#define redGreen 1
#define green	2	
#define greenYellow	3
#define yellow	4
#define yellowGreen	5
#define greenRed	6

void DelayFunc(unsigned long Time)
{
	unsigned long volatile delay;
	
	delay=Time * 727240*200/91; // 2 sec
	
	while(delay)
	{
		delay--;
	}
}


int main(void){ 
	
	unsigned long S = red; // current state
	SType FSM[7]={
		
		{0x01,0,{red,redGreen,red,red}},  //output,time,states
		{0x03,2,{green,green,green,green}},
		{0x02,0,{green,greenYellow,greenRed,green}},
		{0x06,2,{yellow,yellow,yellow,yellow}},
		{0x04,0,{yellow,yellow,yellowGreen,yellow}},
		{0x06,2,{green,green,green,green}},
		{0x03,2,{red,red,red,red}},
	};

	ports_Init(); //for B,E ports init functions
	
	while(1)
	{
    Leds = FSM[S].Out;  				// set leds
    DelayFunc(FSM[S].Time);			//set time
    S = FSM[S].Next[Buttons];  // next state
  }
}