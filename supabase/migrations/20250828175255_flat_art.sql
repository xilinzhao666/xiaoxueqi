-- =====================================================
-- MySQL密码问题快速解决方案
-- 当您看到"Enter password:"提示时的操作指南
-- =====================================================

-- 这个文件提供了解决MySQL密码问题的快速方案

-- =====================================================
-- 情况1：看到"Enter password:"提示
-- =====================================================

-- 当您运行以下命令时：
-- mysql -u root -p hospital_db < sql/hospital_complete_setup.sql
-- 
-- 如果看到：
-- Enter password:
-- 
-- 请尝试以下操作：

-- 操作1：直接按回车键（空密码）
-- 在"Enter password:"提示后，直接按回车键，不输入任何内容

-- =====================================================
-- 情况2：空密码不工作，使用sudo方法
-- =====================================================

-- 如果直接按回车不工作，请在系统命令行中尝试：

-- 方法1：使用sudo导入（推荐）
-- sudo mysql -u root hospital_db < sql/hospital_complete_setup.sql

-- 方法2：分步执行
-- sudo mysql -u root -e "CREATE DATABASE IF NOT EXISTS hospital_db CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;"
-- sudo mysql -u root hospital_db < sql/hospital_complete_setup.sql

-- =====================================================
-- 情况3：在MySQL命令行中手动执行
-- =====================================================

-- 步骤1：登录MySQL
-- sudo mysql -u root

-- 步骤2：在mysql>提示符下执行
CREATE DATABASE IF NOT EXISTS hospital_db CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
USE hospital_db;
SOURCE /home/ada/桌面/share/sql/hospital_complete_setup.sql;

-- =====================================================
-- 设置密码（可选）
-- =====================================================

-- 如果您想设置一个密码，在mysql>提示符下执行：
ALTER USER 'root'@'localhost' IDENTIFIED WITH mysql_native_password BY 'your_password';
FLUSH PRIVILEGES;

-- 验证密码设置
SELECT user, host, authentication_string FROM mysql.user WHERE user='root';

-- =====================================================
-- 验证导入成功
-- =====================================================

-- 检查数据库和表
SHOW DATABASES;
USE hospital_db;
SHOW TABLES;

-- 检查数据
SELECT COUNT(*) FROM users;
SELECT COUNT(*) FROM doctors;
SELECT COUNT(*) FROM patients;

SELECT '密码问题解决方案加载完成' AS status;
SELECT '请按照注释中的步骤操作' AS instruction;