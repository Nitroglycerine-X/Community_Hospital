#include "manager.h"
#include <iostream>

//行政管理功能选择
void Manager::Home() {
	//登录检查，输入emp_id，检索数据库，检查position；
	//输入password，检查前台统一密码:manage123
	string password, check_password = "manage123";
	string position, check_position = "行政管理";
	cout << "请输入你的工号： " << endl;
	cin >> emp_id;
	//检查position
	try {
		sql::mysql::MySQL_Driver* driver = sql::mysql::get_driver_instance();
		sql::Connection* con = driver->connect("tcp://localhost:3306", "root", "123456");
		con->setSchema("community_hospital");
		sql::Statement* stmt = con->createStatement();
		ostringstream sql_ss;
		sql_ss << "SELECT position,schedule_right FROM employee WHERE emp_id = '"
			<< emp_id << "'";
		string sql = sql_ss.str();
		sql::ResultSet* res = stmt->executeQuery(sql.c_str());
		if (res->next() == false) {
			cout << "不存在该员工！请重新选择登录方式。" << endl;
			return;
		}
		position = res->getString("position");
		schedule_right = res->getInt("schedule_right");
		if (position != check_position) {
			cout << "您没有行政管理登录权限！请选择其他登录方式。" << endl;
			return;
		}
		cout << "请输入行政管理统一登录密码：" << endl;
		cin >> password;
		if (password != check_password) {
			cout << "行政管理密码错误！请选择其他登录方式。" << endl;
			return;
		}
	}
	catch (sql::SQLException& e) {
		cout << "MySQL error: " << e.what() << endl;
	}
	//行政管理登录成功
	system("cls");
	while (true) {
		cout << "********************************************" << endl;
		cout << "*******  欢迎使用社区医院行政系统！ ********" << endl;
		cout << "************** 0.退出系统 ******************" << endl;
		cout << "************** 1.排班管理 ******************" << endl;
		cout << "************** 2.账单查询 ******************" << endl;
		cout << "************** 3.查询患者信息 **************" << endl;
		cout << "************** 4.查询员工信息 **************" << endl;
		cout << "************** 5.修改员工信息 **************" << endl;
		cout << "********************************************" << endl;
		cout << "请选择操作： " << endl;
		int op = 0;
		cin >> op;
		switch (op) {
		case 0:
			system("cls");
			return;
			break;
		case 1:
			Make_Schedule();
			break;
		case 2:
			Bill_Query();
			break;
		case 3:
			Get_Patient_Info();
			break;
		case 4:
			Get_Employee_Info();
			break;
		case 5:
			Change_Employee_Info();
			break;
		default:
			cout << "请选择有效操作！" << endl;
			break;
		}
	}
}
//实现诊室及医生排班信息的增添与更改
bool Manager::Make_Schedule() {
	cout << "请选择操作方式：0.查询所有排班信息；1.增添排班信息；2.更改排班信息；3.删除排班信息" << endl;
	int op = 0;
	cin >> op;
	//建立连接
	try {
		sql::mysql::MySQL_Driver* driver = sql::mysql::get_driver_instance();
		sql::Connection* con = driver->connect("tcp://localhost:3306", "root", "123456");
		con->setSchema("community_hospital");
		sql::Statement* stmt = con->createStatement();
		sql::ResultSet* res;
		string sql;
		switch (op) {
		case 0: {
			res = stmt->executeQuery("SELECT * FROM schedule");
			while (res->next()) {
				cout << "排班号: " << res->getInt("schedule_id") << endl;
				cout << "诊室号: " << res->getInt("room_num") << endl;
				cout << "科室: " << res->getString("department") << endl;
				cout << "医生工号: " << res->getInt("doctor_id") << endl;
				cout << "开始时间: " << res->getString("start_time") << endl;
				cout << "结束时间: " << res->getString("end_time") << endl;
				cout << "排班状态: " << res->getString("schedule_status") << endl;
				cout << "------------------------------------------------" << endl;
			}
			break;
		}
		case 1: {
			if (schedule_right == 0) {
				cout << "您暂无权限修改排班信息！" << endl;
				return false;
			}
			int room_num, doctor_id;
			string department, start_time, end_time, schedule_status;
			cout << "请输入诊室号：" << endl; cin >> room_num;
			cout << "请输入科室：" << endl; cin >> department;
			cout << "请输入医生工号（emp_id）：" << endl; cin >> doctor_id;
			cout << "请输入值班起始时间（格式：XXXX-XX-XX_XX:XX:XX）：" << endl; cin >> start_time;
			cout << "请输入值班结束时间（格式：XXXX-XX-XX_XX:XX:XX）：" << endl; cin >> end_time;
			cout << "请输入排班状态（有效/无效）：" << endl; cin >> schedule_status;
			ostringstream case1;
			case1 << "INSERT INTO schedule (room_num,department,doctor_id,start_time,end_time,schedule_status) VALUES ("
				<< room_num << ",'"
				<< department << "',"
				<< doctor_id << ",'"
				<< start_time << "','"
				<< end_time << "','"
				<< schedule_status << "')";
			sql = case1.str();
			stmt->executeUpdate(sql.c_str());
			cout << "插入排班信息成功！" << endl;
			return true;
			break;
		}
		case 2: {
			if (schedule_right == 0) {
				cout << "您暂无权限修改排班信息！" << endl;
				return false;
			}
			int schedule_id;
			cout << "请输入要修改的排班编号：" << endl; cin >> schedule_id;
			int room_num, doctor_id;
			string department, start_time, end_time, schedule_status;
			cout << "请输入修改后诊室号：" << endl; cin >> room_num;
			cout << "请输入修改后科室：" << endl; cin >> department;
			cout << "请输入修改后医生工号（emp_id）：" << endl; cin >> doctor_id;
			cout << "请输入修改后值班起始时间（格式：XXXX-XX-XX_XX:XX:XX）：" << endl; cin >> start_time;
			cout << "请输入修改后值班结束时间（格式：XXXX-XX-XX_XX:XX:XX）：" << endl; cin >> end_time;
			cout << "请输入修改后排班状态（有效/无效）：" << endl; cin >> schedule_status;
			ostringstream case2;
			case2 << "UPDATE schedule SET room_num = "
				<< room_num << ", department = '"
				<< department << "', doctor_id = "
				<< doctor_id << ", start_time = '"
				<< start_time << "', end_time = '"
				<< end_time << "', schedule_status = '"
				<< schedule_status << "' WHERE schedule_id = "
				<< schedule_id;
			sql = case2.str();
			stmt->executeUpdate(sql.c_str());
			cout << "修改排班信息成功！" << endl;
			break;
		}
		case 3: {
			if (schedule_right == 0) {
				cout << "您暂无权限修改排班信息！" << endl;
				return false;
			}
			int schedule_id;
			cout << "请输入要删除的排班编号：" << endl; cin >> schedule_id;
			ostringstream case3;
			case3 << "DELETE FROM schedule WHERE schedule_id = "
				<< schedule_id;
			sql = case3.str();
			stmt->executeUpdate(sql.c_str());
			cout << "删除排班信息成功！" << endl;
			break;
		}
		default:
			return false;
			break;
		}
	}
	catch (sql::SQLException& e) {
		cout << "MySQL error: " << e.what() << endl;
 	}
	return true;
}
//账单查询（ 按日期/科室/医生统计门诊收入） 
bool Manager::Bill_Query() {
	cout << "请选择操作方式：1.时间段内收入；2.医生+时间段内收入；3.科室+时间段内收入" << endl;
	int op = 0;
	cin >> op;
	//建立连接
	try {
		sql::mysql::MySQL_Driver* driver = sql::mysql::get_driver_instance();
		sql::Connection* con = driver->connect("tcp://localhost:3306", "root", "123456");
		con->setSchema("community_hospital");
		sql::Statement* stmt = con->createStatement();
		sql::ResultSet* res;
		string sql;
		switch (op) {
		case 1: {//时间段内收入
			int cnt = 0;
			long double total_income = 0;
			string start_time, end_time;
			cout << "请输入查询起始时间（格式：XXXX-XX-XX_XX:XX:XX）：" << endl; cin >> start_time;
			cout << "请输入查询结束时间（格式：XXXX-XX-XX_XX:XX:XX）：" << endl; cin >> end_time;
			ostringstream case1;
			case1 << "SELECT * FROM bill WHERE pay_time >= '"
				<< start_time << "' AND pay_time <= '"
				<< end_time << "'";
			sql = case1.str();
			res = stmt->executeQuery(sql.c_str());
			while (res->next()) {
				cnt++;
				cout << "账单号: " << res->getInt("bill_id") << endl;
				cout << "总开销: " << res->getDouble("total_fee") << endl;
				cout << "医保承担: " << res->getDouble("medical_insur") << endl;
				cout << "自费部分: " << res->getDouble("self_pay") << endl;
				cout << "----------------------------------------" << endl;
				total_income += res->getDouble("self_pay");
			}
			cout << "该时间段内共有" << cnt << "条收入记录，" << "共收入" << total_income << "元" << endl;
			return true;
			break;
		}
		case 2: {//医生+起止时间确定范围统计收入
			int cnt = 0,doctor_id;
			long double total_income = 0;
			string start_time, end_time;
			cout << "请输入统计目标医生员工号：" << endl; cin >> doctor_id;
			cout << "请输入查询起始时间（格式：XXXX-XX-XX_XX:XX:XX）：" << endl; cin >> start_time;
			cout << "请输入查询结束时间（格式：XXXX-XX-XX_XX:XX:XX）：" << endl; cin >> end_time;
			ostringstream case2;
			case2 << "SELECT * FROM bill WHERE pay_time >= '"
				<< start_time << "' AND pay_time <= '"
				<< end_time << "' AND visit_id IN (SELECT visit_id FROM visit WHERE doctor_id = "
				<< doctor_id << ")";
			sql = case2.str();
			res = stmt->executeQuery(sql.c_str());
			while (res->next()) {
				cnt++;
				cout << "账单号: " << res->getInt("bill_id") << endl;
				cout << "总开销: " << res->getDouble("total_fee") << endl;
				cout << "医保承担: " << res->getDouble("medical_insur") << endl;
				cout << "自费部分: " << res->getDouble("self_pay") << endl;
				cout << "----------------------------------------" << endl;
				total_income += res->getDouble("self_pay");
			}
			cout << "该时间段内,医生：" << doctor_id << "接诊记录中共有" << cnt << "条收入记录，" << "共收入" << total_income << "元" << endl;
			break;
		}
		case 3: {//科室+起止时间确定范围统计收入
			int cnt = 0;
			long double total_income = 0;
			string department,start_time, end_time;
			cout << "请输入统计科室：" << endl; cin >> department;
			cout << "请输入查询起始时间（格式：XXXX-XX-XX_XX:XX:XX）：" << endl; cin >> start_time;
			cout << "请输入查询结束时间（格式：XXXX-XX-XX_XX:XX:XX）：" << endl; cin >> end_time;
			ostringstream case3;
			case3 << "SELECT * FROM bill WHERE pay_time >= '"
				<< start_time << "' AND pay_time <= '"
				<< end_time << "' AND visit_id IN (SELECT visit_id FROM visit V,schedule S WHERE V.room_num = S.room_num AND V.doctor_id = S.doctor_id AND S.department = '"
				<< department  << "')";
			sql = case3.str();
			res = stmt->executeQuery(sql.c_str());
			while (res->next()) {
				cnt++;
				cout << "账单号: " << res->getInt("bill_id") << endl;
				cout << "总开销: " << res->getDouble("total_fee") << endl;
				cout << "医保承担: " << res->getDouble("medical_insur") << endl;
				cout << "自费部分: " << res->getDouble("self_pay") << endl;
				cout << "----------------------------------------" << endl;
				total_income += res->getDouble("self_pay");
			}
			cout << "该时间段内,科室：" << department << "接诊记录中共有" << cnt << "条收入记录，" << "共收入" << total_income << "元" << endl;
			break;
		}
		default:
			return false;
			break;
		}
	}
	catch (sql::SQLException& e) {
		cout << "MySQL error: " << e.what() << endl;
	}
	return true;
}
//查询患者详细信息
bool Manager::Get_Patient_Info() {
	cout << "请输入查询方式：1.身份证号精准查询；2.时间段模糊查询" << endl;
	int op;
	cin >> op;
	try {
		sql::mysql::MySQL_Driver* driver = sql::mysql::get_driver_instance();
		sql::Connection* con = driver->connect("tcp://localhost:3306", "root", "123456");
		con->setSchema("community_hospital");
		sql::Statement* stmt = con->createStatement();
		sql::ResultSet* res;
		string sql;
		switch (op) {
		case 1: {
			string id_card;
			cout << "请输入身份证号：" << endl; cin >> id_card;
			ostringstream case1;
			case1 << "SELECT * FROM patient WHERE id_card = '"
				<< id_card  <<  "'";
			sql = case1.str();
			res = stmt->executeQuery(sql.c_str());
			if (res->next() == false) {
				cout << "不存在该患者！" << endl;
				return false;
			}
			cout << "身份证号: " << res->getString("id_card") << endl;
			cout << "姓名: " << res->getString("name") << endl;
			cout << "性别: " << res->getString("gender") << endl;
			cout << "联系方式: " << res->getString("phone") << endl;
			cout << "就诊状态: " << res->getString("visit_status") << endl;
			cout << "----------------------------------------------" << endl;
			return true;
			break;
		}
		case 2: {
			string start_time, end_time;
			cout << "请输入要查询的时间段起始时间：" << endl; cin >> start_time;
			cout << "请输入要查询的时间段终止时间：" << endl; cin >> end_time;
			ostringstream case2;
			case2 << "SELECT * FROM patient WHERE id_card IN (SELECT id_card FROM visit WHERE visit_time >= '"
				<< start_time << "' AND visit_time <= '"
				<< end_time << "')";
			sql = case2.str();
			res = stmt->executeQuery(sql.c_str());
			int flag = 0;
			while (res->next()) {
				flag++;
				cout << "身份证号: " << res->getString("id_card") << endl;
				cout << "姓名: " << res->getString("name") << endl;
				cout << "性别: " << res->getString("gender") << endl;
				cout << "联系方式: " << res->getString("phone") << endl;
				cout << "就诊状态: " << res->getString("visit_status") << endl;
				cout << "----------------------------------------------" << endl;
			}
			cout << "该时间段内共有" << flag  << "人就诊。"  << endl;
			return true;

			break;
		}
		default:
			return false;
			break;
		}
	}
	catch (sql::SQLException& e) {
		cout << "MySQL error: " << e.what() << endl;
	}
}
//查询员工（医生、护士、行政人员）信息（工号、岗位、所属科室、工作状态）
bool Manager::Get_Employee_Info() {
	try {
		sql::mysql::MySQL_Driver* driver = sql::mysql::get_driver_instance();
		sql::Connection* con = driver->connect("tcp://localhost:3306", "root", "123456");
		con->setSchema("community_hospital");
		sql::Statement* stmt = con->createStatement();
		sql::ResultSet* res;
		string sql;
		int emp_id;
		cout << "请输入工号：" << endl; cin >> emp_id;
		ostringstream case1;
		case1 << "SELECT * FROM employee WHERE emp_id = '"
			<< emp_id << "'";
		sql = case1.str();
		res = stmt->executeQuery(sql.c_str());
		if (res->next() == false) {
			cout << "不存在该员工！" << endl;
			return false;
		}
		cout << "员工号: " << res->getInt("emp_id") << endl;
		cout << "姓名: " << res->getString("name") << endl;
		cout << "岗位: " << res->getString("position") << endl;
		cout << "科室: " << res->getString("department") << endl;
		cout << "职称: " << res->getString("title") << endl;
		cout << "联系方式: " << res->getString("phone") << endl;
		cout << "工作状态: " << res->getString("work_status") << endl;
		cout << "排班权限: " << res->getInt("schedule_right") << endl;
		cout << "----------------------------------------------" << endl;
}
	catch (sql::SQLException& e) {
		cout << "MySQL error: " << e.what() << endl;
	}
	return true;
}
//修改员工信息（联系方式、排班权限、职称）
bool Manager::Change_Employee_Info() {
	int emp_id_change;
	cout << "请输入需要修改的员工的工号：" << endl; cin >> emp_id_change;
	try {
		sql::mysql::MySQL_Driver* driver = sql::mysql::get_driver_instance();
		sql::Connection* con = driver->connect("tcp://localhost:3306", "root", "123456");
		con->setSchema("community_hospital");
		sql::Statement* stmt = con->createStatement();
		sql::ResultSet* res;
		string sql;
		string phone, title;
		int schedule_right_change;
		cout << "请输入修改后联系方式：" << endl; cin >> phone;
		cout << "请输入修改后排班权限：" << endl; cin >> schedule_right_change;
		cout << "请输入修改后职称：" << endl; cin >> title;
		ostringstream sql_ss;
		sql_ss << "UPDATE employee SET phone = '"
			<< phone << "', schedule_right = "
			<< schedule_right_change << ", title = '"
			<< title << "' WHERE emp_id = "
			<< emp_id_change;
		sql = sql_ss.str();
		stmt->executeUpdate(sql.c_str());
		cout << "修改员工信息成功！" << endl;
	}
	catch (sql::SQLException& e) {
		cout << "MySQL error: " << e.what() << endl;
	}
	return true;
}