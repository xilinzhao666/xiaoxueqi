#ifndef HOSPITALIZATION_H
#define HOSPITALIZATION_H

#include <string>
#include <vector>
#include <memory>
#include "DatabaseConnection.h"

class Hospitalization {
private:
    int hospitalizationId;
    int patientId;
    std::string wardNumber;
    std::string bedNumber;
    std::string admissionDate;
    std::string attendingDoctor;

public:
    Hospitalization();
    Hospitalization(int patientId, const std::string& wardNumber, const std::string& bedNumber, const std::string& attendingDoctor);
    
    // Getter methods
    int getHospitalizationId() const { return hospitalizationId; }
    int getPatientId() const { return patientId; }
    std::string getWardNumber() const { return wardNumber; }
    std::string getBedNumber() const { return bedNumber; }
    std::string getAdmissionDate() const { return admissionDate; }
    std::string getAttendingDoctor() const { return attendingDoctor; }
    
    // Setter methods
    void setHospitalizationId(int id) { hospitalizationId = id; }
    void setPatientId(int id) { patientId = id; }
    void setWardNumber(const std::string& ward) { wardNumber = ward; }
    void setBedNumber(const std::string& bed) { bedNumber = bed; }
    void setAdmissionDate(const std::string& date) { admissionDate = date; }
    void setAttendingDoctor(const std::string& doctor) { attendingDoctor = doctor; }
};

class HospitalizationDAO {
private:
    std::shared_ptr<ConnectionPool> connectionPool;
    
public:
    HospitalizationDAO(std::shared_ptr<ConnectionPool> pool);
    
    // CRUD operations
    bool createHospitalization(const Hospitalization& hospitalization);
    std::unique_ptr<Hospitalization> getHospitalizationById(int hospitalizationId);
    std::vector<std::unique_ptr<Hospitalization>> getHospitalizationsByPatientId(int patientId);
    std::vector<std::unique_ptr<Hospitalization>> getHospitalizationsByWard(const std::string& wardNumber);
    std::vector<std::unique_ptr<Hospitalization>> getAllHospitalizations();
    
    bool updateHospitalization(const Hospitalization& hospitalization);
    bool deleteHospitalization(int hospitalizationId);
    
    // Ward and bed management
    std::vector<std::string> getAvailableBeds(const std::string& wardNumber);
    bool isBedOccupied(const std::string& wardNumber, const std::string& bedNumber);
    std::vector<std::string> getAllWards();
    
    // Search operations
    std::vector<std::unique_ptr<Hospitalization>> searchHospitalizations(const std::string& searchTerm);
    int getHospitalizationCount();
    int getCurrentHospitalizationCount();
    
private:
    Hospitalization* mapRowToHospitalization(MYSQL_ROW row, unsigned long* lengths);
};

#endif // HOSPITALIZATION_H