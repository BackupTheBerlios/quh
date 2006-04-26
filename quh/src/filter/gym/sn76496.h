#ifndef SN76496_H_ALREADY_INCLUDED
#define	SN76496_H_ALREADY_INCLUDED

#define MAX_76496 4

struct SN76496interface
{
	int num;	/* total number of 76496 in the machine */
	int baseclock[MAX_76496];
	int volume[MAX_76496];
};

void SN76496Update_16(int chip,void *buffer,int length);
void SN76496Write(int chip,int data);
int SN76496_init(int chip,int clock,int sample_rate);

#endif /* SN76496_H_ALREADY_INCLUDED */
