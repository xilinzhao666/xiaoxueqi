#ifndef DATABASE_SERVICE_H
#define DATABASE_SERVICE_H

#include <memory>
#include "DatabaseConnection.h"
#include "User.h"
#include "Doctor.h"
#include "Patient.h"
#include "Case.h"
#include "Appointment.h"
#include "Hospitalization.h"
#include "Prescription.h"
#include "Medication.h"

class DatabaseService {
private:
    std::shared_ptr<ConnectionPool> connectionPool;
    std::unique_ptr<UserDAO> userDAO;
    std::unique_ptr<DoctorDAO> doctorDAO;
    std::unique_ptr<PatientDAO> patientDAO;
    std::unique_ptr<CaseDAO> caseDAO;
    std::unique_ptr<AppointmentDAO> appointmentDAO;
    std::unique_ptr<HospitalizationDAO> hospitalizationDAO;
    std::unique_ptr<PrescriptionDAO> prescriptionDAO;
    std::unique_ptr<MedicationDAO> medicationDAO;
    
public:
    DatabaseService(const std::string& host, const std::string& username,
                   const std::string& password, const std::string& database,
                   unsigned int port = 3306, size_t maxConnections = 10);
    ~DatabaseService();
    
    // Initialize database
    bool initializeDatabase();
    bool createTables();
    bool dropTables();
    
    // Get DAO instances
    UserDAO* getUserDAO() { return userDAO.get(); }
    DoctorDAO* getDoctorDAO() { return doctorDAO.get(); }
    PatientDAO* getPatientDAO() { return patientDAO.get(); }
    CaseDAO* getCaseDAO() { return caseDAO.get(); }
    AppointmentDAO* getAppointmentDAO() { return appointmentDAO.get(); }
    HospitalizationDAO* getHospitalizationDAO() { return hospitalizationDAO.get(); }
    PrescriptionDAO* getPrescriptionDAO() { return prescriptionDAO.get(); }
    MedicationDAO* getMedicationDAO() { return medicationDAO.get(); }
    
    // Business logic methods
    bool registerUser(const std::string& username, const std::string& password, 
                     UserType userType, const std::string& email = "", 
                     const std::string& phoneNumber = "");
    
    std::unique_ptr<User> loginUser(const std::string& username, const std::string& password);
    
    bool registerDoctor(int userId, const std::string& name, const std::string& department, 
                       const std::string& workingHours, const std::string& title = "");
    
    bool registerPatient(int userId, const std::string& name, Gender gender, 
                        const std::string& birthDate, const std::string& idNumber, 
                        const std::string& phoneNumber = "");
    
    int createMedicalCase(int patientId, const std::string& department, int doctorId, 
                         const std::string& diagnosis);
    
    int bookAppointment(int patientId, int doctorId, const std::string& appointmentTime, 
                       const std::string& department);
    
    int admitPatient(int patientId, const std::string& wardNumber, const std::string& bedNumber, 
                    const std::string& attendingDoctor);
    
    int issuePrescription(int caseId, int doctorId, const std::string& prescriptionContent);
    
    bool addMedication(int prescriptionId, const std::string& medicationName, int quantity, 
                      const std::string& usageInstructions);
    
    // Statistics and reports
    struct DatabaseStats {
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
    };
    
    DatabaseStats getDatabaseStats();
    
    // Complex queries with joins
    struct PatientCaseInfo {
        int patientId;
        std::string patientName;
        std::string idNumber;
        int caseId;
        std::string diagnosis;
        std::string diagnosisDate;
        std::string doctorName;
        std::string department;
    };
    
    std::vector<PatientCaseInfo> getPatientCaseHistory(int patientId);
    
    struct DoctorAppointmentInfo {
        int doctorId;
        std::string doctorName;
        std::string department;
        int appointmentId;
        std::string patientName;
        std::string appointmentTime;
        std::string status;
    };
    
    std::vector<DoctorAppointmentInfo> getDoctorAppointments(int doctorId);
    
    // Data maintenance
    bool backupDatabase(const std::string& backupPath);
    bool optimizeTables();
    bool cleanupOldData(int daysOld = 365);
    
private:
    std::string hashPassword(const std::string& password);
};

#endif // DATABASE_SERVICE_H