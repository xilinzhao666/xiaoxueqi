-- =====================================================
-- 医院管理系统接口测试查询
-- 严格按照文档3.1-3.8接口设计
-- =====================================================

-- 设置数据库
USE hospital_db;

-- =====================================================
-- 3.1 Users Table Interface
-- =====================================================
-- Function: Retrieve user account information
-- Query: SELECT * FROM users WHERE user_id = ?;

SELECT '=== 3.1 Users Table Interface Tests ===' AS test_section;

-- 测试查询用户ID为1的用户
SELECT * FROM users WHERE user_id = 1;

-- 测试查询用户ID为6的用户（患者）
SELECT * FROM users WHERE user_id = 6;

-- 测试查询不存在的用户
SELECT * FROM users WHERE user_id = 999;

-- =====================================================
-- 3.2 Doctors Table Interface
-- =====================================================
-- Function: Retrieve doctor details
-- Query: SELECT * FROM doctors WHERE doctor_id = ?;

SELECT '=== 3.2 Doctors Table Interface Tests ===' AS test_section;

-- 测试查询医生ID为1的医生
SELECT * FROM doctors WHERE doctor_id = 1;

-- 测试查询医生ID为3的医生
SELECT * FROM doctors WHERE doctor_id = 3;

-- 测试查询不存在的医生
SELECT * FROM doctors WHERE doctor_id = 999;

-- =====================================================
-- 3.3 Patients Table Interface
-- =====================================================
-- Function: Retrieve patient details
-- Query: SELECT * FROM patients WHERE patient_id = ?;

SELECT '=== 3.3 Patients Table Interface Tests ===' AS test_section;

-- 测试查询患者ID为1的患者
SELECT * FROM patients WHERE patient_id = 1;

-- 测试查询患者ID为5的患者
SELECT * FROM patients WHERE patient_id = 5;

-- 测试查询不存在的患者
SELECT * FROM patients WHERE patient_id = 999;

-- =====================================================
-- 3.4 Cases Table Interface
-- =====================================================
-- Function: Retrieve case details
-- Query: SELECT * FROM cases WHERE case_id = ?;

SELECT '=== 3.4 Cases Table Interface Tests ===' AS test_section;

-- 测试查询病例ID为1的病例
SELECT * FROM cases WHERE case_id = 1;

-- 测试查询病例ID为5的病例
SELECT * FROM cases WHERE case_id = 5;

-- 测试查询不存在的病例
SELECT * FROM cases WHERE case_id = 999;

-- =====================================================
-- 3.5 Appointments Table Interface
-- =====================================================
-- Function: Retrieve appointment records
-- Query: SELECT * FROM appointments WHERE patient_id = ?;

SELECT '=== 3.5 Appointments Table Interface Tests ===' AS test_section;

-- 测试查询患者ID为1的预约记录
SELECT * FROM appointments WHERE patient_id = 1;

-- 测试查询患者ID为2的预约记录
SELECT * FROM appointments WHERE patient_id = 2;

-- 测试查询没有预约的患者
SELECT * FROM appointments WHERE patient_id = 999;

-- =====================================================
-- 3.6 Hospitalization Table Interface
-- =====================================================
-- Function: Retrieve hospitalization records
-- Query: SELECT * FROM hospitalization WHERE patient_id = ?;

SELECT '=== 3.6 Hospitalization Table Interface Tests ===' AS test_section;

-- 测试查询患者ID为1的住院记录
SELECT * FROM hospitalization WHERE patient_id = 1;

-- 测试查询患者ID为3的住院记录
SELECT * FROM hospitalization WHERE patient_id = 3;

-- 测试查询没有住院记录的患者
SELECT * FROM hospitalization WHERE patient_id = 2;

-- =====================================================
-- 3.7 Prescriptions Table Interface
-- =====================================================
-- Function: Retrieve prescription details
-- Query: SELECT * FROM prescriptions WHERE prescription_id = ?;

SELECT '=== 3.7 Prescriptions Table Interface Tests ===' AS test_section;

-- 测试查询处方ID为1的处方
SELECT * FROM prescriptions WHERE prescription_id = 1;

-- 测试查询处方ID为5的处方
SELECT * FROM prescriptions WHERE prescription_id = 5;

-- 测试查询不存在的处方
SELECT * FROM prescriptions WHERE prescription_id = 999;

-- =====================================================
-- 3.8 Medications Table Interface
-- =====================================================
-- Function: Retrieve medication information
-- Query: SELECT * FROM medications WHERE medication_id = ?;

SELECT '=== 3.8 Medications Table Interface Tests ===' AS test_section;

-- 测试查询药物ID为1的药物
SELECT * FROM medications WHERE medication_id = 1;

-- 测试查询药物ID为10的药物
SELECT * FROM medications WHERE medication_id = 10;

-- 测试查询不存在的药物
SELECT * FROM medications WHERE medication_id = 999;

-- =====================================================
-- 扩展测试查询（验证表关系）
-- =====================================================

SELECT '=== 表关系验证测试 ===' AS test_section;

-- 验证用户-医生关系
SELECT '用户-医生关系验证' AS test_name;
SELECT 
    u.user_id,
    u.username,
    u.user_type,
    d.doctor_id,
    d.name AS doctor_name,
    d.department
FROM users u
JOIN doctors d ON u.user_id = d.user_id
WHERE u.user_type = 'Doctor';

-- 验证用户-患者关系
SELECT '用户-患者关系验证' AS test_name;
SELECT 
    u.user_id,
    u.username,
    u.user_type,
    p.patient_id,
    p.name AS patient_name,
    p.id_number
FROM users u
JOIN patients p ON u.user_id = p.user_id
WHERE u.user_type = 'Patient';

-- 验证患者-病例关系
SELECT '患者-病例关系验证' AS test_name;
SELECT 
    p.patient_id,
    p.name AS patient_name,
    c.case_id,
    c.diagnosis,
    c.diagnosis_date
FROM patients p
JOIN cases c ON p.patient_id = c.patient_id
ORDER BY c.diagnosis_date DESC;

-- 验证医生-病例关系
SELECT '医生-病例关系验证' AS test_name;
SELECT 
    d.doctor_id,
    d.name AS doctor_name,
    d.department,
    c.case_id,
    c.diagnosis,
    c.diagnosis_date
FROM doctors d
JOIN cases c ON d.doctor_id = c.doctor_id
ORDER BY c.diagnosis_date DESC;

-- 验证病例-处方关系
SELECT '病例-处方关系验证' AS test_name;
SELECT 
    c.case_id,
    c.diagnosis,
    pr.prescription_id,
    pr.prescription_content,
    pr.issued_date
FROM cases c
JOIN prescriptions pr ON c.case_id = pr.case_id
ORDER BY pr.issued_date DESC;

-- 验证处方-药物关系
SELECT '处方-药物关系验证' AS test_name;
SELECT 
    pr.prescription_id,
    pr.prescription_content,
    m.medication_id,
    m.medication_name,
    m.quantity,
    m.usage_instructions
FROM prescriptions pr
JOIN medications m ON pr.prescription_id = m.prescription_id
ORDER BY pr.issued_date DESC;

-- =====================================================
-- 业务场景测试查询
-- =====================================================

SELECT '=== 业务场景测试 ===' AS test_section;

-- 场景1：查看特定患者的完整医疗记录
SELECT '场景1：患者完整医疗记录' AS scenario;
SELECT 
    p.name AS patient_name,
    p.id_number,
    c.diagnosis,
    c.diagnosis_date,
    d.name AS doctor_name,
    c.department,
    pr.prescription_content,
    m.medication_name,
    m.quantity
FROM patients p
LEFT JOIN cases c ON p.patient_id = c.patient_id
LEFT JOIN doctors d ON c.doctor_id = d.doctor_id
LEFT JOIN prescriptions pr ON c.case_id = pr.case_id
LEFT JOIN medications m ON pr.prescription_id = m.prescription_id
WHERE p.patient_id = 1
ORDER BY c.diagnosis_date DESC;

-- 场景2：查看特定医生的工作安排
SELECT '场景2：医生工作安排' AS scenario;
SELECT 
    d.name AS doctor_name,
    d.department,
    a.appointment_time,
    p.name AS patient_name,
    a.status
FROM doctors d
JOIN appointments a ON d.doctor_id = a.doctor_id
JOIN patients p ON a.patient_id = p.patient_id
WHERE d.doctor_id = 1
ORDER BY a.appointment_time;

-- 场景3：查看特定科室的所有活动
SELECT '场景3：科室活动统计' AS scenario;
SELECT 
    'Cardiology' AS department,
    COUNT(DISTINCT d.doctor_id) AS doctor_count,
    COUNT(DISTINCT c.case_id) AS case_count,
    COUNT(DISTINCT a.appointment_id) AS appointment_count
FROM doctors d
LEFT JOIN cases c ON d.doctor_id = c.doctor_id AND c.department = 'Cardiology'
LEFT JOIN appointments a ON d.doctor_id = a.doctor_id AND a.department = 'Cardiology'
WHERE d.department = 'Cardiology';

-- 场景4：查看当前住院患者状态
SELECT '场景4：当前住院患者' AS scenario;
SELECT 
    p.name AS patient_name,
    p.id_number,
    h.ward_number,
    h.bed_number,
    h.admission_date,
    h.attending_doctor,
    DATEDIFF(NOW(), h.admission_date) AS days_hospitalized
FROM hospitalization h
JOIN patients p ON h.patient_id = p.patient_id
ORDER BY h.admission_date DESC;

-- =====================================================
-- 数据统计报告
-- =====================================================

SELECT '=== 系统统计报告 ===' AS test_section;

-- 总体统计
SELECT 
    (SELECT COUNT(*) FROM users) AS total_users,
    (SELECT COUNT(*) FROM doctors) AS total_doctors,
    (SELECT COUNT(*) FROM patients) AS total_patients,
    (SELECT COUNT(*) FROM cases) AS total_cases,
    (SELECT COUNT(*) FROM appointments) AS total_appointments,
    (SELECT COUNT(*) FROM hospitalization) AS total_hospitalizations,
    (SELECT COUNT(*) FROM prescriptions) AS total_prescriptions,
    (SELECT COUNT(*) FROM medications) AS total_medications;

-- 按科室统计
SELECT 
    department,
    COUNT(DISTINCT doctor_id) AS doctor_count,
    COUNT(DISTINCT case_id) AS case_count
FROM (
    SELECT d.department, d.doctor_id, c.case_id
    FROM doctors d
    LEFT JOIN cases c ON d.doctor_id = c.doctor_id
) AS dept_stats
GROUP BY department
ORDER BY case_count DESC;

-- 预约状态统计
SELECT 
    status,
    COUNT(*) AS count,
    ROUND(COUNT(*) * 100.0 / (SELECT COUNT(*) FROM appointments), 2) AS percentage
FROM appointments
GROUP BY status;

SELECT '=== 测试完成 ===' AS completion_message;
SELECT '所有表和数据已成功创建，可以开始接口测试！' AS final_message;