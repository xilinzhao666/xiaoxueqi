#include "Appointment.h"
#include <memory>
#include <sstream>
#include <iostream>

// Appointment class implementation
Appointment::Appointment() : appointmentId(0), patientId(0), doctorId(0), status(AppointmentStatus::BOOKED) {}

Appointment::Appointment(int patientId, int doctorId, const std::string& appointmentTime, const std::string& department)
    : appointmentId(0), patientId(patientId), doctorId(doctorId), appointmentTime(appointmentTime), 
      department(department), status(AppointmentStatus::BOOKED) {}

std::string Appointment::statusToString() const {
    switch (status) {
        case AppointmentStatus::BOOKED: return "Booked";
        case AppointmentStatus::ATTENDED: return "Attended";
        case AppointmentStatus::CANCELLED: return "Cancelled";
    }
    return "Booked";
}

AppointmentStatus Appointment::stringToStatus(const std::string& statusStr) {
    if (statusStr == "Attended") return AppointmentStatus::ATTENDED;
    if (statusStr == "Cancelled") return AppointmentStatus::CANCELLED;
    return AppointmentStatus::BOOKED;
}

// AppointmentDAO class implementation
AppointmentDAO::AppointmentDAO(std::shared_ptr<ConnectionPool> pool) : connectionPool(pool) {}

bool AppointmentDAO::createAppointment(const Appointment& appointment) {
    auto conn = connectionPool->getConnection();
    if (!conn) return false;
    
    std::stringstream query;
    query << "INSERT INTO appointments (patient_id, doctor_id, appointment_time, department, status) VALUES ("
          << appointment.getPatientId() << ", "
          << appointment.getDoctorId() << ", "
          << "'" << conn->escapeString(appointment.getAppointmentTime()) << "', "
          << "'" << conn->escapeString(appointment.getDepartment()) << "', "
          << "'" << appointment.statusToString() << "')";
    
    bool result = conn->executeUpdate(query.str());
    connectionPool->returnConnection(std::move(conn));
    return result;
}

std::unique_ptr<Appointment> AppointmentDAO::getAppointmentById(int appointmentId) {
    auto conn = connectionPool->getConnection();
    if (!conn) return nullptr;
    
    std::stringstream query;
    query << "SELECT appointment_id, patient_id, doctor_id, appointment_time, department, status "
          << "FROM appointments WHERE appointment_id = " << appointmentId;
    
    MYSQL_RES* result = conn->executeQuery(query.str());
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return nullptr;
    }
    
    MYSQL_ROW row = mysql_fetch_row(result);
    std::unique_ptr<Appointment> appointment = nullptr;
    
    if (row) {
        unsigned long* lengths = mysql_fetch_lengths(result);
        appointment = std::unique_ptr<Appointment>(mapRowToAppointment(row, lengths));
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return appointment;
}

std::vector<std::unique_ptr<Appointment>> AppointmentDAO::getAppointmentsByPatientId(int patientId) {
    auto conn = connectionPool->getConnection();
    std::vector<std::unique_ptr<Appointment>> appointments;
    if (!conn) return appointments;
    
    std::stringstream query;
    query << "SELECT appointment_id, patient_id, doctor_id, appointment_time, department, status "
          << "FROM appointments WHERE patient_id = " << patientId << " ORDER BY appointment_time DESC";
    
    MYSQL_RES* result = conn->executeQuery(query.str());
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return appointments;
    }
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        unsigned long* lengths = mysql_fetch_lengths(result);
        appointments.push_back(std::unique_ptr<Appointment>(mapRowToAppointment(row, lengths)));
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return appointments;
}

std::vector<std::unique_ptr<Appointment>> AppointmentDAO::getAppointmentsByDoctorId(int doctorId) {
    auto conn = connectionPool->getConnection();
    std::vector<std::unique_ptr<Appointment>> appointments;
    if (!conn) return appointments;
    
    std::stringstream query;
    query << "SELECT appointment_id, patient_id, doctor_id, appointment_time, department, status "
          << "FROM appointments WHERE doctor_id = " << doctorId << " ORDER BY appointment_time DESC";
    
    MYSQL_RES* result = conn->executeQuery(query.str());
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return appointments;
    }
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        unsigned long* lengths = mysql_fetch_lengths(result);
        appointments.push_back(std::unique_ptr<Appointment>(mapRowToAppointment(row, lengths)));
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return appointments;
}

std::vector<std::unique_ptr<Appointment>> AppointmentDAO::getAppointmentsByDepartment(const std::string& department) {
    auto conn = connectionPool->getConnection();
    std::vector<std::unique_ptr<Appointment>> appointments;
    if (!conn) return appointments;
    
    std::stringstream query;
    query << "SELECT appointment_id, patient_id, doctor_id, appointment_time, department, status "
          << "FROM appointments WHERE department = '" << conn->escapeString(department) 
          << "' ORDER BY appointment_time DESC";
    
    MYSQL_RES* result = conn->executeQuery(query.str());
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return appointments;
    }
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        unsigned long* lengths = mysql_fetch_lengths(result);
        appointments.push_back(std::unique_ptr<Appointment>(mapRowToAppointment(row, lengths)));
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return appointments;
}

std::vector<std::unique_ptr<Appointment>> AppointmentDAO::getAppointmentsByStatus(AppointmentStatus status) {
    auto conn = connectionPool->getConnection();
    std::vector<std::unique_ptr<Appointment>> appointments;
    if (!conn) return appointments;
    
    std::stringstream query;
    query << "SELECT appointment_id, patient_id, doctor_id, appointment_time, department, status "
          << "FROM appointments WHERE status = '" << statusToString(status) 
          << "' ORDER BY appointment_time DESC";
    
    MYSQL_RES* result = conn->executeQuery(query.str());
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return appointments;
    }
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        unsigned long* lengths = mysql_fetch_lengths(result);
        appointments.push_back(std::unique_ptr<Appointment>(mapRowToAppointment(row, lengths)));
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return appointments;
}

std::vector<std::unique_ptr<Appointment>> AppointmentDAO::getAllAppointments() {
    auto conn = connectionPool->getConnection();
    std::vector<std::unique_ptr<Appointment>> appointments;
    if (!conn) return appointments;
    
    std::string query = "SELECT appointment_id, patient_id, doctor_id, appointment_time, department, status "
                       "FROM appointments ORDER BY appointment_time DESC";
    
    MYSQL_RES* result = conn->executeQuery(query);
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return appointments;
    }
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        unsigned long* lengths = mysql_fetch_lengths(result);
        appointments.push_back(std::unique_ptr<Appointment>(mapRowToAppointment(row, lengths)));
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return appointments;
}

bool AppointmentDAO::updateAppointment(const Appointment& appointment) {
    auto conn = connectionPool->getConnection();
    if (!conn) return false;
    
    std::stringstream query;
    query << "UPDATE appointments SET "
          << "patient_id = " << appointment.getPatientId() << ", "
          << "doctor_id = " << appointment.getDoctorId() << ", "
          << "appointment_time = '" << conn->escapeString(appointment.getAppointmentTime()) << "', "
          << "department = '" << conn->escapeString(appointment.getDepartment()) << "', "
          << "status = '" << appointment.statusToString() << "' "
          << "WHERE appointment_id = " << appointment.getAppointmentId();
    
    bool result = conn->executeUpdate(query.str());
    connectionPool->returnConnection(std::move(conn));
    return result;
}

bool AppointmentDAO::updateAppointmentStatus(int appointmentId, AppointmentStatus status) {
    auto conn = connectionPool->getConnection();
    if (!conn) return false;
    
    std::stringstream query;
    query << "UPDATE appointments SET status = '" << statusToString(status) 
          << "' WHERE appointment_id = " << appointmentId;
    
    bool result = conn->executeUpdate(query.str());
    connectionPool->returnConnection(std::move(conn));
    return result;
}

bool AppointmentDAO::deleteAppointment(int appointmentId) {
    auto conn = connectionPool->getConnection();
    if (!conn) return false;
    
    std::stringstream query;
    query << "DELETE FROM appointments WHERE appointment_id = " << appointmentId;
    
    bool result = conn->executeUpdate(query.str());
    connectionPool->returnConnection(std::move(conn));
    return result;
}

bool AppointmentDAO::bookAppointment(int appointmentId) {
    return updateAppointmentStatus(appointmentId, AppointmentStatus::BOOKED);
}

bool AppointmentDAO::attendAppointment(int appointmentId) {
    return updateAppointmentStatus(appointmentId, AppointmentStatus::ATTENDED);
}

bool AppointmentDAO::cancelAppointment(int appointmentId) {
    return updateAppointmentStatus(appointmentId, AppointmentStatus::CANCELLED);
}

std::vector<std::unique_ptr<Appointment>> AppointmentDAO::searchAppointments(const std::string& searchTerm) {
    auto conn = connectionPool->getConnection();
    std::vector<std::unique_ptr<Appointment>> appointments;
    if (!conn) return appointments;
    
    std::stringstream query;
    query << "SELECT appointment_id, patient_id, doctor_id, appointment_time, department, status "
          << "FROM appointments WHERE department LIKE '%" << conn->escapeString(searchTerm) << "%' "
          << "ORDER BY appointment_time DESC";
    
    MYSQL_RES* result = conn->executeQuery(query.str());
    if (!result) {
        connectionPool->returnConnection(std::move(conn));
        return appointments;
    }
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        unsigned long* lengths = mysql_fetch_lengths(result);
        appointments.push_back(std::unique_ptr<Appointment>(mapRowToAppointment(row, lengths)));
    }
    
    mysql_free_result(result);
    connectionPool->returnConnection(std::move(conn));
    return appointments;
}

int AppointmentDAO::getAppointmentCount() {
    auto conn = connectionPool->getConnection();
    if (!conn) return 0;
    
    std::string query = "SELECT COUNT(*) FROM appointments";
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

int AppointmentDAO::getAppointmentCountByStatus(AppointmentStatus status) {
    auto conn = connectionPool->getConnection();
    if (!conn) return 0;
    
    std::stringstream query;
    query << "SELECT COUNT(*) FROM appointments WHERE status = '" << statusToString(status) << "'";
    
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

int AppointmentDAO::getAppointmentCountByDoctor(int doctorId) {
    auto conn = connectionPool->getConnection();
    if (!conn) return 0;
    
    std::stringstream query;
    query << "SELECT COUNT(*) FROM appointments WHERE doctor_id = " << doctorId;
    
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

Appointment* AppointmentDAO::mapRowToAppointment(MYSQL_ROW row, unsigned long* lengths) {
    if (!row) return nullptr;
    
    Appointment* appointment = new Appointment();
    
    if (row[0]) appointment->setAppointmentId(std::stoi(row[0]));
    if (row[1]) appointment->setPatientId(std::stoi(row[1]));
    if (row[2]) appointment->setDoctorId(std::stoi(row[2]));
    if (row[3]) appointment->setAppointmentTime(std::string(row[3], lengths[3]));
    if (row[4]) appointment->setDepartment(std::string(row[4], lengths[4]));
    if (row[5]) appointment->setStatus(stringToStatus(std::string(row[5], lengths[5])));
    
    return appointment;
}

std::string AppointmentDAO::statusToString(AppointmentStatus status) {
    switch (status) {
        case AppointmentStatus::BOOKED: return "Booked";
        case AppointmentStatus::ATTENDED: return "Attended";
        case AppointmentStatus::CANCELLED: return "Cancelled";
    }
    return "Booked";
}

AppointmentStatus AppointmentDAO::stringToStatus(const std::string& statusStr) {
    if (statusStr == "Attended") return AppointmentStatus::ATTENDED;
    if (statusStr == "Cancelled") return AppointmentStatus::CANCELLED;
    return AppointmentStatus::BOOKED;
}