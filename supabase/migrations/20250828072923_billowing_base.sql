-- =====================================================
-- 医院管理系统快速设置脚本
-- 用于快速搭建测试环境
-- =====================================================

-- 创建数据库
CREATE DATABASE IF NOT EXISTS hospital_db 
CHARACTER SET utf8mb4 
COLLATE utf8mb4_unicode_ci;

USE hospital_db;

-- 删除已存在的表（按依赖关系顺序）
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

-- 快速建表
CREATE TABLE users (
    user_id INT AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(50) UNIQUE NOT NULL,
    password VARCHAR(255) NOT NULL,
    user_type ENUM('Doctor', 'Patient') NOT NULL DEFAULT 'Patient',
    email VARCHAR(100) UNIQUE,
    phone_number VARCHAR(20),
    created_at DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE doctors (
    doctor_id INT AUTO_INCREMENT PRIMARY KEY,
    user_id INT NOT NULL,
    name VARCHAR(100) NOT NULL,
    department VARCHAR(50) NOT NULL,
    title VARCHAR(50),
    working_hours VARCHAR(100) NOT NULL,
    profile_picture VARCHAR(255),
    FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE patients (
    patient_id INT AUTO_INCREMENT PRIMARY KEY,
    user_id INT NOT NULL,
    name VARCHAR(100) NOT NULL,
    gender ENUM('Male', 'Female') NOT NULL DEFAULT 'Male',
    birth_date DATE NOT NULL,
    id_number VARCHAR(20) UNIQUE NOT NULL,
    phone_number VARCHAR(20),
    FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE cases (
    case_id INT AUTO_INCREMENT PRIMARY KEY,
    patient_id INT NOT NULL,
    department VARCHAR(50) NOT NULL,
    doctor_id INT NOT NULL,
    diagnosis TEXT NOT NULL,
    diagnosis_date DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (patient_id) REFERENCES patients(patient_id) ON DELETE CASCADE,
    FOREIGN KEY (doctor_id) REFERENCES doctors(doctor_id) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE appointments (
    appointment_id INT AUTO_INCREMENT PRIMARY KEY,
    patient_id INT NOT NULL,
    doctor_id INT NOT NULL,
    appointment_time DATETIME NOT NULL,
    department VARCHAR(50) NOT NULL,
    status ENUM('Booked', 'Attended', 'Cancelled') NOT NULL DEFAULT 'Booked',
    FOREIGN KEY (patient_id) REFERENCES patients(patient_id) ON DELETE CASCADE,
    FOREIGN KEY (doctor_id) REFERENCES doctors(doctor_id) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE hospitalization (
    hospitalization_id INT AUTO_INCREMENT PRIMARY KEY,
    patient_id INT NOT NULL,
    ward_number VARCHAR(20) NOT NULL,
    bed_number VARCHAR(20) NOT NULL,
    admission_date DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    attending_doctor VARCHAR(100) NOT NULL,
    FOREIGN KEY (patient_id) REFERENCES patients(patient_id) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE prescriptions (
    prescription_id INT AUTO_INCREMENT PRIMARY KEY,
    case_id INT NOT NULL,
    doctor_id INT NOT NULL,
    prescription_content TEXT NOT NULL,
    issued_date DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (case_id) REFERENCES cases(case_id) ON DELETE CASCADE,
    FOREIGN KEY (doctor_id) REFERENCES doctors(doctor_id) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE medications (
    medication_id INT AUTO_INCREMENT PRIMARY KEY,
    prescription_id INT NOT NULL,
    medication_name VARCHAR(100) NOT NULL,
    quantity INT NOT NULL,
    usage_instructions TEXT NOT NULL,
    FOREIGN KEY (prescription_id) REFERENCES prescriptions(prescription_id) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 快速插入基础测试数据
INSERT INTO users VALUES
(1, 'dr_zhang', SHA2('password123', 256), 'Doctor', 'zhang@hospital.com', '13800138001', NOW()),
(2, 'patient_zhang', SHA2('password123', 256), 'Patient', 'patient@email.com', '13900139001', NOW());

INSERT INTO doctors VALUES
(1, 1, '张医生', 'Cardiology', '主任医师', '周一至周五 8:00-17:00', NULL);

INSERT INTO patients VALUES
(1, 2, '张三', 'Male', '1985-03-15', '110101198503150001', '13900139001');

INSERT INTO cases VALUES
(1, 1, 'Cardiology', 1, '高血压诊断', NOW());

INSERT INTO appointments VALUES
(1, 1, 1, '2024-02-01 09:00:00', 'Cardiology', 'Booked');

INSERT INTO prescriptions VALUES
(1, 1, 1, '氨氯地平片，每日一次', NOW());

INSERT INTO medications VALUES
(1, 1, '氨氯地平片', 30, '每日一次，餐后服用');

SELECT '快速设置完成！' AS status;
SELECT '基础测试数据已插入，可以开始测试！' AS message;