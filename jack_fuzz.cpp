/********************************************************************
*       (c) Copyright 2010, Hogeschool voor de Kunsten Utrecht
*                       Hilversum, the Netherlands
*********************************************************************
*
* File name     : jack_fuzz.cpp
* System name   : digital effects
* 
*
* Description   : fuzz with JACK interface
*
*
* Author        : Marc_G
* E-mail        : marcg@dinkum.nl
*
********************************************************************/
#include <iostream>
#include <math.h>

#define BUFFERSIZE 44100

using namespace std;

#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <jack/jack.h>
#include <jack/midiport.h>


jack_port_t *input_port;
jack_port_t *output_port;



jack_default_audio_sample_t threshold=0.3;
jack_default_audio_sample_t power=0;



int process(jack_nframes_t nframes, void *arg)
{
  jack_default_audio_sample_t *in =
    (jack_default_audio_sample_t *) jack_port_get_buffer(input_port, nframes);
	
  jack_default_audio_sample_t *out =
    (jack_default_audio_sample_t *) jack_port_get_buffer(output_port,nframes);

  power=0;
  // fuzz
  for(unsigned int x=0; x<nframes; x++)
  { 
    out[x] = in[x]<threshold ? in[x] : threshold;
    power += in[x]*in[x];
  }
  threshold = sqrt(power) * 0.1;

  
  return 0;   
} // process()



/*
 * shutdown callback may be called by JACK
 */
void jack_shutdown(void *arg)
{
  exit(1);
}



int updatesamplerate(jack_nframes_t nframes, void *arg)
{
  cout << "Sample rate set to: " << nframes << endl;
  return 0;
}


int main()
{
jack_client_t *client;
const char **ports;

  // Create a new Jack client
  if( (client=jack_client_open("fuzz",(jack_options_t)0,0)) == 0)
  {
    cout << "JACK server not running ?\n";
    return 1;
  }

  // Install the sample processing callback
  jack_set_process_callback(client,process,0);

  // Install a shutdown routine
  jack_on_shutdown(client,jack_shutdown,0); // install a shutdown callback

  // Install a routine 
  jack_set_sample_rate_callback(client,updatesamplerate,0);

  // Open an input port
  input_port = jack_port_register(client,"in",
     JACK_DEFAULT_AUDIO_TYPE,JackPortIsInput, 0);

  cout << "Our input port is called: " << jack_port_name(input_port) << endl;
	
  output_port = jack_port_register(client,"out",
     JACK_DEFAULT_AUDIO_TYPE,JackPortIsOutput,0);

  cout << "Our output port is called: " << jack_port_name(output_port) << endl;

  // Get rollin'
  if(jack_activate(client))
  {
    cout <<  "cannot activate client";
    return 1;
  }

  /*
   * The next calls try to auto-connect to a receiving client
   */

  // See what ports are available to receive our 
  if((ports =
  jack_get_ports(client,"system|meter",0,JackPortIsInput)) == 0)
  //if((ports =
  //jack_get_ports(client,0,0,JackPortIsPhysical|JackPortIsInput)) == 0)
  {
    cout << "Cannot find any physical playback ports\n";
    exit(1);
  }

  // List all ports matching the search criteria
  for(int p=0; ports[p] != 0; p++)
  {
    cout << "Ports found: " << ports[p] << endl;
  }

  // first output
  if(jack_connect(client,jack_port_name(output_port),ports[0]))
  {
    cout << "Cannot connect output ports\n";
  }

/*
  // second output
  if(jack_connect(client,jack_port_name(output_port),ports[1]))
  {
    cout << "Cannot connect output ports\n";
  }
*/
  free(ports); // ports structure no longer needed

  /*
   * Playback is already running now, let's change some sound parameters
   */

  while(1);

  jack_client_close(client);

  return 1;
}

