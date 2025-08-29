#include "DatabaseService.h"
#include <iostream>
#include <sstream>
#include <openssl/sha.h>
#include <iomanip>

DatabaseService::DatabaseService(const std::string& host, const std::string& username,
                                const std::string& password, const std::string& database,
                                unsigned int port, size_t maxConnections) {
    connectionPool = std::make_shared<ConnectionPool>(host, username, password, database, port, maxConnections);
    userDAO = std::make_unique<UserDAO>(connectionPool);
    doctorDAO = std::make_unique<DoctorDAO>(connectionPool);
    patientDAO = std::make_unique<PatientDAO>(connectionPool);
    caseDAO = std::make_unique<CaseDAO>(connectionPool);
    appointmentDAO = std::make_unique<AppointmentDAO>(connectionPool);
    hospitalizationDAO = std::make_unique<HospitalizationDAO>(connectionPool);
    prescriptionDAO = std::make_unique<PrescriptionDAO>(connectionPool);
    medicationDAO = std::make_unique<MedicationDAO>(connectionPool);
}

DatabaseService::~DatabaseService() = default;

bool DatabaseService::initializeDatabase() {
    return createTables();
}

bool DatabaseService::createTables() {
    auto conn = connectionPool->getConnection();
    if (!conn) return false;
    
    std::vector<std::string> createTableQueries = {
        // Users table
        R"(CREATE TABLE IF NOT EXISTS users (
            user_id INT AUTO_INCREMENT PRIMARY KEY,
            username VARCHAR(50) UNIQUE NOT NULL,
            password VARCHAR(255) NOT NULL,
            user_type ENUM('Doctor', 'Patient') NOT NULL DEFAULT 'Patient',
            email VARCHAR(100) UNIQUE,
            phone_number VARCHAR(20),
            created_at DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
            INDEX idx_username (username),
            INDEX idx_email (email),
            INDEX idx_user_type (user_type)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4)",
        
        // Doctors table
        R"(CREATE TABLE IF NOT EXISTS doctors (
            doctor_id INT AUTO_INCREMENT PRIMARY KEY,
            user_id INT NOT NULL,
            name VARCHAR(100) NOT NULL,
            department VARCHAR(50) NOT NULL,
            title VARCHAR(50),
            working_hours VARCHAR(100) NOT NULL,
            profile_picture VARCHAR(255),
            FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE CASCADE,
            INDEX idx_user_id (user_id),
            INDEX idx_department (department),
            INDEX idx_name (name)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4)",
        
        // Patients table
        R"(CREATE TABLE IF NOT EXISTS patients (
            patient_id INT AUTO_INCREMENT PRIMARY KEY,
            user_id INT NOT NULL,
            name VARCHAR(100) NOT NULL,
            gender ENUM('Male', 'Female') NOT NULL DEFAULT 'Male',
            birth_date DATE NOT NULL,
            id_number VARCHAR(20) UNIQUE NOT NULL,
            phone_number VARCHAR(20),
            FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE CASCADE,
            INDEX idx_user_id (user_id),
            INDEX idx_id_number (id_number),
            INDEX idx_name (name)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4)",
        
        // Cases table
        R"(CREATE TABLE IF NOT EXISTS cases (
            case_id INT AUTO_INCREMENT PRIMARY KEY,
            patient_id INT NOT NULL,
            department VARCHAR(50) NOT NULL,
            doctor_id INT NOT NULL,
            diagnosis TEXT NOT NULL,
            diagnosis_date DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (patient_id) REFERENCES patients(patient_id) ON DELETE CASCADE,
            FOREIGN KEY (doctor_id) REFERENCES doctors(doctor_id) ON DELETE CASCADE,
            INDEX idx_patient_id (patient_id),
            INDEX idx_doctor_id (doctor_id),
            INDEX idx_department (department),
            INDEX idx_diagnosis_date (diagnosis_date)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4)",
        
        // Appointments table
        R"(CREATE TABLE IF NOT EXISTS appointments (
            appointment_id INT AUTO_INCREMENT PRIMARY KEY,
            patient_id INT NOT NULL,
            doctor_id INT NOT NULL,
            appointment_time DATETIME NOT NULL,
            department VARCHAR(50) NOT NULL,
            status ENUM('Booked', 'Attended', 'Cancelled') NOT NULL DEFAULT 'Booked',
            FOREIGN KEY (patient_id) REFERENCES patients(patient_id) ON DELETE CASCADE,
            FOREIGN KEY (doctor_id) REFERENCES doctors(doctor_id) ON DELETE CASCADE,
            INDEX idx_patient_id (patient_id),
            INDEX idx_doctor_id (doctor_id),
            INDEX idx_appointment_time (appointment_time),
            INDEX idx_status (status),
            INDEX idx_department (department)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4)",
        
        // Hospitalization table
        R"(CREATE TABLE IF NOT EXISTS hospitalization (
            hospitalization_id INT AUTO_INCREMENT PRIMARY KEY,
            patient_id INT NOT NULL,
            ward_number VARCHAR(20) NOT NULL,
            bed_number VARCHAR(20) NOT NULL,
            admission_date DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
            attending_doctor VARCHAR(100) NOT NULL,
            FOREIGN KEY (patient_id) REFERENCES patients(patient_id) ON DELETE CASCADE,
            INDEX idx_patient_id (patient_id),
            INDEX idx_ward_number (ward_number),
            INDEX idx_bed_number (bed_number),
            INDEX idx_admission_date (admission_date)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4)",
        
        // Prescriptions table
        R"(CREATE TABLE IF NOT EXISTS prescriptions (
            prescription_id INT AUTO_INCREMENT PRIMARY KEY,
            case_id INT NOT NULL,
            doctor_id INT NOT NULL,
            prescription_content TEXT NOT NULL,
            issued_date DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (case_id) REFERENCES cases(case_id) ON DELETE CASCADE,
            FOREIGN KEY (doctor_id) REFERENCES doctors(doctor_id) ON DELETE CASCADE,
            INDEX idx_case_id (case_id),
            INDEX idx_doctor_id (doctor_id),
            INDEX idx_issued_date (issued_date)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4)",
        
        // Medications table
        R"(CREATE TABLE IF NOT EXISTS medications (
            medication_id INT AUTO_INCREMENT PRIMARY KEY,
            prescription_id INT NOT NULL,
            medication_name VARCHAR(100) NOT NULL,
            quantity INT NOT NULL,
            usage_instructions TEXT NOT NULL,
            FOREIGN KEY (prescription_id) REFERENCES prescriptions(prescription_id) ON DELETE CASCADE,
            INDEX idx_prescription_id (prescription_id),
            INDEX idx_medication_name (medication_name)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4)"
    };
    
    if (!conn->beginTransaction()) {
        connectionPool->returnConnection(std::move(conn));
        return false;
    }
    
    for (const auto& query : createTableQueries) {
        if (!conn->executeUpdate(query)) {
            std::cerr << "Failed to create table: " << conn->getError() << std::endl;
            conn->rollback();
            connectionPool->returnConnection(std::move(conn));
            return false;
        }
    }
    
    bool result = conn->commit();
    connectionPool->returnConnection(std::move(conn));
    
    if (result) {
        std::cout << "Hospital database tables created successfully!" << std::endl;
    }
    
    return result;
}

bool DatabaseService::dropTables() {
    auto conn = connectionPool->getConnection();
    if (!conn) return false;
    
    std::vector<std::string> dropTableQueries = {
        "DROP TABLE IF EXISTS medications",
        "DROP TABLE IF EXISTS prescriptions",
        "DROP TABLE IF EXISTS hospitalization",
        "DROP TABLE IF EXISTS appointments",
        "DROP TABLE IF EXISTS cases",
        "DROP TABLE IF EXISTS patients",
        "DROP TABLE IF EXISTS doctors",
        "DROP TABLE IF EXISTS users"
    };
    
    if (!conn->beginTransaction()) {
        connectionPool->returnConnection(std::move(conn));
        return false;
    }
    
    for (const auto& query : dropTableQueries) {
        if (!conn->executeUpdate(query)) {
            std::cerr << "Failed to drop table: " << conn->getError() << std::endl;
            conn->rollback();
            connectionPool->returnConnection(std::move(conn));
            return false;
        }
    }
    
    bool result = conn->commit();
    connectionPool->returnConnection(std::move(conn));
    
    if (result) {
        std::cout << "Hospital database tables dropped successfully!" << std::endl;
    }
    
    return result;
}

bool DatabaseService::registerUser(const std::string& username, const std::string& password,
                                  UserType userType, const std::string& email,
                                  const std::string& phoneNumber) {
    if (username.empty() || password.length() < 6) {
        return false;
    }
    
    if (userDAO->userExists(username)) {
        return false;
    }
    
    User user(username, password, userType);
    user.setEmail(email);
    user.setPhoneNumber(phoneNumber);
    
    return userDAO->createUser(user);
}

std::unique_ptr<User> DatabaseService::loginUser(const std::string& username, const std::string& password) {
    return userDAO->authenticateUser(username, password);
}

bool DatabaseService::registerDoctor(int userId, const std::string& name, const std::string& department,
                                    const std::string& workingHours, const std::string& title) {
    if (name.empty() || department.empty() || workingHours.empty()) {
        return false;
    }
    
    Doctor doctor(userId, name, department, workingHours);
    doctor.setTitle(title);
    
    return doctorDAO->createDoctor(doctor);
}

bool DatabaseService::registerPatient(int userId, const std::string& name, Gender gender,
                                     const std::string& birthDate, const std::string& idNumber,
                                     const std::string& phoneNumber) {
    if (name.empty() || birthDate.empty() || idNumber.empty()) {
        return false;
    }
    
    if (patientDAO->patientExists(idNumber)) {
        return false;
    }
    
    Patient patient(userId, name, gender, birthDate, idNumber);
    patient.setPhoneNumber(phoneNumber);
    
    return patientDAO->createPatient(patient);
}

int DatabaseService::createMedicalCase(int patientId, const std::string& department, int doctorId,
                                      const std::string& diagnosis) {
    if (diagnosis.empty() || department.empty()) {
        return 0;
    }
    
    auto patient = patientDAO->getPatientById(patientId);
    auto doctor = doctorDAO->getDoctorById(doctorId);
    
    if (!patient || !doctor) {
        return 0;
    }
    
    Case medicalCase(patientId, department, doctorId, diagnosis);
    
    auto conn = connectionPool->getConnection();
    if (!conn) return 0;
    
    if (caseDAO->createCase(medicalCase)) {
        return static_cast<int>(conn->getLastInsertId());
    }
    
    connectionPool->returnConnection(std::move(conn));
    return 0;
}

int DatabaseService::bookAppointment(int patientId, int doctorId, const std::string& appointmentTime,
                                    const std::string& department) {
    if (appointmentTime.empty() || department.empty()) {
        return 0;
    }
    
    auto patient = patientDAO->getPatientById(patientId);
    auto doctor = doctorDAO->getDoctorById(doctorId);
    
    if (!patient || !doctor) {
        return 0;
    }
    
    Appointment appointment(patientId, doctorId, appointmentTime, department);
    
    auto conn = connectionPool->getConnection();
    if (!conn) return 0;
    
    if (appointmentDAO->createAppointment(appointment)) {
        return static_cast<int>(conn->getLastInsertId());
    }
    
    connectionPool->returnConnection(std::move(conn));
    return 0;
}

int DatabaseService::admitPatient(int patientId, const std::string& wardNumber, const std::string& bedNumber,
                                 const std::string& attendingDoctor) {
    if (wardNumber.empty() || bedNumber.empty() || attendingDoctor.empty()) {
        return 0;
    }
    
    auto patient = patientDAO->getPatientById(patientId);
    if (!patient) {
        return 0;
    }
    
    if (hospitalizationDAO->isBedOccupied(wardNumber, bedNumber)) {
        return 0; // Bed is already occupied
    }
    
    Hospitalization hospitalization(patientId, wardNumber, bedNumber, attendingDoctor);
    
    auto conn = connectionPool->getConnection();
    if (!conn) return 0;
    
    if (hospitalizationDAO->createHospitalization(hospitalization)) {
        return static_cast<int>(conn->getLastInsertId());
    }
    
    connectionPool->returnConnection(std::move(conn));
    return 0;
}

int DatabaseService::issuePrescription(int caseId, int doctorId, const std::string& prescriptionContent) {
    if (prescriptionContent.empty()) {
        return 0;
    }
    
    auto medicalCase = caseDAO->getCaseById(caseId);
    auto doctor = doctorDAO->getDoctorById(doctorId);
    
    if (!medicalCase || !doctor) {
        return 0;
    }
    
    Prescription prescription(caseId, doctorId, prescriptionContent);
    
    auto conn = connectionPool->getConnection();
    if (!conn) return 0;
    
    if (prescriptionDAO->createPrescription(prescription)) {
        return static_cast<int>(conn->getLastInsertId());
    }
    
    connectionPool->returnConnection(std::move(conn));
    return 0;
}

bool DatabaseService::addMedication(int prescriptionId, const std::string& medicationName, int quantity,
                                   const std::string& usageInstructions) {
    if (medicationName.empty() || quantity <= 0 || usageInstructions.empty()) {
        return false;
    }
    
    auto prescription = prescriptionDAO->getPrescriptionById(prescriptionId);
    if (!prescription) {
        return false;
    }
    
    Medication medication(prescriptionId, medicationName, quantity, usageInstructions);
    
    return medicationDAO->createMedication(medication);
}

DatabaseService::DatabaseStats DatabaseService::getDatabaseStats() {
    DatabaseStats stats = {};
    
    stats.totalUsers = userDAO->getUserCount();
    stats.totalDoctors = doctorDAO->getDoctorCount();
    stats.totalPatients = patientDAO->getPatientCount();
    stats.totalCases = caseDAO->getCaseCount();
    stats.totalAppointments = appointmentDAO->getAppointmentCount();
    stats.bookedAppointments = appointmentDAO->getAppointmentCountByStatus(AppointmentStatus::BOOKED);
    stats.attendedAppointments = appointmentDAO->getAppointmentCountByStatus(AppointmentStatus::ATTENDED);
    stats.cancelledAppointments = appointmentDAO->getAppointmentCountByStatus(AppointmentStatus::CANCELLED);
    stats.totalHospitalizations = hospitalizationDAO->getHospitalizationCount();
    stats.totalPrescriptions = prescriptionDAO->getPrescriptionCount();
    stats.totalMedications = medicationDAO->getMedicationCount();
    
    return stats;
}

std::vector<DatabaseService::PatientCaseInfo> DatabaseService::getPatientCaseHistory(int patientId) {
    auto conn = connectionPool->getConnection();
    std::vector<PatientCaseInfo> caseHistory;
    if (!conn) return caseHistory;
    
    std::stringstream query;
    query << "SELECT p.patient_id, p.name, p.id_number, c.case_id, c.diagnosis, c.diagnosis_date, "
          << "d.name, c.department FROM patients p "
          << "JOIN cases c ON p.patient_id = c.patient_id "
          << "JOIN doctors d ON c.doctor_id = d.doctor_id "
          << "WHERE p.patient_id = " << patientId << " ORDER BY c.diagnosis_date DESC";
    
    MYSQL_RES* result = conn->executeQuery(query.str());
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return caseHistory;
    }
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        PatientCaseInfo info;
        if (row[0]) info.patientId = std::stoi(row[0]);
        if (row[1]) info.patientName = std::string(row[1]);
        if (row[2]) info.idNumber = std::string(row[2]);
        if (row[3]) info.caseId = std::stoi(row[3]);
        if (row[4]) info.diagnosis = std::string(row[4]);
        if (row[5]) info.diagnosisDate = std::string(row[5]);
        if (row[6]) info.doctorName = std::string(row[6]);
        if (row[7]) info.department = std::string(row[7]);
        
        caseHistory.push_back(info);
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return caseHistory;
}

std::vector<DatabaseService::DoctorAppointmentInfo> DatabaseService::getDoctorAppointments(int doctorId) {
    auto conn = connectionPool->getConnection();
    std::vector<DoctorAppointmentInfo> appointments;
    if (!conn) return appointments;
    
    std::stringstream query;
    query << "SELECT d.doctor_id, d.name, d.department, a.appointment_id, p.name, "
          << "a.appointment_time, a.status FROM doctors d "
          << "JOIN appointments a ON d.doctor_id = a.doctor_id "
          << "JOIN patients p ON a.patient_id = p.patient_id "
          << "WHERE d.doctor_id = " << doctorId << " ORDER BY a.appointment_time DESC";
    
    MYSQL_RES* result = conn->executeQuery(query.str());
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return appointments;
    }
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        DoctorAppointmentInfo info;
        if (row[0]) info.doctorId = std::stoi(row[0]);
        if (row[1]) info.doctorName = std::string(row[1]);
        if (row[2]) info.department = std::string(row[2]);
        if (row[3]) info.appointmentId = std::stoi(row[3]);
        if (row[4]) info.patientName = std::string(row[4]);
        if (row[5]) info.appointmentTime = std::string(row[5]);
        if (row[6]) info.status = std::string(row[6]);
        
        appointments.push_back(info);
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return appointments;
}

bool DatabaseService::backupDatabase(const std::string& backupPath) {
    auto conn = connectionPool->getConnection();
    if (!conn) return false;
    
    // Implement database backup using mysqldump command
    std::string command = "mysqldump -h localhost -u root hospital_db > " + backupPath;
    
    // Execute the backup command
    int result = system(command.c_str());
    
    if (result == 0) {
        std::cout << "Database backup completed successfully to: " << backupPath << std::endl;
    } else {
        std::cerr << "Database backup failed. Error code: " << result << std::endl;
    }
    
    connectionPool->returnConnection(std::move(conn));
    return result == 0;
}

bool DatabaseService::optimizeTables() {
    auto conn = connectionPool->getConnection();
    if (!conn) return false;
    
    std::vector<std::string> tables = {
        "users", "doctors", "patients", "cases", "appointments", 
        "hospitalization", "prescriptions", "medications"
    };
    
    for (const auto& table : tables) {
        std::string query = "OPTIMIZE TABLE " + table;
        if (!conn->executeUpdate(query)) {
            std::cerr << "Failed to optimize table " << table << ": " << conn->getError() << std::endl;
        }
    }
    
    connectionPool->returnConnection(std::move(conn));
    return true;
}

bool DatabaseService::cleanupOldData(int daysOld) {
    auto conn = connectionPool->getConnection();
    if (!conn) return false;
    
    if (!conn->beginTransaction()) {
        connectionPool->returnConnection(std::move(conn));
        return false;
    }
    
    // Clean up old completed appointments (keep important data)
    std::stringstream query;
    query << "DELETE FROM appointments WHERE status IN ('Attended', 'Cancelled') "
          << "AND appointment_time < DATE_SUB(NOW(), INTERVAL " << daysOld << " DAY)";
    
    if (!conn->executeUpdate(query.str())) {
        conn->rollback();
        connectionPool->returnConnection(std::move(conn));
        return false;
    }
    
    bool result = conn->commit();
    connectionPool->returnConnection(std::move(conn));
    
    if (result) {
        std::cout << "Old data cleanup completed successfully!" << std::endl;
    }
    
    return result;
}

std::string DatabaseService::hashPassword(const std::string& password) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, password.c_str(), password.length());
    SHA256_Final(hash, &sha256);
    
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}