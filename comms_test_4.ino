//comms_test_4 by Noah Rosenblum
//it's supposed to make sending comms group messages for space capstone 2019 easier, somehow...

//binary to hex converter https://www.rapidtables.com/convert/number/binary-to-hex.html
#include<stdio.h>
#include<stdint.h>

void FromByte(unsigned char c, bool b[8]) //convert bool to char (byte)
{
    for (int i=0; i < 8; ++i)
        b[i] = (c & (1<<i)) != 0;
}


unsigned short crc16(unsigned char* datas, unsigned char length){ //my fourth attempt at the crc. this one isn't even mine
    unsigned char buf;
    unsigned short crc = 0xFFFF;

    while (length--){
        buf = crc >> 8 ^ *datas++;
        buf ^= buf>>4;
        crc = (crc << 8) ^ ((unsigned short)(buf << 12)) ^ ((unsigned short)(buf <<5)) ^ ((unsigned short)buf);
    }
    return crc;
}

/*
//make some FCS verification
byte* makeFCS(byte* info, int len) { //if message is an array of chars instead of an array of bytes
  byte genpol = 0x1021; //generator polynomeal for ccitt-crc, 0x1021 in hex
  byte flipgen = 0x8408; //bit flipped constant value of the generator polynomeal - just trust me, this works :(
  int r = 16; //length of the shift register
  bool flipgen2[r] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0};
  int nBits = len*8; // number of bits in info
  bool sr[r]; //initialize shift register to all ones
  int i,j;
  for(i = 0; i<r; i++) {
    sr[i] = 1;
  } //end shift register initialization
  // loop through all the bits and shift them into the shift register
  for(i = 0; i<nBits; i++) { //master loop
    bool outBit = sr[r]; //bit shifted out of the register
    for(j = r; j > 0; j--) //shifting the bits right by 1
    {
        sr[j] = sr[j-1];
    }
    sr[0]=0;
    bool xorMask[r];
    bool infoi[8];
    FromByte(info[i], infoi); // use above method to save ith element of info as bool array infoi
    for(j = 0; j < r; j++) {
      xorMask[j] = infoi[i] ^ outBit * flipgen2[j]; //input bit xor w/shifted bit
    }
    sr = sr^xorMask; // xor mask applied to shift register
  }
  int i;
  byte *info;
  for (i = r; i>0; i--) {
    info[r-i] = sr[i] * -1 + 1;
  }
  return info;
  
}
*/ //my first attept, doesn't compile
/*
byte* makeFCS(byte* info) {
  byte genpol = 0x1021; //generator polynomeal for ccitt-crc, 0x1021 in hex
  byte flipgen = 0x8408; //bit flipped constant value of the generator polynomeal - just trust me, this works :(
  int r = 16; //length of shift register
  int nBits = 8;
  int sr[r]; //shift register
  int i,j;
  for(i = 0; i<r; i++) {
    sr[i] = 1;
  } //end shift register initialization
  for( i = 0; i<nBits; i++) { //master loop
    bool outBit = sr[r]; //out bit
    for(j = r; j > 0; j--) //shifting the bits right by 1
    {
        sr[j] = sr[j-1];
    }
    sr[0]=0;
    bool xorMask = info[i]^outBit * flipgen; //input bit xor w/shifted bit
    int xormask2 = (int)(xorMask);
    sr = sr^xormask2; // xor mask applied to shift register
  }
}
*/ //my second attempt, also does not compile

char* infoToHex(char* info, int len) {
  int i;
  byte hexInfo[len];
  for (i = 0; i< len; i++) {
    
  }
  //foo bar
}

char* encodePacket(byte* info, int len) { //this function actually works. mostly.
  const byte call[7] = {0x56, 0x41, 0x33, 0x4d, 0x57, 0x41, 0x00}; // call sign and ssid in hex
  byte msg[50];
  msg[0] = {0x7e}; //beginning flag
  int i;
  for (i = 0; i < 6; i++) {
    msg[1+i] = call[i]; //source
    msg[1+7+i] = call[i]; //destination
  }
  msg[15] = 0x03; //control field
  msg[16] = 0xf0; //protocol id (0xf0 for no layer 3 protocol)
  info = infoToHex(info, len);
  //foo add info
  for (i = 0; i<len; i++) {
    msg[17+i] = info[i];
  }
  //
  byte* check = crc16(info,len); //make frame check sequence
  for (i = 0; i<16; i++) {
    msg[i+len+1] = check[i];
  }
  msg[16+3+len] = 0x7e;
  
  
  return msg;
}
/*
uint16_t ax25crc16(unsigned char *data_p, uint16_t lengthe) {////////////////////////////////////////////
    uint16_t crc = 0x8408; // should be 0x1021 but it doesn't work if you change it to that... 0x8408 is it bit reversed.
    uint32_t data;
    uint16_t crc16_table[] = { //the crc lookup table... magic
            0x0000, 0x1081, 0x2102, 0x3183,
            0x4204, 0x5285, 0x6306, 0x7387,
            0x8408, 0x9489, 0xa50a, 0xb58b,
            0xc60c, 0xd68d, 0xe70e, 0xf78f
    };

    while(lengthe--){
        crc = ( crc >> 4 ) ^ crc16_table[(crc & 0xf) ^ (*data_p & 0xf)]; /this makes the magic work... somehow
        crc = ( crc >> 4 ) ^ crc16_table[(crc & 0xf) ^ (*data_p++ >> 4)];
    }

    data = crc;
    crc = (crc << 8) | (data >> 8 & 0xff); // do byte swap here that is needed by AX25 standard
    return (~crc); // invert the whole thing before sending
}
*/ //my third attempt. often gives wrong numbers.
void setupComms() {
  // put your setup code here, to run once:
    unsigned char datap[6] = {'a', 'b', 'c', 'd', 'e', 'f'};
    //int ans= ax25crc16(datap, 6);
    //printf("ans is %d", ans);

    return 0;
}
