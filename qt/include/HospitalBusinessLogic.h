#ifndef HOSPITAL_BUSINESS_LOGIC_H
#define HOSPITAL_BUSINESS_LOGIC_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QList>
#include <QMap>
#include <QDateTime>
#include <QStringList>
#include <memory>
#include "DatabaseManager.h"

// Enums
enum class UserType {
    DOCTOR,
    PATIENT
};

enum class Gender {
    MALE,
    FEMALE
};

enum class AppointmentStatus {
    BOOKED,
    ATTENDED,
    CANCELLED
};

// Data structures
struct UserInfo {
    int userId;
    QString username;
    UserType userType;
    QString email;
    QString phoneNumber;
    QString createdAt;
    
    UserInfo() : userId(0), userType(UserType::PATIENT) {}
};

struct DoctorInfo {
    int doctorId;
    int userId;
    QString name;
    QString department;
    QString title;
    QString workingHours;
    QString profilePicture;
    
    DoctorInfo() : doctorId(0), userId(0) {}
};

struct PatientInfo {
    int patientId;
    int userId;
    QString name;
    Gender gender;
    QString birthDate;
    QString idNumber;
    QString phoneNumber;
    int age;
    
    PatientInfo() : patientId(0), userId(0), gender(Gender::MALE), age(0) {}
};

struct CaseInfo {
    int caseId;
    int patientId;
    QString patientName;
    QString department;
    int doctorId;
    QString doctorName;
    QString diagnosis;
    QString diagnosisDate;
    
    CaseInfo() : caseId(0), patientId(0), doctorId(0) {}
};

struct AppointmentInfo {
    int appointmentId;
    int patientId;
    QString patientName;
    int doctorId;
    QString doctorName;
    QString appointmentTime;
    QString department;
    AppointmentStatus status;
    
    AppointmentInfo() : appointmentId(0), patientId(0), doctorId(0), status(AppointmentStatus::BOOKED) {}
};

struct PrescriptionInfo {
    int prescriptionId;
    int caseId;
    int doctorId;
    QString doctorName;
    QString prescriptionContent;
    QString issuedDate;
    QStringList medications;
    
    PrescriptionInfo() : prescriptionId(0), caseId(0), doctorId(0) {}
};

struct MedicationInfo {
    int medicationId;
    int prescriptionId;
    QString medicationName;
    int quantity;
    QString usageInstructions;
    
    MedicationInfo() : medicationId(0), prescriptionId(0), quantity(0) {}
};

struct HospitalStats {
    int totalUsers;
    int totalDoctors;
    int totalPatients;
    int totalCases;
    int totalAppointments;
    int bookedAppointments;
    int attendedAppointments;
    int cancelledAppointments;
    int totalHospitalizations;
    int totalPrescriptions;
    int totalMedications;
    
    HospitalStats() : totalUsers(0), totalDoctors(0), totalPatients(0), totalCases(0),
                     totalAppointments(0), bookedAppointments(0), attendedAppointments(0),
                     cancelledAppointments(0), totalHospitalizations(0), totalPrescriptions(0),
                     totalMedications(0) {}
};

// Business result template
template<typename T>
class BusinessResult {
private:
    bool success;
    T data;
    QString message;
    QString errorCode;

public:
    BusinessResult(bool success, const T& data, const QString& message, const QString& errorCode = "")
        : success(success), data(data), message(message), errorCode(errorCode) {}
    
    static BusinessResult<T> successResult(const T& data, const QString& message = "操作成功") {
        return BusinessResult<T>(true, data, message);
    }
    
    static BusinessResult<T> failureResult(const QString& message, const QString& errorCode = "BUSINESS_ERROR") {
        return BusinessResult<T>(false, T{}, message, errorCode);
    }
    
    bool isSuccess() const { return success; }
    const T& getData() const { return data; }
    const QString& getMessage() const { return message; }
    const QString& getErrorCode() const { return errorCode; }
};

class HospitalBusinessLogic : public QObject {
    Q_OBJECT
    
private:
    std::shared_ptr<DatabaseManager> dbManager;
    
public:
    explicit HospitalBusinessLogic(std::shared_ptr<DatabaseManager> manager, QObject* parent = nullptr);
    
    // Database connection
    bool connectToDatabase(const DatabaseConfig& config);
    bool isConnected() const;
    
    // User Management
    BusinessResult<int> registerUser(const QString& username, const QString& password, 
                                   UserType userType, const QString& email = "", 
                                   const QString& phoneNumber = "");
    BusinessResult<UserInfo> authenticateUser(const QString& username, const QString& password);
    BusinessResult<QList<UserInfo>> getAllUsers();
    BusinessResult<UserInfo> getUserById(int userId);
    
    // Doctor Management
    BusinessResult<int> registerDoctor(int userId, const QString& name, const QString& department,
                                     const QString& workingHours, const QString& title = "");
    BusinessResult<DoctorInfo> getDoctorById(int doctorId);
    BusinessResult<QList<DoctorInfo>> getAllDoctors();
    BusinessResult<QList<DoctorInfo>> getDoctorsByDepartment(const QString& department);
    BusinessResult<QStringList> getAllDepartments();
    
    // Patient Management
    BusinessResult<int> registerPatient(int userId, const QString& name, Gender gender,
                                      const QString& birthDate, const QString& idNumber,
                                      const QString& phoneNumber = "");
    BusinessResult<PatientInfo> getPatientById(int patientId);
    BusinessResult<PatientInfo> getPatientByIdNumber(const QString& idNumber);
    BusinessResult<QList<PatientInfo>> getAllPatients();
    
    // Case Management
    BusinessResult<int> createMedicalCase(int patientId, const QString& department, 
                                        int doctorId, const QString& diagnosis);
    BusinessResult<CaseInfo> getCaseById(int caseId);
    BusinessResult<QList<CaseInfo>> getPatientCaseHistory(int patientId);
    BusinessResult<QList<CaseInfo>> getDoctorCases(int doctorId);
    
    // Appointment Management
    BusinessResult<int> bookAppointment(int patientId, int doctorId, const QString& appointmentTime,
                                      const QString& department);
    BusinessResult<AppointmentInfo> getAppointmentById(int appointmentId);
    BusinessResult<QList<AppointmentInfo>> getPatientAppointments(int patientId);
    BusinessResult<QList<AppointmentInfo>> getDoctorAppointments(int doctorId);
    BusinessResult<bool> updateAppointmentStatus(int appointmentId, AppointmentStatus status);
    
    // Prescription Management
    BusinessResult<int> issuePrescription(int caseId, int doctorId, const QString& prescriptionContent);
    BusinessResult<PrescriptionInfo> getPrescriptionById(int prescriptionId);
    BusinessResult<QList<PrescriptionInfo>> getCasePrescriptions(int caseId);
    
    // Medication Management
    BusinessResult<bool> addMedication(int prescriptionId, const QString& medicationName,
                                     int quantity, const QString& usageInstructions);
    BusinessResult<QList<MedicationInfo>> getPrescriptionMedications(int prescriptionId);
    
    // Statistics
    BusinessResult<HospitalStats> getHospitalStatistics();
    
    // Utility methods
    static QString userTypeToString(UserType type);
    static UserType stringToUserType(const QString& typeStr);
    static QString genderToString(Gender gender);
    static Gender stringToGender(const QString& genderStr);
    static QString appointmentStatusToString(AppointmentStatus status);
    static AppointmentStatus stringToAppointmentStatus(const QString& statusStr);
    
private:
    // Helper methods
    bool isValidEmail(const QString& email);
    bool isValidPhoneNumber(const QString& phoneNumber);
    bool isValidIdNumber(const QString& idNumber);
    QString hashPassword(const QString& password);
    int calculateAge(const QString& birthDate);
    
signals:
    void operationCompleted(const QString& operation, bool success, const QString& message);
    void errorOccurred(const QString& error);
};

#endif // HOSPITAL_BUSINESS_LOGIC_H