#ifndef DOCTOR_H
#define DOCTOR_H

#include <string>
#include <vector>
#include <memory>
#include "DatabaseConnection.h"

class Doctor {
private:
    int doctorId;
    int userId;
    std::string name;
    std::string department;
    std::string title;
    std::string workingHours;
    std::string profilePicture;

public:
    Doctor();
    Doctor(int userId, const std::string& name, const std::string& department, const std::string& workingHours);
    
    // Getter methods
    int getDoctorId() const { return doctorId; }
    int getUserId() const { return userId; }
    std::string getName() const { return name; }
    std::string getDepartment() const { return department; }
    std::string getTitle() const { return title; }
    std::string getWorkingHours() const { return workingHours; }
    std::string getProfilePicture() const { return profilePicture; }
    
    // Setter methods
    void setDoctorId(int id) { doctorId = id; }
    void setUserId(int id) { userId = id; }
    void setName(const std::string& doctorName) { name = doctorName; }
    void setDepartment(const std::string& dept) { department = dept; }
    void setTitle(const std::string& doctorTitle) { title = doctorTitle; }
    void setWorkingHours(const std::string& hours) { workingHours = hours; }
    void setProfilePicture(const std::string& picture) { profilePicture = picture; }
};

class DoctorDAO {
private:
    std::shared_ptr<ConnectionPool> connectionPool;
    
public:
    DoctorDAO(std::shared_ptr<ConnectionPool> pool);
    
    // CRUD operations
    bool createDoctor(const Doctor& doctor);
    std::unique_ptr<Doctor> getDoctorById(int doctorId);
    std::unique_ptr<Doctor> getDoctorByUserId(int userId);
    std::vector<std::unique_ptr<Doctor>> getAllDoctors();
    std::vector<std::unique_ptr<Doctor>> getDoctorsByDepartment(const std::string& department);
    
    bool updateDoctor(const Doctor& doctor);
    bool deleteDoctor(int doctorId);
    
    // Search operations
    std::vector<std::unique_ptr<Doctor>> searchDoctors(const std::string& searchTerm);
    std::vector<std::string> getAllDepartments();
    int getDoctorCount();
    int getDoctorCountByDepartment(const std::string& department);
    
private:
    Doctor* mapRowToDoctor(MYSQL_ROW row, unsigned long* lengths);
};

#endif // DOCTOR_H