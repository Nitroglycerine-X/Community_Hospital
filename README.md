中山大学网络空间安全学院数据库系统作业项目Community_Hospital
使用MySQL官方库connector C++8.3.0实现C++与数据库连接开发
1.构建合适的数据库，需求分析，概念设计，关系模型，编写SQL语句完成数据库定义（该部分详见实验报告与PPT）。
2.C++控制台项目实现系统主题，代码中由于使用的是connector C++库，调用语句能够很好的体现数据库语句的使用，复杂查询例如多表连接查询，子查询等也有体现。
3.代码中不同登录选项中，连接数据库时的用户做了区分，配合SQL权限管理，实现部分权限管理。

附.数据库构建的SQL语句如下：
CREATE DATABASE IF NOT EXISTS community_hospital CHARACTER SET utf8mb4;
USE community_hospital;
CREATE TABLE IF NOT EXISTS patient (
    id_card VARCHAR(18) PRIMARY KEY COMMENT '身份证号（主键）',
    name VARCHAR(50) NOT NULL COMMENT '姓名',
    gender VARCHAR(10) COMMENT '性别',
    phone VARCHAR(20) COMMENT '联系电话',
    visit_status VARCHAR(20) COMMENT '就诊状态（未就诊/就诊中/已离院）'
) COMMENT '患者信息表';
CREATE TABLE employee (
    emp_id INT PRIMARY KEY COMMENT '工号（主键）',
    name VARCHAR(50) NOT NULL COMMENT '姓名',
    position VARCHAR(50) COMMENT '岗位（医生/护士/行政）',
    department VARCHAR(50) COMMENT '所属科室',
    title VARCHAR(50) COMMENT '职称（主治医师/护士等）',
    phone VARCHAR(20) COMMENT '联系方式',
    work_status VARCHAR(20) COMMENT '工作状态（在职/离职）',
    schedule_right TINYINT(1) COMMENT '排班权限（1=有，0=无）'
) COMMENT '员工信息表';
CREATE TABLE appointment (
    appt_id INT PRIMARY KEY AUTO_INCREMENT COMMENT '预约ID（主键）',
    id_card VARCHAR(18) COMMENT '患者身份证号（外键）',
    department VARCHAR(50) COMMENT '就诊科室',
    expect_time DATETIME COMMENT '预计到达时间',
    appt_status VARCHAR(20) COMMENT '预约状态（未核验/已完成/已取消）',
    -- 外键关联患者表
    FOREIGN KEY (id_card) REFERENCES patient(id_card)
) COMMENT '预约信息表';
CREATE TABLE schedule (
    schedule_id INT PRIMARY KEY AUTO_INCREMENT COMMENT '排班ID（主键）',
    room_num INT COMMENT '诊室号',
    department VARCHAR(50) COMMENT '所属科室',
    doctor_id INT COMMENT '医生工号（外键）',
    start_time DATETIME COMMENT '可接诊起始时间',
    end_time DATETIME COMMENT '可接诊截止时间',
    schedule_status VARCHAR(20) COMMENT '排班状态（有效/无效）',
    -- 外键关联员工表（医生）
    FOREIGN KEY (doctor_id) REFERENCES employee(emp_id)
) COMMENT '排班信息表';
CREATE TABLE visit (
    visit_id INT PRIMARY KEY AUTO_INCREMENT COMMENT '就诊ID（主键）',
    id_card VARCHAR(18) COMMENT '患者身份证号（外键）',
    room_num INT COMMENT '诊室号',
    doctor_id INT COMMENT '医生工号（外键）',
    visit_time DATETIME COMMENT '就诊时间',
    visit_status VARCHAR(20) COMMENT '就诊状态（进行中/已离院）',
    -- 外键关联患者表、员工表
    FOREIGN KEY (id_card) REFERENCES patient(id_card),
    FOREIGN KEY (doctor_id) REFERENCES employee(emp_id)
) COMMENT '就诊信息表';
CREATE TABLE bill (
    bill_id INT PRIMARY KEY AUTO_INCREMENT COMMENT '账单ID（主键）',
    visit_id INT COMMENT '就诊ID（外键）',
    total_fee DECIMAL(10,2) COMMENT '费用总额',
    medical_insur DECIMAL(10,2) COMMENT '医保金额',
    self_pay DECIMAL(10,2) COMMENT '自费金额',
    pay_time DATETIME COMMENT '收费时间',
    pay_method VARCHAR(20) COMMENT '支付方式（医保/自费/混合）',
    actual_pay DECIMAL(10,2) COMMENT '实际收取金额',
    -- 外键关联就诊表
    FOREIGN KEY (visit_id) REFERENCES visit(visit_id)
) COMMENT '账单信息表';
