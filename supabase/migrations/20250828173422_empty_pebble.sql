-- =====================================================
-- MySQL 故障排除指南
-- 解决常见的文件路径和导入问题
-- =====================================================

-- 问题1：文件路径错误
-- 错误信息：Failed to open file 'sql/hospital_complete_setup.sql', error: 2

-- 解决方案：
-- 1. 检查当前工作目录
-- 在系统命令行中执行：pwd

-- 2. 切换到正确的项目目录
-- cd /home/ada/桌面/share

-- 3. 验证文件是否存在
-- ls -la sql/

-- 4. 使用正确的导入方法

-- =====================================================
-- 方法1：系统命令行导入（推荐）
-- =====================================================

-- 步骤1：在系统命令行中切换目录
-- cd /home/ada/桌面/share

-- 步骤2：创建数据库
-- mysql -u root -p -e "CREATE DATABASE IF NOT EXISTS hospital_db CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;"

-- 步骤3：导入数据
-- mysql -u root -p hospital_db < sql/hospital_complete_setup.sql

-- =====================================================
-- 方法2：MySQL命令行内导入
-- =====================================================

-- 步骤1：在系统命令行中切换目录
-- cd /home/ada/桌面/share

-- 步骤2：登录MySQL
-- mysql -u root -p

-- 步骤3：在MySQL中执行（注意：必须在正确的目录中）
USE hospital_db;
SOURCE sql/hospital_complete_setup.sql;

-- =====================================================
-- 方法3：使用绝对路径
-- =====================================================

-- 如果相对路径不工作，使用完整路径：
-- SOURCE /home/ada/桌面/share/sql/hospital_complete_setup.sql;

-- =====================================================
-- 验证导入成功
-- =====================================================

-- 检查表是否创建
SHOW TABLES;

-- 检查数据是否插入
SELECT 
    (SELECT COUNT(*) FROM users) AS total_users,
    (SELECT COUNT(*) FROM doctors) AS total_doctors,
    (SELECT COUNT(*) FROM patients) AS total_patients;

-- 测试基本查询
SELECT * FROM users LIMIT 3;

-- =====================================================
-- 常见问题解决
-- =====================================================

-- 问题：权限不足
-- 解决：使用 sudo mysql -u root

-- 问题：数据库不存在
-- 解决：先创建数据库
CREATE DATABASE IF NOT EXISTS hospital_db CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

-- 问题：表已存在
-- 解决：删除现有表或使用 IF NOT EXISTS

-- 问题：字符编码问题
-- 解决：确保使用 utf8mb4 字符集

-- =====================================================
-- 快速重置环境
-- =====================================================

-- 如果需要重新开始：
DROP DATABASE IF EXISTS hospital_db;
CREATE DATABASE hospital_db CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

-- 然后重新导入数据