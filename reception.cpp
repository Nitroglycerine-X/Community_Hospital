#include "reception.h"
#include <iostream>
using namespace std;
//分诊台功能选择
void Reception::Home() {
	//登录检查，输入emp_id，检索数据库，检查position；
	//输入password，检查前台统一密码:reception123
	string password, check_password = "reception123";
	string position, check_position = "分诊台";
	cout << "请输入你的工号： " << endl;
	cin >> emp_id;
	//检查position
	try {
		sql::mysql::MySQL_Driver* driver = sql::mysql::get_driver_instance();
		sql::Connection* con = driver->connect("tcp://localhost:3306", "root", "123456");
		con->setSchema("community_hospital");
		sql::Statement* stmt = con->createStatement();
		ostringstream sql_ss;
		sql_ss << "SELECT position FROM employee WHERE emp_id = '"
			<< emp_id << "'";
		string sql = sql_ss.str();
		sql::ResultSet* res = stmt->executeQuery(sql.c_str());
		if (res->next() == false) {
			cout << "不存在该员工！请重新选择登录方式。" << endl;
			return;
		}
		position = res->getString("position");
		if (position != check_position) {
			cout << "您没有分诊台登录权限！请选择其他登录方式。" << endl;
			return;
		}
		cout << "请输入分诊台统一登录密码：" << endl;
		cin >> password;
		if (password != check_password) {
			cout << "分诊台密码错误！请选择其他登录方式。" << endl;
			return;
		}
	}
	catch (sql::SQLException& e) {
		cout << "MySQL error: " << e.what() << endl;
	}
	//分诊台登录成功
	system("cls");
	while (true) {
		cout << "********************************************" << endl;
		cout << "*******  欢迎使用社区医院分诊系统！ ********" << endl;
		cout << "************** 0.退出系统 ******************" << endl;
		cout << "************** 1.患者就诊 ******************" << endl;
		cout << "************** 2.预约管理 ******************" << endl;
		cout << "************** 3.就诊状态管理与账单结算 ****" << endl;
		cout << "************** 4.收费收入记录与人次记录 ****" << endl;
		cout << "************** 5.查询患者简要信息 **********" << endl;
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
			No_Appointment_Visit();
			break;
		case 2:
			Appointment_Manage();
			break;
		case 3:
			Settle_And_Bill();
			break;
		case 4:
			Charge_And_Mark();
			break;
		case 5:
			Get_Rough_Infor();
			break;
		default:
			cout << "请选择有效操作！" << endl;
			break;
		}
	}
}
//患者无预约到院分诊台分配直接就诊
bool Reception::No_Appointment_Visit() {
	//获取并插入患者基本信息
	string id_card, name, gender, phone, visit_status,department;
	cout << "请输入患者身份证号: " << endl; cin >> id_card;
	cout << "请输入患者姓名: " << endl; cin >> name;
	cout << "请输入患者性别: " << endl; cin >> gender;
	cout << "请输入患者联系电话: " << endl; cin >> phone;
	cout << "请输入患者挂号科室: " << endl; cin >> department;
	visit_status = "进行中";
	try {
		sql::mysql::MySQL_Driver* driver;
		sql::Connection* con;
		sql::Statement* stmt;
		sql::ResultSet* res;

		driver = sql::mysql::get_driver_instance();
		con = driver->connect("tcp://localhost:3306", "root", "123456");
		con->setSchema("community_hospital");
		stmt = con->createStatement();
		//插入患者基本信息
		std::ostringstream sql_ss;
		sql_ss << "INSERT INTO patient VALUES('"
			<< id_card << "','"
			<< name << "','"
			<< gender << "','"
			<< phone << "','"
			<< visit_status << "')";
		std::string sql = sql_ss.str();
		stmt->executeUpdate(sql.c_str());

		//根据就诊科室和就诊时间(分诊台当前时间)获取诊室号和医生工号
		int room_num, doctor_id;
		std::ostringstream sql_ss1;
		sql_ss1 << "SELECT room_num,doctor_id FROM schedule WHERE department = '"
			<< department << "' AND NOW() >= start_time AND NOW() <= end_time";
		string sql_1 = sql_ss1.str();
		res = stmt->executeQuery(sql_1.c_str());
		if (res->next() == false) {
			cout << "预约时段预约科室无值班安排，请另约他时。" << endl;
			return false;
		}
		room_num = res->getInt("room_num");
		doctor_id = res->getInt("doctor_id");
		std::ostringstream sql_ss3;
		sql_ss3 << "INSERT INTO visit (id_card,room_num,doctor_id,visit_time,visit_status) VALUES('"
			<< id_card << "','"
			<< room_num << "','"
			<< doctor_id << "', NOW(),'"
			<< visit_status << "')";
		string sql_3 = sql_ss3.str();
		stmt->executeUpdate(sql_3.c_str());
		cout << "请前往" << room_num << "室，找" << doctor_id << "医生就医。" << endl;
	}
	catch (sql::SQLException& e) {
		cout << "MySQL error: " << e.what() << endl;
		return false;
	}
	return true;
}
//预约管理，以身份证号识别；“未核验”的预约修改为“已完成”，并转入就诊表
bool Reception::Appointment_Manage() {
	//结合数据库系统中的存储过程，实现对存在的“未核验”预约修改为“已完成”
	//存储过程定义见readme.md中的SQL语句,Appointment_Manage
	string id_card;
	cout << "请输入患者身份证号：" << endl; cin >> id_card;
	try {
		sql::mysql::MySQL_Driver* driver = sql::mysql::get_driver_instance();
		sql::Connection* con = driver->connect("tcp://localhost:3306", "root", "123456");
		con->setSchema("community_hospital");
		sql::Statement* stmt = con->createStatement();
		sql::ResultSet* res;
		
		//根据预约科室和就诊时间获取诊室号和医生工号
		int room_num, doctor_id;
		string department, expect_time;
		//1.获取department和expect_time;
		std::ostringstream sql_ss1;
		sql_ss1 << "SELECT department,expect_time FROM appointment WHERE id_card = '"
			<< id_card << "'";
		string sql_1 = sql_ss1.str();
		res = stmt->executeQuery(sql_1.c_str());
		if (res->next() == false) {
			cout << "请先预约再登记就诊！" << endl;
			return false;
		}
		department = res->getString("department");
		expect_time = res->getString("expect_time");
		//2.利用获取的department和expect_time查询诊室号和医生工号
		std::ostringstream sql_ss2;
		sql_ss2 << "SELECT room_num,doctor_id FROM schedule WHERE department = '"
			<< department << "' AND start_time <= '"
			<< expect_time << "' AND end_time >= '"
			<< expect_time << "'";
		string sql_2 = sql_ss2.str();
		res = stmt->executeQuery(sql_2.c_str());
		if (res->next() == false) {
			cout << "预约时段预约科室无值班安排，请另约他时。" << endl;
			return false;
		}
		room_num = res->getInt("room_num");
		doctor_id = res->getInt("doctor_id");
		string visit_time, visit_status = "进行中";
		std::ostringstream sql_ss3;
		sql_ss3 << "INSERT INTO visit (id_card,room_num,doctor_id,visit_time,visit_status) VALUES('"
			<< id_card << "','"
			<< room_num << "','"
			<< doctor_id << "', NOW(),'"
			<< visit_status << "')";
		string sql_3 = sql_ss3.str();
		stmt->executeUpdate(sql_3.c_str());
		//预约状态appt_status修改为“已完成”
		std::ostringstream sql_ss4;
		sql_ss4 << "UPDATE appointment SET appt_status = '已完成' WHERE id_card = '" << id_card << "'";
		string sql_4 = sql_ss4.str();
		stmt->executeUpdate(sql_4.c_str());
		cout << "请前往" << room_num << "室，找" << doctor_id << "医生就医。" << endl;
	}
	catch (sql::SQLException& e) {
		cout << "MySQL error: " << e.what() << endl;
	}
	return true;
}
//状态转为“已离院”，计算bill
bool Reception::Settle_And_Bill() {
	//根据身份证号查找，修改患者表和就诊表的就诊状态为“已离院”
	string id_card;
	cout << "请输入患者身份证号：" << endl; cin >> id_card;
	try {
		sql::mysql::MySQL_Driver* driver = sql::mysql::get_driver_instance();
		sql::Connection* con = driver->connect("tcp://localhost:3306","root","123456");
		con->setSchema("community_hospital");
		sql::Statement* stmt = con->createStatement();
		sql::ResultSet* res;
		
		//状态转为"已离院",并通过查询获得visit_id用于后续插入bill
		ostringstream sql_ss1;
		sql_ss1 << "UPDATE patient SET visit_status = '已离院' WHERE id_card = '" << id_card << "'";
		string sql = sql_ss1.str();
		stmt->executeUpdate(sql.c_str());
		ostringstream sql_ss2;
		sql_ss2 << "UPDATE visit SET visit_status = '已离院' WHERE id_card = '" << id_card << "'";
		sql = sql_ss2.str();
		stmt->executeUpdate(sql.c_str());

		int visit_id;
		ostringstream sql_ss3;
		sql_ss3 << "SELECT visit_id FROM visit WHERE id_card = '" << id_card << "'";
		sql = sql_ss3.str();
		res = stmt->executeQuery(sql.c_str());
		if (res->next() == false) {
			cout << "不存在就诊记录，无需结算离院！" << endl;
			return false;
		}
		visit_id = res->getInt("visit_id");
		//计算并插入bill
		double total_fee, medical_insur, self_pay, rate;
		cout << "请输入医疗总消费：" << endl; cin >> total_fee;
		cout << "请输入保险报销比例(例如：0.9)：" << endl; cin >> rate;
		medical_insur = total_fee * rate;
		self_pay = total_fee * (1 - rate);

		ostringstream sql_insert;
		sql_insert << "INSERT INTO bill (visit_id,total_fee,medical_insur,self_pay) VALUES ('"
			<< visit_id << "','"
			<< total_fee << "','"
			<< medical_insur << "','"
			<< self_pay << "')";
		sql = sql_insert.str();
		stmt->executeUpdate(sql.c_str());
		cout << "离院结算成功！请提醒患者及时缴费。" << endl;
	}
	catch (sql::SQLException& e) {
		cout << "MySQL error: " << e.what() << endl;
	}
	//插入bill
	return true;
}
//收费，记录支付时间和支付方式
bool Reception::Charge_And_Mark() {
	//根据就诊码查询bill需要自费的部分
	int visit_id;
	double self_pay;
	cout << "请输入就诊码查询账单：" << endl; cin >> visit_id;
	try {
		sql::mysql::MySQL_Driver* driver = sql::mysql::get_driver_instance();
		sql::Connection* con = driver->connect("tcp://localhost:3306", "root", "123456");
		con->setSchema("community_hospital");
		sql::Statement* stmt = con->createStatement();
		sql::ResultSet* res;

		ostringstream sql_ss;
		sql_ss << "SELECT self_pay  FROM bill WHERE visit_id = '"
			<< visit_id << "'";
		string sql = sql_ss.str();
		res = stmt->executeQuery(sql.c_str());
		if (res->next() == false) {
			cout << "暂无该就诊码的收费信息！" << endl;
			return false;
		}
		self_pay = res->getDouble("self_pay");
		//收费
		string pay_method;
		cout << "该顾客需要支付： " << self_pay << "元" << endl;
		cout << "请输入支付方式：" << endl; cin >> pay_method;
		ostringstream sql_update_time;
		sql_update_time << "UPDATE bill SET pay_time = NOW(), pay_method = '"
			<< pay_method << "', actual_pay = '"
			<< self_pay << "' WHERE visit_id = '"
			<< visit_id << "'";
		sql = sql_update_time.str();
		stmt->executeUpdate(sql.c_str());
		cout << "支付成功！" << endl;
	}
	catch (sql::SQLException& e) {
		cout << "MySQL error: " << e.what() << endl;
	}
	return true;
}
//查看患者简略信息（姓名(name)、就诊科室(department)、诊室号(room_num)、就诊状态(visit_status)）
bool Reception::Get_Rough_Infor() {
	string id_card,name,department,visit_status;
	int room_num,doctor_id;
	cout << "请输入需要查询的患者身份证号：" << endl; cin >> id_card;
	try {
		sql::mysql::MySQL_Driver* driver = sql::mysql::get_driver_instance();
		sql::Connection* con = driver->connect("tcp://localhost:3306", "root", "123456");
		con->setSchema("community_hospital");
		sql::Statement* stmt = con->createStatement();
		sql::ResultSet* res;
		//patient表中查询姓名
		string sql;
		ostringstream sql_patient;
		sql_patient << "SELECT name FROM patient WHERE id_card = '"
			<< id_card << "'";
		sql = sql_patient.str();
		res = stmt->executeQuery(sql.c_str());
		if (res->next() == false) {
			cout << "不存在该身份证号的相关信息！" << endl;
			return false;
		}
		name = res->getString("name");
		cout << "姓名：" << name << endl;
		//在visit表中查询诊室号和就诊状态以及doctor_id，再利用诊室号和doctor_id在schedule表中查询科室
		ostringstream sql_visit;
		sql_visit << "SELECT room_num,visit_status,doctor_id FROM visit WHERE id_card = '"
			<< id_card << "'";
		sql = sql_visit.str();
		res = stmt->executeQuery(sql.c_str());
		if (res->next() == false) {
			cout << "该身份证号不存在就诊信息！" << endl;
			return false;
		}
		room_num = res->getInt("room_num");
		visit_status = res->getString("visit_status");
		doctor_id = res->getInt("doctor_id");
		ostringstream sql_schedule;
		sql_schedule << "SELECT department FROM schedule WHERE room_num = '"
			<< room_num << "' AND doctor_id = '"
			<< doctor_id << "'";
		sql = sql_schedule.str();
		res = stmt->executeQuery(sql.c_str());
		if (res->next() == false) {
			cout << "该身份证号科室信息错误！" << endl;
			return false;
		}
		department = res->getString("department");
		cout << "就诊科室：" << department << endl;
		cout << "诊室号：" << room_num << endl;
		cout << "就诊状态：" << visit_status << endl;
	}
	catch (sql::SQLException& e) {
		cout << "MySQL error: " << e.what() << endl;
	}
	return true;
}