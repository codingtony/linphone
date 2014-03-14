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

static bool_t running=TRUE;
static int ringCount=0;
static int maxRingCount=4;

static void stop(int signum){
        running=FALSE;
}
const char *wav=NULL;

static void play_finished() {
	printf("Play finished !\n");
	running=FALSE;
}

/*
 *  * Call state notification callback
 *   */
static void call_state_changed(LinphoneCore *lc, LinphoneCall *call, LinphoneCallState cstate, const char *msg){
        switch(cstate){
                case LinphoneCallOutgoingRinging:
//			ringCount++;
                        printf("It is now ringing remotely !\n");
			if (ringCount >= maxRingCount) {
                        	printf("It is now ringing remotely !\n");
                		//linphone_core_terminate_call(lc,call);
			}
                break;
                case LinphoneCallOutgoingEarlyMedia:
                        printf("Receiving some early media \n");
                break;
                case LinphoneCallConnected:
                        printf("We are connected !\n");
                break;
                case LinphoneCallStreamsRunning:
                        printf("Media streams established !\n");
			linphone_core_set_play_file_with_cb(lc,wav,play_finished);
                break;
                case LinphoneCallEnd:
                        printf("Call is terminated.\n");
			running=FALSE;
                break;
                case LinphoneCallError:
                        printf("Call failure !");
                break;
                default:
                        printf("Unhandled notification %i\n",cstate);
        }
}

int main(int argc, char *argv[]){
        LinphoneCoreVTable vtable={0};
        LinphoneCore *lc;
        LinphoneCall *call=NULL;
	LinphoneCallParams *callParam=NULL;
        const char *dest=NULL;
        const char *conf=NULL;
	int c;
 	static struct option long_options[] = {
		{"conf",required_argument,0,'c'},
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
		printf("Missing arguments!\n");
		return 2;
	}
	printf("Calling %s\n",dest);
	printf("Using %s for configuration\n",conf);
	printf("Will stop after %d ring\n",maxRingCount);
	printf("Will play %s on answer\n", wav);
	
        signal(SIGINT,stop);
        vtable.call_state_changed=call_state_changed;

        lc=linphone_core_new(&vtable,conf,NULL,NULL);
	linphone_core_use_files(lc,TRUE);
	linphone_core_set_remote_ringback_tone(lc,NULL);
	callParam=linphone_core_create_default_call_parameters(lc);
	linphone_call_params_enable_early_media_sending(callParam,FALSE);
        if (dest){
                call=linphone_core_invite_with_params(lc,dest,callParam);
                if (call==NULL){
                        printf("Could not place call to %s\n",dest);
                        goto end;
                }else printf("Call to %s is in progress...",dest);
                linphone_call_ref(call);
        }
        /* main loop for receiving notifications and doing background linphonecore work: */
        while(running){
                linphone_core_iterate(lc);
                ms_usleep(50000);
        }
        if (call && linphone_call_get_state(call)!=LinphoneCallEnd){
                /* terminate the call */
                printf("Terminating the call...\n");
                linphone_core_terminate_call(lc,call);
                /*at this stage we don't need the call object */
                linphone_call_unref(call);
        }

end:
        printf("Shutting down...\n");
        linphone_core_destroy(lc);
        printf("Exited\n");
        return 0;
}
