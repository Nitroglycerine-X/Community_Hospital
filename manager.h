#pragma once
#include <string>
#include <iostream>
#include "my_mysql.h"
using namespace std;
class Manager {
public:
	string emp_id, password;
	int schedule_right;
	//行政管理功能选择
	void Home();
	//实现诊室及医生排班信息的增添与更改
	bool Make_Schedule();
	//账单查询（ 按日期/科室/医生统计门诊收入与就诊人次） 
	bool Bill_Query();
	//查询患者详细信息（根据姓名、 电话、 身份证号、 诊室号或就诊时间）
	bool Get_Patient_Info();
	//查询员工（医生、护士、行政人员）信息（工号、岗位、所属科室、工作状态）
	bool Get_Employee_Info();
	//修改员工信息（联系方式、排班权限、职称等）
	bool Change_Employee_Info();
};