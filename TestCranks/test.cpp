#include	"modules.h"


mBrane::sdk::module::_Module *NewCR1(uint16	ID,uint16	clusterCID,uint16	clusterID){
	return	new	CR1(ID,clusterCID,clusterID);
}