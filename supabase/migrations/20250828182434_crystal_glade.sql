@@ .. @@
 -- 插入用户数据
 INSERT INTO users (username, password, user_type, email, phone_number) VALUES
 -- 医生用户
 ('dr_zhang', SHA2('password123', 256), 'Doctor', 'zhang@hospital.com', '13800138001'),
 ('dr_li', SHA2('password123', 256), 'Doctor', 'li@hospital.com', '13800138002'),
 ('dr_wang', SHA2('password123', 256), 'Doctor', 'wang@hospital.com', '13800138003'),
 ('dr_chen', SHA2('password123', 256), 'Doctor', 'chen@hospital.com', '13800138004'),
 ('dr_liu', SHA2('password123', 256), 'Doctor', 'liu@hospital.com', '13800138005'),
+('dr_new', SHA2('newpassword123', 256), 'Doctor', 'dr_new@hospital.com', '13800138999'),
 
 -- 患者用户
 ('patient_001', SHA2('password123', 256), 'Patient', 'patient001@email.com', '13900139001'),
 ('patient_002', SHA2('password123', 256), 'Patient', 'patient002@email.com', '13900139002'),
 ('patient_003', SHA2('password123', 256), 'Patient', 'patient003@email.com', '13900139003'),
 ('patient_004', SHA2('password123', 256), 'Patient', 'patient004@email.com', '13900139004'),
 ('patient_005', SHA2('password123', 256), 'Patient', 'patient005@email.com', '13900139005'),
 ('patient_006', SHA2('password123', 256), 'Patient', 'patient006@email.com', '13900139006'),
 ('patient_007', SHA2('password123', 256), 'Patient', 'patient007@email.com', '13900139007'),
-('patient_008', SHA2('password123', 256), 'Patient', 'patient008@email.com', '13900139008');
+('patient_008', SHA2('password123', 256), 'Patient', 'patient008@email.com', '13900139008'),
+('patient_new', SHA2('newpassword123', 256), 'Patient', 'patient_new@email.com', '13900139999');