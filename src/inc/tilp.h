#ifndef H_TILP
#define H_TILP

#define PORT_TI PORTD
#define TI_RING 0 // white
#define TI_TIP  1 // red

#define TILP_DELAY 500

#define MACHINE_ID 0x23 //PC

//command codes
#define CMD_VAR   0x06  //Includes a std variable header (used in receiving)
#define CMD_CTS   0x09  //Clear to send: OK to send a variable
#define CMD_DATA  0x15  //Xmit Data Packet (pure data)
#define CMD_FLASH 0x2D  //Used for sending a FLASH app
#define CMD_EXIT  0x36  //Skip/exit - a four byte rejection code (*)
#define CMD_ACK   0x56  //Acknowledgment
#define CMD_ERR   0x5A  //Checksum error: send last packet again
#define CMD_RDY   0x68  //Check if ready
#define CMD_SCR   0x6D  //Request screenshot
#define CMD_CMD   0x87  //Direct command (for remote control for instance)
#define CMD_EOT   0x92  //End Of Transmission: no more variables to send
#define CMD_REQ   0xA2  //Request variable - includes a standard variable
#define CMD_RTS   0xC9  //Request to send - includes a padded variable header

//rejection codes
#define REJ_EXIT  0x01  //EXIT, the entire transfer has been cancelled
#define REJ_SKIP  0x02  //SKIP, the current var has been skipped by the user
#define REJ_MEM   0x03  //MEM, out of memory

//keys
#define KEY_GRAPH  0x44
#define KEY_DRAW   0x2F
#define KEY_ENTER  0x05

#define KEY_RIGHT  0x01
#define KEY_LEFT   0x02
#define KEY_UP     0x03
#define KEY_DOWN   0x04

void tilp_init();
void tilp_press(uint16_t button);
int tilp_tx(uint8_t v);
int tilp_rx(void);

#endif
