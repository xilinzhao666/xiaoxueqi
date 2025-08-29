#ifndef PATIENT_H
#define PATIENT_H

#include <string>
#include <vector>
#include <memory>
#include "DatabaseConnection.h"

enum class Gender {
    MALE,
    FEMALE
};

class Patient {
private:
    int patientId;
    int userId;
    std::string name;
    Gender gender;
    std::string birthDate;
    std::string idNumber;
    std::string phoneNumber;

public:
    Patient();
    Patient(int userId, const std::string& name, Gender gender, const std::string& birthDate, const std::string& idNumber);
    
    // Getter methods
    int getPatientId() const { return patientId; }
    int getUserId() const { return userId; }
    std::string getName() const { return name; }
    Gender getGender() const { return gender; }
    std::string getBirthDate() const { return birthDate; }
    std::string getIdNumber() const { return idNumber; }
    std::string getPhoneNumber() const { return phoneNumber; }
    
    // Setter methods
    void setPatientId(int id) { patientId = id; }
    void setUserId(int id) { userId = id; }
    void setName(const std::string& patientName) { name = patientName; }
    void setGender(Gender patientGender) { gender = patientGender; }
    void setBirthDate(const std::string& date) { birthDate = date; }
    void setIdNumber(const std::string& id) { idNumber = id; }
    void setPhoneNumber(const std::string& phone) { phoneNumber = phone; }
    
    // Utility methods
    std::string genderToString() const;
    static Gender stringToGender(const std::string& genderStr);
    int getAge() const;
};

class PatientDAO {
private:
    std::shared_ptr<ConnectionPool> connectionPool;
    
public:
    PatientDAO(std::shared_ptr<ConnectionPool> pool);
    
    // CRUD operations
    bool createPatient(const Patient& patient);
    std::unique_ptr<Patient> getPatientById(int patientId);
    std::unique_ptr<Patient> getPatientByUserId(int userId);
    std::unique_ptr<Patient> getPatientByIdNumber(const std::string& idNumber);
    std::vector<std::unique_ptr<Patient>> getAllPatients();
    
    bool updatePatient(const Patient& patient);
    bool deletePatient(int patientId);
    
    // Search operations
    std::vector<std::unique_ptr<Patient>> searchPatients(const std::string& searchTerm);
    std::vector<std::unique_ptr<Patient>> getPatientsByGender(Gender gender);
    bool patientExists(const std::string& idNumber);
    int getPatientCount();
    
private:
    Patient* mapRowToPatient(MYSQL_ROW row, unsigned long* lengths);
};

#endif // PATIENT_H