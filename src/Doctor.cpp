#include "Doctor.h"
#include <memory>
#include <sstream>
#include <iostream>

// Doctor class implementation
Doctor::Doctor() : doctorId(0), userId(0) {}

Doctor::Doctor(int userId, const std::string& name, const std::string& department, const std::string& workingHours)
    : doctorId(0), userId(userId), name(name), department(department), workingHours(workingHours) {}

// DoctorDAO class implementation
DoctorDAO::DoctorDAO(std::shared_ptr<ConnectionPool> pool) : connectionPool(pool) {}

bool DoctorDAO::createDoctor(const Doctor& doctor) {
    auto conn = connectionPool->getConnection();
    if (!conn) return false;
    
    std::stringstream query;
    query << "INSERT INTO doctors (user_id, name, department, title, working_hours, profile_picture) VALUES ("
          << doctor.getUserId() << ", "
          << "'" << conn->escapeString(doctor.getName()) << "', "
          << "'" << conn->escapeString(doctor.getDepartment()) << "', "
          << (doctor.getTitle().empty() ? "NULL" : "'" + conn->escapeString(doctor.getTitle()) + "'") << ", "
          << "'" << conn->escapeString(doctor.getWorkingHours()) << "', "
          << (doctor.getProfilePicture().empty() ? "NULL" : "'" + conn->escapeString(doctor.getProfilePicture()) + "'") << ")";
    
    bool result = conn->executeUpdate(query.str());
    connectionPool->returnConnection(std::move(conn));
    return result;
}

std::unique_ptr<Doctor> DoctorDAO::getDoctorById(int doctorId) {
    auto conn = connectionPool->getConnection();
    if (!conn) return nullptr;
    
    std::stringstream query;
    query << "SELECT doctor_id, user_id, name, department, title, working_hours, profile_picture "
          << "FROM doctors WHERE doctor_id = " << doctorId;
    
    MYSQL_RES* result = conn->executeQuery(query.str());
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return nullptr;
    }
    
    MYSQL_ROW row = mysql_fetch_row(result);
    std::unique_ptr<Doctor> doctor = nullptr;
    
    if (row) {
        unsigned long* lengths = mysql_fetch_lengths(result);
        doctor = std::unique_ptr<Doctor>(mapRowToDoctor(row, lengths));
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return doctor;
}

std::unique_ptr<Doctor> DoctorDAO::getDoctorByUserId(int userId) {
    auto conn = connectionPool->getConnection();
    if (!conn) return nullptr;
    
    std::stringstream query;
    query << "SELECT doctor_id, user_id, name, department, title, working_hours, profile_picture "
          << "FROM doctors WHERE user_id = " << userId;
    
    MYSQL_RES* result = conn->executeQuery(query.str());
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return nullptr;
    }
    
    MYSQL_ROW row = mysql_fetch_row(result);
    std::unique_ptr<Doctor> doctor = nullptr;
    
    if (row) {
        unsigned long* lengths = mysql_fetch_lengths(result);
        doctor = std::unique_ptr<Doctor>(mapRowToDoctor(row, lengths));
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return doctor;
}

std::vector<std::unique_ptr<Doctor>> DoctorDAO::getAllDoctors() {
    auto conn = connectionPool->getConnection();
    std::vector<std::unique_ptr<Doctor>> doctors;
    if (!conn) return doctors;
    
    std::string query = "SELECT doctor_id, user_id, name, department, title, working_hours, profile_picture "
                       "FROM doctors ORDER BY name";
    
    MYSQL_RES* result = conn->executeQuery(query);
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return doctors;
    }
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        unsigned long* lengths = mysql_fetch_lengths(result);
        doctors.push_back(std::unique_ptr<Doctor>(mapRowToDoctor(row, lengths)));
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return doctors;
}

std::vector<std::unique_ptr<Doctor>> DoctorDAO::getDoctorsByDepartment(const std::string& department) {
    auto conn = connectionPool->getConnection();
    std::vector<std::unique_ptr<Doctor>> doctors;
    if (!conn) return doctors;
    
    std::stringstream query;
    query << "SELECT doctor_id, user_id, name, department, title, working_hours, profile_picture "
          << "FROM doctors WHERE department = '" << conn->escapeString(department) << "' ORDER BY name";
    
    MYSQL_RES* result = conn->executeQuery(query.str());
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return doctors;
    }
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        unsigned long* lengths = mysql_fetch_lengths(result);
        doctors.push_back(std::unique_ptr<Doctor>(mapRowToDoctor(row, lengths)));
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return doctors;
}

bool DoctorDAO::updateDoctor(const Doctor& doctor) {
    auto conn = connectionPool->getConnection();
    if (!conn) return false;
    
    std::stringstream query;
    query << "UPDATE doctors SET "
          << "name = '" << conn->escapeString(doctor.getName()) << "', "
          << "department = '" << conn->escapeString(doctor.getDepartment()) << "', "
          << "title = " << (doctor.getTitle().empty() ? "NULL" : "'" + conn->escapeString(doctor.getTitle()) + "'") << ", "
          << "working_hours = '" << conn->escapeString(doctor.getWorkingHours()) << "', "
          << "profile_picture = " << (doctor.getProfilePicture().empty() ? "NULL" : "'" + conn->escapeString(doctor.getProfilePicture()) + "'") << " "
          << "WHERE doctor_id = " << doctor.getDoctorId();
    
    bool result = conn->executeUpdate(query.str());
    connectionPool->returnConnection(std::move(conn));
    return result;
}

bool DoctorDAO::deleteDoctor(int doctorId) {
    auto conn = connectionPool->getConnection();
    if (!conn) return false;
    
    std::stringstream query;
    query << "DELETE FROM doctors WHERE doctor_id = " << doctorId;
    
    bool result = conn->executeUpdate(query.str());
    connectionPool->returnConnection(std::move(conn));
    return result;
}

std::vector<std::unique_ptr<Doctor>> DoctorDAO::searchDoctors(const std::string& searchTerm) {
    auto conn = connectionPool->getConnection();
    std::vector<std::unique_ptr<Doctor>> doctors;
    if (!conn) return doctors;
    
    std::stringstream query;
    query << "SELECT doctor_id, user_id, name, department, title, working_hours, profile_picture "
          << "FROM doctors WHERE name LIKE '%" << conn->escapeString(searchTerm) << "%' "
          << "OR department LIKE '%" << conn->escapeString(searchTerm) << "%' "
          << "OR title LIKE '%" << conn->escapeString(searchTerm) << "%' "
          << "ORDER BY name";
    
    MYSQL_RES* result = conn->executeQuery(query.str());
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return doctors;
    }
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        unsigned long* lengths = mysql_fetch_lengths(result);
        doctors.push_back(std::unique_ptr<Doctor>(mapRowToDoctor(row, lengths)));
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return doctors;
}

std::vector<std::string> DoctorDAO::getAllDepartments() {
    auto conn = connectionPool->getConnection();
    std::vector<std::string> departments;
    if (!conn) return departments;
    
    std::string query = "SELECT DISTINCT department FROM doctors ORDER BY department";
    
    MYSQL_RES* result = conn->executeQuery(query);
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return departments;
    }
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        if (row[0]) {
            departments.push_back(std::string(row[0]));
        }
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return departments;
}

int DoctorDAO::getDoctorCount() {
    auto conn = connectionPool->getConnection();
    if (!conn) return 0;
    
    std::string query = "SELECT COUNT(*) FROM doctors";
    MYSQL_RES* result = conn->executeQuery(query);
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return 0;
    }
    
    MYSQL_ROW row = mysql_fetch_row(result);
    int count = 0;
    if (row && row[0]) {
        count = std::stoi(row[0]);
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return count;
}

int DoctorDAO::getDoctorCountByDepartment(const std::string& department) {
    auto conn = connectionPool->getConnection();
    if (!conn) return 0;
    
    std::stringstream query;
    query << "SELECT COUNT(*) FROM doctors WHERE department = '" << conn->escapeString(department) << "'";
    
    MYSQL_RES* result = conn->executeQuery(query.str());
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return 0;
    }
    
    MYSQL_ROW row = mysql_fetch_row(result);
    int count = 0;
    if (row && row[0]) {
        count = std::stoi(row[0]);
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return count;
}

Doctor* DoctorDAO::mapRowToDoctor(MYSQL_ROW row, unsigned long* lengths) {
    if (!row) return nullptr;
    
    Doctor* doctor = new Doctor();
    
    if (row[0]) doctor->setDoctorId(std::stoi(row[0]));
    if (row[1]) doctor->setUserId(std::stoi(row[1]));
    if (row[2]) doctor->setName(std::string(row[2], lengths[2]));
    if (row[3]) doctor->setDepartment(std::string(row[3], lengths[3]));
    if (row[4]) doctor->setTitle(std::string(row[4], lengths[4]));
    if (row[5]) doctor->setWorkingHours(std::string(row[5], lengths[5]));
    if (row[6]) doctor->setProfilePicture(std::string(row[6], lengths[6]));
    
    return doctor;
}