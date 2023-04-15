
/*
Copyright (C) 2014  Frank Duignan

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "realm.h"
#include "stm32l031lib.h"
#include "musical_notes.h"

void SysTick_Handler(void);
void playNote(uint32_t, uint32_t);
void introSong(void);
void baddieSong(void);
void winSong(void);
void loseSong(void);
void magicSong(void);
void goldSong(void);

// Find types: h(ealth),s(trength),m(agic),g(old),w(eapon)
static const char FindTypes[]={'h','s','m','g','w'};


// The following arrays define the bad guys and 
// their battle properies - ordering matters!
// Baddie types : O(gre),T(roll),D(ragon),H(ag)
static const char Baddies[]={'O','T','D','H'};
// The following is 4 sets of 4 damage types
static const byte WeaponDamage[]={10,10,5,25,10,10,5,25,10,15,5,15,5,5,2,10};
#define ICE_SPELL_COST 10
#define FIRE_SPELL_COST 20
#define LIGHTNING_SPELL_COST 30
static const byte FreezeSpellDamage[]={10,20,5,0};
static const byte FireSpellDamage[]={20,10,5,0};
static const byte LightningSpellDamage[]={15,10,25,0};
static const byte BadGuyDamage[]={10,10,15,5};
static int GameStarted = 0;
static tPlayer thePlayer;
static tRealm theRealm;

static volatile uint32_t SoundDuration = 0;

#define CPU_FREQ 16000000


__attribute__((noreturn)) void runGame(void)  
{
	// for buzzer
	RCC->IOPENR |= 1; // enable GPIOA
	pinMode(GPIOA,0,1); // Make PORTA Bit 0 an output
	SysTick->LOAD = 15999; // 16MHz / 16000 = 1kHz
	SysTick->CTRL = 7;
	enable_interrupts();
	
	// for LEDS
	RCC->IOPENR = 1;   // Turn on GPIOA
	pinMode(GPIOA, 1, 1); // Make GPIOA_1 an output - RED
	pinMode(GPIOA, 4, 1); // Make GPIOA_4 an output - GREEN
	pinMode(GPIOA, 5, 1); // Make GPIOA_5 an output - BLUE
	pinMode(GPIOA, 6, 1); // Make GPIOA_6 an output - YELLOW
	pinMode(GPIOA, 7, 1); // Make GPIOA_7 an output - WHITE
	
		
	char ch;
	
	eputs("MicroRealms on the STM32L031\r\n");	
	showHelp();		
	while(GameStarted == 0)
	{
		introSong();			
		showGameMessage("Press S to start a new game\r\n");
		ch = getUserInput();			
		
		if ( (ch == 'S') || (ch == 's') )
			GameStarted = 1;
	} // end while
	
	initRealm(&theRealm);	
	initPlayer(&thePlayer,&theRealm);
	showPlayer(&thePlayer);
	showRealm(&theRealm,&thePlayer);
	showGameMessage("Press H for help");
	
	while (1)
	{
		
		ch = getUserInput();
		ch = ch | 32; // enforce lower case
		switch (ch) {
			case 'h' : {
				showHelp();
				break;
			}
			case 'w' : {
				showGameMessage("North");
				step('w',&thePlayer,&theRealm);
				break;
			}
			case 's' : {
				showGameMessage("South");
				step('s',&thePlayer,&theRealm);
				break;

			}
			case 'd' : {
				showGameMessage("East");
				step('d',&thePlayer,&theRealm);
				break;
			}
			case 'a' : {
				showGameMessage("West");
				step('a',&thePlayer,&theRealm);
				break;
			}
			case '#' : {		
				if (thePlayer.wealth)		
				{
					showRealm(&theRealm,&thePlayer);
					thePlayer.wealth--;
				}
				else
					showGameMessage("No gold!");
				break;
			}
			case 'p' : {				
				showPlayer(&thePlayer);
				break;
			}
		} // end switch
		
	} // end while
	
} // end function run_game()


void step(char Direction,tPlayer *Player,tRealm *Realm)
{
	uint8_t new_x, new_y;
	new_x = Player->x;
	new_y = Player->y;
	byte AreaContents;
	switch (Direction) {
		case 'w' :
		{
			if (new_y > 0)
				new_y--;
			break;
		}
		case 's' :
		{
			if (new_y < MAP_HEIGHT-1)
				new_y++;
			break;
		}
		case 'd' :
		{
			if (new_x <  MAP_WIDTH-1)
				new_x++;
			break;
		}
		case 'a' :
		{
			if (new_x > 0)
				new_x--;
			break;
		}		
	}
	AreaContents = Realm->map[new_y][new_x];
	if ( AreaContents == '*')
	{
		showGameMessage("A rock blocks your path.");
		return;
	}
	Player->x = new_x;
	Player->y = new_y;
	int Consumed = 0;
	switch (AreaContents)
	{
		
		// const char Baddies[]={'O','T','B','H'};
		case 'O' :{
			baddieSong(); 
			GPIOA->ODR = GPIOA->ODR| (1<<4); // turn on green LED
			
			eputs("\r\n	                     __,='`````'=/__");
			eputs("\r\n                      '//  (o) \\(o) \\ `'         _,-,");
			eputs("\r\n                      //|     ,_)   (`\\      ,-'`_,-\\");
			eputs("\r\n                    ,-~~~\\  `'==='  /-,      \\==```` \\__");
			eputs("\r\n                   /        `----'     `\\     \\       \\/");
			eputs("\r\n                ,-`                  ,   \\  ,.-\\       \\");
			eputs("\r\n               /      ,               \\,-`\\`_,-`\\_,..--'\\");
			eputs("\r\n              ,`    ,/,              ,>,   )     \\--`````\\");
			eputs("\r\n              (      `\\`---'`  `-,-'`_,<   \\      \\_,.--'`");
			eputs("\r\n               `.      `--. _,-'`_,-`  |    \\");
			eputs("\r\n                [`-.___   <`_,-'`------(    /");
			eputs("\r\n                (`` _,-\\   \\ --`````````|--`");
			eputs("\r\n                 >-`_,-`\\,-` ,          |");
			eputs("\r\n               <`_,'     ,  /\\          /");
			eputs("\r\n                `  \\/\\,-/ `/  \\/`\\_/V\\_/");
			eputs("\r\n                   (  ._. )    ( .__. )");
			eputs("\r\n                   |      |    |      |");
			eputs("\r\n                    \\,---_|    |_---./");
			eputs("\r\n                    ooOO(_)    (_)OOoo");
			
			showGameMessage("\r\nA smelly green Ogre appears before you");
			Consumed = doChallenge(Player,0);
			break;
		}
		case 'T' :{
			baddieSong();
			GPIOA->ODR = GPIOA->ODR| (1<<5); // turn on blue LED
			
			eputs("\r\n  ,  ,  , , ,");
			eputs("\r\n <(__)> | | |");
			eputs("\r\n | \\/ | \\_|_/");
			eputs("\r\n \\^  ^/   |");
			eputs("\r\n /\\--/\\  /|");
			eputs("\r\n/  \\/  \\/ |");

			
			showGameMessage("\r\nAn evil troll challenges you");
			Consumed = doChallenge(Player,1);
			break;
		}
		case 'D' :{
			baddieSong();
			GPIOA->ODR = GPIOA->ODR| (1<<1); // turn on red LED
			
			eputs("\r\n                __        _");      
			eputs("\r\n          _/  \\    _(\\(o");     
			eputs("\r\n         /     \\  /  _  ^^^o"); 
			eputs("\r\n        /   !   \\/  ! '!!!v'"); 
			eputs("\r\n       !  !  \\ _' ( \\____");    
			eputs("\r\n       ! . \\ _!\\   \\===^\\)");   
			eputs("\r\n        \\ \\_!  / __!");         
			eputs("\r\n         \\!   /    \\");         
			eputs("\r\n   (\\_      _/   _\\ )");        
			eputs("\r\n    \\ ^^--^^ __-^ /(__");       
			eputs("\r\n     ^^----^^    \"^--v'");
			
			showGameMessage("\r\nA smouldering Dragon blocks your way !");
			Consumed = doChallenge(Player,2);			
			break;
		}
		case 'H' :{
			baddieSong();
			GPIOA->ODR = GPIOA->ODR| (1<<6); // turn on yellow LED
			
			eputs("\r\n	                /\\");
			eputs("\r\n                _/__\\_");
			eputs("\r\n                /( o\\");
			eputs("\r\n           /|  // \\-'");
			eputs("\r\n      __  ( o,    /\\");
			eputs("\r\n        ) / |    / _\\");
			eputs("\r\n >>>>==(_(__u---(___ )-----");
			eputs("\r\n                  //");
			eputs("\r\n                  /__)ns");
			
			showGameMessage("\r\nA withered hag cackles at you wickedly");
			Consumed = doChallenge(Player,3);
			break;
		}
		case 'h' :{
			
			magicSong();
			eputs("\r\n	 {}");
			eputs("\r\n	 ||");
			eputs("\r\n	 )(");
			eputs("\r\n	|__|");
			eputs("\r\n	|  |");    
			eputs("\r\n	|__|");
			
			showGameMessage("\r\nYou find an elixer of health");
			setHealth(Player,Player->health+10);
			Consumed = 1;		
			break;
			
		}
		case 's' :{
			
			magicSong();
			eputs("\r\n	 {}");
			eputs("\r\n	 ||");
			eputs("\r\n	 )(");
			eputs("\r\n	|__|");
			eputs("\r\n	|  |");    
			eputs("\r\n	|__|");
			
			showGameMessage("\r\nYou find a potion of strength");
			Consumed = 1;
			setStrength(Player,Player->strength+1);
			break;
		}
		case 'g' :{ 
			
			goldSong();
			showGameMessage("\r\nYou find a shiny golden nugget");
			Player->wealth++;	
			Consumed = 1;
			break;
		}
		case 'm' :{
			magicSong();
			eputs("\r\n									 *");
			eputs("\r\n			 *   *");
			eputs("\r\n		 *    \\* / *");
			eputs("\r\n			 * --.:. *");
			eputs("\r\n			*   * :\\ -");
			eputs("\r\n				.*  | \\");
			eputs("\r\n			 * *     \\");
			eputs("\r\n		 .  *       \\");
			eputs("\r\n			..        /\\.");
			eputs("\r\n		 *          |\\)|");
			eputs("\r\n	 .   *         \\ |");
			eputs("\r\n	. . *           |/\\");
			eputs("\r\n		 .* *         /  \\");
			eputs("\r\n	 *              \\ / \\");
			eputs("\r\n *  .  *           \\   \\");
			eputs("\r\n		* .");  
			eputs("\r\n	 *    *");    
			eputs("\r\n	.   *    *");
				
			showGameMessage("\r\nYou find a magic charm");
			Player->magic++;						
			Consumed = 1;
			break;
		}
		case 'w' :{
			Consumed = addWeapon(Player,(int)random(MAX_WEAPONS-1)+1);
			showPlayer(Player);
			break;			
		}
		case 'X' : {
			// Player landed on the exit
			winSong();
			eputs("\r\n        __________");
			eputs("\r\n       |  __  __  |");
			eputs("\r\n       | |  ||  | |");
			eputs("\r\n       | |  ||  | |");
			eputs("\r\n       | |__||__| |");
			eputs("\r\n       |  __  __()|");
			eputs("\r\n       | |  ||  | |");
			eputs("\r\n       | |  ||  | |");
			eputs("\r\n       | |  ||  | |");
			eputs("\r\n       | |  ||  | |");
			eputs("\r\n       | |__||__| |");
			eputs("\r\n       |__________|");
			
			
			eputs("\r\nA door! You exit into a new realm");
			setHealth(Player,100); // maximize health
			initRealm(&theRealm);
			showRealm(&theRealm,Player);
		}
	}
	if (Consumed)
		Realm->map[new_y][new_x] = '.'; // remove any item that was found
}
int doChallenge(tPlayer *Player,int BadGuyIndex)
{
	char ch;
	char Damage;
	const byte *dmg;
	int BadGuyHealth = 100;
	
	eputs("Press f to fight");
	

	ch = getUserInput() | 32; // get user input and force lower case
	
	//if (0 == (GPIOB->IDR & (1u << 4)))
	if (ch == 'f')
	{
		eputs("\r\nChoose action");
		while ( (Player->health > 0) && (BadGuyHealth > 0) )
		{
			eputs("\r\n");
			// Player takes turn first
			if (Player->magic > ICE_SPELL_COST)
				eputs("\r\n(I)CE spell");
			if (Player->magic > FIRE_SPELL_COST)
				eputs("\r\n(F)ire spell");
			if (Player->magic > LIGHTNING_SPELL_COST)
				eputs("\r\n(L)ightning spell");
			if (Player->Weapon1)
			{
				eputs("\r\n(1)Use ");
				eputs(getWeaponName(Player->Weapon1));
			}	
			if (Player->Weapon2)
			{
				eputs("\r\n(2)Use ");
				eputs(getWeaponName(Player->Weapon2));
			}
			eputs("\r\n(P)unch");
			ch = getUserInput();
			switch (ch)
			{
				case 'i':
				case 'I':
				{
					eputs("\r\nFREEZE!");
					Player->magic -= ICE_SPELL_COST;
					BadGuyHealth -= FreezeSpellDamage[BadGuyIndex]+random(10);
					zap();
					break;
				}
				case 'f':
				case 'F':
				{
					eputs("\r\nBURN!");
					Player->magic -= FIRE_SPELL_COST;
					BadGuyHealth -= FireSpellDamage[BadGuyIndex]+random(10);
					zap();
					break;
				}
				case 'l':
				case 'L':
				{
					eputs("\r\nZAP!");
					Player->magic -= LIGHTNING_SPELL_COST;
					BadGuyHealth -= LightningSpellDamage[BadGuyIndex]+random(10);
					zap();
					break;
				}
				case '1':
				{
					dmg = WeaponDamage+(Player->Weapon1<<2)+BadGuyIndex;
					eputs("\r\nTake that!");
					BadGuyHealth -= *dmg + random(Player->strength);
					setStrength(Player,Player->strength-1);
					break;
				}
				case '2':
				{
					dmg = WeaponDamage+(Player->Weapon2<<2)+BadGuyIndex;
					eputs("\r\nTake that!");
					BadGuyHealth -= *dmg + random(Player->strength);
					setStrength(Player,Player->strength-1);
					break;
				}
				case 'p':
				case 'P':
				{
					eputs("\r\nThump!");
					BadGuyHealth -= 1+random(Player->strength);
					setStrength(Player,Player->strength-1);
					break;
				}
				default: {
					eputs("\r\nYou fumble. Uh oh");
				}
			}
			// Bad guy then gets a go 
			
			if (BadGuyHealth < 0)
				BadGuyHealth = 0;
			Damage = (uint8_t)(BadGuyDamage[BadGuyIndex]+(int)random(5));
			setHealth(Player,Player->health - Damage);
			eputs("\r\nHealth: you "); printDecimal(Player->health);
			eputs(", them " );printDecimal((uint32_t)BadGuyHealth);
			eputs("\r\n");
		}

		if (Player->health == 0)
		{ // You died
			GPIOA->ODR = GPIOA->ODR & ~(1u<<1);
			GPIOA->ODR = GPIOA->ODR & ~(1u<<4);	
			GPIOA->ODR = GPIOA->ODR & ~(1u<<6);
			GPIOA->ODR = GPIOA->ODR & ~(1u<<5);	
			loseSong();
			eputs("\r\nYou are dead. Press Reset to restart");		
			while(1);
		}
		else
		{ // You won!		
			GPIOA->ODR = GPIOA->ODR & ~(1u<<1);
			GPIOA->ODR = GPIOA->ODR & ~(1u<<4);	
			GPIOA->ODR = GPIOA->ODR & ~(1u<<6);
			GPIOA->ODR = GPIOA->ODR & ~(1u<<5);	
					
			Player->wealth = (uint8_t)(50 + random(50));			
			showGameMessage("\r\nYou win! Their gold is yours");
			winSong();
			return 1;
		}
		
	}
	else
	{
		GPIOA->ODR = GPIOA->ODR & ~(1u<<1);
		GPIOA->ODR = GPIOA->ODR & ~(1u<<4);
		GPIOA->ODR = GPIOA->ODR & ~(1u<<6);
		GPIOA->ODR = GPIOA->ODR & ~(1u<<5);		
		loseSong();
		showGameMessage("\r\nOur 'hero' chickens out");
		return 0;
	}
}
int addWeapon(tPlayer *Player, int Weapon)
{
	char c;
	eputs("\r\nYou stumble upon ");
	switch (Weapon)
	{
		case 1:
		{	
			eputs("a mighty axe");
			break;
		}
		case 2:
		{	
			eputs("a sword with mystical runes");
			break;
		}
		case 3:
		{	
			eputs("a bloody flail");
			break;
		}		
		default:
			printDecimal((uint32_t)Weapon);
	}
	if ( (Player->Weapon1) && (Player->Weapon2) )
	{
		// The player has two weapons already.
		showPlayer(Player);
		eputs("\r\nYou already have two weapons\r\n");		
		eputs("(1) drop Weapon1, (2) for Weapon2, (0) skip");
		c = getUserInput();
		eputchar(c);
		switch(c)
		{
			case '0':{
				return 0; // don't pick up
			}
			case '1':{
				Player->Weapon1 = (uint8_t)Weapon;
				break;
			}
			case '2':{
				Player->Weapon2 = (uint8_t)Weapon;
				break;
			}
		}
	}
	else
	{
		if (!Player->Weapon1)
		{
			Player->Weapon1 = (uint8_t)Weapon;	
		}
		else if (!Player->Weapon2)
		{
			Player->Weapon2 = (uint8_t)Weapon;
		}
	}	
	return 1;
}
const char *getWeaponName(int index)
{
	switch (index)
	{
		case 0:return "Empty"; 
		case 1:return "Axe";
		case 2:return "Sword"; 
		case 3:return "Flail"; 
	}
	return "Unknown";
}

void setHealth(tPlayer *Player,int health)
{
	if (health > 100)
		health = 100;
	if (health < 0)
		health = 0;
	Player->health = (uint8_t)health;
}	
void setStrength(tPlayer *Player, byte strength)
{
	if (strength > 100)
		strength = 100;
	Player->strength = strength;
}
void initPlayer(tPlayer *Player,tRealm *Realm)
{
	// get the player name
	int index=0;
	byte x,y;
	char ch=0;
	// Initialize the player's attributes
	eputs("\r\nEnter the player's name: ");
	while ( (index < MAX_NAME_LEN) && (ch != '\n') && (ch != '\r'))
	{
		ch = getUserInput();
		if ( ch > '0' ) // strip conrol characters
		{
			
			Player->name[index++]=ch;
			eputchar(ch);
		}
	}
	Player->name[index]=0; // terminate the name
	setHealth(Player,100);
	Player->strength=(uint8_t)(50+random(50));
	Player->magic=(uint8_t)(50+random(50));	
	Player->wealth=(uint8_t)(10+random(10));
	Player->Weapon1 = 0;
	Player->Weapon2 = 0;
	// Initialize the player's location
	// Make sure the player does not land
	// on an occupied space to begin with
	do {
		x=(uint8_t)random(MAP_WIDTH);
		y=(uint8_t)random(MAP_HEIGHT);
		
	} while(Realm->map[y][x] != '.');
	Player->x=x;
	Player->y=y;
}
void showPlayer(tPlayer *Player)
{
	eputs("\r\nName: ");
	eputs(Player->name);
	eputs("\r\nhealth: ");
	printDecimal(Player->health);
	eputs("\r\nstrength: ");
	printDecimal(Player->strength);
	eputs("\r\nmagic: ");
	printDecimal(Player->magic);
	eputs("\r\nwealth: ");
	printDecimal(Player->wealth);	
	eputs("\r\nLocation : ");
	printDecimal(Player->x);
	eputs(" , ");
	printDecimal(Player->y);	
	eputs("\r\nWeapon1 : ");
	eputs(getWeaponName(Player->Weapon1));
	eputs("\r\nWeapon2 : ");
	eputs(getWeaponName(Player->Weapon2));
}
void initRealm(tRealm *Realm)
{
	unsigned int x,y;
	unsigned int Rnd;
	// clear the map to begin with
	for (y=0;y < MAP_HEIGHT; y++)
	{
		for (x=0; x < MAP_WIDTH; x++)
		{
			Rnd = random(100);
			
			if (Rnd >= 98) // put in some baddies
				Realm->map[y][x]=	Baddies[random(sizeof(Baddies))];
			else if (Rnd >= 95) // put in some good stuff
				Realm->map[y][x]=	FindTypes[random(sizeof(FindTypes))];
			else if (Rnd >= 90) // put in some rocks
				Realm->map[y][x]='*'; 
			else // put in empty space
				Realm->map[y][x] = '.';	
		}
	}
	
	// finally put the exit to the next level in
	x = random(MAP_WIDTH);
	y = random(MAP_HEIGHT);
	Realm->map[y][x]='X';
}
void showRealm(tRealm *Realm,tPlayer *Player)
{
	int x,y;
	eputs("\r\nThe Realm:\r\n");	
	for (y=0;y<MAP_HEIGHT;y++)
	{
		for (x=0;x<MAP_WIDTH;x++)
		{
			
			if ( (x==Player->x) && (y==Player->y))
				eputchar('@');
			else
				eputchar(Realm->map[y][x]);
		}
		eputs("\r\n");
	}
	eputs("\r\nLegend\r\n");
	eputs("(T)roll, (O)gre, (D)ragon, (H)ag, e(X)it\r\n");
	eputs("(w)eapon, (g)old), (m)agic, (s)trength\r\n");
	eputs("@=You\r\n");
}
void showHelp()
{

	eputs("Help\r\n");
	eputs("W,S,D,A : go North, South, East, West\r\n");
	eputs("# : show map (cost: 1 gold piece)\r\n");
	eputs("(H)elp\r\n");
	eputs("(P)layer details\r\n");
	
}

void showGameMessage(char *Msg)
{
	eputs(Msg);
	eputs("\r\nReady\r\n");	
}
char getUserInput()
{
	char ch = 0;
	
	while (ch == 0)
		ch = egetchar();
	return ch;
}
unsigned random(unsigned range)
{
	// Implementing my own version of modulus
	// as it is a lot smaller than the library version
	// To prevent very long subtract loops, the
	// size of the value returned from prbs has been
	// restricted to 8 bits.
	unsigned Rvalue = (prbs()&0xff);
	while (Rvalue >= range)
		Rvalue -= range; 
	return Rvalue;
}
void zap()
{

}
uint32_t prbs()
{
	// This is an unverified 31 bit PRBS generator
	// It should be maximum length but this has not been verified
	static unsigned long shift_register=0xa5a5a5a5;
	unsigned long new_bit=0;
	static int busy=0; // need to prevent re-entrancy here
	if (!busy)
	{
		busy=1;
		new_bit= ((shift_register & (1<<27))>>27) ^ ((shift_register & (1<<30))>>30);
		new_bit= ~new_bit;
		new_bit = new_bit & 1;
		shift_register=shift_register << 1;
		shift_register=shift_register | (new_bit);
		busy=0;
	}
	return shift_register & 0x7ffffff; // return 31 LSB's
}


void SysTick_Handler(void)
{
	if (SoundDuration > 0)
	{
	 GPIOA->ODR ^= 1; // Toggle Port A bit 0
	 SoundDuration --;
	}
}

void playNote(uint32_t Frequency, uint32_t Duration)
{
	SysTick->LOAD = CPU_FREQ / (2 * Frequency);
	// The Systick timer will now interrupt at twice the specified frequency
	// The time between each interrupt is 0.5 * (1 / Frequency) i.e. a half period
	// of the desired sound frequency. This note is supposed to play for "Duration"
	// milliseconds. We need to scale this in units of equal to the time between
	// interrupts. The time between interrupts = 0.5/Frequency or 1/(2*Frequency)
	// Lets suppose you want to play a note of 400Hz for 100ms.
	// Frequency = 400Hz so 1/(2*Frequency) = 1/800.
	// How many intervals of 1/800 will fit into 100ms?
	// Answer : 100x10^-3 / (1 / 800)
	// = 800 * 100 x 10^-3 = 800*100 / 1000 = 80.
	// So the parmater "Duration needs to be scaled as follows:
	// Duration = Duration * 2 * Frequency / 1000
	SoundDuration = (Duration * 2 * Frequency) / 1000;
	while(SoundDuration != 0); // Wait
}

void introSong(void)
{
	// turn each LED on before the corresponding note and off after to sync them
	GPIOA->ODR = GPIOA->ODR| (1<<1);
	playNote(C4,250);
	GPIOA->ODR = GPIOA->ODR & ~(1u<<1);
	
	GPIOA->ODR = GPIOA->ODR| (1<<4);
	playNote(D4,250);
	GPIOA->ODR = GPIOA->ODR & ~(1u<<4);
	
	GPIOA->ODR = GPIOA->ODR| (1<<6);
	playNote(E4,250);
	GPIOA->ODR = GPIOA->ODR & ~(1u<<6);
	
	GPIOA->ODR = GPIOA->ODR| (1<<5);
	playNote(F4,250);
	GPIOA->ODR = GPIOA->ODR & ~(1u<<5);
	
	GPIOA->ODR = GPIOA->ODR| (1<<7);
	playNote(G4,250);
	GPIOA->ODR = GPIOA->ODR & ~(1u<<7);

	GPIOA->ODR = GPIOA->ODR| (1<<5);
	playNote(F4,250);
	GPIOA->ODR = GPIOA->ODR & ~(1u<<5);
	
	GPIOA->ODR = GPIOA->ODR| (1<<6);
	playNote(E4,250);
	GPIOA->ODR = GPIOA->ODR & ~(1u<<6);
	
	GPIOA->ODR = GPIOA->ODR| (1<<4);
	playNote(D4,250);
	GPIOA->ODR = GPIOA->ODR & ~(1u<<4);
	
	GPIOA->ODR = GPIOA->ODR| (1<<1);
	playNote(C4,250);
	GPIOA->ODR = GPIOA->ODR & ~(1u<<1);
}

void baddieSong(void)
{
	//repeat noise and flashing LEDs 6 times
	for(int i = 0; i < 6; i++)
	{
		GPIOA->ODR = GPIOA->ODR| (1<<1);
		playNote(B4,250);
		GPIOA->ODR = GPIOA->ODR & ~(1u<<1);
		
		GPIOA->ODR = GPIOA->ODR| (1<<7);
		playNote(C4,250);
		GPIOA->ODR = GPIOA->ODR & ~(1u<<7);
				
	}
}

void winSong(void)
{

	GPIOA->ODR = GPIOA->ODR| (1<<1);
	playNote(G5,250);
	GPIOA->ODR = GPIOA->ODR & ~(1u<<1);
	
	GPIOA->ODR = GPIOA->ODR| (1<<4);
	playNote(C6,250);
	GPIOA->ODR = GPIOA->ODR & ~(1u<<4);
	
	GPIOA->ODR = GPIOA->ODR| (1<<6);
	playNote(E6,250);
	GPIOA->ODR = GPIOA->ODR & ~(1u<<6);
	
	GPIOA->ODR = GPIOA->ODR| (1<<7);
	playNote(G6,250);
	GPIOA->ODR = GPIOA->ODR & ~(1u<<7);
	
	GPIOA->ODR = GPIOA->ODR| (1<<6);
	playNote(E6,250);
	GPIOA->ODR = GPIOA->ODR & ~(1u<<6);
	
	GPIOA->ODR = GPIOA->ODR| (1<<7);
	playNote(G6,250);
	GPIOA->ODR = GPIOA->ODR & ~(1u<<7);
		
}

void loseSong(void)
{
	GPIOA->ODR = GPIOA->ODR| (1<<7);
	playNote(G6,250);
	GPIOA->ODR = GPIOA->ODR & ~(1u<<7);
	
	GPIOA->ODR = GPIOA->ODR| (1<<6);
	playNote(FS6_Gb6,250);
	GPIOA->ODR = GPIOA->ODR & ~(1u<<6);
	
	GPIOA->ODR = GPIOA->ODR| (1<<4);
	playNote(F6,250);
	GPIOA->ODR = GPIOA->ODR & ~(1u<<4);
	
	GPIOA->ODR = GPIOA->ODR| (1<<1);
	playNote(E6,250);
	GPIOA->ODR = GPIOA->ODR & ~(1u<<1);
	
	
}

void magicSong(void)
{
	playNote(C5,200);
	playNote(E5,200);
	playNote(G5,200);
	playNote(C6,200);
	playNote(G5,200);
	playNote(E5,200);
	playNote(C5,200);
	playNote(E5,200);
	playNote(G5,200);
	playNote(C6,200);
	
}

void goldSong(void)
{
	playNote(B5,200);
	playNote(E6,200);	
}

