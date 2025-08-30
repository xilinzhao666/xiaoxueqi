# MySQL 安装和配置指南

## Ubuntu/Debian 系统安装

### 1. 安装MySQL服务器

```bash
# 更新包列表
sudo apt-get update

# 安装MySQL服务器
sudo apt-get install mysql-server

# 安装MySQL开发库（用于C++开发）
sudo apt-get install libmysqlclient-dev

# 安装其他必要工具
sudo apt-get install mysql-client
```

### 2. 启动和配置MySQL服务

```bash
# 启动MySQL服务
sudo systemctl start mysql

# 设置开机自启
sudo systemctl enable mysql

# 检查服务状态
sudo systemctl status mysql
```

### 3. 安全配置（推荐）

```bash
# 运行安全配置脚本
sudo mysql_secure_installation
```

这个脚本会询问以下问题：
- 是否安装密码验证插件
- 设置root密码
- 是否删除匿名用户
- 是否禁止root远程登录
- 是否删除test数据库

### 4. 登录和创建数据库

```bash
# 重要：登录MySQL的正确方法
# 命令是 mysql，不是 mysql-server
mysql -u root -p
pwd: zxl20050601

# 如果上面命令失败，尝试使用sudo
sudo mysql -u root
```

**重要**：只有看到 `mysql>` 提示符后，才能输入SQL命令：

```sql
-- 在MySQL命令行中输入（注意前面有mysql>提示符）
mysql> CREATE DATABASE hospital_db CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

-- 创建专用用户（可选）
mysql> CREATE USER 'hospital_user'@'localhost' IDENTIFIED BY 'your_password';
For test, usr: wyt, pwd:20030609.
mysql> GRANT ALL PRIVILEGES ON hospital_db.* TO 'hospital_user'@'localhost';
mysql> FLUSH PRIVILEGES;

-- 查看数据库
mysql> SHOW DATABASES;

-- 退出MySQL
mysql> EXIT;
```

### 5. 导入测试数据

退出MySQL后，在系统命令行中执行：

```bash
# 导入完整测试数据
mysql -u root -p hospital_db < sql/hospital_complete_setup.sql

# 验证导入成功
mysql -u root -p hospital_db -e "SHOW TABLES; SELECT COUNT(*) FROM users;"
```

## 完整操作流程示例

```bash
# 1. 检查MySQL服务
sudo systemctl status mysql

# 2. 如果未运行，启动MySQL
sudo systemctl start mysql

# 3. 登录MySQL
mysql -u root -p
# 输入密码后，您会看到：
# Welcome to the MySQL monitor...
# mysql>

# 4. 在mysql>提示符下创建数据库
-- 创建数据库
CREATE DATABASE hospital_db CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

-- 退出MySQL
EXIT;

# 5. 回到系统命令行，导入数据
mysql -u root -p hospital_db < sql/hospital_complete_setup.sql
```

## MySQL密码问题解决

### 情况1：没有设置过密码

如果您刚安装MySQL且没有设置过密码，尝试以下方法：

```bash
# 方法1：使用sudo（最常见）
sudo mysql -u root

# 方法2：直接登录（无密码）
mysql -u root

# 方法3：空密码登录
mysql -u root -p
# 当提示输入密码时，直接按回车键
```

### 情况2：忘记了密码

如果您忘记了MySQL密码，可以重置：

```bash
# 1. 停止MySQL服务
sudo systemctl stop mysql

# 2. 以安全模式启动MySQL
sudo mysqld_safe --skip-grant-tables &

# 3. 无密码登录
mysql -u root

# 4. 在MySQL中重置密码
mysql> USE mysql;
mysql> UPDATE user SET authentication_string=PASSWORD('new_password') WHERE User='root';
mysql> FLUSH PRIVILEGES;
mysql> EXIT;

# 5. 重启MySQL服务
sudo systemctl restart mysql
```

### 情况3：Ubuntu默认配置

在Ubuntu系统中，MySQL可能配置为只允许sudo用户访问：

```bash
# 使用sudo登录
sudo mysql -u root

# 然后设置密码
mysql> ALTER USER 'root'@'localhost' IDENTIFIED WITH mysql_native_password BY 'your_password';
mysql> FLUSH PRIVILEGES;
mysql> EXIT;
```

### 快速测试方法

按顺序尝试以下命令，直到成功登录：

```bash
# 0. 如果提示输入密码，直接按回车键
mysql -u root -p
# 看到"Enter password:"时，直接按回车

# 1. 尝试sudo登录
sudo mysql -u root

# 2. 尝试无密码登录
mysql -u root

# 3. 尝试常见默认密码
mysql -u root -p
# 尝试输入：root, password, 123456, admin
```

### 导入数据的替代方法

如果密码问题暂时无法解决，可以使用以下方法：

```bash
# 方法1：使用sudo导入
sudo mysql -u root hospital_db < sql/hospital_complete_setup.sql

# 方法2：分步执行
sudo mysql -u root -e "CREATE DATABASE IF NOT EXISTS hospital_db CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;"
sudo mysql -u root hospital_db < sql/hospital_complete_setup.sql

# 方法3：在MySQL命令行中执行
sudo mysql -u root
mysql> CREATE DATABASE IF NOT EXISTS hospital_db CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
mysql> USE hospital_db;
mysql> SOURCE /home/ada/桌面/share/sql/hospital_complete_setup.sql;
```

-- 退出
EXIT;
```

## 常见问题解决

### 问题1：mysql-server命令未找到

**错误信息**：`mysql-server：未找到命令`

**解决方案**：
- 正确的命令是 `mysql`，不是 `mysql-server`
- `mysql-server` 是包名，不是命令名

```bash
# 错误的命令
mysql-server

# 正确的命令
mysql -u root -p
```

### 问题2：MySQL服务未启动

**错误信息**：`Can't connect to local MySQL server`

**解决方案**：
```bash
# 检查服务状态
sudo systemctl status mysql

# 启动服务
sudo systemctl start mysql

# 如果启动失败，查看日志
sudo journalctl -u mysql.service
```

### 问题3：无法以root用户登录

**解决方案**：
```bash
# 尝试使用sudo
sudo mysql -u root

# 在MySQL中重置密码
ALTER USER 'root'@'localhost' IDENTIFIED WITH mysql_native_password BY 'new_password';
FLUSH PRIVILEGES;
```

### 问题4：权限被拒绝

**解决方案**：
```bash
# 检查MySQL进程
ps aux | grep mysql

# 检查端口占用
sudo netstat -tlnp | grep :3306

# 重启MySQL服务
sudo systemctl restart mysql
```

## 验证安装

### 1. 检查MySQL版本
```bash
mysql --version
```

### 2. 检查服务状态
```bash
sudo systemctl status mysql
```

### 3. 测试连接
```bash
mysql -u root -p -e "SELECT VERSION();"
```

### 4. 检查数据库
```bash
mysql -u root -p -e "SHOW DATABASES;"
```

## 导入测试数据

### 方法1：命令行导入
```bash
# 重要：确保在正确的目录中
cd /path/to/your/project  # 例如：cd /home/ada/桌面/share

# 导入完整测试环境
mysql -u root -p hospital_db < sql/hospital_complete_setup.sql

# 运行接口测试
mysql -u root -p hospital_db < sql/api_test_examples.sql
```

### 方法2：MySQL命令行内导入
```bash
# 先切换到项目目录
cd /home/ada/桌面/share

# 然后登录MySQL
mysql -u root -p
```

```sql
USE hospital_db;
SOURCE sql/hospital_complete_setup.sql;
SOURCE sql/api_test_examples.sql;
```

### 方法3：使用绝对路径
```bash
mysql -u root -p hospital_db
```

```sql
-- 使用完整路径（请替换为您的实际路径）
SOURCE /home/ada/桌面/share/sql/hospital_complete_setup.sql;
```

### 方法3：分步导入
```bash
# 确保在项目目录中
cd /home/ada/桌面/share

# 1. 创建数据库
mysql -u root -p -e "CREATE DATABASE IF NOT EXISTS hospital_db CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;"

# 2. 导入表结构和数据
mysql -u root -p hospital_db < sql/hospital_complete_setup.sql

# 3. 验证导入
mysql -u root -p hospital_db -e "SHOW TABLES; SELECT COUNT(*) FROM users;"
```

## 卸载MySQL（如果需要重新安装）

```bash
# 停止服务
sudo systemctl stop mysql

# 卸载MySQL
sudo apt-get remove --purge mysql-server mysql-client mysql-common mysql-server-core-* mysql-client-core-*

# 删除配置文件
sudo rm -rf /etc/mysql /var/lib/mysql

# 清理包缓存
sudo apt-get autoremove
sudo apt-get autoclean
```

## 性能调优（可选）

### 编辑MySQL配置文件
```bash
sudo nano /etc/mysql/mysql.conf.d/mysqld.cnf
```

### 常用配置项
```ini
[mysqld]
# 基本设置
bind-address = 127.0.0.1
port = 3306

# 性能设置
innodb_buffer_pool_size = 256M
max_connections = 100
query_cache_size = 16M
query_cache_limit = 1M

# 字符集设置
character-set-server = utf8mb4
collation-server = utf8mb4_unicode_ci
```

### 重启服务使配置生效
```bash
sudo systemctl restart mysql
```