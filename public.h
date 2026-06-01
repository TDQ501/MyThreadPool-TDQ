#pragma once
#include <iostream>

//__FILE__Ë«ÏÂ»®Ïß
#define LOG(str) \
cout<<__FILE__<<":"<<__LINE__<<" "<< \
__TIMESTAMP__<<" : "<< str <<endl;
