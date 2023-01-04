
/* LICENSE: this file is public domain */

#include <inttypes.h>
#include <stdio.h>
#include <assert.h>

#include "saleae_v2_digital.h"

/**
 * QUAD Serial Peripheral Interface for decoding QPI signals (W25QxxJV)
 *
 * Channels 0-5:
 *   0 - FLASH_CS
 *   1 - FLASH_CLK
 *   2 - FLASH_IO0
 *   3 - FLASH_IO1
 *   4 - FLASH_IO2
 *   5 - FLASH_IO3
 *
 * Ideally or remap channels
 *
 */
#define nFLASH_CS_CH        3
#define nFLASH_CLK_CH       6
#define nFLASH_IO0_CH       4
#define nFLASH_IO1_CH       7
#define nFLASH_IO2_CH       5
#define nFLASH_IO3_CH       2

uint8_t byte = 0xFF;
uint64_t counter = 0;

int main(int argc, char **argv) {
	logic_t logic;
	uint32_t state;
	uint64_t ts;
	double tsd;
	int res;
	
	assert(argc>1);
	res = logic_init(&logic, argv[1]);
	if(res>=0) {
		fprintf(stderr,"%d channels found\n",res);
		fprintf(stderr,"initial state: %04x\n",logic.state);
	}
	
	/* trigger on nRE/nWE rising edges */
	while(logic_replay(&logic, &state, &tsd, (1 << nFLASH_CS_CH) , (1 << nFLASH_CLK_CH))) {
		int nCS  = (state>>nFLASH_CS_CH)&1;
		int nCLK = (state>>nFLASH_CLK_CH)&1;
		int nIO0 = (state>>nFLASH_IO0_CH)&1;
		int nIO1 = (state>>nFLASH_IO1_CH)&1;
		int nIO2 = (state>>nFLASH_IO2_CH)&1;
        int nIO3 = (state>>nFLASH_IO3_CH)&1;

		/* chip selected? */
		if(nCS)
			continue;
		
		/* convert timestamp to integer, 1ns units */
		tsd *= 1000000000.0;
		ts = tsd;

        if(counter % 2 == 0)
        {
            byte = 0x0;
            byte |= (nIO0 << 4);
            byte |= (nIO1 << 5);
            byte |= (nIO2 << 6);
            byte |= (nIO3 << 7);
        }
        else
        {
            byte |= (nIO0);
            byte |= (nIO1 << 1);
            byte |= (nIO2 << 2);
            byte |= (nIO3 << 3);
            printf("  => x%02X\n", byte);
        }

        //printf("%"PRIu64": (%d,%d) %d|%d|%d|%d\n", ts, nCS, nCLK, nIO0, nIO1, nIO2, nIO3);
        ++counter;
	}
	
	logic_cleanup(&logic);
	
	return 0;
}
