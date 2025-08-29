#ifndef BUSINESS_LOGIC_H
#define BUSINESS_LOGIC_H

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include "HospitalService.h"
#include "User.h"
#include "Doctor.h"
#include "Patient.h"
#include "Case.h"
#include "Appointment.h"
#include "Hospitalization.h"
#include "Prescription.h"
#include "Medication.h"

// Business result wrapper for error handling
template<typename T>
class BusinessResult {
private:
    bool success;
    T data;
    std::string message;
    std::string errorCode;

public:
    BusinessResult(bool success, const T& data, const std::string& message, const std::string& errorCode = "")
        : success(success), data(data), message(message), errorCode(errorCode) {}
    
    static BusinessResult<T> success(const T& data, const std::string& message = "操作成功") {
        return BusinessResult<T>(true, data, message);
    }
    
    static BusinessResult<T> failure(const std::string& message, const std::string& errorCode = "BUSINESS_ERROR") {
        return BusinessResult<T>(false, T{}, message, errorCode);
    }
    
    bool isSuccess() const { return success; }
    const T& getData() const { return data; }
    const std::string& getMessage() const { return message; }
    const std::string& getErrorCode() const { return errorCode; }
};

// Data Transfer Objects (DTOs)
struct UserRegistrationRequest {
    std::string username;
    std::string password;
    UserType userType;
    std::string email;
    std::string phoneNumber;
};

struct UserInfo {
    int userId;
    std::string username;
    UserType userType;
    std::string email;
    std::string phoneNumber;
    std::string createdAt;
};

struct DoctorRegistrationRequest {
    int userId;
    std::string name;
    std::string department;
    std::string title;
    std::string workingHours;
};

struct DoctorInfo {
    int doctorId;
    int userId;
    std::string name;
    std::string department;
    std::string title;
    std::string workingHours;
    std::string profilePicture;
};

struct PatientRegistrationRequest {
    int userId;
    std::string name;
    Gender gender;
    std::string birthDate;
    std::string idNumber;
    std::string phoneNumber;
};

struct PatientInfo {
    int patientId;
    int userId;
    std::string name;
    Gender gender;
    std::string birthDate;
    std::string idNumber;
    std::string phoneNumber;
    int age;
};

struct CaseCreationRequest {
    int patientId;
    std::string department;
    int doctorId;
    std::string diagnosis;
};

struct CaseInfo {
    int caseId;
    int patientId;
    std::string patientName;
    std::string department;
    int doctorId;
    std::string doctorName;
    std::string diagnosis;
    std::string diagnosisDate;
};

struct AppointmentBookingRequest {
    int patientId;
    int doctorId;
    std::string appointmentTime;
    std::string department;
};

struct AppointmentInfo {
    int appointmentId;
    int patientId;
    std::string patientName;
    int doctorId;
    std::string doctorName;
    std::string appointmentTime;
    std::string department;
    AppointmentStatus status;
};

struct HospitalizationRequest {
    int patientId;
    std::string wardNumber;
    std::string bedNumber;
    std::string attendingDoctor;
};

struct HospitalizationInfo {
    int hospitalizationId;
    int patientId;
    std::string patientName;
    std::string wardNumber;
    std::string bedNumber;
    std::string admissionDate;
    std::string attendingDoctor;
};

struct PrescriptionRequest {
    int caseId;
    int doctorId;
    std::string prescriptionContent;
};

struct PrescriptionInfo {
    int prescriptionId;
    int caseId;
    int doctorId;
    std::string doctorName;
    std::string prescriptionContent;
    std::string issuedDate;
    std::vector<std::string> medications;
};

struct MedicationRequest {
    int prescriptionId;
    std::string medicationName;
    int quantity;
    std::string usageInstructions;
};

struct MedicationInfo {
    int medicationId;
    int prescriptionId;
    std::string medicationName;
    int quantity;
    std::string usageInstructions;
};

// Business Statistics
struct BusinessStatistics {
    int totalUsers;
    int totalDoctors;
    int totalPatients;
    int totalCases;
    int totalAppointments;
    int bookedAppointments;
    int attendedAppointments;
    int cancelledAppointments;
    int totalHospitalizations;
    int currentHospitalizations;
    int totalPrescriptions;
    int totalMedications;
    
    // Department statistics
    std::vector<std::pair<std::string, int>> departmentCaseCounts;
    std::vector<std::pair<std::string, int>> departmentDoctorCounts;
    
    // Doctor performance
    std::vector<std::pair<std::string, int>> topDoctorsByCases;
    std::vector<std::pair<std::string, int>> topDoctorsByAppointments;
};

// Main Business Logic Class
class HospitalBusinessLogic {
private:
    std::shared_ptr<HospitalService> hospitalService;
    
public:
    explicit HospitalBusinessLogic(std::shared_ptr<HospitalService> service);
    
    // User Management Business Logic
    BusinessResult<int> registerNewUser(const UserRegistrationRequest& request);
    BusinessResult<UserInfo> authenticateUser(const std::string& username, const std::string& password);
    BusinessResult<bool> changeUserPassword(int userId, const std::string& oldPassword, const std::string& newPassword);
    BusinessResult<std::vector<UserInfo>> getAllUsers();
    BusinessResult<UserInfo> getUserById(int userId);
    
    // Doctor Management Business Logic
    BusinessResult<int> registerDoctor(const DoctorRegistrationRequest& request);
    BusinessResult<DoctorInfo> getDoctorById(int doctorId);
    BusinessResult<std::vector<DoctorInfo>> getAllDoctors();
    BusinessResult<std::vector<DoctorInfo>> getDoctorsByDepartment(const std::string& department);
    BusinessResult<std::vector<std::string>> getAllDepartments();
    BusinessResult<std::vector<DoctorInfo>> searchDoctors(const std::string& searchTerm);
    
    // Patient Management Business Logic
    BusinessResult<int> registerPatient(const PatientRegistrationRequest& request);
    BusinessResult<PatientInfo> getPatientById(int patientId);
    BusinessResult<PatientInfo> getPatientByIdNumber(const std::string& idNumber);
    BusinessResult<std::vector<PatientInfo>> getAllPatients();
    BusinessResult<std::vector<PatientInfo>> searchPatients(const std::string& searchTerm);
    
    // Case Management Business Logic
    BusinessResult<int> createMedicalCase(const CaseCreationRequest& request);
    BusinessResult<CaseInfo> getCaseById(int caseId);
    BusinessResult<std::vector<CaseInfo>> getPatientCaseHistory(int patientId);
    BusinessResult<std::vector<CaseInfo>> getDoctorCases(int doctorId);
    BusinessResult<std::vector<CaseInfo>> getCasesByDepartment(const std::string& department);
    BusinessResult<std::vector<CaseInfo>> searchCases(const std::string& searchTerm);
    
    // Appointment Management Business Logic
    BusinessResult<int> bookAppointment(const AppointmentBookingRequest& request);
    BusinessResult<AppointmentInfo> getAppointmentById(int appointmentId);
    BusinessResult<std::vector<AppointmentInfo>> getPatientAppointments(int patientId);
    BusinessResult<std::vector<AppointmentInfo>> getDoctorAppointments(int doctorId);
    BusinessResult<std::vector<AppointmentInfo>> getAppointmentsByDepartment(const std::string& department);
    BusinessResult<bool> updateAppointmentStatus(int appointmentId, AppointmentStatus status);
    BusinessResult<bool> cancelAppointment(int appointmentId);
    
    // Hospitalization Management Business Logic
    BusinessResult<int> admitPatient(const HospitalizationRequest& request);
    BusinessResult<HospitalizationInfo> getHospitalizationById(int hospitalizationId);
    BusinessResult<std::vector<HospitalizationInfo>> getPatientHospitalizations(int patientId);
    BusinessResult<std::vector<HospitalizationInfo>> getHospitalizationsByWard(const std::string& wardNumber);
    BusinessResult<std::vector<std::string>> getAvailableWards();
    BusinessResult<bool> dischargePatient(int hospitalizationId);
    
    // Prescription Management Business Logic
    BusinessResult<int> issuePrescription(const PrescriptionRequest& request);
    BusinessResult<PrescriptionInfo> getPrescriptionById(int prescriptionId);
    BusinessResult<std::vector<PrescriptionInfo>> getCasePrescriptions(int caseId);
    BusinessResult<std::vector<PrescriptionInfo>> getDoctorPrescriptions(int doctorId);
    
    // Medication Management Business Logic
    BusinessResult<bool> addMedicationToPrescription(const MedicationRequest& request);
    BusinessResult<std::vector<MedicationInfo>> getPrescriptionMedications(int prescriptionId);
    BusinessResult<std::vector<MedicationInfo>> searchMedications(const std::string& searchTerm);
    
    // Business Analytics and Reports
    BusinessResult<BusinessStatistics> getHospitalStatistics();
    BusinessResult<std::vector<CaseInfo>> getRecentCases(int days = 30);
    BusinessResult<std::vector<AppointmentInfo>> getTodayAppointments();
    BusinessResult<std::vector<AppointmentInfo>> getUpcomingAppointments(int days = 7);
    
    // Complex Business Operations
    BusinessResult<bool> completePatientVisit(int appointmentId, const std::string& diagnosis);
    BusinessResult<std::vector<CaseInfo>> getPatientMedicalHistory(int patientId);
    BusinessResult<bool> transferPatient(int hospitalizationId, const std::string& newWard, const std::string& newBed);
    
    // Data Validation and Business Rules
    bool validateAppointmentTime(const std::string& appointmentTime, int doctorId);
    bool validateDoctorAvailability(int doctorId, const std::string& appointmentTime);
    bool validatePatientEligibility(int patientId);
    
private:
    // Helper methods for validation
    bool isValidEmail(const std::string& email);
    bool isValidPhoneNumber(const std::string& phoneNumber);
    bool isValidIdNumber(const std::string& idNumber);
    bool isValidDate(const std::string& date);
    bool isWorkingHours(const std::string& time, const std::string& workingHours);
    
    // Helper methods for data conversion
    UserInfo convertToUserInfo(const User& user);
    DoctorInfo convertToDoctorInfo(const Doctor& doctor);
    PatientInfo convertToPatientInfo(const Patient& patient);
    CaseInfo convertToCaseInfo(const Case& medicalCase, const std::string& patientName = "", const std::string& doctorName = "");
    AppointmentInfo convertToAppointmentInfo(const Appointment& appointment, const std::string& patientName = "", const std::string& doctorName = "");
    HospitalizationInfo convertToHospitalizationInfo(const Hospitalization& hospitalization, const std::string& patientName = "");
    PrescriptionInfo convertToPrescriptionInfo(const Prescription& prescription, const std::string& doctorName = "");
    MedicationInfo convertToMedicationInfo(const Medication& medication);
    
    // Business rule enforcement
    bool enforceBusinessRules();
    bool validateBusinessConstraints();
    
    // Logging and audit
    void logBusinessOperation(const std::string& operation, int userId, const std::string& details);
    void auditDataChange(const std::string& tableName, int recordId, const std::string& operation);
};

#endif // BUSINESS_LOGIC_H