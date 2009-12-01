#include	"CoreTest.h"

int	main(int	argc,char	**argv){

	uint32 test = (uint32)BSR(6176);
	printf("BSR(6176) = %u\n\n", test);

	Time::Init(1000);


	TimeProbe	probe;
	Timer	_timer;
	while(1){
		probe.set();
		_timer.start(1000,0);
		_timer.wait();
		probe.check();
		std::cout<<"probe1: "<<probe.us()<<std::endl;
		Thread::Sleep(1000);
	}

	return	0;
}
