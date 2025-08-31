 好的，完全没问题！

既然您确定使用 Socket 通信，我们将全面采用您设计的 RPC 风格来重新定义所有功能的 API。这种模式非常适合长连接和双向通信的场景。

下面是根据您的33个功能需求，使用您定义的 RPC 风格（`{"api": "...", "data": {...}}`）编写的完整 API 文档。

---

### **智慧医疗管理系统 - RPC API 定义**

#### **通信协议**

所有请求和响应都通过建立的 Socket 连接以 JSON 字符串的形式进行传输。

#### **基本请求格式**

```json
{
  "api": "模块.资源.动作",
  "data": {
    // 请求的具体参数
  }
}
```

#### **基本响应格式**

*   **成功响应:**
    ```json
    {
      "status": "success",
      "code": 200, // 或其他成功代码
      "message": "操作成功的描述",
      "data": {
        // 返回的具体数据
      }
    }
    ```
*   **失败响应:**
    ```json
    {
      "status": "error",
      "code": 400, // 或其他错误代码，如 401, 404, 500
      "message": "操作失败的详细原因",
      "data": {} // 错误时 data 通常为空
    }
    ```

#### **身份认证**

用户（患者或医生）登录成功后，会获得一个 `token`。后续所有需要登录才能访问的接口，都必须在请求的 `data` 对象中携带此 `token`。

---

### **一、 公共接口 (无需 Token)**

#### **4, 7. 查询医生坐诊安排**
*   **功能**: 查询医生排班信息，用于挂号和预约。
*   **请求**:
    ```json
    {
      "api": "public.schedule.list",
      "data": {
        "departmentId": "dept_cardiology_123", // 可选，按科室筛选
        "date": "2025-09-10",                  // 可选，按日期筛选
        "doctorName": "李"                      // 可选，按医生姓名模糊搜索
      }
    }
    ```
*   **成功响应**:
    ```json
    {
      "status": "success", "code": 200, "message": "获取坐诊安排成功",
      "data": {
        "schedules": [
          {
            "scheduleId": "sched_12345",
            "doctorName": "李医生",
            "department": "心血管内科",
            "date": "2025-09-10",
            "timePeriod": "上午 8:00-12:00",
            "registrationFee": 50.5,
            "patientLimit": 30,
            "bookedCount": 25,
            "remainingCount": 5
          }
        ]
      }
    }
    ```

#### **5. 查看医生详细信息**
*   **功能**: 获取指定医生的详细资料。
*   **请求**:
    ```json
    {
      "api": "public.doctor.get",
      "data": {
        "doctorId": "doc_67890"
      }
    }
    ```
*   **成功响应**:
    ```json
    {
      "status": "success", "code": 200, "message": "获取医生信息成功",
      "data": {
        "employeeId": "DOC10086",
        "name": "李医生",
        "department": "心血管内科",
        "title": "主任医师",
        "photoUrl": "https://example.com/photos/doc_li.jpg",
        "bio": "擅长治疗高血压、冠心病等心血管疾病。",
        "registrationFee": 50.5,
        "dailyPatientLimit": 30
      }
    }
    ```

---

### **二、 患者端 API (需要 Token)**

#### **1. 患者注册**
*   **功能**: 患者使用邮箱和验证码注册账号。
*   **请求**:
    ```json
    {
      "api": "patient.auth.register",
      "data": {
        "email": "patient@example.com",
        "password": "hashed_password_string",
        "verificationCode": "123456"
      }
    }
    ```
*   **成功响应**:
    ```json
    {
      "status": "success", "code": 201, "message": "注册成功",
      "data": {
        "userId": "pat_123456"
      }
    }
    ```

#### **2. 患者登录**
*   **功能**: 患者使用账户和密码登录。
*   **请求**:
    ```json
    {
      "api": "patient.auth.login",
      "data": {
        "account": "patient@example.com",
        "password": "hashed_password_string"
      }
    }
    ```
*   **成功响应**:
    ```json
    {
      "status": "success", "code": 200, "message": "登录成功",
      "data": {
        "token": "a_very_long_jwt_token_string",
        "userId": "pat_123456"
      }
    }
    ```

#### **2. 患者找回密码**
*   **功能**: 通过用户名、邮箱和验证码重置密码。
*   **请求**:
    ```json
    {
      "api": "patient.auth.resetPassword",
      "data": {
        "username": "张三",
        "email": "patient@example.com",
        "verificationCode": "654321",
        "newPassword": "new_hashed_password"
      }
    }
    ```
*   **成功响应**:
    ```json
    {
      "status": "success", "code": 200, "message": "密码重置成功",
      "data": {}
    }
    ```

#### **3. 获取/编辑个人信息**
*   **获取请求**:
    ```json
    {
      "api": "patient.profile.get",
      "data": { "token": "a_very_long_jwt_token_string" }
    }
    ```
*   **编辑请求**:
    ```json
    {
      "api": "patient.profile.update",
      "data": {
        "token": "a_very_long_jwt_token_string",
        "name": "张三",
        "dateOfBirth": "1990-01-15",
        "idCardNumber": "340123199001151234",
        "phone": "13800138000",
        "email": "patient_new@example.com"
      }
    }
    ```
*   **成功响应**:
    ```json
    {
      "status": "success", "code": 200, "message": "操作成功",
      "data": {
        "name": "张三",
        "dateOfBirth": "1990-01-15",
        // ... 其他个人信息
      }
    }
    ```

#### **4, 7. 提交挂号/预约**
*   **请求**:
    ```json
    {
      "api": "patient.appointment.create",
      "data": {
        "token": "a_very_long_jwt_token_string",
        "scheduleId": "sched_12345",
        "timeSlot": "09:00-09:30" // 可选，如果排班分时段
      }
    }
    ```
*   **成功响应**:
    ```json
    {
      "status": "success", "code": 201, "message": "预约成功",
      "data": {
        "appointmentId": "appt_abcde",
        "status": "scheduled"
      }
    }
    ```

#### **6. 查看病历和医嘱**
*   **请求**:
    ```json
    {
      "api": "patient.medicalRecord.list",
      "data": { "token": "a_very_long_jwt_token_string" }
    }
    ```
*   **成功响应**:
    ```json
    {
      "status": "success", "code": 200, "message": "获取病历列表成功",
      "data": {
        "records": [
          {
            "recordId": "rec_001", "date": "2025-08-20", "department": "心血管内科",
            "attendingDoctor": "李医生", "diagnosis": "原发性高血压",
            "doctorAdvice": "低盐饮食，每日监测血压。"
          }
        ]
      }
    }
    ```

#### **8. 查看处方**
*   **列表请求**:
    ```json
    {
      "api": "patient.prescription.list",
      "data": { "token": "a_very_long_jwt_token_string" }
    }
    ```
*   **详情请求**:
    ```json
    {
      "api": "patient.prescription.get",
      "data": {
        "token": "a_very_long_jwt_token_string",
        "prescriptionId": "presc_xyz"
      }
    }
    ```
*   **成功响应 (详情)**:
    ```json
    {
      "status": "success", "code": 200, "message": "获取处方详情成功",
      "data": {
        "prescriptionId": "presc_xyz", "date": "2025-08-20",
        "medicines": [
          { "name": "阿司匹林肠溶片", "dosage": "100mg", "frequency": "每日一次" }
        ]
      }
    }
    ```

#### **9. 医患沟通**
*   **发送消息请求**:
    ```json
    {
      "api": "patient.chat.sendMessage",
      "data": {
        "token": "a_very_long_jwt_token_string",
        "doctorId": "doc_67890",
        "content": "李医生您好，请问我上次开的药吃完后还需要复诊吗？"
      }
    }
    ```
*   **获取历史消息请求**:
    ```json
    {
      "api": "patient.chat.getHistory",
      "data": {
        "token": "a_very_long_jwt_token_string",
        "doctorId": "doc_67890",
        "lastMessageId": "msg_123" // 可选，用于分页加载
      }
    }
    ```

#### **10. 健康评估**
*   **请求**:
    ```json
    {
      "api": "patient.assessment.getLink",
      "data": { "token": "a_very_long_jwt_token_string" }
    }
    ```
*   **成功响应**:
    ```json
    {
      "status": "success", "code": 200, "message": "获取成功",
      "data": {
        "url": "https://example.com/questionnaire/health_check"
      }
    }
    ```

#### **11. 检查结果查询**
*   **请求**:
    ```json
    {
      "api": "patient.labResult.list",
      "data": { "token": "a_very_long_jwt_token_string" }
    }
    ```
*   **成功响应**:
    ```json
    {
      "status": "success", "code": 200, "message": "获取检查结果成功",
      "data": {
        "results": [
          { "resultId": "lab_res_001", "reportName": "血常规检查报告", "date": "2025-08-19", "reportUrl": "..." }
        ]
      }
    }
    ```

#### **12. 线上医疗服务**
*   **请求**:
    ```json
    {
      "api": "patient.consultation.requestOnline",
      "data": { "token": "a_very_long_jwt_token_string" }
    }
    ```
*   **成功响应**:
    ```json
    {
      "status": "success", "code": 200, "message": "匹配成功，正在建立连接",
      "data": {
        "sessionId": "session_xyz",
        "agoraToken": "...", // 用于实时音视频的Token
        "doctorId": "doc_online_007"
      }
    }
    ```

---

### **三、 医生端 API (需要 Token)**

#### **21. 医生找回密码**
*   **请求**:
    ```json
    {
      "api": "doctor.auth.resetPassword",
      "data": {
        "employeeId": "DOC10086",
        "idCardNumber": "340123198001011234",
        "newPassword": "new_hashed_password"
      }
    }
    ```
*   **成功响应**:
    ```json
    {
      "status": "success", "code": 200, "message": "密码重置成功",
      "data": {}
    }
    ```

#### **22. 医生登录**
*   **请求**:
    ```json
    {
      "api": "doctor.auth.login",
      "data": {
        "employeeId": "DOC10086",
        "password": "hashed_password_string"
      }
    }
    ```
*   **成功响应**:
    ```json
    {
      "status": "success", "code": 200, "message": "登录成功",
      "data": {
        "token": "a_very_long_doctor_jwt_token_string",
        "doctorId": "doc_67890"
      }
    }
    ```

#### **23. 获取/编辑医生个人信息**
*   **获取请求**:
    ```json
    {
      "api": "doctor.profile.get",
      "data": { "token": "doctor_token" }
    }
    ```
*   **编辑请求**:
    ```json
    {
      "api": "doctor.profile.update",
      "data": {
        "token": "doctor_token",
        "bio": "更新了个人简介",
        "registrationFee": 60.0
        // ... 其他可修改信息
      }
    }
    ```

#### **24. 查看预约患者信息**
*   **请求**:
    ```json
    {
      "api": "doctor.appointment.list",
      "data": {
        "token": "doctor_token",
        "date": "2025-09-10" // 默认当天
      }
    }
    ```
*   **成功响应**:
    ```json
    {
      "status": "success", "code": 200, "message": "获取预约列表成功",
      "data": {
        "appointments": [
          { "appointmentId": "appt_abcde", "patientName": "张三", "appointmentTime": "2025-09-10T09:00:00Z", "patientId": "pat_123456" }
        ]
      }
    }
    ```

#### **24. 查看患者病历详情**
*   **请求**:
    ```json
    {
      "api": "doctor.patient.getMedicalRecords",
      "data": {
        "token": "doctor_token",
        "patientId": "pat_123456"
      }
    }
    ```
*   **成功响应**: (同患者端查看病历的响应)

#### **25. 远程医疗界面 (更新在线状态)**
*   **请求**:
    ```json
    {
      "api": "doctor.status.update",
      "data": {
        "token": "doctor_token",
        "status": "online" // "online" 或 "offline"
      }
    }
    ```

#### **26. 做出诊断，留下医嘱**
*   **请求**:
    ```json
    {
      "api": "doctor.medicalRecord.create",
      "data": {
        "token": "doctor_token",
        "patientId": "pat_123456",
        "appointmentId": "appt_abcde", // 关联本次就诊
        "diagnosis": "急性上呼吸道感染",
        "doctorAdvice": "多喝水，注意休息，口服XX胶囊。"
      }
    }
    ```

#### **26. 开具处方**
*   **请求**:
    ```json
    {
      "api": "doctor.prescription.create",
      "data": {
        "token": "doctor_token",
        "patientId": "pat_123456",
        "appointmentId": "appt_abcde",
        "medicines": [
          { "name": "XX胶囊", "dosage": "0.5g", "frequency": "每日三次", "quantity": 1 }
        ]
      }
    }
    ```

#### **28. 上传检验报告**
*   **说明**: Socket 传输文件通常比较复杂，一般会先请求一个上传凭证，然后通过 HTTP 上传。这里简化为传递文件内容的 Base64 编码。
*   **请求**:
    ```json
    {
      "api": "doctor.labResult.upload",
      "data": {
        "token": "doctor_token",
        "patientId": "pat_123456",
        "reportName": "血常规报告",
        "fileContentBase64": "JVBERi0xLjUNCiX... (文件的Base64编码字符串)"
      }
    }
    ```

#### **31. 日常打卡**
*   **打卡请求**:
    ```json
    {
      "api": "doctor.attendance.checkIn",
      "data": { "token": "doctor_token" }
    }
    ```
*   **取消打卡请求**:
    ```json
    {
      "api": "doctor.attendance.cancelCheckIn",
      "data": { "token": "doctor_token" }
    }
    ```
*   **查看历史请求**:
    ```json
    {
      "api": "doctor.attendance.getHistory",
      "data": {
        "token": "doctor_token",
        "startDate": "2025-08-01",
        "endDate": "2025-08-28"
      }
    }
    ```

#### **32. 提交请假申请**
*   **请求**:
    ```json
    {
      "api": "doctor.leaveRequest.submit",
      "data": {
        "token": "doctor_token",
        "contactPhone": "13900139000",
        "type": "因私请假", // "因公请假" 或 "因私请假"
        "startDate": "2025-09-15",
        "endDate": "2025-09-16",
        "reason": "家中有事需要处理。"
      }
    }
    ```

#### **33. 销假**
*   **查看请假列表请求**:
    ```json
    {
      "api": "doctor.leaveRequest.list",
      "data": { "token": "doctor_token" }
    }
    ```
*   **销假请求**:
    ```json
    {
      "api": "doctor.leaveRequest.cancel",
      "data": {
        "token": "doctor_token",
        "requestId": "leave_req_123"
      }
    }
    ```