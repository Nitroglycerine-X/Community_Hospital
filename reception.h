#pragma once
#include <string>
#include <iostream>
#include "my_mysql.h"
using namespace std;
class Reception {
public:
	string emp_id, password;
	//分诊台功能选择
	void Home();
	//患者无预约到院直接就诊
	bool No_Appointment_Visit();
	//预约管理，“未核验”的预约修改为“已预约”
	bool Appointment_Manage();
	//状态转为“已离院”，计算bill
	bool Settle_And_Bill();
	//收费，该处直接在函数内通知（调用）患者的结算;记录医院收入（分日期的总收入，以及就诊人次）
	bool Charge_And_Mark();
	//查看患者简略信息（visit表中姓名、就诊科室、诊室号、就诊状态）
	bool Get_Rough_Infor();
};