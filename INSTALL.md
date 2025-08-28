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
# 登录MySQL（注意命令是mysql，不是mysql-server）
mysql -u root -p

# 或者如果没有设置密码
sudo mysql -u root
```

在MySQL命令行中：
```sql
-- 创建数据库
CREATE DATABASE hospital_db CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

-- 创建专用用户（可选）
CREATE USER 'hospital_user'@'localhost' IDENTIFIED BY 'your_password';
GRANT ALL PRIVILEGES ON hospital_db.* TO 'hospital_user'@'localhost';
FLUSH PRIVILEGES;

-- 查看数据库
SHOW DATABASES;

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
# 导入完整测试环境
mysql -u root -p hospital_db < sql/hospital_complete_setup.sql

# 运行接口测试
mysql -u root -p hospital_db < sql/api_test_examples.sql
```

### 方法2：MySQL命令行内导入
```bash
mysql -u root -p
```

```sql
USE hospital_db;
SOURCE sql/hospital_complete_setup.sql;
SOURCE sql/api_test_examples.sql;
```

### 方法3：分步导入
```bash
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