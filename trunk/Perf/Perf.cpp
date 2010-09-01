/*
 * This library provides two modules.
 * 
 * ping - to send a 'ping' message
 * pong - to respond to a 'ping' message
 *
 */

#include "Perf.h"


/* This call is executed when the DLL is loaded into memory.
 * So this is probably the best place to acquire global locks on
 * specific resoruce (audio, limbs, etc.)
 */
void Init(const	std::vector<word32>	&numbers,const	std::vector<std::string>	&strings) {
	int32	a = 0;
	if (numbers.size() > 0) {
		a=numbers[0];
		float32	f=*reinterpret_cast<float32	*>(&a);
		if ( (numbers.size() > 1) && (strings.size() > 0) )
			std::cout << "Perf library loaded with parameters:"<<f<<" "<<numbers[1]<<" "<<strings[0]<<std::endl;
	}
}
