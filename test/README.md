# 医院管理系统 API 测试用例

## 概述

本目录包含医院管理系统API的完整测试用例，专门针对API.md文档中定义的三个端口服务：

- **Public API** - 公共接口测试
- **Patient API** - 患者端接口测试  
- **Doctor API** - 医生端接口测试

## 测试文件结构

```
test/
├── public/                 # Public API测试用例
│   ├── public_schedule_list_with_filters.json
│   ├── public_schedule_list_no_filters.json
│   ├── public_schedule_list_by_department.json
│   ├── public_schedule_list_by_date.json
│   ├── public_doctor_get_valid_id.json
│   ├── public_doctor_get_missing_id.json
│   └── public_doctor_get_invalid_id.json
├── patient/                # Patient API测试用例
│   ├── patient_auth_register_success.json
│   ├── patient_auth_register_missing_email.json
│   ├── patient_auth_register_invalid_email.json
│   ├── patient_auth_register_weak_password.json
│   ├── patient_auth_login_success.json
│   ├── patient_auth_login_wrong_password.json
│   ├── patient_auth_login_missing_account.json
│   ├── patient_auth_reset_password_success.json
│   ├── patient_auth_reset_password_invalid_code.json
│   ├── patient_profile_get_success.json
│   ├── patient_profile_get_invalid_token.json
│   ├── patient_profile_update_success.json
│   ├── patient_profile_update_invalid_phone.json
│   ├── patient_appointment_create_success.json
│   ├── patient_appointment_create_missing_schedule.json
│   ├── patient_medical_record_list_success.json
│   ├── patient_prescription_list_success.json
│   ├── patient_prescription_get_success.json
│   ├── patient_prescription_get_not_found.json
│   ├── patient_chat_send_message_success.json
│   ├── patient_chat_send_message_empty_content.json
│   ├── patient_chat_get_history_success.json
│   ├── patient_chat_get_history_no_pagination.json
│   ├── patient_assessment_get_link_success.json
│   ├── patient_lab_result_list_success.json
│   └── patient_consultation_request_online_success.json
└── doctor/                 # Doctor API测试用例
    ├── doctor_auth_login_success.json
    ├── doctor_auth_login_wrong_password.json
    ├── doctor_auth_login_missing_employee_id.json
    ├── doctor_auth_reset_password_success.json
    ├── doctor_auth_reset_password_wrong_id_card.json
    ├── doctor_profile_get_success.json
    ├── doctor_profile_update_success.json
    ├── doctor_appointment_list_success.json
    ├── doctor_appointment_list_no_date.json
    ├── doctor_patient_get_medical_records_success.json
    ├── doctor_patient_get_medical_records_invalid_patient.json
    ├── doctor_medical_record_create_success.json
    ├── doctor_medical_record_create_empty_diagnosis.json
    ├── doctor_prescription_create_success.json
    ├── doctor_prescription_create_empty_medicines.json
    ├── doctor_lab_result_upload_success.json
    ├── doctor_lab_result_upload_invalid_base64.json
    ├── doctor_status_update_online.json
    ├── doctor_status_update_offline.json
    ├── doctor_status_update_invalid_status.json
    ├── doctor_attendance_check_in_success.json
    ├── doctor_attendance_cancel_check_in_success.json
    ├── doctor_attendance_get_history_success.json
    ├── doctor_attendance_get_history_no_date_range.json
    ├── doctor_leave_request_submit_success.json
    ├── doctor_leave_request_submit_invalid_type.json
    ├── doctor_leave_request_submit_invalid_date_range.json
    ├── doctor_leave_request_list_success.json
    ├── doctor_leave_request_cancel_success.json
    └── doctor_leave_request_cancel_not_found.json
```

## API端点覆盖

### Public API (7个测试用例)
- `public.schedule.list` - 查询医生坐诊安排
- `public.doctor.get` - 查看医生详细信息

### Patient API (26个测试用例)
- `patient.auth.register` - 患者注册
- `patient.auth.login` - 患者登录
- `patient.auth.resetPassword` - 患者找回密码
- `patient.profile.get` - 获取个人信息
- `patient.profile.update` - 编辑个人信息
- `patient.appointment.create` - 提交挂号/预约
- `patient.medicalRecord.list` - 查看病历和医嘱
- `patient.prescription.list` - 查看处方列表
- `patient.prescription.get` - 查看处方详情
- `patient.chat.sendMessage` - 发送消息
- `patient.chat.getHistory` - 获取聊天历史
- `patient.assessment.getLink` - 健康评估
- `patient.labResult.list` - 检查结果查询
- `patient.consultation.requestOnline` - 线上医疗服务

### Doctor API (30个测试用例)
- `doctor.auth.login` - 医生登录
- `doctor.auth.resetPassword` - 医生找回密码
- `doctor.profile.get` - 获取医生个人信息
- `doctor.profile.update` - 编辑医生个人信息
- `doctor.appointment.list` - 查看预约患者信息
- `doctor.patient.getMedicalRecords` - 查看患者病历详情
- `doctor.medicalRecord.create` - 做出诊断，留下医嘱
- `doctor.prescription.create` - 开具处方
- `doctor.labResult.upload` - 上传检验报告
- `doctor.status.update` - 更新在线状态
- `doctor.attendance.checkIn` - 日常打卡
- `doctor.attendance.cancelCheckIn` - 取消打卡
- `doctor.attendance.getHistory` - 查看打卡历史
- `doctor.leaveRequest.submit` - 提交请假申请
- `doctor.leaveRequest.list` - 查看请假列表
- `doctor.leaveRequest.cancel` - 销假

## 使用方法

### 1. 数据库初始化
```bash
mysql -u root -p < hospital_complete_setup.sql
```

### 2. 编译JsonAPI程序
```bash
make jsonapi
```

### 3. 运行所有测试
```bash
chmod +x test/run_all_tests.sh
./test/run_all_tests.sh
```

### 4. 运行单个测试
```bash
chmod +x test/run_single_test.sh
./test/run_single_test.sh test/public/public_schedule_list_no_filters.json
```

### 5. 手动运行测试
```bash
./build/bin/JsonAPI --input test/public/public_schedule_list_no_filters.json --output result.json
```

## 测试数据说明

### 测试用户账户
- **患者账户**: patient@example.com / password123
- **医生账户**: DOC10086 / doctor123

### 测试Token
- **患者Token**: patient_token_123456
- **医生Token**: doctor_token_67890

### 测试ID
- **患者ID**: pat_123456
- **医生ID**: doc_67890
- **排班ID**: sched_12345
- **预约ID**: appt_abcde
- **处方ID**: presc_xyz

## 注意事项

1. **数据依赖**: 测试用例依赖hospital_complete_setup.sql中的初始数据
2. **Token有效性**: 实际测试时需要使用有效的认证token
3. **时间敏感**: 某些测试用例包含日期时间，可能需要根据实际情况调整
4. **数据库状态**: 某些测试可能会修改数据库状态，建议在隔离环境中运行

## 测试结果

测试结果将保存在 `test/results/` 目录下，每个测试用例对应一个结果文件。