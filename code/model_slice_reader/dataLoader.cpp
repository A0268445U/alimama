//
// Created by Minghao Han on 2023/6/10.
//
#include "dataLoader.h"

hdfsLoader::hdfsLoader(int id){
  ID=id;
}

void hdfsLoader::polling(){
  popen("rm -r /home/model;mkdir /home/model", "r");
  while(1) {
    std::string model = check_roll();
    if(model=="") {
      model = check("hdfs dfs -fs hdfs://namenode:9000/ -ls -d /model_*");
    }
    if (model!= "" && model != curModel) {
      update(model);
      curModel = model;
    }
    sleep(10); // 10 sec
  }
}

bool hdfsLoader::getModel(){
  return curModel != "";
}

std::string hdfsLoader::check_roll(){
  if(hdfsExist("rollback.version")){
    std::string command = "hdfs dfs -fs hdfs://namenode:9000/ -cat rollback.version";
    char *cmd = (char*)command.data();
    char result[1024];
    FILE *fp = NULL;
    if((fp = popen(cmd, "r")) != NULL){
      fgets(result, 1000, fp);
    }
    return result;
  }else return "";
}

std::string hdfsLoader::check(std::string command){
  char *cmd = (char*)command.data();
  std::string res = "";
  std::string timeStamp = "";
  char result[1024];
  FILE *fp = NULL;
  if((fp = popen(cmd, "r")) != NULL){
    while (fgets(result, 1000, fp) != NULL){
      std::string tmp = result;
      std::string curModel = "";
      std::string curStamp = "";
      for(int i=tmp.size()-2;tmp[i]!='/';i--){
        curModel = tmp[i]+curModel;
        if(tmp[i]=='_'||tmp[i]>'9'||tmp[i]<'0') continue;
        curStamp = tmp[i]+curStamp;
      }
      if(curStamp>timeStamp && hdfsExist(curModel+"/model.done")){
        timeStamp = curStamp;
        res = curModel;
      }}
    pclose(fp);
    fp = NULL;
  }
  return res;
}

bool hdfsLoader::hdfsExist(std::string filename){
  std::string command = "hdfs dfs -fs hdfs://namenode:9000/ -test -e /"+filename+" ; echo $?";
  char *cmd = (char*)command.data();
  FILE *fp = NULL;
  char result[10];
  if((fp = popen(cmd, "r")) != NULL){
    fgets(result, 10, fp);
  }
  pclose(fp);
  fp = NULL;
  return result[0]=='0';
}

void hdfsLoader::update(std::string model){
  int n = 0;
  std::string command = "hdfs dfs -fs hdfs://namenode:9000/ -cat /"+model+"/model.meta";
  char *cmd = (char*)command.data();
  FILE *fp = NULL;
  if((fp = popen(cmd, "r")) != NULL){
    char tmp[1024];
    while (fgets(tmp, 1000, fp) != NULL) {
      std::string temp = tmp;
      if(temp.size()<=5||temp.substr(0,5)!="slice") continue;
      n = std::max(n, stoi(temp.substr(temp.find_first_of(".") + 1,
                                       temp.find_first_of(","))));
    }
  }
  count0 = n;
  std::cout<<"Updating Model: "<<model<<std::endl;
  for(int i=0;i<=n;i++){
//    if(i%num+1 != ID) continue;
    std::string suffix = add0(i,n-1);
    command =
        "hdfs dfs -fs hdfs://namenode:9000/ -get /"+model+"/model_slice."+ suffix+
            " /home/model/model_slice." + suffix;
    cmd = (char*)command.data();
    if((fp = popen(cmd, "r")) != NULL){
      std::cout<<"model_slice."+suffix<<std::endl;
    }
    pclose(fp);
    fp = NULL;
  }
  for(int i=0;i<=n;i++){
    std::string suffix = add0(i,n-1);
    bool jud = vr[i].Load("/home/model/model_slice." + suffix);
    if(!jud) std::cout<<"Bad!"<<std::endl;
  }
  std::cout<<"Done. \n\n";
}

std::string hdfsLoader::add0(int a,int b){
  std::string res = "";
  while(b!=0){
    res = (char)('0'+a%10)+res;
    a = a/10;
    b = b/10;
  }
  return res;
}