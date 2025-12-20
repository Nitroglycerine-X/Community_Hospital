#pragma once
#include <string>
#include <sstream>
using namespace std;
class Patient {
public:
	string id_card, name, gender, phone, visit_status;
	//患者功能选择
	void Home();
	//预约
	bool Appoint(int x);
	//就诊
	bool Visit();
	//结算
	bool Settle();
};