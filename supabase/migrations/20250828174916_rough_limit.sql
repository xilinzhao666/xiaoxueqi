-- =====================================================
-- MySQL密码问题解决指南
-- 当您不知道MySQL root密码时的解决方案
-- =====================================================

-- 这个文件包含了解决MySQL密码问题的完整指南

-- =====================================================
-- 情况1：刚安装MySQL，没有设置过密码
-- =====================================================

-- 在系统命令行中尝试以下命令（按顺序）：

-- 命令1：使用sudo登录（Ubuntu/Debian最常见）
-- sudo mysql -u root

-- 命令2：直接登录（某些配置下可用）
-- mysql -u root

-- 命令3：空密码登录
-- mysql -u root -p
-- 当提示"Enter password:"时，直接按回车键

-- =====================================================
-- 情况2：成功登录后，设置密码
-- =====================================================

-- 如果上述方法成功登录，在mysql>提示符下执行：

-- 设置新密码
ALTER USER 'root'@'localhost' IDENTIFIED WITH mysql_native_password BY 'your_new_password';
FLUSH PRIVILEGES;

-- 验证密码设置
SELECT user, host, authentication_string FROM mysql.user WHERE user='root';

-- =====================================================
-- 情况3：完全忘记密码，需要重置
-- =====================================================

-- 在系统命令行中执行以下步骤：

-- 步骤1：停止MySQL服务
-- sudo systemctl stop mysql

-- 步骤2：以安全模式启动
-- sudo mysqld_safe --skip-grant-tables &

-- 步骤3：无密码登录
-- mysql -u root

-- 步骤4：在MySQL中重置密码
USE mysql;
UPDATE user SET authentication_string=PASSWORD('new_password') WHERE User='root';
FLUSH PRIVILEGES;
EXIT;

-- 步骤5：重启MySQL服务
-- sudo systemctl restart mysql

-- =====================================================
-- 导入测试数据的不同方法
-- =====================================================

-- 方法1：使用sudo导入（推荐）
-- sudo mysql -u root hospital_db < sql/hospital_complete_setup.sql

-- 方法2：分步执行
-- sudo mysql -u root -e "CREATE DATABASE IF NOT EXISTS hospital_db CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;"
-- sudo mysql -u root hospital_db < sql/hospital_complete_setup.sql

-- 方法3：在MySQL命令行中执行
-- sudo mysql -u root
-- 然后在mysql>提示符下：
CREATE DATABASE IF NOT EXISTS hospital_db CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
USE hospital_db;
SOURCE /home/ada/桌面/share/sql/hospital_complete_setup.sql;

-- =====================================================
-- 验证导入成功
-- =====================================================

-- 检查数据库
SHOW DATABASES;

-- 检查表
USE hospital_db;
SHOW TABLES;

-- 检查数据
SELECT COUNT(*) FROM users;
SELECT COUNT(*) FROM doctors;
SELECT COUNT(*) FROM patients;

-- 测试基本查询
SELECT * FROM users LIMIT 3;

-- =====================================================
-- 常见错误和解决方案
-- =====================================================

-- 错误1：Access denied for user 'root'@'localhost'
-- 解决：使用 sudo mysql -u root

-- 错误2：Can't connect to local MySQL server
-- 解决：sudo systemctl start mysql

-- 错误3：Unknown database 'hospital_db'
-- 解决：先创建数据库
-- CREATE DATABASE hospital_db CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

-- 错误4：File not found
-- 解决：确保在正确目录中，或使用绝对路径

SELECT '密码问题解决指南加载完成' AS status;
SELECT '请按照注释中的步骤操作' AS instruction;