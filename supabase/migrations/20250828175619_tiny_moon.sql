-- =====================================================
-- 医院管理系统API接口测试示例
-- 模拟C++程序中的数据库操作
-- =====================================================

USE hospital_db;

-- =====================================================
-- 用户管理接口测试
-- =====================================================

SELECT '=== 用户管理接口测试 ===' AS section;

-- 1. 用户注册接口测试
SELECT '1. 用户注册接口' AS api_name;

-- 注册新医生用户
INSERT INTO users (username, password, user_type, email, phone_number) 
VALUES ('dr_new', SHA2('newpassword123', 256), 'Doctor', 'dr_new@hospital.com', '13800138999');

-- 获取新注册用户的ID
SET @new_doctor_user_id = LAST_INSERT_ID();
SELECT @new_doctor_user_id AS new_user_id;

-- 注册新患者用户
INSERT INTO users (username, password, user_type, email, phone_number) 
VALUES ('patient_new', SHA2('newpassword123', 256), 'Patient', 'patient_new@email.com', '13900139999');

SET @new_patient_user_id = LAST_INSERT_ID();
SELECT @new_patient_user_id AS new_patient_user_id;

-- 2. 用户认证接口测试
SELECT '2. 用户认证接口' AS api_name;

-- 模拟用户登录验证
SELECT 
    user_id,
    username,
    user_type,
    email,
    phone_number
FROM users 
WHERE username = 'dr_zhang' 
AND password = SHA2('password123', 256);

-- 3. 用户信息查询接口
SELECT '3. 用户信息查询接口' AS api_name;

-- 按用户ID查询（文档3.1接口）
SELECT * FROM users WHERE user_id = 1;

-- 按用户名查询
SELECT * FROM users WHERE username = 'patient_001';

-- 按用户类型查询
SELECT * FROM users WHERE user_type = 'Doctor';

-- =====================================================
-- 医生管理接口测试
-- =====================================================

SELECT '=== 医生管理接口测试 ===' AS section;

-- 1. 医生信息注册
SELECT '1. 医生信息注册' AS api_name;

-- 为新注册的医生用户添加医生信息
INSERT INTO doctors (user_id, name, department, title, working_hours) 
VALUES (@new_doctor_user_id, '新医生', 'Emergency', '住院医师', '周一至周日 24小时');

-- 2. 医生信息查询接口
SELECT '2. 医生信息查询接口' AS api_name;

-- 按医生ID查询（文档3.2接口）
SELECT * FROM doctors WHERE doctor_id = 1;

-- 按科室查询医生
SELECT * FROM doctors WHERE department = 'Cardiology';

-- 按用户ID查询医生信息
SELECT * FROM doctors WHERE user_id = 1;

-- 3. 医生搜索接口
SELECT '3. 医生搜索接口' AS api_name;

-- 按姓名搜索
SELECT * FROM doctors WHERE name LIKE '%张%';

-- 按科室和职称搜索
SELECT * FROM doctors WHERE department = 'Cardiology' AND title = '主任医师';

-- =====================================================
-- 患者管理接口测试
-- =====================================================

SELECT '=== 患者管理接口测试 ===' AS section;

-- 1. 患者信息注册
SELECT '1. 患者信息注册' AS api_name;

-- 为新注册的患者用户添加患者信息
INSERT INTO patients (user_id, name, gender, birth_date, id_number, phone_number) 
VALUES (@new_patient_user_id, '新患者', 'Female', '1995-06-15', '110101199506150099', '13900139999');

-- 2. 患者信息查询接口
SELECT '2. 患者信息查询接口' AS api_name;

-- 按患者ID查询（文档3.3接口）
SELECT * FROM patients WHERE patient_id = 1;

-- 按身份证号查询
SELECT * FROM patients WHERE id_number = '110101198503150001';

-- 按用户ID查询患者信息
SELECT * FROM patients WHERE user_id = 6;

-- 3. 患者搜索接口
SELECT '3. 患者搜索接口' AS api_name;

-- 按姓名搜索
SELECT * FROM patients WHERE name LIKE '%张%';

-- 按性别和年龄范围搜索
SELECT 
    *,
    YEAR(CURDATE()) - YEAR(birth_date) AS age
FROM patients 
WHERE gender = 'Male' 
AND YEAR(CURDATE()) - YEAR(birth_date) BETWEEN 30 AND 50;

-- =====================================================
-- 病例管理接口测试
-- =====================================================

SELECT '=== 病例管理接口测试 ===' AS section;

-- 1. 创建病例接口
SELECT '1. 创建病例接口' AS api_name;

-- 为新患者创建病例
SET @new_patient_id = (SELECT patient_id FROM patients WHERE user_id = @new_patient_user_id);

INSERT INTO cases (patient_id, department, doctor_id, diagnosis) 
VALUES (@new_patient_id, 'Emergency', 1, '急性胃炎，建议清淡饮食，按时服药');

-- 2. 病例查询接口
SELECT '2. 病例查询接口' AS api_name;

-- 按病例ID查询（文档3.4接口）
SELECT * FROM cases WHERE case_id = 1;

-- 按患者ID查询所有病例
SELECT * FROM cases WHERE patient_id = 1 ORDER BY diagnosis_date DESC;

-- 按医生ID查询所有病例
SELECT * FROM cases WHERE doctor_id = 1 ORDER BY diagnosis_date DESC;

-- 按科室查询病例
SELECT * FROM cases WHERE department = 'Cardiology' ORDER BY diagnosis_date DESC;

-- =====================================================
-- 预约管理接口测试
-- =====================================================

SELECT '=== 预约管理接口测试 ===' AS section;

-- 1. 创建预约接口
SELECT '1. 创建预约接口' AS api_name;

-- 为新患者创建预约
INSERT INTO appointments (patient_id, doctor_id, appointment_time, department, status) 
VALUES (@new_patient_id, 2, '2024-02-10 14:30:00', 'Neurology', 'Booked');

-- 2. 预约查询接口
SELECT '2. 预约查询接口' AS api_name;

-- 按患者ID查询预约（文档3.5接口）
SELECT * FROM appointments WHERE patient_id = 1;

-- 按医生ID查询预约
SELECT * FROM appointments WHERE doctor_id = 1 ORDER BY appointment_time;

-- 按状态查询预约
SELECT * FROM appointments WHERE status = 'Booked' ORDER BY appointment_time;

-- 3. 预约状态更新接口
SELECT '3. 预约状态更新接口' AS api_name;

-- 更新预约状态为已就诊
UPDATE appointments SET status = 'Attended' WHERE appointment_id = 3;

-- 验证更新结果
SELECT * FROM appointments WHERE appointment_id = 3;

-- =====================================================
-- 住院管理接口测试
-- =====================================================

SELECT '=== 住院管理接口测试 ===' AS section;

-- 1. 住院登记接口
SELECT '1. 住院登记接口' AS api_name;

-- 为新患者办理住院
INSERT INTO hospitalization (patient_id, ward_number, bed_number, attending_doctor) 
VALUES (@new_patient_id, 'D401', '06', '张医生');

-- 2. 住院记录查询接口
SELECT '2. 住院记录查询接口' AS api_name;

-- 按患者ID查询住院记录（文档3.6接口）
SELECT * FROM hospitalization WHERE patient_id = 1;

-- 按病房查询
SELECT * FROM hospitalization WHERE ward_number = 'A101';

-- 查看所有当前住院患者
SELECT 
    h.*,
    p.name AS patient_name,
    p.id_number
FROM hospitalization h
JOIN patients p ON h.patient_id = p.patient_id
ORDER BY h.admission_date DESC;

-- =====================================================
-- 处方管理接口测试
-- =====================================================

SELECT '=== 处方管理接口测试 ===' AS section;

-- 1. 开具处方接口
SELECT '1. 开具处方接口' AS api_name;

-- 获取新创建的病例ID
SET @new_case_id = (SELECT case_id FROM cases WHERE patient_id = @new_patient_id ORDER BY diagnosis_date DESC LIMIT 1);

-- 为新病例开具处方
INSERT INTO prescriptions (case_id, doctor_id, prescription_content) 
VALUES (@new_case_id, 1, '奥美拉唑肠溶胶囊 20mg，每日两次，餐前服用；蒙脱石散，每日三次，餐后服用');

-- 2. 处方查询接口
SELECT '2. 处方查询接口' AS api_name;

-- 按处方ID查询（文档3.7接口）
SELECT * FROM prescriptions WHERE prescription_id = 1;

-- 按病例ID查询处方
SELECT * FROM prescriptions WHERE case_id = 1;

-- 按医生ID查询处方
SELECT * FROM prescriptions WHERE doctor_id = 1 ORDER BY issued_date DESC;

-- =====================================================
-- 药物管理接口测试
-- =====================================================

SELECT '=== 药物管理接口测试 ===' AS section;

-- 1. 添加药物接口
SELECT '1. 添加药物接口' AS api_name;

-- 获取新创建的处方ID
SET @new_prescription_id = LAST_INSERT_ID();

-- 为新处方添加药物
INSERT INTO medications (prescription_id, medication_name, quantity, usage_instructions) VALUES
(@new_prescription_id, '奥美拉唑肠溶胶囊', 60, '每日两次，每次20mg，餐前30分钟服用，用温水送服'),
(@new_prescription_id, '蒙脱石散', 30, '每日三次，每次3g，餐后服用，用温水冲服');

-- 2. 药物查询接口
SELECT '2. 药物查询接口' AS api_name;

-- 按药物ID查询（文档3.8接口）
SELECT * FROM medications WHERE medication_id = 1;

-- 按处方ID查询所有药物
SELECT * FROM medications WHERE prescription_id = 1;

-- 按药物名称查询
SELECT * FROM medications WHERE medication_name LIKE '%阿司匹林%';

-- =====================================================
-- 综合业务流程测试
-- =====================================================

SELECT '=== 综合业务流程测试 ===' AS section;

-- 完整的就医流程测试
SELECT '完整就医流程测试' AS workflow_test;

-- 1. 患者预约
SELECT '步骤1：患者预约' AS step;
INSERT INTO appointments (patient_id, doctor_id, appointment_time, department, status) 
VALUES (2, 3, '2024-02-15 10:00:00', 'Orthopedics', 'Booked');

SET @test_appointment_id = LAST_INSERT_ID();

-- 2. 患者就诊，创建病例
SELECT '步骤2：患者就诊，创建病例' AS step;
INSERT INTO cases (patient_id, department, doctor_id, diagnosis) 
VALUES (2, 'Orthopedics', 3, '膝关节疼痛，疑似关节炎，建议进一步检查');

SET @test_case_id = LAST_INSERT_ID();

-- 更新预约状态为已就诊
UPDATE appointments SET status = 'Attended' WHERE appointment_id = @test_appointment_id;

-- 3. 医生开具处方
SELECT '步骤3：医生开具处方' AS step;
INSERT INTO prescriptions (case_id, doctor_id, prescription_content) 
VALUES (@test_case_id, 3, '双氯芬酸钠缓释片，消炎止痛；氨基葡萄糖，保护关节软骨');

SET @test_prescription_id = LAST_INSERT_ID();

-- 4. 添加具体药物
SELECT '步骤4：添加具体药物' AS step;
INSERT INTO medications (prescription_id, medication_name, quantity, usage_instructions) VALUES
(@test_prescription_id, '双氯芬酸钠缓释片', 60, '每日两次，每次75mg，餐后服用'),
(@test_prescription_id, '氨基葡萄糖硫酸盐胶囊', 90, '每日三次，每次314mg，餐后服用');

-- 5. 验证完整流程
SELECT '步骤5：验证完整流程' AS step;
SELECT 
    u.username,
    p.name AS patient_name,
    a.appointment_time,
    a.status AS appointment_status,
    c.diagnosis,
    c.diagnosis_date,
    d.name AS doctor_name,
    pr.prescription_content,
    m.medication_name,
    m.quantity,
    m.usage_instructions
FROM users u
JOIN patients p ON u.user_id = p.user_id
JOIN appointments a ON p.patient_id = a.patient_id
JOIN cases c ON p.patient_id = c.patient_id
JOIN doctors d ON c.doctor_id = d.doctor_id
JOIN prescriptions pr ON c.case_id = pr.case_id
JOIN medications m ON pr.prescription_id = m.prescription_id
WHERE a.appointment_id = @test_appointment_id;

-- =====================================================
-- 性能测试查询
-- =====================================================

SELECT '=== 性能测试查询 ===' AS section;

-- 1. 索引效果测试
SELECT '1. 索引效果测试' AS test_name;

-- 测试用户名索引
EXPLAIN SELECT * FROM users WHERE username = 'dr_zhang';

-- 测试身份证号索引
EXPLAIN SELECT * FROM patients WHERE id_number = '110101198503150001';

-- 测试预约时间索引
EXPLAIN SELECT * FROM appointments WHERE appointment_time >= '2024-02-01';

-- 2. 复杂JOIN查询测试
SELECT '2. 复杂JOIN查询测试' AS test_name;

-- 测试多表JOIN性能
EXPLAIN SELECT 
    p.name AS patient_name,
    c.diagnosis,
    d.name AS doctor_name,
    pr.prescription_content,
    m.medication_name
FROM patients p
JOIN cases c ON p.patient_id = c.patient_id
JOIN doctors d ON c.doctor_id = d.doctor_id
JOIN prescriptions pr ON c.case_id = pr.case_id
JOIN medications m ON pr.prescription_id = m.prescription_id
WHERE p.patient_id = 1;

-- =====================================================
-- 数据统计和报告测试
-- =====================================================

SELECT '=== 数据统计和报告测试 ===' AS section;

-- 1. 医院整体统计
SELECT '1. 医院整体统计' AS report_name;
SELECT 
    '医院整体数据统计' AS report_title,
    (SELECT COUNT(*) FROM users WHERE user_type = 'Doctor') AS total_doctors,
    (SELECT COUNT(*) FROM users WHERE user_type = 'Patient') AS total_patients,
    (SELECT COUNT(*) FROM cases) AS total_cases,
    (SELECT COUNT(*) FROM appointments) AS total_appointments,
    (SELECT COUNT(*) FROM prescriptions) AS total_prescriptions,
    (SELECT COUNT(*) FROM medications) AS total_medications;

-- 2. 科室工作量统计
SELECT '2. 科室工作量统计' AS report_name;
SELECT 
    d.department,
    COUNT(DISTINCT d.doctor_id) AS doctor_count,
    COUNT(DISTINCT c.case_id) AS case_count,
    COUNT(DISTINCT a.appointment_id) AS appointment_count
FROM doctors d
LEFT JOIN cases c ON d.doctor_id = c.doctor_id
LEFT JOIN appointments a ON d.doctor_id = a.doctor_id
GROUP BY d.department
ORDER BY case_count DESC;

-- 3. 医生工作量排名
SELECT '3. 医生工作量排名' AS report_name;
SELECT 
    d.name AS doctor_name,
    d.department,
    COUNT(DISTINCT c.case_id) AS case_count,
    COUNT(DISTINCT a.appointment_id) AS appointment_count,
    COUNT(DISTINCT pr.prescription_id) AS prescription_count
FROM doctors d
LEFT JOIN cases c ON d.doctor_id = c.doctor_id
LEFT JOIN appointments a ON d.doctor_id = a.doctor_id
LEFT JOIN prescriptions pr ON d.doctor_id = pr.doctor_id
GROUP BY d.doctor_id, d.name, d.department
ORDER BY case_count DESC;

-- 4. 患者就诊频率统计
SELECT '4. 患者就诊频率统计' AS report_name;
SELECT 
    p.name AS patient_name,
    p.id_number,
    COUNT(DISTINCT c.case_id) AS case_count,
    COUNT(DISTINCT a.appointment_id) AS appointment_count,
    MAX(c.diagnosis_date) AS last_visit_date
FROM patients p
LEFT JOIN cases c ON p.patient_id = c.patient_id
LEFT JOIN appointments a ON p.patient_id = a.patient_id
GROUP BY p.patient_id, p.name, p.id_number
HAVING case_count > 0
ORDER BY case_count DESC;

SELECT '=== API接口测试完成 ===' AS completion;
SELECT '所有接口测试已完成，可以开始C++程序开发！' AS final_message;