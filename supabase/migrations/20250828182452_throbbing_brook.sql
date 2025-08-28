@@ .. @@
 -- 1. 医院整体统计
 SELECT '1. 医院整体统计' AS report_name;
 SELECT 
     '医院整体数据统计' AS report_title,
-    (SELECT COUNT(*) FROM users WHERE user_type = 'Doctor') AS total_doctors,
-    (SELECT COUNT(*) FROM users WHERE user_type = 'Patient') AS total_patients,
+    (SELECT COUNT(*) FROM users WHERE user_type = 'Doctor') AS total_doctors, -- 应该是6个
+    (SELECT COUNT(*) FROM users WHERE user_type = 'Patient') AS total_patients, -- 应该是9个
     (SELECT COUNT(*) FROM cases) AS total_cases,
     (SELECT COUNT(*) FROM appointments) AS total_appointments,
     (SELECT COUNT(*) FROM prescriptions) AS total_prescriptions,
     (SELECT COUNT(*) FROM medications) AS total_medications;