#pragma once
#include <bits/stdc++.h>
#include <unistd.h>
#include "ModelSliceReader.h"
#include "../entrance/entrance.h"

class hdfsLoader{
 public:
  hdfsLoader(int id);

  void polling();

  bool getModel();

 private:
  int ID;
  int num = 6;
  int count0 = 0;

  std::string curModel = "";

  std::string check_roll();

  std::string check(std::string command);

  bool hdfsExist(std::string filename);

  void update(std::string model);

  std::string add0(int a,int b);
};