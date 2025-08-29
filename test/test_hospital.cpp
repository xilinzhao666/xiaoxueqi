#include <iostream>
#include <cassert>
#include <vector>
#include <memory>
#include "../include/HospitalService.h"

class HospitalTest {
private:
    std::unique_ptr<HospitalService> hospitalService;
    
public:
    HospitalTest() {
        hospitalService = std::make_unique<HospitalService>(
            "localhost", "root", "", "test_hospital_db"
        );
    }
    
    bool runAllTests() {
        std::cout << "=== 开始医院管理系统测试 ===" << std::endl;
        
        if (!initializeTestDatabase()) {
            std::cerr << "测试数据库初始化失败！" << std::endl;
            return false;
        }
        
        bool allPassed = true;
        
        allPassed &= testUserOperations();
        allPassed &= testDoctorOperations();
        allPassed &= testPatientOperations();
        allPassed &= testCaseOperations();
        allPassed &= testAppointmentOperations();
        allPassed &= testHospitalizationOperations();
        allPassed &= testPrescriptionOperations();
        allPassed &= testMedicationOperations();
        allPassed &= testComplexQueries();
        
        cleanupTestDatabase();
        
        if (allPassed) {
            std::cout << "=== 所有测试通过！ ===" << std::endl;
        } else {
            std::cout << "=== 部分测试失败！ ===" << std::endl;
        }
        
        return allPassed;
    }
    
private:
    bool initializeTestDatabase() {
        std::cout << "初始化测试数据库..." << std::endl;
        return hospitalService->initializeDatabase();
    }
    
    void cleanupTestDatabase() {
        std::cout << "清理测试数据库..." << std::endl;
        hospitalService->dropTables();
    }
    
    bool testUserOperations() {
        std::cout << "测试用户操作..." << std::endl;
        
        // Test user registration
        bool registerResult = hospitalService->registerUser(
            "testdoctor", "password123", UserType::DOCTOR, "doctor@test.com", "1234567890"
        );
        assert(registerResult && "医生用户注册失败");
        
        registerResult = hospitalService->registerUser(
            "testpatient", "password123", UserType::PATIENT, "patient@test.com", "0987654321"
        );
        assert(registerResult && "患者用户注册失败");
        
        // Test user login
        auto user = hospitalService->loginUser("testdoctor", "password123");
        assert(user != nullptr && "用户登录失败");
        assert(user->getUsername() == "testdoctor" && "用户名不匹配");
        assert(user->getUserType() == UserType::DOCTOR && "用户类型不匹配");
        
        // Test invalid login
        auto invalidUser = hospitalService->loginUser("testdoctor", "wrongpassword");
        assert(invalidUser == nullptr && "错误密码应该登录失败");
        
        std::cout << "用户操作测试通过！" << std::endl;
        return true;
    }
    
    bool testDoctorOperations() {
        std::cout << "测试医生操作..." << std::endl;
        
        // Get doctor user
        auto doctorUser = hospitalService->loginUser("testdoctor", "password123");
        assert(doctorUser != nullptr && "医生用户不存在");
        
        // Register doctor info
        bool registerResult = hospitalService->registerDoctor(
            doctorUser->getUserId(), "Dr. Test", "Cardiology", "9:00-17:00", "Chief"
        );
        assert(registerResult && "医生信息注册失败");
        
        // Get doctor by user ID
        auto doctor = hospitalService->getDoctorDAO()->getDoctorByUserId(doctorUser->getUserId());
        assert(doctor != nullptr && "获取医生信息失败");
        assert(doctor->getName() == "Dr. Test" && "医生姓名不匹配");
        assert(doctor->getDepartment() == "Cardiology" && "医生科室不匹配");
        
        std::cout << "医生操作测试通过！" << std::endl;
        return true;
    }
    
    bool testPatientOperations() {
        std::cout << "测试患者操作..." << std::endl;
        
        // Get patient user
        auto patientUser = hospitalService->loginUser("testpatient", "password123");
        assert(patientUser != nullptr && "患者用户不存在");
        
        // Register patient info
        bool registerResult = hospitalService->registerPatient(
            patientUser->getUserId(), "Test Patient", Gender::MALE, "1990-01-01", "123456789012345678", "1234567890"
        );
        assert(registerResult && "患者信息注册失败");
        
        // Get patient by user ID
        auto patient = hospitalService->getPatientDAO()->getPatientByUserId(patientUser->getUserId());
        assert(patient != nullptr && "获取患者信息失败");
        assert(patient->getName() == "Test Patient" && "患者姓名不匹配");
        assert(patient->getIdNumber() == "123456789012345678" && "患者身份证号不匹配");
        
        // Test duplicate ID number
        bool duplicateResult = hospitalService->registerPatient(
            patientUser->getUserId(), "Another Patient", Gender::FEMALE, "1985-05-05", "123456789012345678", "0987654321"
        );
        assert(!duplicateResult && "重复身份证号应该注册失败");
        
        std::cout << "患者操作测试通过！" << std::endl;
        return true;
    }
    
    bool testCaseOperations() {
        std::cout << "测试病例操作..." << std::endl;
        
        // Get patient and doctor
        auto patients = hospitalService->getPatientDAO()->getAllPatients();
        auto doctors = hospitalService->getDoctorDAO()->getAllDoctors();
        
        if (patients.empty() || doctors.empty()) {
            std::cerr << "需要患者和医生数据进行病例测试" << std::endl;
            return false;
        }
        
        int patientId = patients[0]->getPatientId();
        int doctorId = doctors[0]->getDoctorId();
        
        // Create medical case
        int caseId = hospitalService->createMedicalCase(
            patientId, "Cardiology", doctorId, "Hypertension diagnosis"
        );
        assert(caseId > 0 && "病例创建失败");
        
        // Get case
        auto medicalCase = hospitalService->getCaseDAO()->getCaseById(caseId);
        assert(medicalCase != nullptr && "获取病例失败");
        assert(medicalCase->getPatientId() == patientId && "病例患者ID不匹配");
        assert(medicalCase->getDoctorId() == doctorId && "病例医生ID不匹配");
        
        std::cout << "病例操作测试通过！" << std::endl;
        return true;
    }
    
    bool testAppointmentOperations() {
        std::cout << "测试预约操作..." << std::endl;
        
        // Get patient and doctor
        auto patients = hospitalService->getPatientDAO()->getAllPatients();
        auto doctors = hospitalService->getDoctorDAO()->getAllDoctors();
        
        if (patients.empty() || doctors.empty()) {
            std::cerr << "需要患者和医生数据进行预约测试" << std::endl;
            return false;
        }
        
        int patientId = patients[0]->getPatientId();
        int doctorId = doctors[0]->getDoctorId();
        
        // Book appointment
        int appointmentId = hospitalService->bookAppointment(
            patientId, doctorId, "2024-01-15 10:00:00", "Cardiology"
        );
        assert(appointmentId > 0 && "预约创建失败");
        
        // Get appointment
        auto appointment = hospitalService->getAppointmentDAO()->getAppointmentById(appointmentId);
        assert(appointment != nullptr && "获取预约失败");
        assert(appointment->getStatus() == AppointmentStatus::BOOKED && "预约状态不正确");
        
        // Update appointment status
        bool updateResult = hospitalService->getAppointmentDAO()->attendAppointment(appointmentId);
        assert(updateResult && "预约状态更新失败");
        
        std::cout << "预约操作测试通过！" << std::endl;
        return true;
    }
    
    bool testHospitalizationOperations() {
        std::cout << "测试住院操作..." << std::endl;
        
        // Get patient
        auto patients = hospitalService->getPatientDAO()->getAllPatients();
        if (patients.empty()) {
            std::cerr << "需要患者数据进行住院测试" << std::endl;
            return false;
        }
        
        int patientId = patients[0]->getPatientId();
        
        // Admit patient
        int hospitalizationId = hospitalService->admitPatient(
            patientId, "A101", "01", "Dr. Test"
        );
        assert(hospitalizationId > 0 && "住院登记失败");
        
        // Get hospitalization
        auto hospitalization = hospitalService->getHospitalizationDAO()->getHospitalizationById(hospitalizationId);
        assert(hospitalization != nullptr && "获取住院记录失败");
        assert(hospitalization->getWardNumber() == "A101" && "病房号不匹配");
        assert(hospitalization->getBedNumber() == "01" && "床位号不匹配");
        
        std::cout << "住院操作测试通过！" << std::endl;
        return true;
    }
    
    bool testPrescriptionOperations() {
        std::cout << "测试处方操作..." << std::endl;
        
        // Get case and doctor
        auto cases = hospitalService->getCaseDAO()->getAllCases();
        auto doctors = hospitalService->getDoctorDAO()->getAllDoctors();
        
        if (cases.empty() || doctors.empty()) {
            std::cerr << "需要病例和医生数据进行处方测试" << std::endl;
            return false;
        }
        
        int caseId = cases[0]->getCaseId();
        int doctorId = doctors[0]->getDoctorId();
        
        // Issue prescription
        int prescriptionId = hospitalService->issuePrescription(
            caseId, doctorId, "Take medication as prescribed for hypertension"
        );
        assert(prescriptionId > 0 && "处方开具失败");
        
        // Get prescription
        auto prescription = hospitalService->getPrescriptionDAO()->getPrescriptionById(prescriptionId);
        assert(prescription != nullptr && "获取处方失败");
        assert(prescription->getCaseId() == caseId && "处方病例ID不匹配");
        
        std::cout << "处方操作测试通过！" << std::endl;
        return true;
    }
    
    bool testMedicationOperations() {
        std::cout << "测试药物操作..." << std::endl;
        
        // Get prescription
        auto prescriptions = hospitalService->getPrescriptionDAO()->getAllPrescriptions();
        if (prescriptions.empty()) {
            std::cerr << "需要处方数据进行药物测试" << std::endl;
            return false;
        }
        
        int prescriptionId = prescriptions[0]->getPrescriptionId();
        
        // Add medication
        bool addResult = hospitalService->addMedication(
            prescriptionId, "Lisinopril", 30, "Take one tablet daily with food"
        );
        assert(addResult && "药物添加失败");
        
        // Get medications
        auto medications = hospitalService->getMedicationDAO()->getMedicationsByPrescriptionId(prescriptionId);
        assert(!medications.empty() && "获取药物列表失败");
        assert(medications[0]->getMedicationName() == "Lisinopril" && "药物名称不匹配");
        
        std::cout << "药物操作测试通过！" << std::endl;
        return true;
    }
    
    bool testComplexQueries() {
        std::cout << "测试复杂查询..." << std::endl;
        
        // Test patient case history
        auto patients = hospitalService->getPatientDAO()->getAllPatients();
        if (!patients.empty()) {
            int patientId = patients[0]->getPatientId();
            auto caseHistory = hospitalService->getPatientCaseHistory(patientId);
            // Should have at least one case from previous tests
        }
        
        // Test doctor appointments
        auto doctors = hospitalService->getDoctorDAO()->getAllDoctors();
        if (!doctors.empty()) {
            int doctorId = doctors[0]->getDoctorId();
            auto appointments = hospitalService->getDoctorAppointments(doctorId);
            // Should have at least one appointment from previous tests
        }
        
        // Test statistics
        auto stats = hospitalService->getHospitalStats();
        assert(stats.totalUsers > 0 && "用户统计错误");
        assert(stats.totalDoctors > 0 && "医生统计错误");
        assert(stats.totalPatients > 0 && "患者统计错误");
        
        std::cout << "复杂查询测试通过！" << std::endl;
        return true;
    }
};

int main() {
    try {
        HospitalTest test;
        bool result = test.runAllTests();
        return result ? 0 : 1;
        
    } catch (const std::exception& e) {
        std::cerr << "测试异常: " << e.what() << std::endl;
        return 1;
    }
}