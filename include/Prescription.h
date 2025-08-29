#ifndef PRESCRIPTION_H
#define PRESCRIPTION_H

#include <string>
#include <vector>
#include <memory>
#include "DatabaseConnection.h"

class Prescription {
private:
    int prescriptionId;
    int caseId;
    int doctorId;
    std::string prescriptionContent;
    std::string issuedDate;

public:
    Prescription();
    Prescription(int caseId, int doctorId, const std::string& prescriptionContent);
    
    // Getter methods
    int getPrescriptionId() const { return prescriptionId; }
    int getCaseId() const { return caseId; }
    int getDoctorId() const { return doctorId; }
    std::string getPrescriptionContent() const { return prescriptionContent; }
    std::string getIssuedDate() const { return issuedDate; }
    
    // Setter methods
    void setPrescriptionId(int id) { prescriptionId = id; }
    void setCaseId(int id) { caseId = id; }
    void setDoctorId(int id) { doctorId = id; }
    void setPrescriptionContent(const std::string& content) { prescriptionContent = content; }
    void setIssuedDate(const std::string& date) { issuedDate = date; }
};

class PrescriptionDAO {
private:
    std::shared_ptr<ConnectionPool> connectionPool;
    
public:
    PrescriptionDAO(std::shared_ptr<ConnectionPool> pool);
    
    // CRUD operations
    bool createPrescription(const Prescription& prescription);
    std::unique_ptr<Prescription> getPrescriptionById(int prescriptionId);
    std::vector<std::unique_ptr<Prescription>> getPrescriptionsByCaseId(int caseId);
    std::vector<std::unique_ptr<Prescription>> getPrescriptionsByDoctorId(int doctorId);
    std::vector<std::unique_ptr<Prescription>> getAllPrescriptions();
    
    bool updatePrescription(const Prescription& prescription);
    bool deletePrescription(int prescriptionId);
    
    // Search operations
    std::vector<std::unique_ptr<Prescription>> searchPrescriptions(const std::string& searchTerm);
    int getPrescriptionCount();
    int getPrescriptionCountByDoctor(int doctorId);
    
private:
    Prescription* mapRowToPrescription(MYSQL_ROW row, unsigned long* lengths);
};

#endif // PRESCRIPTION_H