#ifndef __Infinite_Timer__
#define __Infinite_Timer__


#include <stdio.h>
#include <stdlib.h>
#include <sys/neutrino.h>		//timer_create(), ConnectAttach(), MsgReceive(), struct _pulse etc.
#include <sys/netmgr.h>			//ND_LOCAL_NODE

/*
//for mesurement of sampling time average
#include <sys/syspage.h>		//SYSPAGE_ENTRY()
#include <inttypes.h>			//uint64_t
*/

#define		HIGH_PRIO	60
#define		MY_PULSE_CODE	_PULSE_CODE_MINAVAIL


// messege definition for MsgReceive()
typedef union {
	//_pulse is QNX's unique event
	//, sent without waiting for acknowledgment from the receiver
	struct _pulse	pulse;

	// your other message structures would go here too 

} 
my_message_t;


void Infinite_Timer(int Samp_Cycle,void(*function)(FILE *), FILE *fp)
{

	my_message_t msg;

	struct sigevent event;			//Structure that specify the event
	struct itimerspec itime;		//Structure that specify the timer, be compliant with POSIX

	timer_t timer_id;			//timer ID
	int chid;				//channel ID
	int rcvid;				//received message ID

	int filedes = 0;			//File Descriptor, 0 means stdin

/*
	//for mesurement of sampling time average
	uint64_t CPS, cycle1, cycle2;
	int cycletime=0;
	float sec;
*/

	//setprio() sets priority of this process.
	setprio(0, HIGH_PRIO);

	//ChannelCreate() requests the kernel to create a channel
	// that this process can use to receive messages and pulses.
	chid = ChannelCreate(0);

	//The kind of event
	event.sigev_notify = SIGEV_PULSE;

	//The connection ID
	//ConnectAttach() establish a connection between a process and a channel.
	event.sigev_coid = ConnectAttach(ND_LOCAL_NODE, 0, chid, _NTO_SIDE_CHANNEL, 0);

	//The priority of the pulse
	event.sigev_priority = getprio(0);

	//The ID of the pulse
	//We can set a code in the range from _PULSE_CODE_MINAVAIL through _PULSE_CODE_MAXAVAI
	event.sigev_code = MY_PULSE_CODE;


	//The first interval
	itime.it_value.tv_sec = 0;
	itime.it_value.tv_nsec = Samp_Cycle * 1000000;		//1msec=1000000nsec

	//The continuing interval
	itime.it_interval.tv_sec = 0;
	itime.it_interval.tv_nsec = Samp_Cycle * 1000000;	//1msec=1000000nsec

	//timer_create() stores the ID of the new timer at &timer_id.
	timer_create(CLOCK_REALTIME, &event, &timer_id);

	//timer_settime() start timer
	timer_settime(timer_id, 0, &itime, NULL);

/*
	// get CPU counter value just before start timer
	cycle1 = ClockCycles();
*/

	//tcischars() determines the number of characters waiting to be read
	while(!tcischars(filedes)) {	//message loop until hit any key at stdin
		rcvid = MsgReceive(chid, &msg, sizeof(msg), NULL);
		if (rcvid == 0) {
			if (msg.pulse.code == MY_PULSE_CODE) {
				function(fp);
				//cycletime++;
			}
		}
	}


/*
	// get CPU counter value immediately after timer stop
	cycle2 = ClockCycles();

	// find out how many cycles per second
	CPS = SYSPAGE_ENTRY(qtime)->cycles_per_sec;

	sec=(float)((cycle2 - cycle1) / cycletime) / CPS;

	//printf("\n This system has %lld cycles/sec.\n", CPS);
	//printf("\n The cycles in seconds is %f \n", sec);
*/

	timer_delete(timer_id);

	ChannelDestroy(chid);

}

#endif
