#include "Case.h"
#include <memory>
#include <sstream>
#include <iostream>

// Case class implementation
Case::Case() : caseId(0), patientId(0), doctorId(0) {}

Case::Case(int patientId, const std::string& department, int doctorId, const std::string& diagnosis)
    : caseId(0), patientId(patientId), department(department), doctorId(doctorId), diagnosis(diagnosis) {}

// CaseDAO class implementation
CaseDAO::CaseDAO(std::shared_ptr<ConnectionPool> pool) : connectionPool(pool) {}

bool CaseDAO::createCase(const Case& medicalCase) {
    auto conn = connectionPool->getConnection();
    if (!conn) return false;
    
    std::stringstream query;
    query << "INSERT INTO cases (patient_id, department, doctor_id, diagnosis) VALUES ("
          << medicalCase.getPatientId() << ", "
          << "'" << conn->escapeString(medicalCase.getDepartment()) << "', "
          << medicalCase.getDoctorId() << ", "
          << "'" << conn->escapeString(medicalCase.getDiagnosis()) << "')";
    
    bool result = conn->executeUpdate(query.str());
    connectionPool->returnConnection(std::move(conn));
    return result;
}

std::unique_ptr<Case> CaseDAO::getCaseById(int caseId) {
    auto conn = connectionPool->getConnection();
    if (!conn) return nullptr;
    
    std::stringstream query;
    query << "SELECT case_id, patient_id, department, doctor_id, diagnosis, diagnosis_date "
          << "FROM cases WHERE case_id = " << caseId;
    
    MYSQL_RES* result = conn->executeQuery(query.str());
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return nullptr;
    }
    
    MYSQL_ROW row = mysql_fetch_row(result);
    std::unique_ptr<Case> medicalCase = nullptr;
    
    if (row) {
        unsigned long* lengths = mysql_fetch_lengths(result);
        medicalCase = std::unique_ptr<Case>(mapRowToCase(row, lengths));
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return medicalCase;
}

std::vector<std::unique_ptr<Case>> CaseDAO::getCasesByPatientId(int patientId) {
    auto conn = connectionPool->getConnection();
    std::vector<std::unique_ptr<Case>> cases;
    if (!conn) return cases;
    
    std::stringstream query;
    query << "SELECT case_id, patient_id, department, doctor_id, diagnosis, diagnosis_date "
          << "FROM cases WHERE patient_id = " << patientId << " ORDER BY diagnosis_date DESC";
    
    MYSQL_RES* result = conn->executeQuery(query.str());
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return cases;
    }
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        unsigned long* lengths = mysql_fetch_lengths(result);
        cases.push_back(std::unique_ptr<Case>(mapRowToCase(row, lengths)));
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return cases;
}

std::vector<std::unique_ptr<Case>> CaseDAO::getCasesByDoctorId(int doctorId) {
    auto conn = connectionPool->getConnection();
    std::vector<std::unique_ptr<Case>> cases;
    if (!conn) return cases;
    
    std::stringstream query;
    query << "SELECT case_id, patient_id, department, doctor_id, diagnosis, diagnosis_date "
          << "FROM cases WHERE doctor_id = " << doctorId << " ORDER BY diagnosis_date DESC";
    
    MYSQL_RES* result = conn->executeQuery(query.str());
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return cases;
    }
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        unsigned long* lengths = mysql_fetch_lengths(result);
        cases.push_back(std::unique_ptr<Case>(mapRowToCase(row, lengths)));
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return cases;
}

std::vector<std::unique_ptr<Case>> CaseDAO::getCasesByDepartment(const std::string& department) {
    auto conn = connectionPool->getConnection();
    std::vector<std::unique_ptr<Case>> cases;
    if (!conn) return cases;
    
    std::stringstream query;
    query << "SELECT case_id, patient_id, department, doctor_id, diagnosis, diagnosis_date "
          << "FROM cases WHERE department = '" << conn->escapeString(department) 
          << "' ORDER BY diagnosis_date DESC";
    
    MYSQL_RES* result = conn->executeQuery(query.str());
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return cases;
    }
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        unsigned long* lengths = mysql_fetch_lengths(result);
        cases.push_back(std::unique_ptr<Case>(mapRowToCase(row, lengths)));
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return cases;
}

std::vector<std::unique_ptr<Case>> CaseDAO::getAllCases() {
    auto conn = connectionPool->getConnection();
    std::vector<std::unique_ptr<Case>> cases;
    if (!conn) return cases;
    
    std::string query = "SELECT case_id, patient_id, department, doctor_id, diagnosis, diagnosis_date "
                       "FROM cases ORDER BY diagnosis_date DESC";
    
    MYSQL_RES* result = conn->executeQuery(query);
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return cases;
    }
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        unsigned long* lengths = mysql_fetch_lengths(result);
        cases.push_back(std::unique_ptr<Case>(mapRowToCase(row, lengths)));
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return cases;
}

bool CaseDAO::updateCase(const Case& medicalCase) {
    auto conn = connectionPool->getConnection();
    if (!conn) return false;
    
    std::stringstream query;
    query << "UPDATE cases SET "
          << "patient_id = " << medicalCase.getPatientId() << ", "
          << "department = '" << conn->escapeString(medicalCase.getDepartment()) << "', "
          << "doctor_id = " << medicalCase.getDoctorId() << ", "
          << "diagnosis = '" << conn->escapeString(medicalCase.getDiagnosis()) << "' "
          << "WHERE case_id = " << medicalCase.getCaseId();
    
    bool result = conn->executeUpdate(query.str());
    connectionPool->returnConnection(std::move(conn));
    return result;
}

bool CaseDAO::deleteCase(int caseId) {
    auto conn = connectionPool->getConnection();
    if (!conn) return false;
    
    std::stringstream query;
    query << "DELETE FROM cases WHERE case_id = " << caseId;
    
    bool result = conn->executeUpdate(query.str());
    connectionPool->returnConnection(std::move(conn));
    return result;
}

std::vector<std::unique_ptr<Case>> CaseDAO::searchCases(const std::string& searchTerm) {
    auto conn = connectionPool->getConnection();
    std::vector<std::unique_ptr<Case>> cases;
    if (!conn) return cases;
    
    std::stringstream query;
    query << "SELECT case_id, patient_id, department, doctor_id, diagnosis, diagnosis_date "
          << "FROM cases WHERE diagnosis LIKE '%" << conn->escapeString(searchTerm) << "%' "
          << "OR department LIKE '%" << conn->escapeString(searchTerm) << "%' "
          << "ORDER BY diagnosis_date DESC";
    
    MYSQL_RES* result = conn->executeQuery(query.str());
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return cases;
    }
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        unsigned long* lengths = mysql_fetch_lengths(result);
        cases.push_back(std::unique_ptr<Case>(mapRowToCase(row, lengths)));
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return cases;
}

int CaseDAO::getCaseCount() {
    auto conn = connectionPool->getConnection();
    if (!conn) return 0;
    
    std::string query = "SELECT COUNT(*) FROM cases";
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

int CaseDAO::getCaseCountByDoctor(int doctorId) {
    auto conn = connectionPool->getConnection();
    if (!conn) return 0;
    
    std::stringstream query;
    query << "SELECT COUNT(*) FROM cases WHERE doctor_id = " << doctorId;
    
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

int CaseDAO::getCaseCountByPatient(int patientId) {
    auto conn = connectionPool->getConnection();
    if (!conn) return 0;
    
    std::stringstream query;
    query << "SELECT COUNT(*) FROM cases WHERE patient_id = " << patientId;
    
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

Case* CaseDAO::mapRowToCase(MYSQL_ROW row, unsigned long* lengths) {
    if (!row) return nullptr;
    
    Case* medicalCase = new Case();
    
    if (row[0]) medicalCase->setCaseId(std::stoi(row[0]));
    if (row[1]) medicalCase->setPatientId(std::stoi(row[1]));
    if (row[2]) medicalCase->setDepartment(std::string(row[2], lengths[2]));
    if (row[3]) medicalCase->setDoctorId(std::stoi(row[3]));
    if (row[4]) medicalCase->setDiagnosis(std::string(row[4], lengths[4]));
    if (row[5]) medicalCase->setDiagnosisDate(std::string(row[5], lengths[5]));
    
    return medicalCase;
}