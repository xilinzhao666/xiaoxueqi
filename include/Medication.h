#ifndef MEDICATION_H
#define MEDICATION_H

#include <string>
#include <vector>
#include <memory>
#include "DatabaseConnection.h"

class Medication {
private:
    int medicationId;
    int prescriptionId;
    std::string medicationName;
    int quantity;
    std::string usageInstructions;

public:
    Medication();
    Medication(int prescriptionId, const std::string& medicationName, int quantity, const std::string& usageInstructions);
    
    // Getter methods
    int getMedicationId() const { return medicationId; }
    int getPrescriptionId() const { return prescriptionId; }
    std::string getMedicationName() const { return medicationName; }
    int getQuantity() const { return quantity; }
    std::string getUsageInstructions() const { return usageInstructions; }
    
    // Setter methods
    void setMedicationId(int id) { medicationId = id; }
    void setPrescriptionId(int id) { prescriptionId = id; }
    void setMedicationName(const std::string& name) { medicationName = name; }
    void setQuantity(int qty) { quantity = qty; }
    void setUsageInstructions(const std::string& instructions) { usageInstructions = instructions; }
};

class MedicationDAO {
private:
    std::shared_ptr<ConnectionPool> connectionPool;
    
public:
    MedicationDAO(std::shared_ptr<ConnectionPool> pool);
    
    // CRUD operations
    bool createMedication(const Medication& medication);
    std::unique_ptr<Medication> getMedicationById(int medicationId);
    std::vector<std::unique_ptr<Medication>> getMedicationsByPrescriptionId(int prescriptionId);
    std::vector<std::unique_ptr<Medication>> getAllMedications();
    
    bool updateMedication(const Medication& medication);
    bool deleteMedication(int medicationId);
    
    // Search operations
    std::vector<std::unique_ptr<Medication>> searchMedications(const std::string& searchTerm);
    std::vector<std::unique_ptr<Medication>> getMedicationsByName(const std::string& medicationName);
    int getMedicationCount();
    int getTotalQuantityByName(const std::string& medicationName);
    
private:
    Medication* mapRowToMedication(MYSQL_ROW row, unsigned long* lengths);
};

#endif // MEDICATION_H