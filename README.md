# 医院管理系统 C++ MySQL 数据库

基于提供的数据库架构文档构建的完整医院管理系统，使用C++和MySQL实现。

## 数据库架构

### 表结构

根据提供的架构文档，系统包含以下8个核心表：

1. **users** - 用户账户表（医生和患者）
2. **doctors** - 医生详细信息表
3. **patients** - 患者个人信息表
4. **cases** - 患者病例信息表
5. **appointments** - 患者预约记录表
6. **hospitalization** - 患者住院记录表
7. **prescriptions** - 医生开具的处方表
8. **medications** - 处方药物表

### 表关系

```
users (1:1) doctors
users (1:1) patients
patients (1:N) cases
doctors (1:N) cases
patients (1:N) appointments
doctors (1:N) appointments
patients (1:N) hospitalization
cases (1:N) prescriptions
prescriptions (1:N) medications
```

## 系统架构

```
├── include/                    # 头文件
│   ├── DatabaseConnection.h   # 数据库连接和连接池
│   ├── User.h                  # 用户实体和DAO
│   ├── Doctor.h                # 医生实体和DAO
│   ├── Patient.h               # 患者实体和DAO
│   ├── Case.h                  # 病例实体和DAO
│   ├── Appointment.h           # 预约实体和DAO
│   ├── Hospitalization.h       # 住院实体和DAO
│   ├── Prescription.h          # 处方实体和DAO
│   ├── Medication.h            # 药物实体和DAO
│   └── HospitalService.h       # 医院服务层
├── src/                        # 源文件
│   ├── DatabaseConnection.cpp
│   ├── User.cpp
│   ├── Doctor.cpp
│   ├── Patient.cpp
│   ├── Case.cpp
│   ├── Appointment.cpp
│   ├── Hospitalization.cpp
│   ├── Prescription.cpp
│   ├── Medication.cpp
│   ├── HospitalService.cpp
│   └── main.cpp               # 主程序
├── sql/                       # SQL脚本
│   └── hospital_schema.sql    # 数据库架构
├── CMakeLists.txt             # CMake构建文件
├── Makefile                   # Make构建文件
└── README.md                  # 说明文档
```

## 核心功能

### 用户管理
- 用户注册（医生/患者）
- 用户登录认证
- 用户信息管理

### 医生管理
- 医生信息注册
- 按科室查询医生
- 医生搜索功能

### 患者管理
- 患者信息注册
- 患者信息查询
- 按身份证号查找患者

### 病例管理
- 创建医疗病例
- 查看患者病例历史
- 按医生/科室查询病例

### 预约管理
- 预约挂号
- 预约状态管理（已预约/已就诊/已取消）
- 医生预约列表查看

### 住院管理
- 住院登记
- 病房床位管理
- 住院记录查询

### 处方管理
- 开具处方
- 处方查询
- 处方药物管理

## 环境要求

### 系统要求
- Linux/macOS/Windows
- C++17 或更高版本
- CMake 3.10+
- MySQL 5.7+ 或 MariaDB 10.2+

### 依赖库
- MySQL C++ Connector (libmysqlclient-dev)
- OpenSSL (libssl-dev)
- pthread

## 安装和编译

### Ubuntu/Debian 系统

1. **安装依赖**:
```bash
# 安装编译工具
sudo apt-get update
sudo apt-get install build-essential cmake

# 安装MySQL开发库
sudo apt-get install libmysqlclient-dev mysql-server

# 安装OpenSSL开发库
sudo apt-get install libssl-dev
```

2. **启动和配置MySQL服务**:
```bash
# 启动MySQL服务
sudo systemctl start mysql

# 设置MySQL开机自启
sudo systemctl enable mysql

# 检查MySQL服务状态
sudo systemctl status mysql

# 登录MySQL（如果不知道密码，使用sudo）
sudo mysql -u root
# 或者尝试：mysql -u root -p（密码为空时直接按回车）
```

### 常见问题解决

#### 0. MySQL密码问题
```bash
# 如果不知道密码，按顺序尝试：

# 方法1：使用sudo（最常见）
sudo mysql -u root

# 方法2：无密码登录
mysql -u root

# 方法3：空密码（按回车）
mysql -u root -p

# 成功登录后，可以设置密码：
mysql> ALTER USER 'root'@'localhost' IDENTIFIED WITH mysql_native_password BY 'your_password';
mysql> FLUSH PRIVILEGES;
```

#### 1. MySQL服务未启动
```bash
# 检查MySQL服务状态
sudo systemctl status mysql

# 如果未运行，启动服务
sudo systemctl start mysql

# 如果启动失败，查看日志
sudo journalctl -u mysql.service
```

#### 2. 无法连接MySQL
```bash
# 检查MySQL是否在监听端口
sudo netstat -tlnp | grep :3306

# 或使用ss命令
sudo ss -tlnp | grep :3306
```

#### 3. 权限问题
```bash
# 如果无法以root用户登录，尝试
sudo mysql -u root

# 然后设置root密码
ALTER USER 'root'@'localhost' IDENTIFIED WITH mysql_native_password BY 'your_password';
FLUSH PRIVILEGES;
```

## 数据库测试环境搭建

### 快速搭建测试环境

我们提供了三个SQL脚本来帮助您快速搭建测试环境：

#### 1. **完整测试环境** (`sql/hospital_complete_setup.sql`)
包含完整的表结构、测试数据、视图和存储过程：

```bash
# 重要：请按照以下步骤操作

# 第一步：确保MySQL服务正在运行
sudo systemctl status mysql

# 如果MySQL未运行，启动它
sudo systemctl start mysql

# 第二步：登录MySQL（注意是mysql命令，不是mysql-server）
mysql -u root -p
# 或者如果没有设置密码
sudo mysql -u root

# 第三步：在MySQL命令行中创建数据库
# 看到 mysql> 提示符后，输入以下命令：
CREATE DATABASE hospital_db CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

# 第四步：退出MySQL
EXIT;

# 第五步：导入完整脚本
mysql -u root -p hospital_db < sql/hospital_complete_setup.sql

# 或者在MySQL命令行中执行（需要正确的文件路径）：
# 方法1：使用绝对路径
mysql -u root -p hospital_db
mysql> source /home/ada/桌面/share/sql/hospital_complete_setup.sql;

# 方法2：先切换到项目目录
cd /home/ada/桌面/share
mysql -u root -p hospital_db
mysql> source sql/hospital_complete_setup.sql;

# 方法3：使用系统命令行导入（推荐）
cd /home/ada/桌面/share
mysql -u root -p hospital_db < sql/hospital_complete_setup.sql
```

**包含的测试数据：**
- 5个医生（涵盖心内科、神经科、骨科、儿科、内科）
- 8个患者（不同性别、年龄）
- 8个病例（各科室典型病例）
- 10个预约记录（不同状态）
- 5个住院记录（不同病房）
- 8个处方记录
- 16个药物记录

#### 2. **API接口测试** (`sql/api_test_examples.sql`)
专门用于测试文档3.1-3.8接口的脚本：

```bash
# 在完整环境基础上运行接口测试
mysql -u root -p hospital_db < sql/api_test_examples.sql
```

**测试内容：**
- 用户管理接口测试（注册、登录、查询）
- 医生管理接口测试（信息注册、查询、搜索）
- 患者管理接口测试（信息注册、查询、搜索）
- 病例管理接口测试（创建、查询）
- 预约管理接口测试（创建、状态更新）
- 住院管理接口测试（登记、查询）
- 处方管理接口测试（开具、查询）
- 药物管理接口测试（添加、查询）
- 综合业务流程测试
- 性能测试和错误处理测试

#### 3. **快速验证环境** (`sql/quick_setup.sql`)
最小化的测试环境，用于快速验证：

```bash
# 快速创建基础测试环境
mysql -u root -p < sql/quick_setup.sql
```

### 完整的测试流程

```bash
# 1. 安装MySQL
sudo apt-get update
sudo apt-get install mysql-server

# 2. 启动MySQL服务
sudo systemctl start mysql
sudo systemctl enable mysql

# 3. 登录MySQL并创建数据库
mysql -u root -p
CREATE DATABASE hospital_db CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
EXIT;

# 4. 导入完整测试数据
mysql -u root -p hospital_db < sql/hospital_complete_setup.sql

# 5. 运行接口测试
mysql -u root -p hospital_db < sql/api_test_examples.sql

# 6. 验证数据
mysql -u root -p hospital_db
SHOW TABLES;
SELECT COUNT(*) FROM users;
SELECT COUNT(*) FROM doctors;
SELECT COUNT(*) FROM patients;
```

### 测试数据验证

运行完整设置后，可以通过以下查询验证数据：

```sql
-- 验证表创建
SHOW TABLES;

-- 验证数据插入
SELECT 
    (SELECT COUNT(*) FROM users) AS total_users,
    (SELECT COUNT(*) FROM doctors) AS total_doctors,
    (SELECT COUNT(*) FROM patients) AS total_patients,
    (SELECT COUNT(*) FROM cases) AS total_cases,
    (SELECT COUNT(*) FROM appointments) AS total_appointments,
    (SELECT COUNT(*) FROM hospitalization) AS total_hospitalizations,
    (SELECT COUNT(*) FROM prescriptions) AS total_prescriptions,
    (SELECT COUNT(*) FROM medications) AS total_medications;

-- 验证表关系
SELECT 
    u.username,
    u.user_type,
    COALESCE(d.name, p.name) AS name,
    COALESCE(d.department, '患者') AS department
FROM users u
LEFT JOIN doctors d ON u.user_id = d.user_id
LEFT JOIN patients p ON u.user_id = p.user_id;
```

### 接口测试示例

按照文档3.1-3.8接口进行测试：

```sql
-- 3.1 用户表接口测试
SELECT * FROM users WHERE user_id = 1;

-- 3.2 医生表接口测试
SELECT * FROM doctors WHERE doctor_id = 1;

-- 3.3 患者表接口测试
SELECT * FROM patients WHERE patient_id = 1;

-- 3.4 病例表接口测试
SELECT * FROM cases WHERE case_id = 1;

-- 3.5 预约表接口测试
SELECT * FROM appointments WHERE patient_id = 1;

-- 3.6 住院表接口测试
SELECT * FROM hospitalization WHERE patient_id = 1;

-- 3.7 处方表接口测试
SELECT * FROM prescriptions WHERE prescription_id = 1;

-- 3.8 药物表接口测试
SELECT * FROM medications WHERE medication_id = 1;
```

### 业务流程测试

测试完整的医院业务流程：

```sql
-- 1. 患者预约
INSERT INTO appointments (patient_id, doctor_id, appointment_time, department, status) 
VALUES (1, 1, '2024-03-01 09:00:00', 'Cardiology', 'Booked');

-- 2. 患者就诊，创建病例
INSERT INTO cases (patient_id, department, doctor_id, diagnosis) 
VALUES (1, 'Cardiology', 1, '复查高血压，血压控制良好');

-- 3. 更新预约状态
UPDATE appointments SET status = 'Attended' WHERE appointment_id = LAST_INSERT_ID();

-- 4. 开具处方
INSERT INTO prescriptions (case_id, doctor_id, prescription_content) 
VALUES (LAST_INSERT_ID(), 1, '继续服用降压药，定期监测');

-- 5. 添加药物
INSERT INTO medications (prescription_id, medication_name, quantity, usage_instructions) 
VALUES (LAST_INSERT_ID(), '氨氯地平片', 30, '每日一次，餐后服用');
```

### 清理测试环境

如需重置测试环境：

```sql
-- 清理所有数据（保留表结构）
SET FOREIGN_KEY_CHECKS = 0;
TRUNCATE TABLE medications;
TRUNCATE TABLE prescriptions;
TRUNCATE TABLE hospitalization;
TRUNCATE TABLE appointments;
TRUNCATE TABLE cases;
TRUNCATE TABLE patients;
TRUNCATE TABLE doctors;
TRUNCATE TABLE users;
SET FOREIGN_KEY_CHECKS = 1;

-- 或完全删除数据库重新开始
DROP DATABASE IF EXISTS hospital_db;
```

3. **编译项目**:

使用 Make:
```bash
# 安装依赖
make install-deps

# 创建数据库
make create-db

# 编译
make

# 运行
make run
```

使用 CMake:
```bash
mkdir build
cd build
cmake ..
make
./HospitalManagementSystem
```

## 使用说明

### 启动程序

```bash
./bin/HospitalSystem
```

### 主要功能操作流程

1. **初始化系统**
   - 启动程序
   - 选择"初始化数据库"创建所有表

2. **用户注册**
   - 注册用户账户（医生或患者）
   - 根据用户类型注册详细信息

3. **医生工作流程**
   - 登录系统
   - 查看预约列表
   - 创建病例
   - 开具处方
   - 添加药物

4. **患者使用流程**
   - 登录系统
   - 预约挂号
   - 查看病例历史
   - 查看处方信息

## API 接口

### 用户管理 (UserDAO)
```cpp
// 用户CRUD操作
bool createUser(const User& user);
std::unique_ptr<User> getUserById(int userId);
std::unique_ptr<User> getUserByUsername(const std::string& username);

// 用户认证
std::unique_ptr<User> authenticateUser(const std::string& username, const std::string& password);
```

### 医生管理 (DoctorDAO)
```cpp
// 医生信息管理
bool createDoctor(const Doctor& doctor);
std::unique_ptr<Doctor> getDoctorById(int doctorId);
std::vector<std::unique_ptr<Doctor>> getDoctorsByDepartment(const std::string& department);
```

### 患者管理 (PatientDAO)
```cpp
// 患者信息管理
bool createPatient(const Patient& patient);
std::unique_ptr<Patient> getPatientById(int patientId);
std::unique_ptr<Patient> getPatientByIdNumber(const std::string& idNumber);
```

### 病例管理 (CaseDAO)
```cpp
// 病例管理
bool createCase(const Case& medicalCase);
std::vector<std::unique_ptr<Case>> getCasesByPatientId(int patientId);
std::vector<std::unique_ptr<Case>> getCasesByDoctorId(int doctorId);
```

### 预约管理 (AppointmentDAO)
```cpp
// 预约管理
bool createAppointment(const Appointment& appointment);
std::vector<std::unique_ptr<Appointment>> getAppointmentsByPatientId(int patientId);
bool updateAppointmentStatus(int appointmentId, AppointmentStatus status);
```

## 数据库接口

严格按照提供的文档实现以下查询接口：

### 3.1 用户表接口
```sql
SELECT * FROM users WHERE user_id = ?;
```

### 3.2 医生表接口
```sql
SELECT * FROM doctors WHERE doctor_id = ?;
```

### 3.3 患者表接口
```sql
SELECT * FROM patients WHERE patient_id = ?;
```

### 3.4 病例表接口
```sql
SELECT * FROM cases WHERE case_id = ?;
```

### 3.5 预约表接口
```sql
SELECT * FROM appointments WHERE patient_id = ?;
```

### 3.6 住院表接口
```sql
SELECT * FROM hospitalization WHERE patient_id = ?;
```

### 3.7 处方表接口
```sql
SELECT * FROM prescriptions WHERE prescription_id = ?;
```

### 3.8 药物表接口
```sql
SELECT * FROM medications WHERE medication_id = ?;
```

## 安全特性

### 数据安全
- 密码使用SHA-256哈希存储
- SQL注入防护
- 输入验证和清理
- 事务管理确保数据一致性

### 访问控制
- 用户认证机制
- 基于用户类型的权限控制
- 数据访问日志

## 性能优化

### 数据库优化
- 所有表使用InnoDB引擎
- 关键字段建立索引
- 外键约束确保数据完整性
- 复杂查询使用视图优化

### 应用层优化
- 连接池减少连接开销
- 智能指针管理内存
- 事务管理确保数据一致性

## 故障排除

### 常见问题

1. **编译错误**:
   - 检查MySQL开发库安装
   - 确认C++17支持
   - 检查OpenSSL库

2. **连接失败**:
   - 确认MySQL服务运行
   - 检查连接参数
   - 验证数据库权限

3. **运行时错误**:
   - 检查数据库是否存在
   - 确认表结构正确
   - 查看错误日志

## 扩展功能

### 可扩展模块
- Web API接口
- 报告生成系统
- 数据分析功能
- 移动端接口
- 消息通知系统

## 许可证

本项目采用 MIT 许可证。

## 注意事项

- 这是基于提供的数据库架构文档严格实现的系统
- 所有表名、字段名、数据类型都与文档保持一致
- 表关系严格按照文档中的外键关系实现
- 接口查询严格按照文档中的3.1-3.8接口设计实现