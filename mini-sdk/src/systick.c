#include "systick.h"

#define bits_on(x, m)   ((x) |= (m))
#define bits_off(x, m)  ((x) &= ~(m))

struct systick_hw {
	uint csr;
	uint rvr;
	uint cvr;
	uint calib;
};
static unsigned long csr_local;

#define systick 	((volatile struct systick_hw*)(PPB_BASE + 0xE010))

#define CSR_COUNTFLAG 	(1 << 16)
#define CSR_CLKSRC 		(1 << 2)
#define CSR_TICKINT 	(1 << 1)
#define CSR_ENABLE 		(1 << 0)


void systick_init() {
	systick->csr = CSR_ENABLE | CSR_CLKSRC | CSR_TICKINT;
}

void systick_deinit() {
	systick->csr = 0;
}

void systick_set(uint value) {
	systick->rvr = value;
	systick->cvr = value;
}

uint systick_get() {
	return systick->cvr;
}
void systick_enable() {
    bits_on(csr_local, (1 << CSR_ENABLE));
    systick->csr = csr_local;
}

void systick_disable() {
    bits_off(csr_local, (1 << CSR_ENABLE));
    systick->csr = csr_local;
}

