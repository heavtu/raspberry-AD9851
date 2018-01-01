#ifndef libad9851_h__
#define libad9851_h__

extern void setup(void);
extern void setupWithParameters(unsigned long ddsClock, unsigned long freqMax, int	freqUpdate, int clock, int data);
extern void sendFrequency(unsigned long frequency);

#endif  // libad9851_h