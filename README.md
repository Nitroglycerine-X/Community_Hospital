# Community_Hospital 社区医院管理系统
基于 MySQL Connector C++ 8.3.0 实现的社区医院数据库管理系统，涵盖数据库设计、C++ 控制台交互、权限管理等核心功能。

## 项目简介
本项目为中山大学网络空间安全学院数据库系统课程作业，主要实现以下目标：
1. 设计并构建符合社区医院业务需求的关系型数据库
2. 使用 MySQL Connector C++ 实现 C++ 与数据库的交互开发
3. 实现多角色权限管理与复杂数据库查询（多表连接、子查询等）
4. 提供控制台交互界面，支持医院日常业务的核心操作

## 技术栈
- **数据库**：MySQL 8.0+
- **开发语言**：C++
- **数据库连接库**：MySQL Connector C++ 8.3.0
- **开发环境**：Visual Studio/GCC/Clang（支持C++11及以上标准）

## 数据库设计
### 数据库概览
数据库名：`community_hospital`，采用 `utf8mb4` 字符集，包含6张核心业务表，覆盖患者、员工、预约、排班、就诊、账单全流程管理。

### 表结构定义
```sql
CREATE DATABASE IF NOT EXISTS community_hospital CHARACTER SET utf8mb4;
USE community_hospital;

-- 患者信息表
CREATE TABLE IF NOT EXISTS patient (
    id_card VARCHAR(18) PRIMARY KEY COMMENT '身份证号（主键）',
    name VARCHAR(50) NOT NULL COMMENT '姓名',
    gender VARCHAR(10) COMMENT '性别',
    phone VARCHAR(20) COMMENT '联系电话',
    visit_status VARCHAR(20) COMMENT '就诊状态（未就诊/就诊中/已离院）'
) COMMENT '患者信息表';

-- 员工信息表
CREATE TABLE IF NOT EXISTS employee (
    emp_id INT PRIMARY KEY COMMENT '工号（主键）',
    name VARCHAR(50) NOT NULL COMMENT '姓名',
    position VARCHAR(50) COMMENT '岗位（医生/护士/行政）',
    department VARCHAR(50) COMMENT '所属科室',
    title VARCHAR(50) COMMENT '职称（主治医师/护士等）',
    phone VARCHAR(20) COMMENT '联系方式',
    work_status VARCHAR(20) COMMENT '工作状态（在职/离职）',
    schedule_right TINYINT(1) COMMENT '排班权限（1=有，0=无）'
) COMMENT '员工信息表';

-- 预约信息表
CREATE TABLE IF NOT EXISTS appointment (
    appt_id INT PRIMARY KEY AUTO_INCREMENT COMMENT '预约ID（主键）',
    id_card VARCHAR(18) COMMENT '患者身份证号（外键）',
    department VARCHAR(50) COMMENT '就诊科室',
    expect_time DATETIME COMMENT '预计到达时间',
    appt_status VARCHAR(20) COMMENT '预约状态（未核验/已完成/已取消）',
    -- 外键关联患者表
    FOREIGN KEY (id_card) REFERENCES patient(id_card)
) COMMENT '预约信息表';

-- 排班信息表
CREATE TABLE IF NOT EXISTS schedule (
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

-- 就诊信息表
CREATE TABLE IF NOT EXISTS visit (
    visit_id INT PRIMARY KEY AUTO_INCREMENT COMMENT '就诊ID（主键）',
    id_card VARCHAR(18) COMMENT '患者身份证号（外键）',
    room_num INT COMMENT '诊室号',
    doctor_id INT COMMENT '医生工号（外键）',
    visit_time DATETIME COMMENT '就诊时间',
    visit_status VARCHAR(20) COMMENT '就诊状态（未就诊/就诊中/已离院）',
    -- 外键关联患者表、员工表
    FOREIGN KEY (id_card) REFERENCES patient(id_card),
    FOREIGN KEY (doctor_id) REFERENCES employee(emp_id)
) COMMENT '就诊信息表';

-- 账单信息表
CREATE TABLE IF NOT EXISTS bill (
    bill_id INT PRIMARY KEY AUTO_INCREMENT COMMENT '账单ID（主键）',
    visit_id INT COMMENT '就诊ID（外键）',
    total_fee DECIMAL(10,2) COMMENT '费用总额',
    medical_insur DECIMAL(10,2) COMMENT '医保金额',
    self_pay DECIMAL(10,2) COMMENT '自费金额',
    pay_time DATETIME COMMENT '收费时间',
    pay_method VARCHAR(20) COMMENT '支付方式（银行卡支付/微信支付/支付宝支付/现金支付）',
    actual_pay DECIMAL(10,2) COMMENT '实际收取金额',
    -- 外键关联就诊表
    FOREIGN KEY (visit_id) REFERENCES visit(visit_id)
) COMMENT '账单信息表';
```
### 权限用户管理
```sql
-- 1. 创建用户
CREATE USER IF NOT EXISTS 'patient'@'localhost' 
IDENTIFIED BY '123456';

-- 2. 对目标表逐个赋予所有权限
GRANT ALL PRIVILEGES ON community_hospital.patient TO 'patient'@'localhost';
GRANT ALL PRIVILEGES ON community_hospital.appointment TO 'patient'@'localhost';
GRANT ALL PRIVILEGES ON community_hospital.schedule TO 'patient'@'localhost';
GRANT ALL PRIVILEGES ON community_hospital.visit TO 'patient'@'localhost';
GRANT ALL PRIVILEGES ON community_hospital.bill TO 'patient'@'localhost';

-- 3. 刷新权限
FLUSH PRIVILEGES;
```

### 表关系说明
- **患者表(patient)** 为核心基础表，关联预约表(appointment)、就诊表(visit)
- **员工表(employee)** 关联排班表(schedule)、就诊表(visit)（医生工号）
- **就诊表(visit)** 作为中间表，关联患者表、员工表、账单表(bill)
- 所有外键均采用参照完整性约束，保证数据一致性

## 核心功能
### 1. 多角色权限管理
系统支持不同角色登录，配合 MySQL 数据库权限管理实现细粒度访问控制：
- **管理员**：拥有全表操作权限，可进行数据维护与权限配置
- **医生**：可查看本人排班、患者就诊信息，更新就诊状态
- **护士**：可核验预约、管理患者基本信息
- **行政人员**：可管理账单、统计业务数据

### 2. 数据库交互特性
- 使用 MySQL Connector C++ 原生 API 实现数据库连接与操作
- 包含多表连接查询（如患者+就诊+账单关联查询）、子查询、聚合查询等复杂SQL操作
- 实现事务处理，保证关键业务操作的原子性

### 3. 业务功能模块
- **患者管理**：注册、信息查询、状态更新
- **员工管理**：员工信息维护、权限配置
- **预约管理**：预约创建、核验、取消
- **排班管理**：医生排班创建、修改、查询
- **就诊管理**：就诊记录创建、状态跟踪
- **账单管理**：费用计算、支付记录、医保结算

## 快速开始
### 环境准备
1. 安装 MySQL 8.0+ 并启动服务
2. 下载并配置 [MySQL Connector C++ 8.3.0](https://dev.mysql.com/downloads/connector/cpp/)
3. 配置环境变量（确保连接器库可被编译器找到）

## 注意事项
1. 确保 MySQL 服务正常运行，且连接器版本与 MySQL 服务器版本兼容
2. 运行前需修改数据库连接配置（用户名、密码、主机地址等）
3. 建议在测试环境中先导入测试数据，再进行功能验证

## 作业相关说明
本项目为数据库系统课程作业，包含以下核心考核点：
- 数据库需求分析、概念设计、关系模型设计（详见实验报告与PPT）
- SQL 语句的编写与优化（DDL、DML、复杂查询）
- C++ 与数据库的交互开发
- 数据库权限管理与安全设计

## 许可证
本项目仅用于学习交流，禁止商用。

## 致谢
感谢中山大学网络空间安全学院数据库系统课程组的指导与支持。
