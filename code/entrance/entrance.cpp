#include "entrance.h"

std::vector<ModelSliceReader> vr;

std::string read_vr(uint64_t slice_partition, uint64_t start, uint64_t len) {
  char buf[1000] = {0};
  bool jud = vr[slice_partition].Read(start,len,buf);
  if(!jud) std::cout<<"Bad!!!"<<std::endl;
  std::string s(buf,len);
  return s;
}

int main(int argc, char** argv) {
    for(int i=0;i<1000;i++) {
      ModelSliceReader node;
      vr.push_back(node);
    }
	std::string nodeId(argv[1]);
	int idNum = std::stoi(nodeId);

	hdfsLoader hdfs_loader(idNum);
	std::thread t1(&hdfsLoader::polling, &hdfs_loader);
	t1.detach();
	while (!hdfs_loader.getModel()) { sleep(1); } /* Warm Up until one model is Done */

	std::thread t2(RunGrpc, idNum);

	std::thread t3(runTCPServer);

	std::thread t4(etcdPart, nodeId, "", "50000");

	t2.detach();
	t3.detach();
	t4.detach();
    while(true){
      sleep(1000000);
    }

	return 0;
}