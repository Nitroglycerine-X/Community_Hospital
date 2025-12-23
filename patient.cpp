#include "patient.h"
#include <iostream>
//患者功能选择
void Patient::Home() {
	cout << "请输入您的身份证号: " << endl; cin >> id_card;
	cout << "请输入您的姓名: " << endl; cin >> name;
	cout << "请输入您的性别: " << endl; cin >> gender;
	cout << "请输入您的联系电话: " << endl; cin >> phone;
	visit_status = "未就诊";
	//基本信息插入数据库
	try {
		sql::mysql::MySQL_Driver* driver;
		sql::Connection* con;
		sql::Statement* stmt;
		
		driver = sql::mysql::get_driver_instance();
		//使用患者用户登录，实现权限管理
		con = driver->connect("tcp://localhost:3306", "patient", "123456");
		con->setSchema("community_hospital");
		stmt = con->createStatement();
		//SQL拼接代码
		std::ostringstream sql_ss;
		sql_ss << "INSERT INTO patient VALUES('"
			<< id_card << "','"
			<< name << "','"
			<< gender << "','"
			<< phone << "','"
			<< visit_status << "')";
		std::string sql = sql_ss.str();
		stmt->executeUpdate(sql.c_str()); // 注意：executeUpdate接收const char*，需转成c_str()
	}
	catch (sql::SQLException& e) {
		cout << "MySQL error: " << e.what() << endl;
	}
	system("cls");
	while (true) {
		cout << "********************************************" << endl;
		cout << "*******  欢迎使用社区医院问诊系统！ ********" << endl;
		cout << "************** 0.退出系统 ******************" << endl;
		cout << "************** 1.预约就诊 ******************" << endl;
		cout << "************** 2.查询预约 ******************" << endl;
		cout << "************** 3.取消预约 ******************" << endl;
		cout << "************** 4.登记就诊 ******************" << endl;
		cout << "************** 5.结算离开 ******************" << endl;
		cout << "********************************************" << endl;
		cout << "请选择操作： " << endl;
		int op = 0;
		cin >> op;
		switch (op) {
		case 0:
			system("cls");
			return;
			break;
		case 1:case 2: case 3:
			Appoint(op);
			break;
		case 4:
			Visit();
			break;
		case 5:
			Settle();
			break;
		default:
			cout << "请选择有效操作！" << endl;
			break;
		}
	}
}
//预约
bool Patient::Appoint(int x) {
	try {
		sql::mysql::MySQL_Driver* driver;
		sql::Connection* con;
		sql::Statement* stmt;

		driver = sql::mysql::get_driver_instance();
		//使用病人用户登录，实现权限管理
		con = driver->connect("tcp://localhost:3306", "root", "123456");
		con->setSchema("community_hospital");
		stmt = con->createStatement();
		if (x == 1) {//新增预约
			string department, expect_time, appt_status;
			cout << "请输入预约科室：" << endl; cin >> department;
			cout << "请输入预计到达时间（'XXXX-XX-XX_00:00:00格式'）：" << endl; cin >> expect_time;
			appt_status = "未核验";  //初始自行预约是未核验状态，前台登记就诊后可修改为“已完成”，取消后修改为“已取消”
			//SQL拼接代码
			std::ostringstream sql_ss;
			sql_ss << "INSERT INTO appointment (id_card,department,expect_time,appt_status) VALUES('"
				<< id_card << "','"
				<< department << "','"
				<< expect_time << "','"
				<< appt_status << "')";
			std::string sql = sql_ss.str();
			stmt->executeUpdate(sql.c_str()); // 注意：executeUpdate接收const char*，需转成c_str()
			cout << "预约成功！请在预约时间内就医。" << endl;
		}
		else if (x == 2) {//查询预约
			sql::ResultSet* res;
			std::ostringstream sql_ss;
			sql_ss << "SELECT * FROM appointment WHERE id_card='"
				<< id_card << "'";
			std::string sql = sql_ss.str();
			res = stmt->executeQuery(sql.c_str());
			int cnt = 0;
			while (res->next()) {
				cnt++;
				cout << "预约号: " << res->getInt("appt_id") << endl;
				cout << "身份证号: " << res->getString("id_card") << endl;
				cout << "科室: " << res->getString("department") << endl;
				cout << "预约时间: " << res->getString("expect_time") << endl;
				cout << "预约状态: " << res->getString("appt_status") << endl;
				cout << "-------------------------------------------" << endl;
			}
			if (cnt == 0) {
				cout << "未查询到您的预约信息！" << endl;
			}
			else {
				cout << "共查询到您的 " << cnt << " 条预约信息。" << endl;
			}
		}
		else {
			std::ostringstream sql_ss;
			sql_ss << "UPDATE appointment SET appt_status = '已取消' WHERE id_card ='"
				<< id_card << "'";
			std::string sql = sql_ss.str();
			stmt->executeUpdate(sql.c_str());
			cout << "取消成功！" << endl;
		}
	}
	catch (sql::SQLException& e) {
		cout << "MySQL error: " << e.what() << endl;
		return false;
	}
	return true;
}
//线上自行预约患者就诊
bool Patient::Visit() {
	try {
		sql::mysql::MySQL_Driver* driver;
		sql::Connection* con;
		sql::Statement* stmt;
		sql::ResultSet* res;
		
		driver = sql::mysql::get_driver_instance();
		con = driver->connect("tcp://localhost:3306", "root", "123456");
		con->setSchema("community_hospital");
		stmt = con->createStatement();

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
		//患者信息visit_status修改为“进行中”
		std::ostringstream sql_ss5;
		sql_ss5 << "UPDATE patient SET visit_status = '"
			<< visit_status << "'WHERE id_card = "
			<< id_card;
		string sql_5 = sql_ss5.str();
		stmt->executeUpdate(sql_5.c_str());
		cout << "请前往" << room_num << "室，找" << doctor_id << "医生就医。" << endl;
	}
	catch (sql::SQLException& e) {
		cout << "MySQL error: " << e.what() << endl;
		return false;
	}
	return true;
}
//结算
bool Patient::Settle() {
	try {
		sql::mysql::MySQL_Driver* driver;
		sql::Connection* con;
		sql::Statement* stmt;
		sql::ResultSet* res;

		driver = sql::mysql::get_driver_instance();
		con = driver->connect("tcp://localhost:3306", "root", "123456");
		con->setSchema("community_hospital");
		stmt = con->createStatement();

		std::ostringstream sql_ss;
		sql_ss << "SELECT SUM(total_fee) AS total_fee, SUM(medical_insur) AS medical_insur, SUM(self_pay) AS total_price "
			<< "FROM bill JOIN visit ON bill.visit_id = visit.visit_id "
			<< "WHERE visit.id_card = '" << id_card << "' "
			<< "GROUP BY visit.id_card;";;
		string sql = sql_ss.str();
		res = stmt->executeQuery(sql.c_str());
		if (res->next() == false) {
			cout << "没有您的消费记录或未结清账单！请先到前台登记出院方可生成账单。" << endl;
			return false;
		}
		double total_fee,medical_insur,total_price;
		total_fee = res->getDouble("total_fee");
		medical_insur = res->getDouble("medical_insur");
		total_price = res->getDouble("total_price");
		cout << "医疗总费用：" << total_fee << "元；其中医保承担：" << medical_insur << "元；您需支付：" << total_price << "元。" << endl;
	}
	catch (sql::SQLException& e) {
		cout << "MySQL error: " << e.what() << endl;
		return false;
	}
	return true;
}