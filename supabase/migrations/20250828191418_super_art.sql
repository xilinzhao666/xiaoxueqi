-- =====================================================
-- 前端UI专用纯净数据库脚本
-- 确保数据数量与预期完全一致
-- =====================================================

-- 创建数据库
CREATE DATABASE IF NOT EXISTS hospital_db 
CHARACTER SET utf8mb4 
COLLATE utf8mb4_unicode_ci;

USE hospital_db;

-- =====================================================
-- 删除所有现有表（清理环境）
-- =====================================================
SET FOREIGN_KEY_CHECKS = 0;
DROP TABLE IF EXISTS medications;
DROP TABLE IF EXISTS prescriptions;
DROP TABLE IF EXISTS hospitalization;
DROP TABLE IF EXISTS appointments;
DROP TABLE IF EXISTS cases;
DROP TABLE IF EXISTS patients;
DROP TABLE IF EXISTS doctors;
DROP TABLE IF EXISTS users;
SET FOREIGN_KEY_CHECKS = 1;

-- =====================================================
-- 1. 建表语句
-- =====================================================

-- 1. Users Table
CREATE TABLE users (
    user_id INT AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(50) UNIQUE NOT NULL,
    password VARCHAR(255) NOT NULL,
    user_type ENUM('Doctor', 'Patient') NOT NULL DEFAULT 'Patient',
    email VARCHAR(100) UNIQUE,
    phone_number VARCHAR(20),
    created_at DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    
    INDEX idx_username (username),
    INDEX idx_email (email),
    INDEX idx_user_type (user_type)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 2. Doctors Table
CREATE TABLE doctors (
    doctor_id INT AUTO_INCREMENT PRIMARY KEY,
    user_id INT NOT NULL,
    name VARCHAR(100) NOT NULL,
    department VARCHAR(50) NOT NULL,
    title VARCHAR(50),
    working_hours VARCHAR(100) NOT NULL,
    profile_picture VARCHAR(255),
    
    FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE CASCADE,
    INDEX idx_user_id (user_id),
    INDEX idx_department (department),
    INDEX idx_name (name)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 3. Patients Table
CREATE TABLE patients (
    patient_id INT AUTO_INCREMENT PRIMARY KEY,
    user_id INT NOT NULL,
    name VARCHAR(100) NOT NULL,
    gender ENUM('Male', 'Female') NOT NULL DEFAULT 'Male',
    birth_date DATE NOT NULL,
    id_number VARCHAR(20) UNIQUE NOT NULL,
    phone_number VARCHAR(20),
    
    FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE CASCADE,
    INDEX idx_user_id (user_id),
    INDEX idx_id_number (id_number),
    INDEX idx_name (name)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 4. Cases Table
CREATE TABLE cases (
    case_id INT AUTO_INCREMENT PRIMARY KEY,
    patient_id INT NOT NULL,
    department VARCHAR(50) NOT NULL,
    doctor_id INT NOT NULL,
    diagnosis TEXT NOT NULL,
    diagnosis_date DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    
    FOREIGN KEY (patient_id) REFERENCES patients(patient_id) ON DELETE CASCADE,
    FOREIGN KEY (doctor_id) REFERENCES doctors(doctor_id) ON DELETE CASCADE,
    INDEX idx_patient_id (patient_id),
    INDEX idx_doctor_id (doctor_id),
    INDEX idx_department (department)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 5. Appointments Table
CREATE TABLE appointments (
    appointment_id INT AUTO_INCREMENT PRIMARY KEY,
    patient_id INT NOT NULL,
    doctor_id INT NOT NULL,
    appointment_time DATETIME NOT NULL,
    department VARCHAR(50) NOT NULL,
    status ENUM('Booked', 'Attended', 'Cancelled') NOT NULL DEFAULT 'Booked',
    
    FOREIGN KEY (patient_id) REFERENCES patients(patient_id) ON DELETE CASCADE,
    FOREIGN KEY (doctor_id) REFERENCES doctors(doctor_id) ON DELETE CASCADE,
    INDEX idx_patient_id (patient_id),
    INDEX idx_doctor_id (doctor_id),
    INDEX idx_appointment_time (appointment_time)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 6. Hospitalization Table
CREATE TABLE hospitalization (
    hospitalization_id INT AUTO_INCREMENT PRIMARY KEY,
    patient_id INT NOT NULL,
    ward_number VARCHAR(20) NOT NULL,
    bed_number VARCHAR(20) NOT NULL,
    admission_date DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    attending_doctor VARCHAR(100) NOT NULL,
    
    FOREIGN KEY (patient_id) REFERENCES patients(patient_id) ON DELETE CASCADE,
    INDEX idx_patient_id (patient_id),
    INDEX idx_ward_number (ward_number)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 7. Prescriptions Table
CREATE TABLE prescriptions (
    prescription_id INT AUTO_INCREMENT PRIMARY KEY,
    case_id INT NOT NULL,
    doctor_id INT NOT NULL,
    prescription_content TEXT NOT NULL,
    issued_date DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    
    FOREIGN KEY (case_id) REFERENCES cases(case_id) ON DELETE CASCADE,
    FOREIGN KEY (doctor_id) REFERENCES doctors(doctor_id) ON DELETE CASCADE,
    INDEX idx_case_id (case_id),
    INDEX idx_doctor_id (doctor_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 8. Medications Table
CREATE TABLE medications (
    medication_id INT AUTO_INCREMENT PRIMARY KEY,
    prescription_id INT NOT NULL,
    medication_name VARCHAR(100) NOT NULL,
    quantity INT NOT NULL,
    usage_instructions TEXT NOT NULL,
    
    FOREIGN KEY (prescription_id) REFERENCES prescriptions(prescription_id) ON DELETE CASCADE,
    INDEX idx_prescription_id (prescription_id),
    INDEX idx_medication_name (medication_name)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- =====================================================
-- 2. 插入精确的测试数据（严格按照预期数量）
-- =====================================================

-- 插入用户数据（13个用户：5个医生 + 8个患者）
INSERT INTO users (username, password, user_type, email, phone_number) VALUES
-- 5个医生用户
('dr_zhang', SHA2('password123', 256), 'Doctor', 'zhang@hospital.com', '13800138001'),
('dr_li', SHA2('password123', 256), 'Doctor', 'li@hospital.com', '13800138002'),
('dr_wang', SHA2('password123', 256), 'Doctor', 'wang@hospital.com', '13800138003'),
('dr_chen', SHA2('password123', 256), 'Doctor', 'chen@hospital.com', '13800138004'),
('dr_liu', SHA2('password123', 256), 'Doctor', 'liu@hospital.com', '13800138005'),

-- 8个患者用户
('patient_001', SHA2('password123', 256), 'Patient', 'patient001@email.com', '13900139001'),
('patient_002', SHA2('password123', 256), 'Patient', 'patient002@email.com', '13900139002'),
('patient_003', SHA2('password123', 256), 'Patient', 'patient003@email.com', '13900139003'),
('patient_004', SHA2('password123', 256), 'Patient', 'patient004@email.com', '13900139004'),
('patient_005', SHA2('password123', 256), 'Patient', 'patient005@email.com', '13900139005'),
('patient_006', SHA2('password123', 256), 'Patient', 'patient006@email.com', '13900139006'),
('patient_007', SHA2('password123', 256), 'Patient', 'patient007@email.com', '13900139007'),
('patient_008', SHA2('password123', 256), 'Patient', 'patient008@email.com', '13900139008');

-- 插入医生信息（5个医生：心内科、神经科、骨科、儿科、内科）
INSERT INTO doctors (user_id, name, department, title, working_hours) VALUES
(1, '张医生', 'Cardiology', '主任医师', '周一至周五 8:00-17:00'),
(2, '李医生', 'Neurology', '副主任医师', '周一至周五 9:00-18:00'),
(3, '王医生', 'Orthopedics', '主治医师', '周二至周六 8:30-17:30'),
(4, '陈医生', 'Pediatrics', '主任医师', '周一至周五 8:00-16:00'),
(5, '刘医生', 'Internal Medicine', '副主任医师', '周一至周日 8:00-20:00');

-- 插入患者信息（8个患者：不同性别、年龄）
INSERT INTO patients (user_id, name, gender, birth_date, id_number, phone_number) VALUES
(6, '张三', 'Male', '1985-03-15', '110101198503150001', '13900139001'),
(7, '李四', 'Female', '1990-07-22', '110101199007220002', '13900139002'),
(8, '王五', 'Male', '1978-11-08', '110101197811080003', '13900139003'),
(9, '赵六', 'Female', '1995-05-12', '110101199505120004', '13900139004'),
(10, '钱七', 'Male', '1982-09-30', '110101198209300005', '13900139005'),
(11, '孙八', 'Female', '1988-12-25', '110101198812250006', '13900139006'),
(12, '周九', 'Male', '1992-04-18', '110101199204180007', '13900139007'),
(13, '吴十', 'Female', '1987-08-03', '110101198708030008', '13900139008');

-- 插入病例数据（8个病例：各科室典型病例）
INSERT INTO cases (patient_id, department, doctor_id, diagnosis, diagnosis_date) VALUES
(1, 'Cardiology', 1, '高血压，建议定期监测血压，调整生活方式', '2024-01-15 10:30:00'),
(2, 'Neurology', 2, '偏头痛，开具止痛药物，建议充足休息', '2024-01-16 14:20:00'),
(3, 'Orthopedics', 3, '腰椎间盘突出，建议物理治疗和康复训练', '2024-01-17 09:15:00'),
(4, 'Pediatrics', 4, '儿童感冒，开具退烧药和抗生素', '2024-01-18 11:45:00'),
(5, 'Internal Medicine', 5, '胃炎，建议饮食调理和药物治疗', '2024-01-19 16:30:00'),
(6, 'Cardiology', 1, '心律不齐，需要进一步检查和治疗', '2024-01-20 08:45:00'),
(7, 'Neurology', 2, '失眠症，开具安眠药物，建议心理咨询', '2024-01-21 13:20:00'),
(8, 'Orthopedics', 3, '肩周炎，建议物理治疗和功能锻炼', '2024-01-22 15:10:00');

-- 插入预约数据（10个预约记录：不同状态）
INSERT INTO appointments (patient_id, doctor_id, appointment_time, department, status) VALUES
(1, 1, '2024-02-01 09:00:00', 'Cardiology', 'Booked'),
(2, 2, '2024-02-01 10:30:00', 'Neurology', 'Booked'),
(3, 3, '2024-02-01 14:00:00', 'Orthopedics', 'Attended'),
(4, 4, '2024-02-02 09:30:00', 'Pediatrics', 'Booked'),
(5, 5, '2024-02-02 11:00:00', 'Internal Medicine', 'Attended'),
(6, 1, '2024-02-03 08:30:00', 'Cardiology', 'Cancelled'),
(7, 2, '2024-02-03 15:30:00', 'Neurology', 'Booked'),
(8, 3, '2024-02-04 10:00:00', 'Orthopedics', 'Booked'),
(1, 5, '2024-02-05 16:00:00', 'Internal Medicine', 'Booked'),
(2, 4, '2024-02-06 09:15:00', 'Pediatrics', 'Attended');

-- 插入住院数据（5个住院记录：不同病房）
INSERT INTO hospitalization (patient_id, ward_number, bed_number, admission_date, attending_doctor) VALUES
(1, 'A101', '01', '2024-01-25 08:00:00', '张医生'),
(3, 'B205', '03', '2024-01-26 10:30:00', '王医生'),
(5, 'C301', '05', '2024-01-27 14:20:00', '刘医生'),
(6, 'A102', '02', '2024-01-28 09:45:00', '张医生'),
(8, 'B206', '04', '2024-01-29 11:15:00', '王医生');

-- 插入处方数据（8个处方：对应病例）
INSERT INTO prescriptions (case_id, doctor_id, prescription_content, issued_date) VALUES
(1, 1, '氨氯地平片 5mg，每日一次，餐后服用；阿司匹林肠溶片 100mg，每日一次', '2024-01-15 10:45:00'),
(2, 2, '布洛芬缓释胶囊 300mg，每日两次，疼痛时服用；维生素B1片，每日三次', '2024-01-16 14:35:00'),
(3, 3, '双氯芬酸钠缓释片 75mg，每日两次；甲钴胺片 0.5mg，每日三次', '2024-01-17 09:30:00'),
(4, 4, '小儿退热栓，发热时使用；阿莫西林颗粒，每日三次，餐后服用', '2024-01-18 12:00:00'),
(5, 5, '奥美拉唑肠溶胶囊 20mg，每日两次，餐前服用；铝碳酸镁片，每日三次', '2024-01-19 16:45:00'),
(6, 1, '美托洛尔缓释片 47.5mg，每日一次；单硝酸异山梨酯缓释片 60mg，每日一次', '2024-01-20 09:00:00'),
(7, 2, '艾司唑仑片 1mg，睡前服用；谷维素片 10mg，每日三次', '2024-01-21 13:35:00'),
(8, 3, '塞来昔布胶囊 200mg，每日两次；氨基葡萄糖硫酸盐胶囊，每日三次', '2024-01-22 15:25:00');

-- 插入药物数据（16个药物记录：详细用药说明）
INSERT INTO medications (prescription_id, medication_name, quantity, usage_instructions) VALUES
-- 处方1的药物（2个）
(1, '氨氯地平片', 30, '每日一次，每次5mg，餐后服用，用温水送服'),
(1, '阿司匹林肠溶片', 30, '每日一次，每次100mg，餐后服用，不可咀嚼'),

-- 处方2的药物（2个）
(2, '布洛芬缓释胶囊', 20, '每日两次，每次300mg，疼痛时服用，餐后服用'),
(2, '维生素B1片', 90, '每日三次，每次10mg，餐后服用'),

-- 处方3的药物（2个）
(3, '双氯芬酸钠缓释片', 60, '每日两次，每次75mg，餐后服用，用温水送服'),
(3, '甲钴胺片', 90, '每日三次，每次0.5mg，餐后服用'),

-- 处方4的药物（2个）
(4, '小儿退热栓', 6, '发热时使用，体温超过38.5°C时使用，每次1粒'),
(4, '阿莫西林颗粒', 21, '每日三次，每次125mg，餐后服用，用温水冲服'),

-- 处方5的药物（2个）
(5, '奥美拉唑肠溶胶囊', 60, '每日两次，每次20mg，餐前30分钟服用'),
(5, '铝碳酸镁片', 90, '每日三次，每次2片，餐后1小时服用'),

-- 处方6的药物（2个）
(6, '美托洛尔缓释片', 30, '每日一次，每次47.5mg，早晨服用，不可咀嚼'),
(6, '单硝酸异山梨酯缓释片', 30, '每日一次，每次60mg，早晨空腹服用'),

-- 处方7的药物（2个）
(7, '艾司唑仑片', 14, '睡前服用，每次1mg，不可长期使用'),
(7, '谷维素片', 90, '每日三次，每次10mg，餐后服用'),

-- 处方8的药物（2个）
(8, '塞来昔布胶囊', 60, '每日两次，每次200mg，餐后服用'),
(8, '氨基葡萄糖硫酸盐胶囊', 90, '每日三次，每次314mg，餐后服用');

-- =====================================================
-- 3. 数据验证
-- =====================================================

SELECT '=== 前端UI专用数据库创建完成 ===' AS status;

-- 验证数据数量（应该与预期完全一致）
SELECT 
    '数据验证' AS verification_type,
    (SELECT COUNT(*) FROM users) AS total_users,
    (SELECT COUNT(*) FROM users WHERE user_type = 'Doctor') AS total_doctors,
    (SELECT COUNT(*) FROM users WHERE user_type = 'Patient') AS total_patients,
    (SELECT COUNT(*) FROM cases) AS total_cases,
    (SELECT COUNT(*) FROM appointments) AS total_appointments,
    (SELECT COUNT(*) FROM hospitalization) AS total_hospitalizations,
    (SELECT COUNT(*) FROM prescriptions) AS total_prescriptions,
    (SELECT COUNT(*) FROM medications) AS total_medications;

-- 预期结果验证
SELECT 
    CASE 
        WHEN (SELECT COUNT(*) FROM users) = 13 THEN '✓ 用户数量正确 (13)'
        ELSE '✗ 用户数量错误'
    END AS users_check,
    CASE 
        WHEN (SELECT COUNT(*) FROM users WHERE user_type = 'Doctor') = 5 THEN '✓ 医生数量正确 (5)'
        ELSE '✗ 医生数量错误'
    END AS doctors_check,
    CASE 
        WHEN (SELECT COUNT(*) FROM users WHERE user_type = 'Patient') = 8 THEN '✓ 患者数量正确 (8)'
        ELSE '✗ 患者数量错误'
    END AS patients_check,
    CASE 
        WHEN (SELECT COUNT(*) FROM cases) = 8 THEN '✓ 病例数量正确 (8)'
        ELSE '✗ 病例数量错误'
    END AS cases_check,
    CASE 
        WHEN (SELECT COUNT(*) FROM appointments) = 10 THEN '✓ 预约数量正确 (10)'
        ELSE '✗ 预约数量错误'
    END AS appointments_check,
    CASE 
        WHEN (SELECT COUNT(*) FROM hospitalization) = 5 THEN '✓ 住院数量正确 (5)'
        ELSE '✗ 住院数量错误'
    END AS hospitalization_check,
    CASE 
        WHEN (SELECT COUNT(*) FROM prescriptions) = 8 THEN '✓ 处方数量正确 (8)'
        ELSE '✗ 处方数量错误'
    END AS prescriptions_check,
    CASE 
        WHEN (SELECT COUNT(*) FROM medications) = 16 THEN '✓ 药物数量正确 (16)'
        ELSE '✗ 药物数量错误'
    END AS medications_check;

SELECT '前端UI专用数据库设置完成！' AS final_message;
SELECT '数据数量严格按照预期：13用户(5医生+8患者)，8病例，10预约，5住院，8处方，16药物' AS data_summary;