#ifndef APPOINTMENT_H
#define APPOINTMENT_H

#include <string>
#include <vector>
#include <memory>
#include "DatabaseConnection.h"

enum class AppointmentStatus {
    BOOKED,
    ATTENDED,
    CANCELLED
};

class Appointment {
private:
    int appointmentId;
    int patientId;
    int doctorId;
    std::string appointmentTime;
    std::string department;
    AppointmentStatus status;

public:
    Appointment();
    Appointment(int patientId, int doctorId, const std::string& appointmentTime, const std::string& department);
    
    // Getter methods
    int getAppointmentId() const { return appointmentId; }
    int getPatientId() const { return patientId; }
    int getDoctorId() const { return doctorId; }
    std::string getAppointmentTime() const { return appointmentTime; }
    std::string getDepartment() const { return department; }
    AppointmentStatus getStatus() const { return status; }
    
    // Setter methods
    void setAppointmentId(int id) { appointmentId = id; }
    void setPatientId(int id) { patientId = id; }
    void setDoctorId(int id) { doctorId = id; }
    void setAppointmentTime(const std::string& time) { appointmentTime = time; }
    void setDepartment(const std::string& dept) { department = dept; }
    void setStatus(AppointmentStatus appointmentStatus) { status = appointmentStatus; }
    
    // Utility methods
    std::string statusToString() const;
    static AppointmentStatus stringToStatus(const std::string& statusStr);
};

class AppointmentDAO {
private:
    std::shared_ptr<ConnectionPool> connectionPool;
    
public:
    AppointmentDAO(std::shared_ptr<ConnectionPool> pool);
    
    // CRUD operations
    bool createAppointment(const Appointment& appointment);
    std::unique_ptr<Appointment> getAppointmentById(int appointmentId);
    std::vector<std::unique_ptr<Appointment>> getAppointmentsByPatientId(int patientId);
    std::vector<std::unique_ptr<Appointment>> getAppointmentsByDoctorId(int doctorId);
    std::vector<std::unique_ptr<Appointment>> getAppointmentsByDepartment(const std::string& department);
    std::vector<std::unique_ptr<Appointment>> getAppointmentsByStatus(AppointmentStatus status);
    std::vector<std::unique_ptr<Appointment>> getAllAppointments();
    
    bool updateAppointment(const Appointment& appointment);
    bool updateAppointmentStatus(int appointmentId, AppointmentStatus status);
    bool deleteAppointment(int appointmentId);
    
    // Status management
    bool bookAppointment(int appointmentId);
    bool attendAppointment(int appointmentId);
    bool cancelAppointment(int appointmentId);
    
    // Search and statistics
    std::vector<std::unique_ptr<Appointment>> searchAppointments(const std::string& searchTerm);
    int getAppointmentCount();
    int getAppointmentCountByStatus(AppointmentStatus status);
    int getAppointmentCountByDoctor(int doctorId);
    
private:
    Appointment* mapRowToAppointment(MYSQL_ROW row, unsigned long* lengths);
    std::string statusToString(AppointmentStatus status);
    AppointmentStatus stringToStatus(const std::string& statusStr);
};

#endif // APPOINTMENT_H