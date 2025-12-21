#include <iostream>
#include "patient.h"
#include "reception.h"
#include "manager.h"
using namespace std;

int main() {
	system("CHCP 65001");
	int op = 0;
	while (true) {
		//系统入口
		cout << "********************************************" << endl;
		cout << "*******  欢迎使用社区医院管理系统！ ********" << endl;
		cout << "************** 0.退出系统 ******************" << endl;
		cout << "************** 1.问诊登录 ******************" << endl;
		cout << "************** 2.前台登录 ******************" << endl;
		cout << "************** 3.管理登录 ******************" << endl;
		cout << "********************************************" << endl;
		cout << "请选择登录方式： " << endl;
		cin >> op;
		switch (op) {
		case 0:
			cout << "再见！" << endl;
			system("pause");
			exit(0);
			break;
		case 1: {
			Patient P;
			P.Patient::Home();
			break;
		}
		case 2: {
			Reception R;
			R.Reception::Home();
			break;
		}
		case 3: {
			Manager M;
			M.Manager::Home();
			break;
		}
		default:
			system("cls");
			break;
		}
	}

	/*数据库操作模板
	try {
		//创建MySQL驱动对象
		sql::mysql::MySQL_Driver* driver;
		sql::Connection* con;
		sql::Statement* stmt;
		sql::ResultSet* res;
		
		//初始化驱动
		driver = sql::mysql::get_mysql_driver_instance();
		//连接到数据库系统
		con = driver->connect("tcp://localhost:3306", "root", "123456");
		//选择数据库
		con->setSchema("community_hospital");
		//创建查询语句
		stmt = con->createStatement();
		//执行查询语句
		res = stmt->executeQuery("SELECT * FROM doctors");

		//遍历输出结果
		while (res->next()) {
			cout << "doctor_id: " << res->getInt("doctor_id") << "\n";
			cout << "title: " << res->getString("title") << "\n";
		}
	}
	catch (sql::SQLException& e){
		cerr << "MySQL error: " << e.what() << endl;
	}*/
	return 0;
}