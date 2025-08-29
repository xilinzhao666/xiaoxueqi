#include "Patient.h"
#include <sstream>
#include <iostream>
#include <ctime>

// Patient class implementation
Patient::Patient() : patientId(0), userId(0), gender(Gender::MALE) {}

Patient::Patient(int userId, const std::string& name, Gender gender, const std::string& birthDate, const std::string& idNumber)
    : patientId(0), userId(userId), name(name), gender(gender), birthDate(birthDate), idNumber(idNumber) {}

std::string Patient::genderToString() const {
    switch (gender) {
        case Gender::MALE: return "Male";
        case Gender::FEMALE: return "Female";
    }
    return "Male";
}

Gender Patient::stringToGender(const std::string& genderStr) {
    if (genderStr == "Female") return Gender::FEMALE;
    return Gender::MALE;
}

int Patient::getAge() const {
    // Simple age calculation - in real implementation, use proper date library
    time_t now = time(0);
    tm* ltm = localtime(&now);
    int currentYear = 1900 + ltm->tm_year;
    
    // Extract year from birthDate (assuming YYYY-MM-DD format)
    if (birthDate.length() >= 4) {
        int birthYear = std::stoi(birthDate.substr(0, 4));
        return currentYear - birthYear;
    }
    return 0;
}

// PatientDAO class implementation
PatientDAO::PatientDAO(std::shared_ptr<ConnectionPool> pool) : connectionPool(pool) {}

bool PatientDAO::createPatient(const Patient& patient) {
    auto conn = connectionPool->getConnection();
    if (!conn) return false;
    
    std::stringstream query;
    query << "INSERT INTO patients (user_id, name, gender, birth_date, id_number, phone_number) VALUES ("
          << patient.getUserId() << ", "
          << "'" << conn->escapeString(patient.getName()) << "', "
          << "'" << patient.genderToString() << "', "
          << "'" << conn->escapeString(patient.getBirthDate()) << "', "
          << "'" << conn->escapeString(patient.getIdNumber()) << "', "
          << (patient.getPhoneNumber().empty() ? "NULL" : "'" + conn->escapeString(patient.getPhoneNumber()) + "'") << ")";
    
    bool result = conn->executeUpdate(query.str());
    connectionPool->returnConnection(std::move(conn));
    return result;
}

std::unique_ptr<Patient> PatientDAO::getPatientById(int patientId) {
    auto conn = connectionPool->getConnection();
    if (!conn) return nullptr;
    
    std::stringstream query;
    query << "SELECT patient_id, user_id, name, gender, birth_date, id_number, phone_number "
          << "FROM patients WHERE patient_id = " << patientId;
    
    MYSQL_RES* result = conn->executeQuery(query.str());
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return nullptr;
    }
    
    MYSQL_ROW row = mysql_fetch_row(result);
    std::unique_ptr<Patient> patient = nullptr;
    
    if (row) {
        unsigned long* lengths = mysql_fetch_lengths(result);
        patient = std::unique_ptr<Patient>(mapRowToPatient(row, lengths));
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return patient;
}

std::unique_ptr<Patient> PatientDAO::getPatientByUserId(int userId) {
    auto conn = connectionPool->getConnection();
    if (!conn) return nullptr;
    
    std::stringstream query;
    query << "SELECT patient_id, user_id, name, gender, birth_date, id_number, phone_number "
          << "FROM patients WHERE user_id = " << userId;
    
    MYSQL_RES* result = conn->executeQuery(query.str());
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return nullptr;
    }
    
    MYSQL_ROW row = mysql_fetch_row(result);
    std::unique_ptr<Patient> patient = nullptr;
    
    if (row) {
        unsigned long* lengths = mysql_fetch_lengths(result);
        patient = std::unique_ptr<Patient>(mapRowToPatient(row, lengths));
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return patient;
}

std::unique_ptr<Patient> PatientDAO::getPatientByIdNumber(const std::string& idNumber) {
    auto conn = connectionPool->getConnection();
    if (!conn) return nullptr;
    
    std::stringstream query;
    query << "SELECT patient_id, user_id, name, gender, birth_date, id_number, phone_number "
          << "FROM patients WHERE id_number = '" << conn->escapeString(idNumber) << "'";
    
    MYSQL_RES* result = conn->executeQuery(query.str());
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return nullptr;
    }
    
    MYSQL_ROW row = mysql_fetch_row(result);
    std::unique_ptr<Patient> patient = nullptr;
    
    if (row) {
        unsigned long* lengths = mysql_fetch_lengths(result);
        patient = std::unique_ptr<Patient>(mapRowToPatient(row, lengths));
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return patient;
}

std::vector<std::unique_ptr<Patient>> PatientDAO::getAllPatients() {
    auto conn = connectionPool->getConnection();
    std::vector<std::unique_ptr<Patient>> patients;
    if (!conn) return patients;
    
    std::string query = "SELECT patient_id, user_id, name, gender, birth_date, id_number, phone_number "
                       "FROM patients ORDER BY name";
    
    MYSQL_RES* result = conn->executeQuery(query);
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return patients;
    }
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        unsigned long* lengths = mysql_fetch_lengths(result);
        patients.push_back(std::unique_ptr<Patient>(mapRowToPatient(row, lengths)));
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return patients;
}

bool PatientDAO::updatePatient(const Patient& patient) {
    auto conn = connectionPool->getConnection();
    if (!conn) return false;
    
    std::stringstream query;
    query << "UPDATE patients SET "
          << "name = '" << conn->escapeString(patient.getName()) << "', "
          << "gender = '" << patient.genderToString() << "', "
          << "birth_date = '" << conn->escapeString(patient.getBirthDate()) << "', "
          << "id_number = '" << conn->escapeString(patient.getIdNumber()) << "', "
          << "phone_number = " << (patient.getPhoneNumber().empty() ? "NULL" : "'" + conn->escapeString(patient.getPhoneNumber()) + "'") << " "
          << "WHERE patient_id = " << patient.getPatientId();
    
    bool result = conn->executeUpdate(query.str());
    connectionPool->returnConnection(std::move(conn));
    return result;
}

bool PatientDAO::deletePatient(int patientId) {
    auto conn = connectionPool->getConnection();
    if (!conn) return false;
    
    std::stringstream query;
    query << "DELETE FROM patients WHERE patient_id = " << patientId;
    
    bool result = conn->executeUpdate(query.str());
    connectionPool->returnConnection(std::move(conn));
    return result;
}

std::vector<std::unique_ptr<Patient>> PatientDAO::searchPatients(const std::string& searchTerm) {
    auto conn = connectionPool->getConnection();
    std::vector<std::unique_ptr<Patient>> patients;
    if (!conn) return patients;
    
    std::stringstream query;
    query << "SELECT patient_id, user_id, name, gender, birth_date, id_number, phone_number "
          << "FROM patients WHERE name LIKE '%" << conn->escapeString(searchTerm) << "%' "
          << "OR id_number LIKE '%" << conn->escapeString(searchTerm) << "%' "
          << "OR phone_number LIKE '%" << conn->escapeString(searchTerm) << "%' "
          << "ORDER BY name";
    
    MYSQL_RES* result = conn->executeQuery(query.str());
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return patients;
    }
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        unsigned long* lengths = mysql_fetch_lengths(result);
        patients.push_back(std::unique_ptr<Patient>(mapRowToPatient(row, lengths)));
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return patients;
}

std::vector<std::unique_ptr<Patient>> PatientDAO::getPatientsByGender(Gender gender) {
    auto conn = connectionPool->getConnection();
    std::vector<std::unique_ptr<Patient>> patients;
    if (!conn) return patients;
    
    Patient tempPatient;
    tempPatient.setGender(gender);
    
    std::stringstream query;
    query << "SELECT patient_id, user_id, name, gender, birth_date, id_number, phone_number "
          << "FROM patients WHERE gender = '" << tempPatient.genderToString() << "' ORDER BY name";
    
    MYSQL_RES* result = conn->executeQuery(query.str());
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return patients;
    }
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        unsigned long* lengths = mysql_fetch_lengths(result);
        patients.push_back(std::unique_ptr<Patient>(mapRowToPatient(row, lengths)));
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return patients;
}

bool PatientDAO::patientExists(const std::string& idNumber) {
    auto conn = connectionPool->getConnection();
    if (!conn) return false;
    
    std::stringstream query;
    query << "SELECT COUNT(*) FROM patients WHERE id_number = '" << conn->escapeString(idNumber) << "'";
    
    MYSQL_RES* result = conn->executeQuery(query.str());
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return false;
    }
    
    MYSQL_ROW row = mysql_fetch_row(result);
    bool exists = false;
    if (row && row[0]) {
        exists = std::stoi(row[0]) > 0;
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return exists;
}

int PatientDAO::getPatientCount() {
    auto conn = connectionPool->getConnection();
    if (!conn) return 0;
    
    std::string query = "SELECT COUNT(*) FROM patients";
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

Patient* PatientDAO::mapRowToPatient(MYSQL_ROW row, unsigned long* lengths) {
    if (!row) return nullptr;
    
    Patient* patient = new Patient();
    
    if (row[0]) patient->setPatientId(std::stoi(row[0]));
    if (row[1]) patient->setUserId(std::stoi(row[1]));
    if (row[2]) patient->setName(std::string(row[2], lengths[2]));
    if (row[3]) patient->setGender(Patient::stringToGender(std::string(row[3], lengths[3])));
    if (row[4]) patient->setBirthDate(std::string(row[4], lengths[4]));
    if (row[5]) patient->setIdNumber(std::string(row[5], lengths[5]));
    if (row[6]) patient->setPhoneNumber(std::string(row[6], lengths[6]));
    
    return patient;
}