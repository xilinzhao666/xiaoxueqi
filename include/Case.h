#ifndef CASE_H
#define CASE_H

#include <string>
#include <vector>
#include <memory>
#include "DatabaseConnection.h"

class Case {
private:
    int caseId;
    int patientId;
    std::string department;
    int doctorId;
    std::string diagnosis;
    std::string diagnosisDate;

public:
    Case();
    Case(int patientId, const std::string& department, int doctorId, const std::string& diagnosis);
    
    // Getter methods
    int getCaseId() const { return caseId; }
    int getPatientId() const { return patientId; }
    std::string getDepartment() const { return department; }
    int getDoctorId() const { return doctorId; }
    std::string getDiagnosis() const { return diagnosis; }
    std::string getDiagnosisDate() const { return diagnosisDate; }
    
    // Setter methods
    void setCaseId(int id) { caseId = id; }
    void setPatientId(int id) { patientId = id; }
    void setDepartment(const std::string& dept) { department = dept; }
    void setDoctorId(int id) { doctorId = id; }
    void setDiagnosis(const std::string& diag) { diagnosis = diag; }
    void setDiagnosisDate(const std::string& date) { diagnosisDate = date; }
};

class CaseDAO {
private:
    std::shared_ptr<ConnectionPool> connectionPool;
    
public:
    CaseDAO(std::shared_ptr<ConnectionPool> pool);
    
    // CRUD operations
    bool createCase(const Case& medicalCase);
    std::unique_ptr<Case> getCaseById(int caseId);
    std::vector<std::unique_ptr<Case>> getCasesByPatientId(int patientId);
    std::vector<std::unique_ptr<Case>> getCasesByDoctorId(int doctorId);
    std::vector<std::unique_ptr<Case>> getCasesByDepartment(const std::string& department);
    std::vector<std::unique_ptr<Case>> getAllCases();
    
    bool updateCase(const Case& medicalCase);
    bool deleteCase(int caseId);
    
    // Search operations
    std::vector<std::unique_ptr<Case>> searchCases(const std::string& searchTerm);
    int getCaseCount();
    int getCaseCountByDoctor(int doctorId);
    int getCaseCountByPatient(int patientId);
    
private:
    Case* mapRowToCase(MYSQL_ROW row, unsigned long* lengths);
};

#endif // CASE_H