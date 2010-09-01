#include	"CoreTest.h"

int	main(int	argc,char	**argv){

//	uint32 test = (uint32)BSR(6176);
//	printf("BSR(6176) = %u\n\n", test);

	Time::Init(1000);

	int n;

	printf("Testing single timer...\n");
	TimeProbe	probe;
	Timer	_timer;
	for (n=0; n<10; n++) {
		probe.set();
		_timer.start(1000,0);
		_timer.wait();
		probe.check();
		std::cout<<"probe1: "<<probe.us()<<std::endl;
	}

	printf("Testing periodic timer...\n");
	_timer.start(1000,1000);
	for (n=0; n<10; n++) {
		probe.set();
		_timer.wait();
		probe.check();
		std::cout<<"probe1: "<<probe.us()<<std::endl;
	}

	return	0;
}
