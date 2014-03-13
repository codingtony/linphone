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
#include "lpconfig.h"
#include <ortp/event.h>
#include <ortp/b64.h>
#include <math.h>

#include "mediastreamer2/mediastream.h"
#include "mediastreamer2/msvolume.h"
#include "mediastreamer2/msequalizer.h"
#include "mediastreamer2/msfileplayer.h"
#include "mediastreamer2/msjpegwriter.h"
#include "mediastreamer2/mseventqueue.h"
#include "mediastreamer2/mssndcard.h"





#include <signal.h>

static bool_t running=TRUE;

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
                        printf("It is now ringing remotely !\n");
                break;
                case LinphoneCallOutgoingEarlyMedia:
                        printf("Receiving some early media\n");
                break;
                case LinphoneCallConnected:
                        printf("We are connected !\n");
                break;
                case LinphoneCallStreamsRunning:
                        printf("Media streams established !\n");
			linphone_core_set_play_file_with_cb(lc,wav,play_finished);
		//	ms_filter_set_notify_callback(call->audiostream->soundread,play_finished,NULL);
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
        const char *dest=NULL;
        const char *conf=NULL;

        /* take the destination sip uri from the command line arguments */
        if (argc<3){
		return  2;
        }
	conf=argv[1];
	dest=argv[2];
	wav=argv[3];

        signal(SIGINT,stop);

        vtable.call_state_changed=call_state_changed;
//	vtable.dtmf_received=linphonec_dtmf_received;


        lc=linphone_core_new(&vtable,conf,NULL,NULL);
	linphone_core_use_files(lc,TRUE);
        if (dest){
                call=linphone_core_invite(lc,dest);
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
