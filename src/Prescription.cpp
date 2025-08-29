#include "Prescription.h"
#include <memory>
#include <sstream>
#include <iostream>

// Prescription class implementation
Prescription::Prescription() : prescriptionId(0), caseId(0), doctorId(0) {}

Prescription::Prescription(int caseId, int doctorId, const std::string& prescriptionContent)
    : prescriptionId(0), caseId(caseId), doctorId(doctorId), prescriptionContent(prescriptionContent) {}

// PrescriptionDAO class implementation
PrescriptionDAO::PrescriptionDAO(std::shared_ptr<ConnectionPool> pool) : connectionPool(pool) {}

bool PrescriptionDAO::createPrescription(const Prescription& prescription) {
    auto conn = connectionPool->getConnection();
    if (!conn) return false;
    
    std::stringstream query;
    query << "INSERT INTO prescriptions (case_id, doctor_id, prescription_content) VALUES ("
          << prescription.getCaseId() << ", "
          << prescription.getDoctorId() << ", "
          << "'" << conn->escapeString(prescription.getPrescriptionContent()) << "')";
    
    bool result = conn->executeUpdate(query.str());
    connectionPool->returnConnection(std::move(conn));
    return result;
}

std::unique_ptr<Prescription> PrescriptionDAO::getPrescriptionById(int prescriptionId) {
    auto conn = connectionPool->getConnection();
    if (!conn) return nullptr;
    
    std::stringstream query;
    query << "SELECT prescription_id, case_id, doctor_id, prescription_content, issued_date "
          << "FROM prescriptions WHERE prescription_id = " << prescriptionId;
    
    MYSQL_RES* result = conn->executeQuery(query.str());
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return nullptr;
    }
    
    MYSQL_ROW row = mysql_fetch_row(result);
    std::unique_ptr<Prescription> prescription = nullptr;
    
    if (row) {
        unsigned long* lengths = mysql_fetch_lengths(result);
        prescription = std::unique_ptr<Prescription>(mapRowToPrescription(row, lengths));
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return prescription;
}

std::vector<std::unique_ptr<Prescription>> PrescriptionDAO::getPrescriptionsByCaseId(int caseId) {
    auto conn = connectionPool->getConnection();
    std::vector<std::unique_ptr<Prescription>> prescriptions;
    if (!conn) return prescriptions;
    
    std::stringstream query;
    query << "SELECT prescription_id, case_id, doctor_id, prescription_content, issued_date "
          << "FROM prescriptions WHERE case_id = " << caseId << " ORDER BY issued_date DESC";
    
    MYSQL_RES* result = conn->executeQuery(query.str());
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return prescriptions;
    }
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        unsigned long* lengths = mysql_fetch_lengths(result);
        prescriptions.push_back(std::unique_ptr<Prescription>(mapRowToPrescription(row, lengths)));
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return prescriptions;
}

std::vector<std::unique_ptr<Prescription>> PrescriptionDAO::getPrescriptionsByDoctorId(int doctorId) {
    auto conn = connectionPool->getConnection();
    std::vector<std::unique_ptr<Prescription>> prescriptions;
    if (!conn) return prescriptions;
    
    std::stringstream query;
    query << "SELECT prescription_id, case_id, doctor_id, prescription_content, issued_date "
          << "FROM prescriptions WHERE doctor_id = " << doctorId << " ORDER BY issued_date DESC";
    
    MYSQL_RES* result = conn->executeQuery(query.str());
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return prescriptions;
    }
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        unsigned long* lengths = mysql_fetch_lengths(result);
        prescriptions.push_back(std::unique_ptr<Prescription>(mapRowToPrescription(row, lengths)));
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return prescriptions;
}

std::vector<std::unique_ptr<Prescription>> PrescriptionDAO::getAllPrescriptions() {
    auto conn = connectionPool->getConnection();
    std::vector<std::unique_ptr<Prescription>> prescriptions;
    if (!conn) return prescriptions;
    
    std::string query = "SELECT prescription_id, case_id, doctor_id, prescription_content, issued_date "
                       "FROM prescriptions ORDER BY issued_date DESC";
    
    MYSQL_RES* result = conn->executeQuery(query);
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return prescriptions;
    }
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        unsigned long* lengths = mysql_fetch_lengths(result);
        prescriptions.push_back(std::unique_ptr<Prescription>(mapRowToPrescription(row, lengths)));
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return prescriptions;
}

bool PrescriptionDAO::updatePrescription(const Prescription& prescription) {
    auto conn = connectionPool->getConnection();
    if (!conn) return false;
    
    std::stringstream query;
    query << "UPDATE prescriptions SET "
          << "case_id = " << prescription.getCaseId() << ", "
          << "doctor_id = " << prescription.getDoctorId() << ", "
          << "prescription_content = '" << conn->escapeString(prescription.getPrescriptionContent()) << "' "
          << "WHERE prescription_id = " << prescription.getPrescriptionId();
    
    bool result = conn->executeUpdate(query.str());
    connectionPool->returnConnection(std::move(conn));
    return result;
}

bool PrescriptionDAO::deletePrescription(int prescriptionId) {
    auto conn = connectionPool->getConnection();
    if (!conn) return false;
    
    std::stringstream query;
    query << "DELETE FROM prescriptions WHERE prescription_id = " << prescriptionId;
    
    bool result = conn->executeUpdate(query.str());
    connectionPool->returnConnection(std::move(conn));
    return result;
}

std::vector<std::unique_ptr<Prescription>> PrescriptionDAO::searchPrescriptions(const std::string& searchTerm) {
    auto conn = connectionPool->getConnection();
    std::vector<std::unique_ptr<Prescription>> prescriptions;
    if (!conn) return prescriptions;
    
    std::stringstream query;
    query << "SELECT prescription_id, case_id, doctor_id, prescription_content, issued_date "
          << "FROM prescriptions WHERE prescription_content LIKE '%" << conn->escapeString(searchTerm) << "%' "
          << "ORDER BY issued_date DESC";
    
    MYSQL_RES* result = conn->executeQuery(query.str());
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return prescriptions;
    }
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        unsigned long* lengths = mysql_fetch_lengths(result);
        prescriptions.push_back(std::unique_ptr<Prescription>(mapRowToPrescription(row, lengths)));
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return prescriptions;
}

int PrescriptionDAO::getPrescriptionCount() {
    auto conn = connectionPool->getConnection();
    if (!conn) return 0;
    
    std::string query = "SELECT COUNT(*) FROM prescriptions";
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

int PrescriptionDAO::getPrescriptionCountByDoctor(int doctorId) {
    auto conn = connectionPool->getConnection();
    if (!conn) return 0;
    
    std::stringstream query;
    query << "SELECT COUNT(*) FROM prescriptions WHERE doctor_id = " << doctorId;
    
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

Prescription* PrescriptionDAO::mapRowToPrescription(MYSQL_ROW row, unsigned long* lengths) {
    if (!row) return nullptr;
    
    Prescription* prescription = new Prescription();
    
    if (row[0]) prescription->setPrescriptionId(std::stoi(row[0]));
    if (row[1]) prescription->setCaseId(std::stoi(row[1]));
    if (row[2]) prescription->setDoctorId(std::stoi(row[2]));
    if (row[3]) prescription->setPrescriptionContent(std::string(row[3], lengths[3]));
    if (row[4]) prescription->setIssuedDate(std::string(row[4], lengths[4]));
    
    return prescription;
}