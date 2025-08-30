#ifndef API_HANDLER_H
#define API_HANDLER_H

#include <string>
#include <memory>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <chrono>
#include "HospitalService.h"

// 尝试包含nlohmann/json，支持不同的安装路径
#if __has_include(<nlohmann/json.hpp>)
    #include <nlohmann/json.hpp>
#elif __has_include(<json/json.hpp>)
    #include <json/json.hpp>
#else
    #error "nlohmann/json library not found. Please install nlohmann-json3-dev package."
#endif

using json = nlohmann::json;

class ApiHandler {
public:
    // API响应结构
    struct ApiResponse {
        std::string status;
        int code;
        std::string message;
        json data;
        
        ApiResponse(const std::string& status = "error", int code = 500, 
                   const std::string& message = "Internal server error", 
                   const json& data = json::object())
            : status(status), code(code), message(message), data(data) {}
        
        std::string toJson() const;
    };

    // 用户会话管理
    struct UserSession {
        int userId;
        UserType userType;
        std::string username;
        std::chrono::system_clock::time_point loginTime;
        std::chrono::system_clock::time_point lastActivity;
        
        bool isValid() const;
        void updateActivity();
    };

private:
    std::shared_ptr<HospitalService> hospitalService;
    std::unordered_map<std::string, UserSession> activeSessions;
    std::mutex sessionMutex;
    
    // API处理函数类型定义
    using ApiHandlerFunc = std::function<ApiResponse(const json&)>;
    std::unordered_map<std::string, ApiHandlerFunc> apiHandlers;
    
    // 会话管理
    std::string generateToken(int userId, UserType userType, const std::string& username);
    bool validateToken(const std::string& token, UserSession& session);
    void cleanupExpiredSessions();
    
    // 输入验证
    bool validateEmail(const std::string& email);
    bool validatePhone(const std::string& phone);
    bool validatePassword(const std::string& password);
    bool validateIdNumber(const std::string& idNumber);
    
    // 公共接口处理函数
    ApiResponse handlePublicScheduleList(const json& data);
    ApiResponse handlePublicDoctorGet(const json& data);
    
    // 患者端接口处理函数
    ApiResponse handlePatientRegister(const json& data);
    ApiResponse handlePatientLogin(const json& data);
    ApiResponse handlePatientResetPassword(const json& data);
    ApiResponse handlePatientProfileGet(const json& data);
    ApiResponse handlePatientProfileUpdate(const json& data);
    ApiResponse handlePatientAppointmentCreate(const json& data);
    ApiResponse handlePatientMedicalRecordList(const json& data);
    ApiResponse handlePatientPrescriptionList(const json& data);
    ApiResponse handlePatientPrescriptionGet(const json& data);
    ApiResponse handlePatientLabResultList(const json& data);
    ApiResponse handlePatientChatSendMessage(const json& data);
    ApiResponse handlePatientChatGetHistory(const json& data);
    ApiResponse handlePatientAssessmentGetLink(const json& data);
    ApiResponse handlePatientConsultationRequestOnline(const json& data);
    
    // 医生端接口处理函数
    ApiResponse handleDoctorLogin(const json& data);
    ApiResponse handleDoctorResetPassword(const json& data);
    ApiResponse handleDoctorProfileGet(const json& data);
    ApiResponse handleDoctorProfileUpdate(const json& data);
    ApiResponse handleDoctorAppointmentList(const json& data);
    ApiResponse handleDoctorPatientGetMedicalRecords(const json& data);
    ApiResponse handleDoctorMedicalRecordCreate(const json& data);
    ApiResponse handleDoctorPrescriptionCreate(const json& data);
    ApiResponse handleDoctorLabResultUpload(const json& data);
    ApiResponse handleDoctorAttendanceCheckIn(const json& data);
    ApiResponse handleDoctorAttendanceGetHistory(const json& data);
    ApiResponse handleDoctorLeaveRequestSubmit(const json& data);
    ApiResponse handleDoctorLeaveRequestList(const json& data);
    ApiResponse handleDoctorLeaveRequestCancel(const json& data);
    ApiResponse handleDoctorStatusUpdate(const json& data);
    ApiResponse handleDoctorAttendanceCancelCheckIn(const json& data);
    
    // 工具函数
    std::string hashPassword(const std::string& password);
    std::string getCurrentDateTime();
    json userToJson(const User& user);
    json doctorToJson(const Doctor& doctor);
    json patientToJson(const Patient& patient);
    json caseToJson(const Case& medicalCase);
    json appointmentToJson(const Appointment& appointment);
    json prescriptionToJson(const Prescription& prescription);
    json medicationToJson(const Medication& medication);
    
public:
    explicit ApiHandler(std::shared_ptr<HospitalService> service);
    ~ApiHandler();
    
    // 主要接口函数
    std::string processApiRequest(const std::string& jsonInput);
    
    // 会话管理
    void logoutUser(const std::string& token);
    int getActiveSessionCount();
    
    // 系统状态
    json getSystemStats();
};

#endif // API_HANDLER_H