/*
 * This library provides two modules.
 * 
 * ping - to send a 'ping' message
 * pong - to respond to a 'ping' message
 *
 */

#include "pingpong.h"


/* This call is executed when the DLL is loaded into memory.
 * So this is probably the best place to acquire global locks on
 * specific resoruce (audio, limbs, etc.)
 */
void Init(const	std::vector<word32>	&numbers,const	std::vector<std::string>	&strings) {
	std::cout << "pingpong library loaded"<<std::endl;
}
