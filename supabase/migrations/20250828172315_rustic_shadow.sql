-- =====================================================
-- 医院管理系统分步骤设置指南
-- 请按照注释中的步骤操作
-- =====================================================

-- 步骤1：首先在系统命令行中登录MySQL
-- 命令：mysql -u root -p
-- 或者：sudo mysql -u root

-- 步骤2：看到mysql>提示符后，复制粘贴以下命令创建数据库
CREATE DATABASE IF NOT EXISTS hospital_db 
CHARACTER SET utf8mb4 
COLLATE utf8mb4_unicode_ci;

-- 步骤3：验证数据库创建成功
SHOW DATABASES;

-- 步骤4：选择数据库
USE hospital_db;

-- 步骤5：退出MySQL
-- 输入：EXIT;

-- 步骤6：回到系统命令行，导入完整数据
-- 命令：mysql -u root -p hospital_db < sql/hospital_complete_setup.sql

-- 或者，如果您想在MySQL命令行中直接执行：
-- 重新登录：mysql -u root -p hospital_db
-- 然后执行：SOURCE sql/hospital_complete_setup.sql;

-- =====================================================
-- 验证安装成功的查询
-- =====================================================

-- 在导入数据后，可以运行以下查询验证：

-- 查看所有表
SHOW TABLES;

-- 查看数据统计
SELECT 
    (SELECT COUNT(*) FROM users) AS total_users,
    (SELECT COUNT(*) FROM doctors) AS total_doctors,
    (SELECT COUNT(*) FROM patients) AS total_patients,
    (SELECT COUNT(*) FROM cases) AS total_cases;

-- 测试基本查询（按照文档3.1接口）
SELECT * FROM users WHERE user_id = 1;

-- 测试表关系
SELECT 
    u.username,
    u.user_type,
    COALESCE(d.name, p.name) AS name
FROM users u
LEFT JOIN doctors d ON u.user_id = d.user_id
LEFT JOIN patients p ON u.user_id = p.user_id
LIMIT 5;