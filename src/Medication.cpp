#include "Medication.h"
#include <memory>
#include <sstream>
#include <iostream>

// Medication class implementation
Medication::Medication() : medicationId(0), prescriptionId(0), quantity(0) {}

Medication::Medication(int prescriptionId, const std::string& medicationName, int quantity, const std::string& usageInstructions)
    : medicationId(0), prescriptionId(prescriptionId), medicationName(medicationName), quantity(quantity), usageInstructions(usageInstructions) {}

// MedicationDAO class implementation
MedicationDAO::MedicationDAO(std::shared_ptr<ConnectionPool> pool) : connectionPool(pool) {}

bool MedicationDAO::createMedication(const Medication& medication) {
    auto conn = connectionPool->getConnection();
    if (!conn) return false;
    
    std::stringstream query;
    query << "INSERT INTO medications (prescription_id, medication_name, quantity, usage_instructions) VALUES ("
          << medication.getPrescriptionId() << ", "
          << "'" << conn->escapeString(medication.getMedicationName()) << "', "
          << medication.getQuantity() << ", "
          << "'" << conn->escapeString(medication.getUsageInstructions()) << "')";
    
    bool result = conn->executeUpdate(query.str());
    connectionPool->returnConnection(std::move(conn));
    return result;
}

std::unique_ptr<Medication> MedicationDAO::getMedicationById(int medicationId) {
    auto conn = connectionPool->getConnection();
    if (!conn) return nullptr;
    
    std::stringstream query;
    query << "SELECT medication_id, prescription_id, medication_name, quantity, usage_instructions "
          << "FROM medications WHERE medication_id = " << medicationId;
    
    MYSQL_RES* result = conn->executeQuery(query.str());
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return nullptr;
    }
    
    MYSQL_ROW row = mysql_fetch_row(result);
    std::unique_ptr<Medication> medication = nullptr;
    
    if (row) {
        unsigned long* lengths = mysql_fetch_lengths(result);
        medication = std::unique_ptr<Medication>(mapRowToMedication(row, lengths));
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return medication;
}

std::vector<std::unique_ptr<Medication>> MedicationDAO::getMedicationsByPrescriptionId(int prescriptionId) {
    auto conn = connectionPool->getConnection();
    std::vector<std::unique_ptr<Medication>> medications;
    if (!conn) return medications;
    
    std::stringstream query;
    query << "SELECT medication_id, prescription_id, medication_name, quantity, usage_instructions "
          << "FROM medications WHERE prescription_id = " << prescriptionId << " ORDER BY medication_name";
    
    MYSQL_RES* result = conn->executeQuery(query.str());
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return medications;
    }
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        unsigned long* lengths = mysql_fetch_lengths(result);
        medications.push_back(std::unique_ptr<Medication>(mapRowToMedication(row, lengths)));
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return medications;
}

std::vector<std::unique_ptr<Medication>> MedicationDAO::getAllMedications() {
    auto conn = connectionPool->getConnection();
    std::vector<std::unique_ptr<Medication>> medications;
    if (!conn) return medications;
    
    std::string query = "SELECT medication_id, prescription_id, medication_name, quantity, usage_instructions "
                       "FROM medications ORDER BY medication_name";
    
    MYSQL_RES* result = conn->executeQuery(query);
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return medications;
    }
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        unsigned long* lengths = mysql_fetch_lengths(result);
        medications.push_back(std::unique_ptr<Medication>(mapRowToMedication(row, lengths)));
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return medications;
}

bool MedicationDAO::updateMedication(const Medication& medication) {
    auto conn = connectionPool->getConnection();
    if (!conn) return false;
    
    std::stringstream query;
    query << "UPDATE medications SET "
          << "prescription_id = " << medication.getPrescriptionId() << ", "
          << "medication_name = '" << conn->escapeString(medication.getMedicationName()) << "', "
          << "quantity = " << medication.getQuantity() << ", "
          << "usage_instructions = '" << conn->escapeString(medication.getUsageInstructions()) << "' "
          << "WHERE medication_id = " << medication.getMedicationId();
    
    bool result = conn->executeUpdate(query.str());
    connectionPool->returnConnection(std::move(conn));
    return result;
}

bool MedicationDAO::deleteMedication(int medicationId) {
    auto conn = connectionPool->getConnection();
    if (!conn) return false;
    
    std::stringstream query;
    query << "DELETE FROM medications WHERE medication_id = " << medicationId;
    
    bool result = conn->executeUpdate(query.str());
    connectionPool->returnConnection(std::move(conn));
    return result;
}

std::vector<std::unique_ptr<Medication>> MedicationDAO::searchMedications(const std::string& searchTerm) {
    auto conn = connectionPool->getConnection();
    std::vector<std::unique_ptr<Medication>> medications;
    if (!conn) return medications;
    
    std::stringstream query;
    query << "SELECT medication_id, prescription_id, medication_name, quantity, usage_instructions "
          << "FROM medications WHERE medication_name LIKE '%" << conn->escapeString(searchTerm) << "%' "
          << "OR usage_instructions LIKE '%" << conn->escapeString(searchTerm) << "%' "
          << "ORDER BY medication_name";
    
    MYSQL_RES* result = conn->executeQuery(query.str());
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return medications;
    }
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        unsigned long* lengths = mysql_fetch_lengths(result);
        medications.push_back(std::unique_ptr<Medication>(mapRowToMedication(row, lengths)));
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return medications;
}

std::vector<std::unique_ptr<Medication>> MedicationDAO::getMedicationsByName(const std::string& medicationName) {
    auto conn = connectionPool->getConnection();
    std::vector<std::unique_ptr<Medication>> medications;
    if (!conn) return medications;
    
    std::stringstream query;
    query << "SELECT medication_id, prescription_id, medication_name, quantity, usage_instructions "
          << "FROM medications WHERE medication_name = '" << conn->escapeString(medicationName) 
          << "' ORDER BY quantity DESC";
    
    MYSQL_RES* result = conn->executeQuery(query.str());
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return medications;
    }
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        unsigned long* lengths = mysql_fetch_lengths(result);
        medications.push_back(std::unique_ptr<Medication>(mapRowToMedication(row, lengths)));
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return medications;
}

int MedicationDAO::getMedicationCount() {
    auto conn = connectionPool->getConnection();
    if (!conn) return 0;
    
    std::string query = "SELECT COUNT(*) FROM medications";
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

int MedicationDAO::getTotalQuantityByName(const std::string& medicationName) {
    auto conn = connectionPool->getConnection();
    if (!conn) return 0;
    
    std::stringstream query;
    query << "SELECT SUM(quantity) FROM medications WHERE medication_name = '" 
          << conn->escapeString(medicationName) << "'";
    
    MYSQL_RES* result = conn->executeQuery(query.str());
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return 0;
    }
    
    MYSQL_ROW row = mysql_fetch_row(result);
    int total = 0;
    if (row && row[0]) {
        total = std::stoi(row[0]);
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return total;
}

Medication* MedicationDAO::mapRowToMedication(MYSQL_ROW row, unsigned long* lengths) {
    if (!row) return nullptr;
    
    Medication* medication = new Medication();
    
    if (row[0]) medication->setMedicationId(std::stoi(row[0]));
    if (row[1]) medication->setPrescriptionId(std::stoi(row[1]));
    if (row[2]) medication->setMedicationName(std::string(row[2], lengths[2]));
    if (row[3]) medication->setQuantity(std::stoi(row[3]));
    if (row[4]) medication->setUsageInstructions(std::string(row[4], lengths[4]));
    
    return medication;
}