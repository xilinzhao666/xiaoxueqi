-- =====================================================
-- 数据库数据验证脚本
-- 验证实际数据与预期数据是否一致
-- =====================================================

USE hospital_db;

-- =====================================================
-- 数据统计验证
-- =====================================================

SELECT '=== 数据库数据统计验证 ===' AS section;

-- 详细统计
SELECT 
    '用户统计' AS category,
    (SELECT COUNT(*) FROM users) AS total_count,
    (SELECT COUNT(*) FROM users WHERE user_type = 'Doctor') AS doctor_count,
    (SELECT COUNT(*) FROM users WHERE user_type = 'Patient') AS patient_count;

SELECT 
    '医生统计' AS category,
    (SELECT COUNT(*) FROM doctors) AS total_doctors,
    '预期: 6个医生' AS expected;

SELECT 
    '患者统计' AS category,
    (SELECT COUNT(*) FROM patients) AS total_patients,
    '预期: 9个患者' AS expected;

-- =====================================================
-- 详细数据列表验证
-- =====================================================

SELECT '=== 医生详细列表 ===' AS section;
SELECT 
    d.doctor_id,
    d.name,
    d.department,
    d.title,
    u.username
FROM doctors d
JOIN users u ON d.user_id = u.user_id
ORDER BY d.doctor_id;

SELECT '=== 患者详细列表 ===' AS section;
SELECT 
    p.patient_id,
    p.name,
    p.gender,
    p.birth_date,
    p.id_number,
    u.username
FROM patients p
JOIN users u ON p.user_id = u.user_id
ORDER BY p.patient_id;

-- =====================================================
-- 数据完整性验证
-- =====================================================

SELECT '=== 数据完整性验证 ===' AS section;

-- 验证用户-医生关系
SELECT 
    '用户-医生关系' AS check_type,
    COUNT(*) AS valid_relations,
    '应该等于医生总数' AS note
FROM doctors d
JOIN users u ON d.user_id = u.user_id
WHERE u.user_type = 'Doctor';

-- 验证用户-患者关系
SELECT 
    '用户-患者关系' AS check_type,
    COUNT(*) AS valid_relations,
    '应该等于患者总数' AS note
FROM patients p
JOIN users u ON p.user_id = u.user_id
WHERE u.user_type = 'Patient';

-- =====================================================
-- 预期vs实际对比
-- =====================================================

SELECT '=== 预期vs实际数据对比 ===' AS section;

SELECT 
    '医生数量' AS data_type,
    6 AS expected_count,
    (SELECT COUNT(*) FROM doctors) AS actual_count,
    CASE 
        WHEN (SELECT COUNT(*) FROM doctors) = 6 THEN '✓ 正确'
        ELSE '✗ 不匹配'
    END AS status;

SELECT 
    '患者数量' AS data_type,
    9 AS expected_count,
    (SELECT COUNT(*) FROM patients) AS actual_count,
    CASE 
        WHEN (SELECT COUNT(*) FROM patients) = 9 THEN '✓ 正确'
        ELSE '✗ 不匹配'
    END AS status;

SELECT 
    '用户总数' AS data_type,
    15 AS expected_count,
    (SELECT COUNT(*) FROM users) AS actual_count,
    CASE 
        WHEN (SELECT COUNT(*) FROM users) = 15 THEN '✓ 正确'
        ELSE '✗ 不匹配'
    END AS status;

SELECT '=== 验证完成 ===' AS completion;