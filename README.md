# 智慧医疗管理系统 (Hospital Management System)

一个基于C++和MySQL的现代化医院管理系统，支持患者挂号、医生诊疗、病历管理等核心医疗业务功能。系统提供两种运行模式：终端交互模式和JSON API模式。

## 📋 目录

- [项目概述](#项目概述)
- [文件结构](#文件结构)
- [功能特性](#功能特性)
- [系统要求](#系统要求)
- [部署说明](#部署说明)
- [接口文档](#接口文档)
- [运行说明](#运行说明)
- [测试方法](#测试方法)
- [开发指南](#开发指南)

## 🏥 项目概述

智慧医疗管理系统是一个全功能的医院信息管理平台，支持：

- **患者端功能**：注册登录、个人信息管理、预约挂号、查看病历、处方管理、医患沟通等
- **医生端功能**：登录认证、患者管理、诊断开方、检验报告、考勤管理、请假管理等
- **公共服务**：医生排班查询、医生信息查看等

系统采用RPC风格的Socket通信协议，支持实时双向通信。

## 📁 文件结构

```
hospital-management-system/
├── README.md                    # 项目说明文档
├── Makefile                     # 编译配置文件
├── docs/                        # 文档目录
│   ├── API.md                   # API接口文档
│   └── test.md                  # 测试用例说明
├── include/                     # 头文件目录
│   ├── ApiHandler.h             # API处理器头文件
│   ├── DatabaseConnection.h     # 数据库连接头文件
│   ├── HospitalService.h        # 医院服务头文件
│   ├── User.h                   # 用户类头文件
│   ├── Doctor.h                 # 医生类头文件
│   ├── Patient.h                # 患者类头文件
│   ├── Case.h                   # 病例类头文件
│   ├── Appointment.h            # 预约类头文件
│   ├── Hospitalization.h        # 住院类头文件
│   ├── Prescription.h           # 处方类头文件
│   └── Medication.h             # 药物类头文件
├── src/                         # 源代码目录
│   ├── main.cpp                 # 终端交互模式主程序
│   ├── JsonAPI.cpp              # JSON API模式主程序
│   ├── ApiHandler.cpp           # API处理器实现
│   ├── DatabaseConnection.cpp   # 数据库连接实现
│   ├── HospitalService.cpp      # 医院服务实现
│   ├── User.cpp                 # 用户类实现
│   ├── Doctor.cpp               # 医生类实现
│   ├── Patient.cpp              # 患者类实现
│   ├── Case.cpp                 # 病例类实现
│   ├── Appointment.cpp          # 预约类实现
│   ├── Hospitalization.cpp      # 住院类实现
│   ├── Prescription.cpp         # 处方类实现
│   └── Medication.cpp           # 药物类实现
├── sql/                         # 数据库脚本
│   └── hospital_complete_setup.sql  # 完整数据库初始化脚本
├── test/                        # 测试目录
│   ├── README.md                # 测试说明文档
│   ├── run_all_tests.sh         # 批量测试脚本
│   ├── run_single_test.sh       # 单个测试脚本
│   ├── public/                  # 公共API测试用例
│   ├── patient/                 # 患者API测试用例
│   ├── doctor/                  # 医生API测试用例
│   └── results/                 # 测试结果输出目录
└── build/                       # 编译输出目录
    ├── obj/                     # 对象文件
    ├── bin/                     # 可执行文件
    │   ├── Terminal             # 终端交互模式程序
    │   └── JsonAPI              # JSON API模式程序
    └── lib/                     # 静态库文件
```

## ✨ 功能特性

### 🏥 核心业务功能

#### **患者端功能 (14个API服务)**
1. **身份认证**
   - 患者注册 (`patient.auth.register`)
   - 患者登录 (`patient.auth.login`)
   - 找回密码 (`patient.auth.resetPassword`)

2. **个人信息管理**
   - 获取个人信息 (`patient.profile.get`)
   - 编辑个人信息 (`patient.profile.update`)

3. **预约挂号**
   - 提交挂号/预约 (`patient.appointment.create`)

4. **病历管理**
   - 查看病历和医嘱 (`patient.medicalRecord.list`)

5. **处方管理**
   - 查看处方列表 (`patient.prescription.list`)
   - 查看处方详情 (`patient.prescription.get`)

6. **医患沟通**
   - 发送消息 (`patient.chat.sendMessage`)
   - 获取聊天历史 (`patient.chat.getHistory`)

7. **健康服务**
   - 健康评估 (`patient.assessment.getLink`)
   - 检查结果查询 (`patient.labResult.list`)
   - 线上医疗服务 (`patient.consultation.requestOnline`)

#### **医生端功能 (16个API服务)**
1. **身份认证**
   - 医生登录 (`doctor.auth.login`)
   - 找回密码 (`doctor.auth.resetPassword`)

2. **个人信息管理**
   - 获取个人信息 (`doctor.profile.get`)
   - 编辑个人信息 (`doctor.profile.update`)

3. **患者管理**
   - 查看预约患者信息 (`doctor.appointment.list`)
   - 查看患者病历详情 (`doctor.patient.getMedicalRecords`)

4. **诊疗服务**
   - 做出诊断，留下医嘱 (`doctor.medicalRecord.create`)
   - 开具处方 (`doctor.prescription.create`)
   - 上传检验报告 (`doctor.labResult.upload`)

5. **状态管理**
   - 更新在线状态 (`doctor.status.update`)

6. **考勤管理**
   - 日常打卡 (`doctor.attendance.checkIn`)
   - 取消打卡 (`doctor.attendance.cancelCheckIn`)
   - 查看考勤历史 (`doctor.attendance.getHistory`)

7. **请假管理**
   - 提交请假申请 (`doctor.leaveRequest.submit`)
   - 查看请假列表 (`doctor.leaveRequest.list`)
   - 销假 (`doctor.leaveRequest.cancel`)

#### **公共服务 (2个API服务)**
- 查询医生坐诊安排 (`public.schedule.list`)
- 查看医生详细信息 (`public.doctor.get`)

### 🔧 技术特性

- **双运行模式**：终端交互模式 + JSON API模式
- **连接池管理**：高效的MySQL连接池
- **安全认证**：SHA256密码加密 + Token会话管理
- **异常处理**：完善的错误处理和日志记录
- **模块化设计**：清晰的MVC架构和DAO模式
- **内存管理**：智能指针和RAII模式

## 🛠 系统要求

### 操作系统
- Ubuntu 18.04+ / Debian 9+
- CentOS 7+ / RHEL 7+
- 其他支持MySQL和OpenSSL的Linux发行版

### 依赖库
- **编译器**: GCC 7.0+ (支持C++17)
- **数据库**: MySQL 5.7+ / MariaDB 10.2+
- **加密库**: OpenSSL 1.1.0+
- **JSON库**: nlohmann/json 3.0+
- **构建工具**: Make, CMake (可选)

### 硬件要求
- **内存**: 最小512MB，推荐2GB+
- **存储**: 最小100MB，推荐1GB+
- **CPU**: 支持x86_64架构

## 🚀 部署说明

### 1. 安装依赖库

#### Ubuntu/Debian系统：
```bash
# 更新包管理器
sudo apt-get update

# 安装编译工具
sudo apt-get install -y build-essential cmake

# 安装MySQL开发库
sudo apt-get install -y libmysqlclient-dev mysql-server

# 安装OpenSSL开发库
sudo apt-get install -y libssl-dev

# 安装nlohmann/json库
sudo apt-get install -y nlohmann-json3-dev

# 或者使用Makefile自动安装
make install-deps
```

#### CentOS/RHEL系统：
```bash
# 安装编译工具
sudo yum groupinstall -y "Development Tools"
sudo yum install -y cmake

# 安装MySQL开发库
sudo yum install -y mysql-devel mysql-server

# 安装OpenSSL开发库
sudo yum install -y openssl-devel

# 手动安装nlohmann/json库
git clone https://github.com/nlohmann/json.git
cd json && mkdir build && cd build
cmake .. && make -j4 && sudo make install
```

### 2. 配置MySQL数据库

```bash
# 启动MySQL服务
sudo systemctl start mysql
sudo systemctl enable mysql

# 创建数据库（如果需要）
make create-db

# 或手动创建
mysql -u root -p -e "CREATE DATABASE IF NOT EXISTS hospital_db CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;"
```

### 3. 初始化数据库

```bash
# 使用提供的SQL脚本初始化数据库
mysql -u root -p < sql/hospital_complete_setup.sql
```

### 4. 编译项目

```bash
# 编译所有可执行文件
make all

# 或分别编译
make terminal    # 编译终端交互模式
make jsonapi     # 编译JSON API模式

# 编译调试版本
make debug

# 静态链接版本
make static
```

### 5. 验证安装

```bash
# 检查可执行文件
ls -la build/bin/

# 运行基本测试
make test-jsonapi
```

## 📚 接口文档

### API通信协议

系统使用RPC风格的JSON通信协议：

#### **请求格式**
```json
{
  "api": "模块.资源.动作",
  "data": {
    // 请求参数
  }
}
```

#### **响应格式**
```json
{
  "status": "success|error",
  "code": 200,
  "message": "操作结果描述",
  "data": {
    // 响应数据
  }
}
```

### API端点分类

#### **公共接口 (无需认证)**
- `public.schedule.list` - 查询医生坐诊安排
- `public.doctor.get` - 查看医生详细信息

#### **患者端接口 (需要Token)**
- **认证**: `patient.auth.*`
- **个人信息**: `patient.profile.*`
- **预约挂号**: `patient.appointment.*`
- **病历处方**: `patient.medicalRecord.*`, `patient.prescription.*`
- **沟通服务**: `patient.chat.*`
- **健康服务**: `patient.assessment.*`, `patient.labResult.*`, `patient.consultation.*`

#### **医生端接口 (需要Token)**
- **认证**: `doctor.auth.*`
- **个人信息**: `doctor.profile.*`
- **患者管理**: `doctor.appointment.*`, `doctor.patient.*`
- **诊疗服务**: `doctor.medicalRecord.*`, `doctor.prescription.*`, `doctor.labResult.*`
- **状态管理**: `doctor.status.*`
- **考勤管理**: `doctor.attendance.*`
- **请假管理**: `doctor.leaveRequest.*`

详细的API接口文档请参考：[docs/API.md](docs/API.md)

## 🏃 运行说明

### 运行模式

系统提供两种运行模式：

#### **1. 终端交互模式**
```bash
# 编译并运行
make run-terminal

# 或直接运行
./build/bin/Terminal
```

**功能特点**：
- 交互式菜单操作
- 适合管理员使用
- 支持所有核心功能
- 实时数据库操作

#### **2. JSON API模式**
```bash
# 基本用法
./build/bin/JsonAPI --input <输入文件> --output <输出文件> [选项]

# 完整参数示例
./build/bin/JsonAPI \
  --input test/public/public_doctor_get_valid_id.json \
  --output result.json \
  --host localhost \
  --user root \
  --password your_password \
  --database hospital_db
```

**命令行参数**：
- `--input <文件>`：输入JSON请求文件路径
- `--output <文件>`：输出JSON响应文件路径
- `--host <地址>`：数据库主机地址（默认：localhost）
- `--user <用户名>`：数据库用户名（默认：root）
- `--password <密码>`：数据库密码（默认：空）
- `--database <数据库名>`：数据库名称（默认：hospital_db）
- `--help`：显示帮助信息

### 使用示例

#### **查询医生信息**
```bash
# 创建请求文件
echo '{"api": "public.doctor.get", "data": {"doctorId": "1"}}' > request.json

# 执行请求
./build/bin/JsonAPI --input request.json --output response.json --password your_password

# 查看结果
cat response.json
```

#### **患者登录**
```bash
# 创建登录请求
echo '{
  "api": "patient.auth.login",
  "data": {
    "account": "patient001@email.com",
    "password": "password123"
  }
}' > login.json

# 执行登录
./build/bin/JsonAPI --input login.json --output login_result.json --password your_password
```

## 🧪 测试方法

### 测试环境准备

1. **确保数据库已初始化**
```bash
mysql -u root -p < sql/hospital_complete_setup.sql
```

2. **编译测试程序**
```bash
make jsonapi
```

### 测试执行

#### **运行所有测试**
```bash
# 给脚本执行权限
chmod +x test/run_all_tests.sh

# 运行所有测试用例
./test/run_all_tests.sh
```

**测试输出示例**：
```
=== Public API 测试 ===
运行测试: public_schedule_list_no_filters ... PASS
运行测试: public_doctor_get_valid_id ... PASS

=== Patient API 测试 ===
运行测试: patient_auth_login_success ... PASS
运行测试: patient_profile_get_success ... PASS

=== Doctor API 测试 ===
运行测试: doctor_auth_login_success ... PASS
运行测试: doctor_appointment_list_success ... PASS

=== 测试结果统计 ===
总测试数: 63
通过测试: 58
失败测试: 5
```

#### **运行单个测试**
```bash
# 给脚本执行权限
chmod +x test/run_single_test.sh

# 运行指定测试
./test/run_single_test.sh test/public/public_doctor_get_valid_id.json
```

#### **手动测试**
```bash
# 直接使用JsonAPI程序测试
./build/bin/JsonAPI \
  --input test/patient/patient_auth_login_success.json \
  --output result.json \
  --host localhost \
  --user root \
  --password your_password \
  --database hospital_db

# 查看测试结果
cat result.json
```

### 测试用例说明

#### **测试覆盖范围**
- **Public API**: 7个测试用例
- **Patient API**: 26个测试用例  
- **Doctor API**: 30个测试用例
- **总计**: 63个测试用例

#### **测试场景类型**
- ✅ **正常功能测试** - 有效输入和预期输出
- ❌ **异常处理测试** - 无效输入和错误处理
- 🔒 **认证测试** - Token验证和权限控制
- 📝 **参数验证测试** - 必需参数和格式验证

#### **测试数据**
系统使用预置的测试数据：
- **测试患者账户**: patient001@email.com / password123
- **测试医生账户**: dr_zhang / password123
- **测试Token**: patient_token_123456, doctor_token_67890

### 测试结果分析

测试结果保存在`test/results/`目录下：
```bash
# 查看测试结果
ls test/results/

# 分析特定测试结果
cat test/results/public/public_doctor_get_valid_id.json
```

## 🔧 开发指南

### 编译选项

```bash
# 查看所有可用目标
make help

# 清理编译文件
make clean

# 编译调试版本
make debug

# 编译静态链接版本
make static
```

### 添加新功能

1. **添加新的API端点**：
   - 在`ApiHandler.h`中声明处理函数
   - 在`ApiHandler.cpp`中实现处理逻辑
   - 在构造函数中注册API路由

2. **添加新的数据模型**：
   - 创建对应的头文件和实现文件
   - 实现DAO模式的数据访问层
   - 在`HospitalService`中集成新功能

3. **添加测试用例**：
   - 在对应的`test/`子目录中创建JSON测试文件
   - 更新测试脚本以包含新的测试用例

### 调试技巧

1. **数据库连接问题**：
```bash
# 测试数据库连接
mysql -u root -p -e "USE hospital_db; SHOW TABLES;"
```

2. **编译问题**：
```bash
# 查看详细编译信息
make clean && make all VERBOSE=1
```

3. **运行时问题**：
```bash
# 使用调试版本
make debug
gdb ./build/bin/JsonAPI
```
