#ifndef TMR0_H
#define	TMR0_H

#include <xc.h>
#include <stdint.h>

#define _tmr0_value 158 // Tiempo = 100 mS

void tmr0_init(uint8_t prescaler);

void tmr0_reload(void);

#endif	/* TMR0_H */