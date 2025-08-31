#include "ApiHandler.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>
#include <regex>
#include <random>
#include <chrono>

ApiHandler::ApiHandler(std::shared_ptr<HospitalService> service) : hospitalService(service) {
    // 注册公共接口处理函数
    apiHandlers["public.schedule.list"] = [this](const json& data) { return handlePublicScheduleList(data); };
    apiHandlers["public.doctor.get"] = [this](const json& data) { return handlePublicDoctorGet(data); };
    
    // 注册患者端接口处理函数
    apiHandlers["patient.auth.register"] = [this](const json& data) { return handlePatientRegister(data); };
    apiHandlers["patient.auth.login"] = [this](const json& data) { return handlePatientLogin(data); };
    apiHandlers["patient.auth.resetPassword"] = [this](const json& data) { return handlePatientResetPassword(data); };
    apiHandlers["patient.profile.get"] = [this](const json& data) { return handlePatientProfileGet(data); };
    apiHandlers["patient.profile.update"] = [this](const json& data) { return handlePatientProfileUpdate(data); };
    apiHandlers["patient.appointment.create"] = [this](const json& data) { return handlePatientAppointmentCreate(data); };
    apiHandlers["patient.medicalRecord.list"] = [this](const json& data) { return handlePatientMedicalRecordList(data); };
    apiHandlers["patient.prescription.list"] = [this](const json& data) { return handlePatientPrescriptionList(data); };
    apiHandlers["patient.prescription.get"] = [this](const json& data) { return handlePatientPrescriptionGet(data); };
    apiHandlers["patient.labResult.list"] = [this](const json& data) { return handlePatientLabResultList(data); };
    apiHandlers["patient.chat.sendMessage"] = [this](const json& data) { return handlePatientChatSendMessage(data); };
    apiHandlers["patient.chat.getHistory"] = [this](const json& data) { return handlePatientChatGetHistory(data); };
    apiHandlers["patient.assessment.getLink"] = [this](const json& data) { return handlePatientAssessmentGetLink(data); };
    apiHandlers["patient.consultation.requestOnline"] = [this](const json& data) { return handlePatientConsultationRequestOnline(data); };
    
    // 注册医生端接口处理函数
    apiHandlers["doctor.auth.login"] = [this](const json& data) { return handleDoctorLogin(data); };
    apiHandlers["doctor.auth.resetPassword"] = [this](const json& data) { return handleDoctorResetPassword(data); };
    apiHandlers["doctor.profile.get"] = [this](const json& data) { return handleDoctorProfileGet(data); };
    apiHandlers["doctor.profile.update"] = [this](const json& data) { return handleDoctorProfileUpdate(data); };
    apiHandlers["doctor.appointment.list"] = [this](const json& data) { return handleDoctorAppointmentList(data); };
    apiHandlers["doctor.patient.getMedicalRecords"] = [this](const json& data) { return handleDoctorPatientGetMedicalRecords(data); };
    apiHandlers["doctor.medicalRecord.create"] = [this](const json& data) { return handleDoctorMedicalRecordCreate(data); };
    apiHandlers["doctor.prescription.create"] = [this](const json& data) { return handleDoctorPrescriptionCreate(data); };
    apiHandlers["doctor.labResult.upload"] = [this](const json& data) { return handleDoctorLabResultUpload(data); };
    apiHandlers["doctor.status.update"] = [this](const json& data) { return handleDoctorStatusUpdate(data); };
    apiHandlers["doctor.attendance.checkIn"] = [this](const json& data) { return handleDoctorAttendanceCheckIn(data); };
    apiHandlers["doctor.attendance.cancelCheckIn"] = [this](const json& data) { return handleDoctorAttendanceCancelCheckIn(data); };
    apiHandlers["doctor.attendance.getHistory"] = [this](const json& data) { return handleDoctorAttendanceGetHistory(data); };
    apiHandlers["doctor.leaveRequest.submit"] = [this](const json& data) { return handleDoctorLeaveRequestSubmit(data); };
    apiHandlers["doctor.leaveRequest.list"] = [this](const json& data) { return handleDoctorLeaveRequestList(data); };
    apiHandlers["doctor.leaveRequest.cancel"] = [this](const json& data) { return handleDoctorLeaveRequestCancel(data); };
}

ApiHandler::~ApiHandler() = default;

std::string ApiHandler::processApiRequest(const std::string& jsonInput) {
    try {
        json request = json::parse(jsonInput);
        
        if (!request.contains("api") || !request.contains("data")) {
            return ApiResponse("error", 400, "Invalid request format", json::object()).toJson();
        }
        
        std::string apiName = request["api"];
        json data = request["data"];
        
        auto it = apiHandlers.find(apiName);
        if (it == apiHandlers.end()) {
            return ApiResponse("error", 404, "API endpoint not found", json::object()).toJson();
        }
        
        return it->second(data).toJson();
        
    } catch (const json::parse_error& e) {
        return ApiResponse("error", 400, "Invalid JSON format: ", std::string(e.what())).toJson();
    } catch (const std::exception& e) {
        return ApiResponse("error", 500, "Internal server error: ", std::string(e.what())).toJson();
    }
}

std::string ApiHandler::ApiResponse::toJson() const {
    json response;
    response["status"] = status;
    response["code"] = code;
    response["message"] = message;
    response["data"] = data;
    return response.dump();
}

// Token验证函数 - 新的基于数据库的验证逻辑
bool ApiHandler::validateToken(const std::string& token, int& userId, UserType& userType) {
    if (token.empty()) {
        return false;
    }
    
    // 获取所有用户，计算每个用户的token并进行匹配
    auto users = hospitalService->getUserDAO()->getAllUsers();
    
    for (const auto& user : users) {
        std::string expectedToken = generateTokenForUser(user->getUserId(), user->getUsername());
        if (expectedToken == token) {
            userId = user->getUserId();
            userType = user->getUserType();
            return true;
        }
    }
    
    return false;
}

// 生成用户token的函数
std::string ApiHandler::generateTokenForUser(int userId, const std::string& username) {
    std::string tokenSource = std::to_string(userId) + username;
    return hashPassword(tokenSource);
}

// 公共接口处理函数
ApiHandler::ApiResponse ApiHandler::handlePublicScheduleList(const json& data) {
    try {
        // 模拟医生排班数据查询
        json schedules = json::array();
        
        // 从数据库获取医生排班信息
        auto doctors = hospitalService->getDoctorDAO()->getAllDoctors();
        
        for (const auto& doctor : doctors) {
            json schedule = {
                {"scheduleId", "sched_" + std::to_string(doctor->getDoctorId())},
                {"doctorName", doctor->getName()},
                {"department", doctor->getDepartment()},
                {"date", getCurrentDateTime().substr(0, 10)}, // 今天的日期
                {"timePeriod", doctor->getWorkingHours()},
                {"registrationFee", 50.0},
                {"patientLimit", 30},
                {"bookedCount", hospitalService->getAppointmentDAO()->getAppointmentCountByDoctor(doctor->getDoctorId())},
                {"remainingCount", 30 - hospitalService->getAppointmentDAO()->getAppointmentCountByDoctor(doctor->getDoctorId())}
            };
            schedules.push_back(schedule);
        }
        
        json responseData;
        responseData["schedules"] = schedules;
        
        return ApiResponse("success", 200, "获取坐诊安排成功", responseData);
        
    } catch (const std::exception& e) {
        return ApiResponse("error", 500, "获取坐诊安排失败:", std::string(e.what()));
    }
}

ApiHandler::ApiResponse ApiHandler::handlePublicDoctorGet(const json& data) {
    try {
        if (!data.contains("doctorId")) {
            return ApiResponse("error", 400, "缺少医生ID参数", json::object());
        }
        
        // doctorId传入为doc_xxx形式
        std::string doctorIdStr = data["doctorId"];
        int doctorId = std::stoi(doctorIdStr.substr(4));
        
        auto doctor = hospitalService->getDoctorDAO()->getDoctorById(doctorId);
        if (!doctor) {
            return ApiResponse("error", 404, "医生不存在", json::object());
        }
        
        json responseData;
        responseData["employeeId"] = "DOC" + std::to_string(doctor->getDoctorId());
        responseData["name"] = doctor->getName();
        responseData["department"] = doctor->getDepartment();
        responseData["title"] = doctor->getTitle();
        responseData["photoUrl"] = doctor->getProfilePicture().empty() ? 
            "https://example.com/default_doctor.jpg" : doctor->getProfilePicture();
        responseData["bio"] = "专业医生，经验丰富";
        responseData["registrationFee"] = 50.0;
        responseData["dailyPatientLimit"] = 30;
        
        return ApiResponse("success", 200, "获取医生信息成功", responseData);
        
    } catch (const std::exception& e) {
        return ApiResponse("error", 500, "获取医生信息失败", json::object());
    }
}

// 患者端接口处理函数
ApiHandler::ApiResponse ApiHandler::handlePatientRegister(const json& data) {
    try {
        if (!data.contains("email") || !data.contains("password")) {
            return ApiResponse("error", 400, "缺少必要参数", json::object());
        }
        
        std::string email = data["email"];
        std::string password = data["password"];
        
        if (!validateEmail(email) || !validatePassword(password)) {
            return ApiResponse("error", 400, "邮箱格式或密码不符合要求", json::object());
        }
        
        // 检查用户是否已存在
        if (hospitalService->getUserDAO()->userExists(email, email)) {
            return ApiResponse("error", 409, "用户已存在", json::object());
        }
        
        // 创建用户
        if (hospitalService->registerUser(email, password, UserType::PATIENT, email)) {
            auto user = hospitalService->getUserDAO()->getUserByEmail(email);
            if (user) {
                json responseData;
                responseData["userId"] = "pat_" + std::to_string(user->getUserId());
                return ApiResponse("success", 201, "注册成功", responseData);
            }
        }
        
        return ApiResponse("error", 500, "注册失败", json::object());
        
    } catch (const std::exception& e) {
        return ApiResponse("error", 500, "注册失败", std::string(e.what()));
    }
}

ApiHandler::ApiResponse ApiHandler::handlePatientLogin(const json& data) {
    try {
        if (!data.contains("account") || !data.contains("password")) {
            return ApiResponse("error", 400, "缺少必需参数: account 或 password");
        }
        
        std::string account = data["account"];
        std::string password = data["password"];
        
        auto user = hospitalService->loginUser(account, password);
        if (!user || user->getUserType() != UserType::PATIENT) {
            return ApiResponse("error", 401, "登录失败，账户或密码错误");
        }
        
        std::string token = generateTokenForUser(user->getUserId(), user->getUsername());
        
        json responseData;
        responseData["token"] = token;
        responseData["userId"] = "pat_" + std::to_string(user->getUserId());
        
        return ApiResponse("success", 200, "登录成功", responseData);
        
    } catch (const std::exception& e) {
        return ApiResponse("error", 500, "登录失败", std::string(e.what()));
    }
}

ApiHandler::ApiResponse ApiHandler::handlePatientResetPassword(const json& data) {
    try {
        if (!data.contains("username") || !data.contains("email") || !data.contains("newPassword")) {
            return ApiResponse("error", 400, "缺少必要参数", json::object());
        }
        
        std::string username = data["username"];
        std::string email = data["email"];
        std::string verificationCode = data["verificationCode"];
        std::string newPassword = data["newPassword"];
        
        auto user = hospitalService->getUserDAO()->getUserByUsername(username);
        if (!user) {
            return ApiResponse("error", 404, "用户不存在", json::object());
        }

        if (user->getEmail() != email) {
            return ApiResponse("error", 404, "用户邮箱不匹配", json::object());
        }

        if (user->getUserType() != UserType::PATIENT) {
            return ApiResponse("error", 404, "用户类型不匹配", json::object());
        }

        // 可以设计一套验证码机制，此处仅针对传入json采用最简单的验证码匹配
        if (verificationCode != "654321") {
            return ApiResponse("error", 404, "验证码不匹配", json::object());
        }
        
        // 更新密码
        if (hospitalService->getUserDAO()->resetPassword(user->getUserId(), newPassword)) {
            return ApiResponse("success", 200, "密码重置成功", json::object());
        }
        
        return ApiResponse("error", 500, "密码重置失败", json::object());
        
    } catch (const std::exception& e) {
        return ApiResponse("error", 500, "密码重置失败", json::object());
    }
}

ApiHandler::ApiResponse ApiHandler::handlePatientProfileGet(const json& data) {
    try {
        if (!data.contains("token")) {
            return ApiResponse("error", 401, "缺少认证token", json::object());
        }
        
        std::string token = data["token"];
        int userId;
        UserType userType;
        
        if (!validateToken(token, userId, userType) || userType != UserType::PATIENT) {
            return ApiResponse("error", 401, "无效的认证token", json::object());
        }
        
        auto user = hospitalService->getUserDAO()->getUserById(userId);
        auto patient = hospitalService->getPatientDAO()->getPatientByUserId(userId);
        
        if (!user || !patient) {
            return ApiResponse("error", 404, "患者信息不存在", json::object());
        }
        
        json responseData;
        responseData["name"] = patient->getName();
        responseData["dateOfBirth"] = patient->getBirthDate();
        responseData["idCardNumber"] = patient->getIdNumber();
        responseData["phone"] = patient->getPhoneNumber();
        responseData["email"] = user->getEmail();
        responseData["gender"] = patient->genderToString();
        responseData["age"] = patient->getAge();
        
        return ApiResponse("success", 200, "获取个人信息成功", responseData);
        
    } catch (const std::exception& e) {
        return ApiResponse("error", 500, "获取个人信息失败", json::object());
    }
}

ApiHandler::ApiResponse ApiHandler::handlePatientProfileUpdate(const json& data) {
    try {
        if (!data.contains("token")) {
            return ApiResponse("error", 401, "缺少认证token", json::object());
        }
        
        std::string token = data["token"];
        int userId;
        UserType userType;
        
        if (!validateToken(token, userId, userType) || userType != UserType::PATIENT) {
            return ApiResponse("error", 401, "无效的认证token", json::object());
        }
        
        auto user = hospitalService->getUserDAO()->getUserById(userId);
        auto patient = hospitalService->getPatientDAO()->getPatientByUserId(userId);
        
        if (!user || !patient) {
            return ApiResponse("error", 404, "患者信息不存在", json::object());
        }
        
        // 更新患者信息
        if (data.contains("name")) patient->setName(data["name"]);
        if (data.contains("dateOfBirth")) patient->setBirthDate(data["dateOfBirth"]);
        if (data.contains("idCardNumber")) patient->setIdNumber(data["idCardNumber"]);
        if (data.contains("phone")) patient->setPhoneNumber(data["phone"]);
        
        // 更新用户信息
        if (data.contains("email")) user->setEmail(data["email"]);
        
        bool patientUpdated = hospitalService->getPatientDAO()->updatePatient(*patient);
        bool userUpdated = hospitalService->getUserDAO()->updateUser(*user);
        
        if (patientUpdated && userUpdated) {
            json responseData;
            responseData["name"] = patient->getName();
            responseData["dateOfBirth"] = patient->getBirthDate();
            responseData["idCardNumber"] = patient->getIdNumber();
            responseData["phone"] = patient->getPhoneNumber();
            responseData["email"] = user->getEmail();
            
            return ApiResponse("success", 200, "个人信息更新成功", responseData);
        }
        
        return ApiResponse("error", 500, "个人信息更新失败", json::object());
        
    } catch (const std::exception& e) {
        return ApiResponse("error", 500, "个人信息更新失败", json::object());
    }
}

ApiHandler::ApiResponse ApiHandler::handlePatientAppointmentCreate(const json& data) {
    try {
        if (!data.contains("token") || !data.contains("scheduleId")) {
            return ApiResponse("error", 400, "缺少必要参数", json::object());
        }
        
        std::string token = data["token"];
        int userId;
        UserType userType;
        
        if (!validateToken(token, userId, userType) || userType != UserType::PATIENT) {
            return ApiResponse("error", 401, "无效的认证token", json::object());
        }
        
        auto patient = hospitalService->getPatientDAO()->getPatientByUserId(userId);
        if (!patient) {
            return ApiResponse("error", 404, "患者信息不存在", json::object());
        }
        
        std::string scheduleId = data["scheduleId"];
        // 从scheduleId解析出doctorId (假设格式为 "sched_doctorId")
        int doctorId = 1; // 默认值，实际应从scheduleId解析
        if (scheduleId.find("sched_") == 0) {
            doctorId = std::stoi(scheduleId.substr(6));
        }
        
        auto doctor = hospitalService->getDoctorDAO()->getDoctorById(doctorId);
        if (!doctor) {
            return ApiResponse("error", 404, "医生不存在", json::object());
        }
        
        std::string appointmentTime = getCurrentDateTime();
        int appointmentId = hospitalService->bookAppointment(
            patient->getPatientId(), doctorId, appointmentTime, doctor->getDepartment());
        
        if (appointmentId > 0) {
            json responseData;
            responseData["appointmentId"] = "appt_" + std::to_string(appointmentId);
            responseData["status"] = "scheduled";
            
            return ApiResponse("success", 201, "预约成功", responseData);
        }
        
        return ApiResponse("error", 500, "预约失败", json::object());
        
    } catch (const std::exception& e) {
        return ApiResponse("error", 500, "预约失败", json::object());
    }
}

ApiHandler::ApiResponse ApiHandler::handlePatientMedicalRecordList(const json& data) {
    try {
        if (!data.contains("token")) {
            return ApiResponse("error", 401, "缺少认证token", json::object());
        }
        
        std::string token = data["token"];
        int userId;
        UserType userType;
        
        if (!validateToken(token, userId, userType) || userType != UserType::PATIENT) {
            return ApiResponse("error", 401, "无效的认证token", json::object());
        }
        
        auto patient = hospitalService->getPatientDAO()->getPatientByUserId(userId);
        if (!patient) {
            return ApiResponse("error", 404, "患者信息不存在", json::object());
        }
        
        auto cases = hospitalService->getCaseDAO()->getCasesByPatientId(patient->getPatientId());
        
        json records = json::array();
        for (const auto& medicalCase : cases) {
            auto doctor = hospitalService->getDoctorDAO()->getDoctorById(medicalCase->getDoctorId());
            
            json record;
            record["recordId"] = "rec_" + std::to_string(medicalCase->getCaseId());
            record["date"] = medicalCase->getDiagnosisDate();
            record["department"] = medicalCase->getDepartment();
            record["attendingDoctor"] = doctor ? doctor->getName() : "未知医生";
            record["diagnosis"] = medicalCase->getDiagnosis();
            record["doctorAdvice"] = medicalCase->getDiagnosis(); // 简化处理
            
            records.push_back(record);
        }
        
        json responseData;
        responseData["records"] = records;
        
        return ApiResponse("success", 200, "获取病历列表成功", responseData);
        
    } catch (const std::exception& e) {
        return ApiResponse("error", 500, "获取病历列表失败", json::object());
    }
}

ApiHandler::ApiResponse ApiHandler::handlePatientPrescriptionList(const json& data) {
    try {
        if (!data.contains("token")) {
            return ApiResponse("error", 401, "缺少认证token", json::object());
        }
        
        std::string token = data["token"];
        int userId;
        UserType userType;
        
        if (!validateToken(token, userId, userType) || userType != UserType::PATIENT) {
            return ApiResponse("error", 401, "无效的认证token", json::object());
        }
        
        auto patient = hospitalService->getPatientDAO()->getPatientByUserId(userId);
        if (!patient) {
            return ApiResponse("error", 404, "患者信息不存在", json::object());
        }
        
        // 获取患者的所有病例
        auto cases = hospitalService->getCaseDAO()->getCasesByPatientId(patient->getPatientId());
        
        json prescriptions = json::array();
        for (const auto& medicalCase : cases) {
            auto casePrescriptions = hospitalService->getPrescriptionDAO()->getPrescriptionsByCaseId(medicalCase->getCaseId());
            
            for (const auto& prescription : casePrescriptions) {
                json prescData;
                prescData["prescriptionId"] = "presc_" + std::to_string(prescription->getPrescriptionId());
                prescData["date"] = prescription->getIssuedDate();
                prescData["content"] = prescription->getPrescriptionContent();
                
                prescriptions.push_back(prescData);
            }
        }
        
        json responseData;
        responseData["prescriptions"] = prescriptions;
        
        return ApiResponse("success", 200, "获取处方列表成功", responseData);
        
    } catch (const std::exception& e) {
        return ApiResponse("error", 500, "获取处方列表失败", json::object());
    }
}

ApiHandler::ApiResponse ApiHandler::handlePatientPrescriptionGet(const json& data) {
    try {
        if (!data.contains("token") || !data.contains("prescriptionId")) {
            return ApiResponse("error", 400, "缺少必要参数", json::object());
        }
        
        std::string token = data["token"];
        int userId;
        UserType userType;
        
        if (!validateToken(token, userId, userType) || userType != UserType::PATIENT) {
            return ApiResponse("error", 401, "无效的认证token", json::object());
        }
        
        std::string prescriptionIdStr = data["prescriptionId"];
        int prescriptionId = std::stoi(prescriptionIdStr.substr(6)); // 去掉 "presc_" 前缀
        
        auto prescription = hospitalService->getPrescriptionDAO()->getPrescriptionById(prescriptionId);
        if (!prescription) {
            return ApiResponse("error", 404, "处方不存在", json::object());
        }
        
        auto medications = hospitalService->getMedicationDAO()->getMedicationsByPrescriptionId(prescriptionId);
        
        json medicines = json::array();
        for (const auto& medication : medications) {
            json medicine;
            medicine["name"] = medication->getMedicationName();
            medicine["dosage"] = std::to_string(medication->getQuantity()) + "片";
            medicine["frequency"] = medication->getUsageInstructions();
            
            medicines.push_back(medicine);
        }
        
        json responseData;
        responseData["prescriptionId"] = prescriptionIdStr;
        responseData["date"] = prescription->getIssuedDate();
        responseData["medicines"] = medicines;
        
        return ApiResponse("success", 200, "获取处方详情成功", responseData);
        
    } catch (const std::exception& e) {
        return ApiResponse("error", 500, "获取处方详情失败", json::object());
    }
}

ApiHandler::ApiResponse ApiHandler::handlePatientLabResultList(const json& data) {
    try {
        if (!data.contains("token")) {
            return ApiResponse("error", 401, "缺少认证token", json::object());
        }
        
        std::string token = data["token"];
        int userId;
        UserType userType;
        
        if (!validateToken(token, userId, userType) || userType != UserType::PATIENT) {
            return ApiResponse("error", 401, "无效的认证token", json::object());
        }
        
        // 模拟检验结果数据
        json results = json::array();
        json result;
        result["resultId"] = "lab_res_001";
        result["reportName"] = "血常规检查报告";
        result["date"] = "2025-08-19";
        result["reportUrl"] = "https://example.com/reports/lab_res_001.pdf";
        
        results.push_back(result);
        
        json responseData;
        responseData["results"] = results;
        
        return ApiResponse("success", 200, "获取检查结果成功", responseData);
        
    } catch (const std::exception& e) {
        return ApiResponse("error", 500, "获取检查结果失败", json::object());
    }
}

ApiHandler::ApiResponse ApiHandler::handlePatientChatSendMessage(const json& data) {
    try {
        if (!data.contains("token") || !data.contains("doctorId") || !data.contains("content")) {
            return ApiResponse("error", 400, "缺少必要参数", json::object());
        }
        
        std::string token = data["token"];
        int userId;
        UserType userType;
        
        if (!validateToken(token, userId, userType) || userType != UserType::PATIENT) {
            return ApiResponse("error", 401, "无效的认证token", json::object());
        }
        
        std::string doctorId = data["doctorId"];
        std::string content = data["content"];
        
        if (content.empty()) {
            return ApiResponse("error", 400, "消息内容不能为空", json::object());
        }
        
        // 模拟消息发送成功
        json responseData;
        responseData["messageId"] = "msg_" + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count());
        responseData["sentTime"] = getCurrentDateTime();
        
        return ApiResponse("success", 200, "消息发送成功", responseData);
        
    } catch (const std::exception& e) {
        return ApiResponse("error", 500, "消息发送失败", json::object());
    }
}

ApiHandler::ApiResponse ApiHandler::handlePatientChatGetHistory(const json& data) {
    try {
        if (!data.contains("token") || !data.contains("doctorId")) {
            return ApiResponse("error", 400, "缺少必要参数", json::object());
        }
        
        std::string token = data["token"];
        int userId;
        UserType userType;
        
        if (!validateToken(token, userId, userType) || userType != UserType::PATIENT) {
            return ApiResponse("error", 401, "无效的认证token", json::object());
        }
        
        // 模拟聊天历史数据
        json messages = json::array();
        json message;
        message["messageId"] = "msg_001";
        message["senderType"] = "patient";
        message["content"] = "医生您好，我想咨询一下病情";
        message["sentTime"] = "2025-08-20T10:30:00Z";
        
        messages.push_back(message);
        
        json responseData;
        responseData["messages"] = messages;
        
        return ApiResponse("success", 200, "获取聊天历史成功", responseData);
        
    } catch (const std::exception& e) {
        return ApiResponse("error", 500, "获取聊天历史失败", json::object());
    }
}

ApiHandler::ApiResponse ApiHandler::handlePatientAssessmentGetLink(const json& data) {
    try {
        if (!data.contains("token")) {
            return ApiResponse("error", 401, "缺少认证token", json::object());
        }
        
        std::string token = data["token"];
        int userId;
        UserType userType;
        
        if (!validateToken(token, userId, userType) || userType != UserType::PATIENT) {
            return ApiResponse("error", 401, "无效的认证token", json::object());
        }
        
        json responseData;
        responseData["url"] = "https://example.com/questionnaire/health_check";
        
        return ApiResponse("success", 200, "获取成功", responseData);
        
    } catch (const std::exception& e) {
        return ApiResponse("error", 500, "获取失败", json::object());
    }
}

ApiHandler::ApiResponse ApiHandler::handlePatientConsultationRequestOnline(const json& data) {
    try {
        if (!data.contains("token")) {
            return ApiResponse("error", 401, "缺少认证token", json::object());
        }
        
        std::string token = data["token"];
        int userId;
        UserType userType;
        
        if (!validateToken(token, userId, userType) || userType != UserType::PATIENT) {
            return ApiResponse("error", 401, "无效的认证token", json::object());
        }
        
        json responseData;
        responseData["sessionId"] = "session_xyz";
        responseData["agoraToken"] = "agora_token_example";
        responseData["doctorId"] = "doc_online_007";
        
        return ApiResponse("success", 200, "匹配成功，正在建立连接", responseData);
        
    } catch (const std::exception& e) {
        return ApiResponse("error", 500, "线上医疗服务请求失败", json::object());
    }
}

// 医生端接口处理函数
ApiHandler::ApiResponse ApiHandler::handleDoctorLogin(const json& data) {
    try {
        if (!data.contains("employeeId") || !data.contains("password")) {
            return ApiResponse("error", 400, "缺少工号或密码", json::object());
        }
        
        std::string employeeId = data["employeeId"];
        std::string password = data["password"];
        
        // 从employeeId提取用户名 (假设格式为 "DOC" + 数字)
        std::string username = employeeId;
        if (employeeId.find("DOC") == 0) {
            username = "dr_" + employeeId.substr(3);
        }
        
        auto user = hospitalService->loginUser(username, password);
        if (!user || user->getUserType() != UserType::DOCTOR) {
            return ApiResponse("error", 401, "登录失败，工号或密码错误", json::object());
        }
        
        std::string token = generateTokenForUser(user->getUserId(), user->getUsername());
        
        json responseData;
        responseData["token"] = token;
        responseData["doctorId"] = "doc_" + std::to_string(user->getUserId());
        
        return ApiResponse("success", 200, "登录成功", responseData);
        
    } catch (const std::exception& e) {
        return ApiResponse("error", 500, "登录失败", json::object());
    }
}

ApiHandler::ApiResponse ApiHandler::handleDoctorResetPassword(const json& data) {
    try {
        if (!data.contains("employeeId") || !data.contains("idCardNumber") || !data.contains("newPassword")) {
            return ApiResponse("error", 400, "缺少必要参数", json::object());
        }
        
        std::string employeeId = data["employeeId"];
        std::string idCardNumber = data["idCardNumber"];
        std::string newPassword = data["newPassword"];
        
        // 简化处理：通过employeeId找到用户
        std::string username = employeeId;
        if (employeeId.find("DOC") == 0) {
            username = "dr_" + employeeId.substr(3);
        }
        
        auto user = hospitalService->getUserDAO()->getUserByUsername(username);
        if (!user || user->getUserType() != UserType::DOCTOR) {
            return ApiResponse("error", 404, "医生不存在", json::object());
        }
        
        // 更新密码
        if (hospitalService->getUserDAO()->changePassword(user->getUserId(), user->getPasswordHash(), newPassword)) {
            return ApiResponse("success", 200, "密码重置成功", json::object());
        }
        
        return ApiResponse("error", 500, "密码重置失败", json::object());
        
    } catch (const std::exception& e) {
        return ApiResponse("error", 500, "密码重置失败", json::object());
    }
}

ApiHandler::ApiResponse ApiHandler::handleDoctorProfileGet(const json& data) {
    try {
        if (!data.contains("token")) {
            return ApiResponse("error", 401, "缺少认证token", json::object());
        }
        
        std::string token = data["token"];
        int userId;
        UserType userType;
        
        if (!validateToken(token, userId, userType) || userType != UserType::DOCTOR) {
            return ApiResponse("error", 401, "无效的认证token", json::object());
        }
        
        auto user = hospitalService->getUserDAO()->getUserById(userId);
        auto doctor = hospitalService->getDoctorDAO()->getDoctorByUserId(userId);
        
        if (!user || !doctor) {
            return ApiResponse("error", 404, "医生信息不存在", json::object());
        }
        
        json responseData;
        responseData["employeeId"] = "DOC" + std::to_string(doctor->getDoctorId());
        responseData["name"] = doctor->getName();
        responseData["department"] = doctor->getDepartment();
        responseData["title"] = doctor->getTitle();
        responseData["workingHours"] = doctor->getWorkingHours();
        responseData["email"] = user->getEmail();
        responseData["phone"] = user->getPhoneNumber();
        responseData["bio"] = "专业医生，经验丰富";
        responseData["registrationFee"] = 50.0;
        
        return ApiResponse("success", 200, "获取医生信息成功", responseData);
        
    } catch (const std::exception& e) {
        return ApiResponse("error", 500, "获取医生信息失败", json::object());
    }
}

ApiHandler::ApiResponse ApiHandler::handleDoctorProfileUpdate(const json& data) {
    try {
        if (!data.contains("token")) {
            return ApiResponse("error", 401, "缺少认证token", json::object());
        }
        
        std::string token = data["token"];
        int userId;
        UserType userType;
        
        if (!validateToken(token, userId, userType) || userType != UserType::DOCTOR) {
            return ApiResponse("error", 401, "无效的认证token", json::object());
        }
        
        auto user = hospitalService->getUserDAO()->getUserById(userId);
        auto doctor = hospitalService->getDoctorDAO()->getDoctorByUserId(userId);
        
        if (!user || !doctor) {
            return ApiResponse("error", 404, "医生信息不存在", json::object());
        }
        
        // 更新医生信息
        if (data.contains("bio")) {
            // bio字段在当前数据库结构中没有，这里忽略
        }
        if (data.contains("registrationFee")) {
            // registrationFee字段在当前数据库结构中没有，这里忽略
        }
        
        // 更新用户信息
        if (data.contains("email")) user->setEmail(data["email"]);
        if (data.contains("phone")) user->setPhoneNumber(data["phone"]);
        
        bool userUpdated = hospitalService->getUserDAO()->updateUser(*user);
        bool doctorUpdated = hospitalService->getDoctorDAO()->updateDoctor(*doctor);
        
        if (userUpdated && doctorUpdated) {
            json responseData;
            responseData["employeeId"] = "DOC" + std::to_string(doctor->getDoctorId());
            responseData["name"] = doctor->getName();
            responseData["department"] = doctor->getDepartment();
            responseData["title"] = doctor->getTitle();
            responseData["email"] = user->getEmail();
            responseData["phone"] = user->getPhoneNumber();
            
            return ApiResponse("success", 200, "医生信息更新成功", responseData);
        }
        
        return ApiResponse("error", 500, "医生信息更新失败", json::object());
        
    } catch (const std::exception& e) {
        return ApiResponse("error", 500, "医生信息更新失败", json::object());
    }
}

ApiHandler::ApiResponse ApiHandler::handleDoctorAppointmentList(const json& data) {
    try {
        if (!data.contains("token")) {
            return ApiResponse("error", 401, "缺少认证token", json::object());
        }
        
        std::string token = data["token"];
        int userId;
        UserType userType;
        
        if (!validateToken(token, userId, userType) || userType != UserType::DOCTOR) {
            return ApiResponse("error", 401, "无效的认证token", json::object());
        }
        
        auto doctor = hospitalService->getDoctorDAO()->getDoctorByUserId(userId);
        if (!doctor) {
            return ApiResponse("error", 404, "医生信息不存在", json::object());
        }
        
        auto appointments = hospitalService->getAppointmentDAO()->getAppointmentsByDoctorId(doctor->getDoctorId());
        
        json appointmentList = json::array();
        for (const auto& appointment : appointments) {
            auto patient = hospitalService->getPatientDAO()->getPatientById(appointment->getPatientId());
            
            json appt;
            appt["appointmentId"] = "appt_" + std::to_string(appointment->getAppointmentId());
            appt["patientName"] = patient ? patient->getName() : "未知患者";
            appt["appointmentTime"] = appointment->getAppointmentTime();
            appt["patientId"] = "pat_" + std::to_string(appointment->getPatientId());
            appt["status"] = appointment->statusToString();
            
            appointmentList.push_back(appt);
        }
        
        json responseData;
        responseData["appointments"] = appointmentList;
        
        return ApiResponse("success", 200, "获取预约列表成功", responseData);
        
    } catch (const std::exception& e) {
        return ApiResponse("error", 500, "获取预约列表失败", json::object());
    }
}

ApiHandler::ApiResponse ApiHandler::handleDoctorPatientGetMedicalRecords(const json& data) {
    try {
        if (!data.contains("token") || !data.contains("patientId")) {
            return ApiResponse("error", 400, "缺少必要参数", json::object());
        }
        
        std::string token = data["token"];
        int userId;
        UserType userType;
        
        if (!validateToken(token, userId, userType) || userType != UserType::DOCTOR) {
            return ApiResponse("error", 401, "无效的认证token", json::object());
        }
        
        std::string patientIdStr = data["patientId"];
        int patientId = std::stoi(patientIdStr.substr(4)); // 去掉 "pat_" 前缀
        
        auto patient = hospitalService->getPatientDAO()->getPatientById(patientId);
        if (!patient) {
            return ApiResponse("error", 404, "患者不存在", json::object());
        }
        
        auto cases = hospitalService->getCaseDAO()->getCasesByPatientId(patientId);
        
        json records = json::array();
        for (const auto& medicalCase : cases) {
            auto caseDoctor = hospitalService->getDoctorDAO()->getDoctorById(medicalCase->getDoctorId());
            
            json record;
            record["recordId"] = "rec_" + std::to_string(medicalCase->getCaseId());
            record["date"] = medicalCase->getDiagnosisDate();
            record["department"] = medicalCase->getDepartment();
            record["attendingDoctor"] = caseDoctor ? caseDoctor->getName() : "未知医生";
            record["diagnosis"] = medicalCase->getDiagnosis();
            record["doctorAdvice"] = medicalCase->getDiagnosis();
            
            records.push_back(record);
        }
        
        json responseData;
        responseData["records"] = records;
        
        return ApiResponse("success", 200, "获取患者病历成功", responseData);
        
    } catch (const std::exception& e) {
        return ApiResponse("error", 500, "获取患者病历失败", json::object());
    }
}

ApiHandler::ApiResponse ApiHandler::handleDoctorMedicalRecordCreate(const json& data) {
    try {
        if (!data.contains("token") || !data.contains("patientId") || 
            !data.contains("diagnosis") || !data.contains("doctorAdvice")) {
            return ApiResponse("error", 400, "缺少必要参数", json::object());
        }
        
        std::string token = data["token"];
        int userId;
        UserType userType;
        
        if (!validateToken(token, userId, userType) || userType != UserType::DOCTOR) {
            return ApiResponse("error", 401, "无效的认证token", json::object());
        }
        
        auto doctor = hospitalService->getDoctorDAO()->getDoctorByUserId(userId);
        if (!doctor) {
            return ApiResponse("error", 404, "医生信息不存在", json::object());
        }
        
        std::string patientIdStr = data["patientId"];
        int patientId = std::stoi(patientIdStr.substr(4)); // 去掉 "pat_" 前缀
        
        std::string diagnosis = data["diagnosis"];
        std::string doctorAdvice = data["doctorAdvice"];
        
        // 创建病例记录
        int caseId = hospitalService->createMedicalCase(patientId, doctor->getDepartment(), 
                                                       doctor->getDoctorId(), diagnosis + " " + doctorAdvice);
        
        if (caseId > 0) {
            json responseData;
            responseData["recordId"] = "rec_" + std::to_string(caseId);
            responseData["date"] = getCurrentDateTime();
            
            return ApiResponse("success", 201, "病历创建成功", responseData);
        }
        
        return ApiResponse("error", 500, "病历创建失败", json::object());
        
    } catch (const std::exception& e) {
        return ApiResponse("error", 500, "病历创建失败", json::object());
    }
}

ApiHandler::ApiResponse ApiHandler::handleDoctorPrescriptionCreate(const json& data) {
    try {
        if (!data.contains("token") || !data.contains("patientId") || !data.contains("medicines")) {
            return ApiResponse("error", 400, "缺少必要参数", json::object());
        }
        
        std::string token = data["token"];
        int userId;
        UserType userType;
        
        if (!validateToken(token, userId, userType) || userType != UserType::DOCTOR) {
            return ApiResponse("error", 401, "无效的认证token", json::object());
        }
        
        auto doctor = hospitalService->getDoctorDAO()->getDoctorByUserId(userId);
        if (!doctor) {
            return ApiResponse("error", 404, "医生信息不存在", json::object());
        }
        
        std::string patientIdStr = data["patientId"];
        int patientId = std::stoi(patientIdStr.substr(4)); // 去掉 "pat_" 前缀
        
        json medicines = data["medicines"];
        if (medicines.empty()) {
            return ApiResponse("error", 400, "药物列表不能为空", json::object());
        }
        
        // 获取患者最新的病例
        auto cases = hospitalService->getCaseDAO()->getCasesByPatientId(patientId);
        if (cases.empty()) {
            return ApiResponse("error", 404, "患者没有病例记录", json::object());
        }
        
        int caseId = cases[0]->getCaseId(); // 使用最新的病例
        
        // 构建处方内容
        std::stringstream prescriptionContent;
        for (const auto& medicine : medicines) {
            prescriptionContent << medicine["name"].get<std::string>() << " "
                              << medicine["dosage"].get<std::string>() << " "
                              << medicine["frequency"].get<std::string>() << "; ";
        }
        
        // 创建处方
        int prescriptionId = hospitalService->issuePrescription(caseId, doctor->getDoctorId(), 
                                                               prescriptionContent.str());
        
        if (prescriptionId > 0) {
            // 添加药物详情
            for (const auto& medicine : medicines) {
                hospitalService->addMedication(prescriptionId, 
                                             medicine["name"].get<std::string>(),
                                             medicine.contains("quantity") ? medicine["quantity"].get<int>() : 1,
                                             medicine["frequency"].get<std::string>());
            }
            
            json responseData;
            responseData["prescriptionId"] = "presc_" + std::to_string(prescriptionId);
            responseData["date"] = getCurrentDateTime();
            
            return ApiResponse("success", 201, "处方开具成功", responseData);
        }
        
        return ApiResponse("error", 500, "处方开具失败", json::object());
        
    } catch (const std::exception& e) {
        return ApiResponse("error", 500, "处方开具失败", json::object());
    }
}

ApiHandler::ApiResponse ApiHandler::handleDoctorLabResultUpload(const json& data) {
    try {
        if (!data.contains("token") || !data.contains("patientId") || 
            !data.contains("reportName") || !data.contains("fileContentBase64")) {
            return ApiResponse("error", 400, "缺少必要参数", json::object());
        }
        
        std::string token = data["token"];
        int userId;
        UserType userType;
        
        if (!validateToken(token, userId, userType) || userType != UserType::DOCTOR) {
            return ApiResponse("error", 401, "无效的认证token", json::object());
        }
        
        std::string patientIdStr = data["patientId"];
        std::string reportName = data["reportName"];
        std::string fileContentBase64 = data["fileContentBase64"];
        
        // 验证Base64格式
        if (fileContentBase64.empty()) {
            return ApiResponse("error", 400, "文件内容不能为空", json::object());
        }
        
        // 模拟文件上传成功
        json responseData;
        responseData["resultId"] = "lab_res_" + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count());
        responseData["reportUrl"] = "https://example.com/reports/" + responseData["resultId"].get<std::string>() + ".pdf";
        responseData["uploadTime"] = getCurrentDateTime();
        
        return ApiResponse("success", 201, "检验报告上传成功", responseData);
        
    } catch (const std::exception& e) {
        return ApiResponse("error", 500, "检验报告上传失败", json::object());
    }
}

ApiHandler::ApiResponse ApiHandler::handleDoctorStatusUpdate(const json& data) {
    try {
        if (!data.contains("token") || !data.contains("status")) {
            return ApiResponse("error", 400, "缺少必要参数", json::object());
        }
        
        std::string token = data["token"];
        int userId;
        UserType userType;
        
        if (!validateToken(token, userId, userType) || userType != UserType::DOCTOR) {
            return ApiResponse("error", 401, "无效的认证token", json::object());
        }
        
        std::string status = data["status"];
        if (status != "online" && status != "offline") {
            return ApiResponse("error", 400, "无效的状态值", json::object());
        }
        
        // 模拟状态更新成功
        json responseData;
        responseData["status"] = status;
        responseData["updateTime"] = getCurrentDateTime();
        
        return ApiResponse("success", 200, "状态更新成功", responseData);
        
    } catch (const std::exception& e) {
        return ApiResponse("error", 500, "状态更新失败", json::object());
    }
}

ApiHandler::ApiResponse ApiHandler::handleDoctorAttendanceCheckIn(const json& data) {
    try {
        if (!data.contains("token")) {
            return ApiResponse("error", 401, "缺少认证token", json::object());
        }
        
        std::string token = data["token"];
        int userId;
        UserType userType;
        
        if (!validateToken(token, userId, userType) || userType != UserType::DOCTOR) {
            return ApiResponse("error", 401, "无效的认证token", json::object());
        }
        
        // 模拟打卡成功
        json responseData;
        responseData["checkInTime"] = getCurrentDateTime();
        responseData["status"] = "checked_in";
        
        return ApiResponse("success", 200, "打卡成功", responseData);
        
    } catch (const std::exception& e) {
        return ApiResponse("error", 500, "打卡失败", json::object());
    }
}

ApiHandler::ApiResponse ApiHandler::handleDoctorAttendanceCancelCheckIn(const json& data) {
    try {
        if (!data.contains("token")) {
            return ApiResponse("error", 401, "缺少认证token", json::object());
        }
        
        std::string token = data["token"];
        int userId;
        UserType userType;
        
        if (!validateToken(token, userId, userType) || userType != UserType::DOCTOR) {
            return ApiResponse("error", 401, "无效的认证token", json::object());
        }
        
        // 模拟取消打卡成功
        json responseData;
        responseData["cancelTime"] = getCurrentDateTime();
        responseData["status"] = "cancelled";
        
        return ApiResponse("success", 200, "取消打卡成功", responseData);
        
    } catch (const std::exception& e) {
        return ApiResponse("error", 500, "取消打卡失败", json::object());
    }
}

ApiHandler::ApiResponse ApiHandler::handleDoctorAttendanceGetHistory(const json& data) {
    try {
        if (!data.contains("token")) {
            return ApiResponse("error", 401, "缺少认证token", json::object());
        }
        
        std::string token = data["token"];
        int userId;
        UserType userType;
        
        if (!validateToken(token, userId, userType) || userType != UserType::DOCTOR) {
            return ApiResponse("error", 401, "无效的认证token", json::object());
        }
        
        // 模拟考勤历史数据
        json history = json::array();
        json record;
        record["date"] = "2025-01-15";
        record["checkInTime"] = "08:30:00";
        record["checkOutTime"] = "17:30:00";
        record["status"] = "normal";
        
        history.push_back(record);
        
        json responseData;
        responseData["history"] = history;
        
        return ApiResponse("success", 200, "获取考勤历史成功", responseData);
        
    } catch (const std::exception& e) {
        return ApiResponse("error", 500, "获取考勤历史失败", json::object());
    }
}

ApiHandler::ApiResponse ApiHandler::handleDoctorLeaveRequestSubmit(const json& data) {
    try {
        if (!data.contains("token") || !data.contains("contactPhone") || 
            !data.contains("type") || !data.contains("startDate") || 
            !data.contains("endDate") || !data.contains("reason")) {
            return ApiResponse("error", 400, "缺少必要参数", json::object());
        }
        
        std::string token = data["token"];
        int userId;
        UserType userType;
        
        if (!validateToken(token, userId, userType) || userType != UserType::DOCTOR) {
            return ApiResponse("error", 401, "无效的认证token", json::object());
        }
        
        std::string leaveType = data["type"];
        if (leaveType != "因公请假" && leaveType != "因私请假") {
            return ApiResponse("error", 400, "无效的请假类型", json::object());
        }
        
        // 模拟请假申请提交成功
        json responseData;
        responseData["requestId"] = "leave_req_" + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count());
        responseData["status"] = "pending";
        responseData["submittedAt"] = getCurrentDateTime();
        
        return ApiResponse("success", 201, "请假申请提交成功", responseData);
        
    } catch (const std::exception& e) {
        return ApiResponse("error", 500, "请假申请提交失败", json::object());
    }
}

ApiHandler::ApiResponse ApiHandler::handleDoctorLeaveRequestList(const json& data) {
    try {
        if (!data.contains("token")) {
            return ApiResponse("error", 401, "缺少认证token", json::object());
        }
        
        std::string token = data["token"];
        int userId;
        UserType userType;
        
        if (!validateToken(token, userId, userType) || userType != UserType::DOCTOR) {
            return ApiResponse("error", 401, "无效的认证token", json::object());
        }
        
        // 模拟请假列表数据
        json requests = json::array();
        json request;
        request["requestId"] = "leave_req_123";
        request["type"] = "因私请假";
        request["startDate"] = "2025-09-15";
        request["endDate"] = "2025-09-16";
        request["reason"] = "家中有事需要处理";
        request["status"] = "pending";
        request["submittedAt"] = "2025-09-10T10:00:00Z";
        
        requests.push_back(request);
        
        json responseData;
        responseData["requests"] = requests;
        
        return ApiResponse("success", 200, "获取请假列表成功", responseData);
        
    } catch (const std::exception& e) {
        return ApiResponse("error", 500, "获取请假列表失败", json::object());
    }
}

ApiHandler::ApiResponse ApiHandler::handleDoctorLeaveRequestCancel(const json& data) {
    try {
        if (!data.contains("token") || !data.contains("requestId")) {
            return ApiResponse("error", 400, "缺少必要参数", json::object());
        }
        
        std::string token = data["token"];
        int userId;
        UserType userType;
        
        if (!validateToken(token, userId, userType) || userType != UserType::DOCTOR) {
            return ApiResponse("error", 401, "无效的认证token", json::object());
        }
        
        std::string requestId = data["requestId"];
        
        // 模拟销假成功
        json responseData;
        responseData["requestId"] = requestId;
        responseData["status"] = "cancelled";
        responseData["cancelledAt"] = getCurrentDateTime();
        
        return ApiResponse("success", 200, "销假成功", responseData);
        
    } catch (const std::exception& e) {
        return ApiResponse("error", 500, "销假失败", json::object());
    }
}

// 工具函数实现
bool ApiHandler::validateEmail(const std::string& email) {
    const std::regex emailPattern(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    return std::regex_match(email, emailPattern);
}

bool ApiHandler::validatePhone(const std::string& phone) {
    const std::regex phonePattern(R"(^1[3-9]\d{9}$)");
    return std::regex_match(phone, phonePattern);
}

bool ApiHandler::validatePassword(const std::string& password) {
    return password.length() >= 6;
}

bool ApiHandler::validateIdNumber(const std::string& idNumber) {
    const std::regex idPattern(R"(^\d{17}[\dXx]$)");
    return std::regex_match(idNumber, idPattern);
}

std::string ApiHandler::hashPassword(const std::string& password) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, password.c_str(), password.length());
    SHA256_Final(hash, &sha256);
    
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

std::string ApiHandler::getCurrentDateTime() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto tm = *std::localtime(&time_t);
    
    std::stringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

json ApiHandler::userToJson(const User& user) {
    json userJson;
    userJson["userId"] = user.getUserId();
    userJson["username"] = user.getUsername();
    userJson["userType"] = user.userTypeToString();
    userJson["email"] = user.getEmail();
    userJson["phoneNumber"] = user.getPhoneNumber();
    userJson["createdAt"] = user.getCreatedAt();
    userJson["isActive"] = user.getIsActive();
    return userJson;
}

json ApiHandler::doctorToJson(const Doctor& doctor) {
    json doctorJson;
    doctorJson["doctorId"] = doctor.getDoctorId();
    doctorJson["userId"] = doctor.getUserId();
    doctorJson["name"] = doctor.getName();
    doctorJson["department"] = doctor.getDepartment();
    doctorJson["title"] = doctor.getTitle();
    doctorJson["workingHours"] = doctor.getWorkingHours();
    doctorJson["profilePicture"] = doctor.getProfilePicture();
    return doctorJson;
}

json ApiHandler::patientToJson(const Patient& patient) {
    json patientJson;
    patientJson["patientId"] = patient.getPatientId();
    patientJson["userId"] = patient.getUserId();
    patientJson["name"] = patient.getName();
    patientJson["gender"] = patient.genderToString();
    patientJson["birthDate"] = patient.getBirthDate();
    patientJson["idNumber"] = patient.getIdNumber();
    patientJson["phoneNumber"] = patient.getPhoneNumber();
    patientJson["age"] = patient.getAge();
    return patientJson;
}

json ApiHandler::caseToJson(const Case& medicalCase) {
    json caseJson;
    caseJson["caseId"] = medicalCase.getCaseId();
    caseJson["patientId"] = medicalCase.getPatientId();
    caseJson["department"] = medicalCase.getDepartment();
    caseJson["doctorId"] = medicalCase.getDoctorId();
    caseJson["diagnosis"] = medicalCase.getDiagnosis();
    caseJson["diagnosisDate"] = medicalCase.getDiagnosisDate();
    return caseJson;
}

json ApiHandler::appointmentToJson(const Appointment& appointment) {
    json appointmentJson;
    appointmentJson["appointmentId"] = appointment.getAppointmentId();
    appointmentJson["patientId"] = appointment.getPatientId();
    appointmentJson["doctorId"] = appointment.getDoctorId();
    appointmentJson["appointmentTime"] = appointment.getAppointmentTime();
    appointmentJson["department"] = appointment.getDepartment();
    appointmentJson["status"] = appointment.statusToString();
    return appointmentJson;
}

json ApiHandler::prescriptionToJson(const Prescription& prescription) {
    json prescriptionJson;
    prescriptionJson["prescriptionId"] = prescription.getPrescriptionId();
    prescriptionJson["caseId"] = prescription.getCaseId();
    prescriptionJson["doctorId"] = prescription.getDoctorId();
    prescriptionJson["prescriptionContent"] = prescription.getPrescriptionContent();
    prescriptionJson["issuedDate"] = prescription.getIssuedDate();
    return prescriptionJson;
}

json ApiHandler::medicationToJson(const Medication& medication) {
    json medicationJson;
    medicationJson["medicationId"] = medication.getMedicationId();
    medicationJson["prescriptionId"] = medication.getPrescriptionId();
    medicationJson["medicationName"] = medication.getMedicationName();
    medicationJson["quantity"] = medication.getQuantity();
    medicationJson["usageInstructions"] = medication.getUsageInstructions();
    return medicationJson;
}

json ApiHandler::getSystemStats() {
    auto stats = hospitalService->getHospitalStats();
    
    json systemStats;
    systemStats["totalUsers"] = stats.totalUsers;
    systemStats["totalDoctors"] = stats.totalDoctors;
    systemStats["totalPatients"] = stats.totalPatients;
    systemStats["totalCases"] = stats.totalCases;
    systemStats["totalAppointments"] = stats.totalAppointments;
    systemStats["bookedAppointments"] = stats.bookedAppointments;
    systemStats["attendedAppointments"] = stats.attendedAppointments;
    systemStats["cancelledAppointments"] = stats.cancelledAppointments;
    systemStats["totalHospitalizations"] = stats.totalHospitalizations;
    systemStats["totalPrescriptions"] = stats.totalPrescriptions;
    systemStats["totalMedications"] = stats.totalMedications;
    
    return systemStats;
}