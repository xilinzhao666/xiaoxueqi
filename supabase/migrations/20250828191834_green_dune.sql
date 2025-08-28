-- =====================================================
-- 测试查询脚本 - 修复SQL语法错误
-- =====================================================

USE hospital_db;

-- =====================================================
-- 正确的SQL查询语句
-- =====================================================

-- 查询1：用户和对应的详细信息（修复版本）
SELECT 
    u.username,
    u.user_type,
    COALESCE(d.name, p.name) AS name,
    COALESCE(d.department, '患者') AS department
FROM users u
LEFT JOIN doctors d ON u.user_id = d.user_id
LEFT JOIN patients p ON u.user_id = p.user_id
ORDER BY u.user_type, u.user_id;

-- =====================================================
-- 其他有用的测试查询
-- =====================================================

-- 查询2：验证数据数量
SELECT 
    '数据统计' AS category,
    (SELECT COUNT(*) FROM users) AS total_users,
    (SELECT COUNT(*) FROM users WHERE user_type = 'Doctor') AS total_doctors,
    (SELECT COUNT(*) FROM users WHERE user_type = 'Patient') AS total_patients;

-- 查询3：查看所有医生信息
SELECT 
    d.doctor_id,
    d.name AS doctor_name,
    d.department,
    d.title,
    u.username
FROM doctors d
JOIN users u ON d.user_id = u.user_id
ORDER BY d.department, d.name;

-- 查询4：查看所有患者信息
SELECT 
    p.patient_id,
    p.name AS patient_name,
    p.gender,
    p.birth_date,
    YEAR(CURDATE()) - YEAR(p.birth_date) AS age,
    u.username
FROM patients p
JOIN users u ON p.user_id = u.user_id
ORDER BY p.name;

-- 查询5：查看病例统计
SELECT 
    d.department,
    COUNT(c.case_id) AS case_count,
    d.name AS doctor_name
FROM doctors d
LEFT JOIN cases c ON d.doctor_id = c.doctor_id
GROUP BY d.doctor_id, d.department, d.name
ORDER BY case_count DESC;

-- 查询6：查看预约状态统计
SELECT 
    status,
    COUNT(*) AS count,
    ROUND(COUNT(*) * 100.0 / (SELECT COUNT(*) FROM appointments), 2) AS percentage
FROM appointments
GROUP BY status
ORDER BY count DESC;

-- =====================================================
-- 简化版查询（如果上面的查询太复杂）
-- =====================================================

-- 简单查询：所有用户基本信息
SELECT user_id, username, user_type, email FROM users ORDER BY user_type, user_id;

-- 简单查询：所有医生
SELECT doctor_id, name, department, title FROM doctors ORDER BY department;

-- 简单查询：所有患者
SELECT patient_id, name, gender, birth_date FROM patients ORDER BY name;