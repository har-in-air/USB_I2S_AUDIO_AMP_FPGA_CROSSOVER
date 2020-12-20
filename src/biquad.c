#include "main.h"
#include <math.h>
#include "spi.h"
#include "biquad.h"


#define LOW_PASS  0
#define HIGH_PASS 1

static const double cf_PI = 3.14159265359;
static void biquad_spiTransfer(uint8_t* pCmd, uint8_t* pResponse);
static void biquad_calcFilterCoeffs(double* pCoeff, int type, double fs, double fc, double qfactor );


void biquad_calcFilterCoeffs(double* pCoeffs, int type, double fs, double fc, double qfactor ) {
	//double cf_a0, cf_a1, cf_a2, cf_b1, cf_b2, norm;
	double norm;
	double cf_K = tan(cf_PI*fc/fs);

	if (type == HIGH_PASS) {
		norm = 1.0 / (1.0 + cf_K / qfactor + cf_K*cf_K);
		pCoeffs[0] = 1.0*norm;
		pCoeffs[1] = -2.0 * pCoeffs[0];
		pCoeffs[2] = pCoeffs[0];
//		pCoeffs[3] = -(2.0 * (cf_K*cf_K - 1.0)*norm);
//		pCoeffs[4] = -(1.0 - cf_K / qfactor + cf_K*cf_K) * norm;
		pCoeffs[3] = (2.0 * (cf_K*cf_K - 1.0)*norm);
		pCoeffs[4] = (1.0 - cf_K / qfactor + cf_K*cf_K) * norm;
		}
	else 
	if (type == LOW_PASS) {
		norm = 1.0 / (1.0 + cf_K / qfactor + cf_K * cf_K);
		pCoeffs[0] = cf_K * cf_K * norm;
		pCoeffs[1] = 2.0 * pCoeffs[0];
		pCoeffs[2] = pCoeffs[0];
//		pCoeffs[3] = -2.0 * (cf_K * cf_K - 1.0) * norm;
//		pCoeffs[4] = -(1.0 - cf_K / qfactor + cf_K * cf_K) * norm;
		pCoeffs[3] = 2.0 * (cf_K * cf_K - 1.0) * norm;
		pCoeffs[4] = (1.0 - cf_K / qfactor + cf_K * cf_K) * norm;
	}
}


void biquad_spiTransfer(uint8_t* pCmd, uint8_t* pResponse) {
  uint8_t response[5];  // if read command, 5 response bytes = coefficient value
  spi_select();
  spi_xmit_rcv(pCmd[0]); // command
  response[0] = spi_xmit_rcv(pCmd[1]);
  response[1] = spi_xmit_rcv(pCmd[2]);
  response[2] = spi_xmit_rcv(pCmd[3]);
  response[3] = spi_xmit_rcv(pCmd[4]);
  response[4] = spi_xmit_rcv(pCmd[5]);
  spi_deselect();
  memcpy(pResponse, response, 5);
  }
  

int biquad_loadCoeffs_LR(double fsHz){
  double iir_coeffs[5] = {0.0};  
  int64_t icoeff[5] = {0};
  uint64_t ucoeff;
  uint8_t command_table[20][6] = {0};
  uint8_t addr;
  int inx;

      
  printMsg("\r\nFs = %.1lfHz, Fc = %.1lfHz, Q = %lf\r\n\n", fsHz, BIQUAD_CROSSOVER_FREQ_HZ,  BIQUAD_Q);
  
  biquad_calcFilterCoeffs(iir_coeffs, LOW_PASS, fsHz, BIQUAD_CROSSOVER_FREQ_HZ, BIQUAD_Q );
  for (inx = 0; inx < 5; inx++) {
    icoeff[inx] = (int64_t)(iir_coeffs[inx] * (((int64_t)1) << 36)); // 4.36 fixed point format
    ucoeff = (uint64_t)icoeff[inx];
    addr = (uint8_t)inx;
    command_table[inx][0] = (uint8_t) (0x20 | addr); // write command
    command_table[inx][1] = (uint8_t)((ucoeff>>32)&0xff);
    command_table[inx][2] = (uint8_t)((ucoeff>>24)&0xff);
    command_table[inx][3] = (uint8_t)((ucoeff>>16)&0xff);
    command_table[inx][4] = (uint8_t)((ucoeff>>8)&0xff);
    command_table[inx][5] = (uint8_t)(ucoeff&0xff);
    }
    printMsg("LP0 b0 = %lf %lld\r\n",  iir_coeffs[0], icoeff[0]);
    printMsg("LP0 b1 = %lf %lld\r\n",  iir_coeffs[1], icoeff[1]);
    printMsg("LP0 b2 = %lf %lld\r\n",  iir_coeffs[2], icoeff[2]);
    printMsg("LP0 a1 = %lf %lld\r\n",  iir_coeffs[3], icoeff[3]);
    printMsg("LP0 a2 = %lf %lld\r\n",  iir_coeffs[4], icoeff[4]);

   // Linkwitz-Riley, second biquad is identical
  for (inx = 5; inx < 10; inx++) {
    addr = (uint8_t)inx;
    command_table[inx][0] = (uint8_t)(0x20 | addr); // write command
    command_table[inx][1] = command_table[inx-5][1];
    command_table[inx][2] = command_table[inx-5][2];
    command_table[inx][3] = command_table[inx-5][3];
    command_table[inx][4] = command_table[inx-5][4];
    command_table[inx][5] = command_table[inx-5][5];
    }

  printMsg("\r\n");
  
  biquad_calcFilterCoeffs(iir_coeffs, HIGH_PASS, fsHz, BIQUAD_CROSSOVER_FREQ_HZ, BIQUAD_Q);
  for (inx = 0; inx < 5; inx++) {
    icoeff[inx] = (int64_t)(iir_coeffs[inx] * (((int64_t)1) << 36)); // 4.36 fixed point format
    ucoeff = (uint64_t)icoeff[inx];
    addr = (uint8_t)(10+inx);
    command_table[10+inx][0] = (uint8_t)(0x20 | addr); // write command
    command_table[10+inx][1] = (uint8_t)((ucoeff>>32)&0xff);
    command_table[10+inx][2] = (uint8_t)((ucoeff>>24)&0xff);
    command_table[10+inx][3] = (uint8_t)((ucoeff>>16)&0xff);
    command_table[10+inx][4] = (uint8_t)((ucoeff>>8)&0xff);
    command_table[10+inx][5] = (uint8_t)(ucoeff&0xff);
    }
    printMsg("HP0 b0 = %lf %lld\r\n",  iir_coeffs[0], icoeff[0]);
    printMsg("HP0 b1 = %lf %lld\r\n",  iir_coeffs[1], icoeff[1]);
    printMsg("HP0 b2 = %lf %lld\r\n",  iir_coeffs[2], icoeff[2]);
    printMsg("HP0 a1 = %lf %lld\r\n",  iir_coeffs[3], icoeff[3]);
    printMsg("HP0 a2 = %lf %lld\r\n",  iir_coeffs[4], icoeff[4]);

   // Linkwitz-Riley, second biquad is identical
  for (inx = 15; inx < 20; inx++) {
    addr = (uint8_t)inx;
    command_table[inx][0] = (uint8_t)(0x20 | addr); // write command
    command_table[inx][1] = command_table[inx-5][1];
    command_table[inx][2] = command_table[inx-5][2];
    command_table[inx][3] = command_table[inx-5][3];
    command_table[inx][4] = command_table[inx-5][4];
    command_table[inx][5] = command_table[inx-5][5];
  }


  printMsg("\r\nSPI command byte buffers\r\n");
  for (inx = 0; inx < 20; inx++) {
    printMsg("%d : %02X:%02X%02X%02X%02X%02X\r\n",
    inx, command_table[inx][0], command_table[inx][1], command_table[inx][2], command_table[inx][3], command_table[inx][4],command_table[inx][5]);
    if (inx%5 == 4) printMsg("\r\n");
    }

    uint8_t response[5] = {0};

    printMsg("\r\nTransmitting coefficients ");
    for (inx = 0; inx < 20; inx++) {
      biquad_spiTransfer(command_table[inx], response);
      printMsg(".");
      }

    uint8_t cmd[6] = {0};
    int flagError = 0;
    printMsg("\r\nReading back coefficients\r\n");
    for (inx = 0; inx < 20; inx++) {
      cmd[0] = 0x40 | (uint8_t)inx; // read command
      memset(response, 0, 5);
      biquad_spiTransfer(cmd, response);
      printMsg("Coeff[%d] = 0x%02X%02X%02X%02X%02X\r\n", inx, response[0],response[1],response[2],response[3],response[4]);
      if (inx%5 == 4) printMsg("\r\n");
      if ( (response[0] != command_table[inx][1])  ||
          (response[1] != command_table[inx][2])  ||
          (response[2] != command_table[inx][3])  ||
          (response[3] != command_table[inx][4])  ||
          (response[4] != command_table[inx][5])) {
            flagError = 1;
            break;
            }
        }

    if (flagError) {
      printMsg("Error coefficient read/write mismatch\r\n");
      return 0;  
      }
    else {
      printMsg("\r\nFlag biquad coefficients OK to load\r\n");
      cmd[0] = 0x60; // command to signal FPGA audiosystem to load new coefficients
      biquad_spiTransfer(cmd, response);
      return 1;
      }
    }


/*
Fs = 48000.0Hz, Fc = 340.0Hz, Q = 0.707000

LP0 b0 = 0.000480 32985939
LP0 b1 = 0.000960 65971878
LP0 b2 = 0.000480 32985939
LP0 a1 = -1.937070 -133114440989
LP0 a2 = 0.938990 64526908010

HP0 b0 = 0.969015 66590206433
HP0 b1 = -1.938030 -133180412867
HP0 b2 = 0.969015 66590206433
HP0 a1 = -1.937070 -133114440989
HP0 a2 = 0.938990 64526908010

SPI command byte buffers
0 : 20:0001F75353
1 : 21:0003EEA6A6
2 : 22:0001F75353
3 : 23:E101C2D2E3
4 : 24:0F061A7A6A

5 : 25:0001F75353
6 : 26:0003EEA6A6
7 : 27:0001F75353
8 : 28:E101C2D2E3
9 : 29:0F061A7A6A

10 : 2A:0F8115E9E1
11 : 2B:E0FDD42C3D
12 : 2C:0F8115E9E1
13 : 2D:E101C2D2E3
14 : 2E:0F061A7A6A

15 : 2F:0F8115E9E1
16 : 30:E0FDD42C3D
17 : 31:0F8115E9E1
18 : 32:E101C2D2E3
19 : 33:0F061A7A6A
 */

