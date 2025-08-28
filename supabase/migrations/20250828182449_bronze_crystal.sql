@@ .. @@
 -- 验证数据插入
 SELECT '=== 数据统计 ===' AS verification_step;
 SELECT 
     (SELECT COUNT(*) FROM users) AS total_users,
     (SELECT COUNT(*) FROM doctors) AS total_doctors,
     (SELECT COUNT(*) FROM patients) AS total_patients,
     (SELECT COUNT(*) FROM cases) AS total_cases,
     (SELECT COUNT(*) FROM appointments) AS total_appointments,
     (SELECT COUNT(*) FROM hospitalization) AS total_hospitalizations,
     (SELECT COUNT(*) FROM prescriptions) AS total_prescriptions,
     (SELECT COUNT(*) FROM medications) AS total_medications;