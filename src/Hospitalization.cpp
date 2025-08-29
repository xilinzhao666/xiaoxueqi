#include "Hospitalization.h"
#include <memory>
#include <sstream>
#include <iostream>

// Hospitalization class implementation
Hospitalization::Hospitalization() : hospitalizationId(0), patientId(0) {}

Hospitalization::Hospitalization(int patientId, const std::string& wardNumber, const std::string& bedNumber, const std::string& attendingDoctor)
    : hospitalizationId(0), patientId(patientId), wardNumber(wardNumber), bedNumber(bedNumber), attendingDoctor(attendingDoctor) {}

// HospitalizationDAO class implementation
HospitalizationDAO::HospitalizationDAO(std::shared_ptr<ConnectionPool> pool) : connectionPool(pool) {}

bool HospitalizationDAO::createHospitalization(const Hospitalization& hospitalization) {
    auto conn = connectionPool->getConnection();
    if (!conn) return false;
    
    std::stringstream query;
    query << "INSERT INTO hospitalization (patient_id, ward_number, bed_number, attending_doctor) VALUES ("
          << hospitalization.getPatientId() << ", "
          << "'" << conn->escapeString(hospitalization.getWardNumber()) << "', "
          << "'" << conn->escapeString(hospitalization.getBedNumber()) << "', "
          << "'" << conn->escapeString(hospitalization.getAttendingDoctor()) << "')";
    
    bool result = conn->executeUpdate(query.str());
    connectionPool->returnConnection(std::move(conn));
    return result;
}

std::unique_ptr<Hospitalization> HospitalizationDAO::getHospitalizationById(int hospitalizationId) {
    auto conn = connectionPool->getConnection();
    if (!conn) return nullptr;
    
    std::stringstream query;
    query << "SELECT hospitalization_id, patient_id, ward_number, bed_number, admission_date, attending_doctor "
          << "FROM hospitalization WHERE hospitalization_id = " << hospitalizationId;
    
    MYSQL_RES* result = conn->executeQuery(query.str());
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return nullptr;
    }
    
    MYSQL_ROW row = mysql_fetch_row(result);
    std::unique_ptr<Hospitalization> hospitalization = nullptr;
    
    if (row) {
        unsigned long* lengths = mysql_fetch_lengths(result);
        hospitalization = std::unique_ptr<Hospitalization>(mapRowToHospitalization(row, lengths));
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return hospitalization;
}

std::vector<std::unique_ptr<Hospitalization>> HospitalizationDAO::getHospitalizationsByPatientId(int patientId) {
    auto conn = connectionPool->getConnection();
    std::vector<std::unique_ptr<Hospitalization>> hospitalizations;
    if (!conn) return hospitalizations;
    
    std::stringstream query;
    query << "SELECT hospitalization_id, patient_id, ward_number, bed_number, admission_date, attending_doctor "
          << "FROM hospitalization WHERE patient_id = " << patientId << " ORDER BY admission_date DESC";
    
    MYSQL_RES* result = conn->executeQuery(query.str());
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return hospitalizations;
    }
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        unsigned long* lengths = mysql_fetch_lengths(result);
        hospitalizations.push_back(std::unique_ptr<Hospitalization>(mapRowToHospitalization(row, lengths)));
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return hospitalizations;
}

std::vector<std::unique_ptr<Hospitalization>> HospitalizationDAO::getHospitalizationsByWard(const std::string& wardNumber) {
    auto conn = connectionPool->getConnection();
    std::vector<std::unique_ptr<Hospitalization>> hospitalizations;
    if (!conn) return hospitalizations;
    
    std::stringstream query;
    query << "SELECT hospitalization_id, patient_id, ward_number, bed_number, admission_date, attending_doctor "
          << "FROM hospitalization WHERE ward_number = '" << conn->escapeString(wardNumber) 
          << "' ORDER BY bed_number";
    
    MYSQL_RES* result = conn->executeQuery(query.str());
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return hospitalizations;
    }
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        unsigned long* lengths = mysql_fetch_lengths(result);
        hospitalizations.push_back(std::unique_ptr<Hospitalization>(mapRowToHospitalization(row, lengths)));
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return hospitalizations;
}

std::vector<std::unique_ptr<Hospitalization>> HospitalizationDAO::getAllHospitalizations() {
    auto conn = connectionPool->getConnection();
    std::vector<std::unique_ptr<Hospitalization>> hospitalizations;
    if (!conn) return hospitalizations;
    
    std::string query = "SELECT hospitalization_id, patient_id, ward_number, bed_number, admission_date, attending_doctor "
                       "FROM hospitalization ORDER BY admission_date DESC";
    
    MYSQL_RES* result = conn->executeQuery(query);
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return hospitalizations;
    }
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        unsigned long* lengths = mysql_fetch_lengths(result);
        hospitalizations.push_back(std::unique_ptr<Hospitalization>(mapRowToHospitalization(row, lengths)));
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return hospitalizations;
}

bool HospitalizationDAO::updateHospitalization(const Hospitalization& hospitalization) {
    auto conn = connectionPool->getConnection();
    if (!conn) return false;
    
    std::stringstream query;
    query << "UPDATE hospitalization SET "
          << "patient_id = " << hospitalization.getPatientId() << ", "
          << "ward_number = '" << conn->escapeString(hospitalization.getWardNumber()) << "', "
          << "bed_number = '" << conn->escapeString(hospitalization.getBedNumber()) << "', "
          << "attending_doctor = '" << conn->escapeString(hospitalization.getAttendingDoctor()) << "' "
          << "WHERE hospitalization_id = " << hospitalization.getHospitalizationId();
    
    bool result = conn->executeUpdate(query.str());
    connectionPool->returnConnection(std::move(conn));
    return result;
}

bool HospitalizationDAO::deleteHospitalization(int hospitalizationId) {
    auto conn = connectionPool->getConnection();
    if (!conn) return false;
    
    std::stringstream query;
    query << "DELETE FROM hospitalization WHERE hospitalization_id = " << hospitalizationId;
    
    bool result = conn->executeUpdate(query.str());
    connectionPool->returnConnection(std::move(conn));
    return result;
}

std::vector<std::string> HospitalizationDAO::getAvailableBeds(const std::string& wardNumber) {
    // This would require a beds table or predefined bed numbers
    // For now, return empty vector - implement based on hospital's bed management system
    return std::vector<std::string>();
}

bool HospitalizationDAO::isBedOccupied(const std::string& wardNumber, const std::string& bedNumber) {
    auto conn = connectionPool->getConnection();
    if (!conn) return false;
    
    std::stringstream query;
    query << "SELECT COUNT(*) FROM hospitalization WHERE ward_number = '" 
          << conn->escapeString(wardNumber) << "' AND bed_number = '" 
          << conn->escapeString(bedNumber) << "'";
    
    MYSQL_RES* result = conn->executeQuery(query.str());
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return false;
    }
    
    MYSQL_ROW row = mysql_fetch_row(result);
    bool occupied = false;
    if (row && row[0]) {
        occupied = std::stoi(row[0]) > 0;
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return occupied;
}

std::vector<std::string> HospitalizationDAO::getAllWards() {
    auto conn = connectionPool->getConnection();
    std::vector<std::string> wards;
    if (!conn) return wards;
    
    std::string query = "SELECT DISTINCT ward_number FROM hospitalization ORDER BY ward_number";
    
    MYSQL_RES* result = conn->executeQuery(query);
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return wards;
    }
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        if (row[0]) {
            wards.push_back(std::string(row[0]));
        }
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return wards;
}

std::vector<std::unique_ptr<Hospitalization>> HospitalizationDAO::searchHospitalizations(const std::string& searchTerm) {
    auto conn = connectionPool->getConnection();
    std::vector<std::unique_ptr<Hospitalization>> hospitalizations;
    if (!conn) return hospitalizations;
    
    std::stringstream query;
    query << "SELECT hospitalization_id, patient_id, ward_number, bed_number, admission_date, attending_doctor "
          << "FROM hospitalization WHERE ward_number LIKE '%" << conn->escapeString(searchTerm) << "%' "
          << "OR bed_number LIKE '%" << conn->escapeString(searchTerm) << "%' "
          << "OR attending_doctor LIKE '%" << conn->escapeString(searchTerm) << "%' "
          << "ORDER BY admission_date DESC";
    
    MYSQL_RES* result = conn->executeQuery(query.str());
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return hospitalizations;
    }
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        unsigned long* lengths = mysql_fetch_lengths(result);
        hospitalizations.push_back(std::unique_ptr<Hospitalization>(mapRowToHospitalization(row, lengths)));
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return hospitalizations;
}

int HospitalizationDAO::getHospitalizationCount() {
    auto conn = connectionPool->getConnection();
    if (!conn) return 0;
    
    std::string query = "SELECT COUNT(*) FROM hospitalization";
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

int HospitalizationDAO::getCurrentHospitalizationCount() {
    // Assuming current hospitalizations are those without discharge date
    // Since discharge_date is not in the schema, we count all records
    return getHospitalizationCount();
}

Hospitalization* HospitalizationDAO::mapRowToHospitalization(MYSQL_ROW row, unsigned long* lengths) {
    if (!row) return nullptr;
    
    Hospitalization* hospitalization = new Hospitalization();
    
    if (row[0]) hospitalization->setHospitalizationId(std::stoi(row[0]));
    if (row[1]) hospitalization->setPatientId(std::stoi(row[1]));
    if (row[2]) hospitalization->setWardNumber(std::string(row[2], lengths[2]));
    if (row[3]) hospitalization->setBedNumber(std::string(row[3], lengths[3]));
    if (row[4]) hospitalization->setAdmissionDate(std::string(row[4], lengths[4]));
    if (row[5]) hospitalization->setAttendingDoctor(std::string(row[5], lengths[5]));
    
    return hospitalization;
}