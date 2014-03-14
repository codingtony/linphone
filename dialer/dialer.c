/*
 * linphone
 * Copyright (C) 2010  Belledonne Communications SARL 
 *  (simon.morlat@linphone.org)
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *  */

#include <linphonecore.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <signal.h>
#include <time.h>




static bool_t running=TRUE;
static int ringCount=0;
static int maxRingCount=3;
static int expectedDtmf=0;
static bool_t ringing=FALSE;
static bool_t confirmWithDtmf=FALSE;
static bool_t gotSomeone=FALSE;
static clock_t startRingingClock = NULL;
const char *wav=NULL;

static void stop(int signum){
        running=FALSE;
}

static void play_finished() {
	running=FALSE;
}

/*
 *  * Call state notification callback
 *   */
static void call_state_changed(LinphoneCore *lc, LinphoneCall *call, LinphoneCallState cstate, const char *msg){
        switch(cstate){
                case LinphoneCallError:
                case LinphoneCallConnected:
		case LinphoneCallOutgoingInit:
		case LinphoneCallOutgoingProgress:
                break;
                case LinphoneCallOutgoingRinging:
                case LinphoneCallOutgoingEarlyMedia:
			printf("Ringing\n");
			ringing=TRUE;
			startRingingClock=clock();
                break;
                case LinphoneCallStreamsRunning:
			ringing=FALSE;
			printf("Start LinphoneCallStreamsRunning\n");
			linphone_core_set_play_file_with_cb(lc,wav,play_finished);
			if (!confirmWithDtmf) {
			  gotSomeone=TRUE;
			}
			printf("End LinphoneCallStreamsRunning\n");
                break;
                case LinphoneCallEnd:
			running=FALSE;
                break;
                default:
                        printf("Unhandled notification %i\n",cstate);
        }
}

static void dtmf_received(LinphoneCore *lc, LinphoneCall *call, int dtmf){
	printf("DTMF %i\n",dtmf);
	if (!gotSomeone && confirmWithDtmf) {
	  printf("Expected DTMF %i\n",expectedDtmf);
	  if (expectedDtmf == dtmf) {
	      gotSomeone=TRUE;
	      running=FALSE;
	  }
	}
}

int main(int argc, char *argv[]){
	ortp_set_log_level_mask(ORTP_FATAL);
        LinphoneCoreVTable vtable={0};
        LinphoneCore *lc;
        LinphoneCall *call=NULL;
	LinphoneCallParams *callParam=NULL;
        const char *dest=NULL;
        const char *conf=NULL;
	
	int c;
 	static struct option long_options[] = {
		{"conf",required_argument,0,'c'},
		{"confirm",required_argument,0,'d'},
		{"number",required_argument,0,'n'},
		{"ring",required_argument,0,'r'},
		{"file",required_argument,0,'f'},
	 	{NULL, 0, 0, 0}
	};

	int option_index=0;
	while (1) {
		c = getopt_long (argc,argv,"c:n:r:f:",long_options,&option_index);
	
		if (c == -1)	{
			break;
		}
		switch (c) {
			case 'c':
				conf = optarg;
			break;
			case 'd':
				expectedDtmf = strtol(optarg, 0,10);
				expectedDtmf  = expectedDtmf + '0';
				confirmWithDtmf=TRUE;
			break;
			case 'n':
				dest = optarg;
			break;
			case 'f':
				wav  = optarg;
			break;
			case 'r':
				maxRingCount = strtol(optarg, 0,10);
			break;
			default:
				return 2;
			break;
				
		}
	}
	if (conf == NULL || dest == NULL || wav == NULL) {
		printf("Dialer 0.1 by Tony Bussieres (tony@codingtony.com)\n");
		printf("==================================================\n");
		printf("Arguments :\n");
		printf("--conf : configuration file (required)\n");
		printf("--number : outgoing number to call (required)\n");
		printf("--file : wave file to play (required)\n");
		printf("--confirm : if the dtmf in parameter is sent, terminate the call and exit 0, otherwise exit 1\n");
		printf("--ring : number of ring to wait before hangup.\n");
		return 2;
	}
	printf("Calling %s\n",dest);
	printf("Using %s for configuration\n",conf);
	printf("Will stop after %d ring\n",maxRingCount);
	printf("Will play %s on answer\n", wav);
	
        signal(SIGINT,stop);
        vtable.call_state_changed=call_state_changed;
	vtable.dtmf_received=dtmf_received;

        lc=linphone_core_new(&vtable,conf,NULL,NULL);
	linphone_core_use_files(lc,TRUE);
	//linphone_core_set_remote_ringback_tone(lc,NULL);
	callParam=linphone_core_create_default_call_parameters(lc);
	//linphone_call_params_enable_early_media_sending(callParam,FALSE);
        if (dest){
                call=linphone_core_invite_with_params(lc,dest,callParam);
                if (call==NULL){
                        printf("Could not place call to %s\n",dest);
                        goto end;
                } else {
		  printf("Call to %s is in progress...\n",dest);
		}
                linphone_call_ref(call);
        }
        /* main loop for receiving notifications and doing background linphonecore work: */
	long sleep=0;
        while(running){
		if (ringing) {
		  sleep += 1;
		  if ((sleep % 200) == 0) {
		    ringCount++;
		    if (ringCount > maxRingCount) {
		      running=FALSE;
		    }
		  }
		}
		linphone_core_iterate(lc);
                ms_usleep(10000);
		
        }
        if (call && linphone_call_get_state(call)!=LinphoneCallEnd){
                printf("Terminating the call...\n");
                linphone_core_terminate_call(lc,call);
                linphone_call_unref(call);
        }

end:
        printf("Shutting down...\n");
        linphone_core_destroy(lc);
        printf("Exited\n");
	if (gotSomeone) {
	    return 0;
	}
        return 1;
}
